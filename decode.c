#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "pq.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "i:o:vh"

union {
  uint8_t b[sizeof(Header)];
  Header s;
} hh;

int main(int argc, char **argv) {
  // sets up the vars and the default values
  int fin = STDIN_FILENO;
  int fout = STDOUT_FILENO;
  int verbose = 0;

  int opt = 0;
  // Parses the command line args
  while (((opt = getopt(argc, argv, OPTIONS)) != -1)) {
    // Swtich statement based off the command line arg passed
    switch (opt) {
    case 'i':
      // opens the file based on the passed in name
      fin = open(optarg, O_RDWR);
      if (fin == -1) {
        fprintf(stderr, "File opening failed!\n");
        return 1;
      }
      break;
    case 'o':
      fout = open(optarg, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
      if (fout == -1) {
        fprintf(stderr, "File opening failed!\n");
        return 1;
      }
      break;
    case 'v':
      // sets flag
      verbose = 1;
      break;
    case 'h':
      // help message
      fprintf(stderr, "%s",
              "SYNOPSIS\n"
              "  A Huffman decoder.\n"
              "  Decompresses a file using the Huffman coding algorithm.\n"
              "\n"
              "USAGE\n"
              "  ./decode [-h] [-i infile] [-o outfile]\n"
              "\n"
              "OPTIONS\n"
              "  -h             Program usage and help.\n"
              "  -v             Print compression statistics.\n"
              "  -i infile      Input file to decompress.\n"
              "  -o outfile     Output of decompressed data.\n");
      return 0;
    case '?':
      fprintf(stderr, "%s",
              "SYNOPSIS\n"
              "  A Huffman decoder.\n"
              "  Decompresses a file using the Huffman coding algorithm.\n"
              "\n"
              "USAGE\n"
              "  ./decode [-h] [-i infile] [-o outfile]\n"
              "\n"
              "OPTIONS\n"
              "  -h             Program usage and help.\n"
              "  -v             Print compression statistics.\n"
              "  -i infile      Input file to decompress.\n"
              "  -o outfile     Output of decompressed data.\n");
      return 1;
    }
  }
  // inits union var
  union hh;
  read_bytes(fin, hh.b, sizeof(Header));
  uint16_t ts = (hh.s).tree_size; // holds tree size
  if ((hh.s).magic != MAGIC)      // checks if the magic is the same else error
  {
    fprintf(stderr, "Error: unable to read header.\n");
    return 1;
  }

  uint8_t tarr[ts];                    // makes tree array with given size
  memset(tarr, 0, sizeof(tarr));       // zeros out
  read_bytes(fin, tarr, ts);           // reads into the arr
  Node *root = rebuild_tree(ts, tarr); // rebuilds tree

  uint64_t bywr;
  Node *rootcopy = root;
  fchmod(fout, (hh.s).permissions); // sets permissions
  for (bywr = 0;
       bywr < (hh.s).file_size;) // loops until the end of the file size
  {
    uint8_t bit;
    if ((rootcopy->left == NULL) &&
        (rootcopy->right == NULL)) // checks for leafs
    {
      uint8_t *sym = &rootcopy->symbol;
      write_bytes(fout, sym, 1); // writs out symbol
      bywr += 1;
      rootcopy = root;
    }
    if (read_bit(fin, &bit) == false) // checks if no more bits
    {
      break;
    }
    if ((bit == 0) && (rootcopy->left != NULL)) {
      rootcopy = rootcopy->left;
    }
    if ((bit == 1) && (rootcopy->right != NULL)) {
      rootcopy = rootcopy->right;
    }
  }

  if (verbose == 1) // checks verbose flag
  {
    double spacesave = 100 * (1 - ((double)bytes_read / bytes_written));
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
    fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_written);
    fprintf(stderr, "Space saving: %0.2f%%\n", spacesave);
  }

  delete_tree(&root); // deletes tree
  close(fin);         // closes files
  close(fout);

  return 0;
}

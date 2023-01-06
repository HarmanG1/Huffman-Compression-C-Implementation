#define _GNU_SOURCE
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
  uint32_t A = ALPHABET;
  int B = BLOCK;

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
              "  A Huffman encoder.\n"
              "  Compresses a file using the Huffman coding algorithm.\n"
              "\n"
              "USAGE\n"
              "  ./encode [-h] [-i infile] [-o outfile]\n"
              "\n"
              "OPTIONS\n"
              "  -h             Program usage and help.\n"
              "  -v             Print compression statistics.\n"
              "  -i infile      Input file to compress.\n"
              "  -o outfile     Output of compressed data.\n");
      return 0;
    case '?':
      fprintf(stderr, "%s",
              "SYNOPSIS\n"
              "  A Huffman encoder.\n"
              "  Compresses a file using the Huffman coding algorithm.\n"
              "\n"
              "USAGE\n"
              "  ./encode [-h] [-i infile] [-o outfile]\n"
              "\n"
              "OPTIONS\n"
              "  -h             Program usage and help.\n"
              "  -v             Print compression statistics.\n"
              "  -i infile      Input file to compress.\n"
              "  -o outfile     Output of compressed data.\n");
      return 1;
    }
  }
  // creats the hist of size ALPHABET
  uint64_t hist[A];
  memset(hist, 0, sizeof(hist)); // sets it all to zero
  hist[0] = 1;                   // sets the first two elements as instructed
  hist[1] = 1;

  int ftmp = 0;     // temp file var
  int rby = 0;      // var to hold read bit count
  uint8_t buff[B];  // buffer of size block
  int madetemp = 0; // flag to check if a temp file has been made
  int seekcheck =
      lseek(fin, 0, SEEK_SET); // attempts to go the the start of the infile
  if (seekcheck == -1) // if infile cannot be rewound then makes a temp file
  {
    madetemp = 1; // sets flag
    ftmp = open("/tmp", O_TMPFILE | O_RDWR, S_IRWXU);
    for (rby = read_bytes(fin, buff, B);
         rby != 0;) // loops over the infile and fills up the tempfile
    {
      write_bytes(ftmp, buff, rby);
      rby = read_bytes(fin, buff, B);
    }
  }
  if (madetemp) // checks the flag
  {
    fin = ftmp; // if flag is set then set the fin var to the temp file
    lseek(fin, 0, SEEK_SET); // rewind the file to the start
  }
  for (rby = read_bytes(fin, buff, B); rby != 0;) // loop through fin
  {
    for (uint32_t i = 0; (int)i < rby;
         i++) // loop until i reaches the bits read
    {
      uint8_t hindex = buff[i];
      hist[hindex] += 1; // sets the hist indidices based off the buffer
    }
    rby = read_bytes(fin, buff, B);
  }
  Node *root = build_tree(hist); // calls build tree and gets the root
  Code table[A];                 // creats a table and zeros it out
  memset(table, 0, sizeof(table));
  build_codes(root, table); // calls build codes with the table and root

  union hh; // inits a union var to hold the header
  // sets the header values as given in the .h file
  (hh.s).magic = MAGIC;
  struct stat buf;           // creates a stat var to hold file info
  fstat(fin, &buf);          // calls f \stat to fill stat var
  fchmod(fout, buf.st_mode); // sets file permissions
  (hh.s).permissions = buf.st_mode;
  uint16_t uscount = 0;
  for (uint32_t i = 0; i < A;
       i++) // loops through hist and counts each symbol that has been set
  {
    if (hist[i] != 0) {
      uscount += 1;
    }
  }
  (hh.s).tree_size = (3 * uscount) - 1;
  (hh.s).file_size = buf.st_size;

  write_bytes(fout, hh.b, sizeof(Header)); // writes header to outfile
  dump_tree(fout, root);                   // calls dump tree with the root

  lseek(fin, 0, SEEK_SET); // returns the file to the start
  for (rby = read_bytes(fin, buff, B);
       rby != 0;) // reads the file until there is no more bits to read
  {
    for (uint32_t i = 0; (int)i < rby; i++) {
      uint8_t tindex = buff[i]; // gets the table index from the buffer
      Code c = table[tindex];   // gets the code from the table
      write_code(fout,
                 &c); // calls write code with the code revcided from the table
    }
    rby = read_bytes(fin, buff, B);
  }
  flush_codes(fout); // flushes out any remaining bits

  if (verbose == 1) // checks the verbose flag
  {
    if (!madetemp) // checks if a temp file has been made and adjusts the stats
                   // accordingly
    {
      double spacesave = 100 * (1 - ((double)bytes_written / buf.st_size));
      fprintf(stderr, "Uncompressed file size: %lu bytes\n", buf.st_size);
      fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
      fprintf(stderr, "Space saving: %0.2f%%\n", spacesave);
    } else if (madetemp) {
      double spacesave =
          100 * (1 - ((double)(bytes_written - buf.st_size) / buf.st_size));
      fprintf(stderr, "Uncompressed file size: %lu bytes\n", buf.st_size);
      fprintf(stderr, "Compressed file size: %lu bytes\n",
              (bytes_written - buf.st_size));
      fprintf(stderr, "Space saving: %0.2f%%\n", spacesave);
    }
  }

  delete_tree(&root); // deletes the made tree
  if (madetemp)       // checks if temp file needs to be closed
  {
    close(ftmp);
  }
  close(fin); // closes the input and output files
  close(fout);

  return 0;
}

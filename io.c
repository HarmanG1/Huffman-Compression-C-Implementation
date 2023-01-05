#include "io.h"
#include "code.h"
#include "defines.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
// Stats vars
uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint8_t buffer[BLOCK]; // buffer var
static uint64_t index = 0;    // keeps track of current buffer index
static uint64_t size = 0;     // used in read_bit to track size
static uint32_t blockeight = BLOCK * 8;

// Function loops calls to read() until either its read all the bytes or there
// are no more left
int read_bytes(int infile, uint8_t *buf, int nbytes) {
  int byr, zerocheck; // inits vars to use in the loop
  for (byr = 0; byr != nbytes;
       byr +=
       zerocheck) // loops until bytes read is equal to the desired ammount
  {
    zerocheck = read(infile, buf + byr,
                     nbytes - byr); // reads the file while passed in a reutrn
    if (zerocheck == 0)             // checks if there are no more bytes left
    {
      break;
    }
  }
  bytes_read += byr; // updates the stats var with the bytes read
  return byr;
}
// Function loops calls to write() until either its written all the bytes or
// there are no more left
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  int byw, zerocheck; // inits vars to use in the loop
  for (byw = 0; byw != nbytes;
       byw +=
       zerocheck) // loops until bytes written is equal to the desired ammount
  {
    zerocheck = write(outfile, buf + byw, nbytes - byw);
    if (zerocheck == 0) // checks if there are no more bytes left
    {
      break;
    }
  }
  bytes_written += byw; // updates the stats var with the bytes written
  return byw;
}
// Function reads bits one at a time from a buffer
bool read_bit(int infile, uint8_t *bit) {
  if (index == 0) // if the buffer is empty then fill it up
  {
    size = read_bytes(infile, buffer, BLOCK); // keep track of size with the var
  }

  *bit = (buffer[index / 8] >> (index % 8)) & 1; // sets the bit from the buffer
  index++;
  if (index ==
      ((size * 8) + 1)) // checks if the buffer has been fully read and returns
  {
    return false;
  }
  return true;
}
// Function buffers the passed in codes
void write_code(int outfile, Code *c) {
  for (uint32_t i = 0; i < code_size(c);
       i++) // loops from 0 to the end of the code
  {
    if (code_get_bit(c, i)) // checks if the bit is 1 or 0 and updates the
                            // buffer accordingly
    {
      buffer[index / 8] |= (1 << (index % 8));
    } else if (!(code_get_bit(c, i))) {
      buffer[index / 8] &= ~(1 << (index % 8));
    }

    index++;
    if (index >= blockeight) // checks if the buffer is full
    {
      write_bytes(outfile, buffer, BLOCK); // writes out the buffer
      index = 0; // resets the buffer index to the start
    }
  }
}
// Function writes out any left over bits in the buffer
void flush_codes(int outfile) {
  if (index != 0) // checks if buffer is not empty
  {
    int nbyt =
        (index % (blockeight)); // sets a var to pass in for the byte count
    write_bytes(outfile, buffer,
                (nbyt / 8) + 1); // writes the remaining bits out
  }
}

#include "code.h"
#include "defines.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
// Function to init the code var
Code code_init(void) {
  Code c;
  c.top = 0;
  memset(c.bits, 0, sizeof(c.bits)); // sets the elements of c.bits to 0
  return c;
}
// Function returns code size
uint32_t code_size(Code *c) { return c->top; }
// Function checks if code is empty
bool code_empty(Code *c) {
  if (c->top == 0) {
    return true;
  }
  return false;
}
// Function checks if code is full
bool code_full(Code *c) {
  if (c->top == MAX_CODE_SIZE) {
    return true;
  }
  return false;
}
// Function sets a bit in the code
bool code_set_bit(Code *c, uint32_t i) {
  if (i > MAX_CODE_SIZE) {
    return false;
  }
  c->bits[i / 8] |= (1 << (i % 8)); // sets the bit using the passed in i
  return true;
}
// Function clears a bit in the code
bool code_clr_bit(Code *c, uint32_t i) {
  if (i > MAX_CODE_SIZE) {
    return false;
  }
  c->bits[i / 8] &= ~(1 << (i % 8)); // clears the bit using the passed in i
  return true;
}
// Function gets a bit in the code
bool code_get_bit(Code *c, uint32_t i) {
  if (i > MAX_CODE_SIZE) {
    return false;
  }

  if ((c->bits[i / 8] & (1 << i % 8)) >>
      (i % 8)) // gets the bit using the passed in i
  {
    return true;
  }

  return false;
}
// Function pushes a bit out of code
bool code_push_bit(Code *c, uint8_t bit) {
  if (code_full(c)) // checks if code is full
  {
    return false;
  }
  if (bit == 1) // sets or clears the bit depending if it is 1 or 0
  {
    code_set_bit(c, c->top);
  } else if (bit == 0) {
    code_clr_bit(c, c->top);
  }
  c->top++;
  return true;
}
// Function pops a bit off of code and passes it into bit
bool code_pop_bit(Code *c, uint8_t *bit) {
  if (code_empty(c)) // checks if code is empty
  {
    return false;
  }
  c->top += -1;
  if (code_get_bit(
          c, c->top)) // if the bit is 1 then it also clears it while passing
  {
    *bit = 1;
    code_clr_bit(c, c->top);
  } else {
    *bit = 0;
  }
  return true;
}
// Function to print out the code
void code_print(Code *c) {
  for (uint32_t i = 0; i < (code_size(c)); i++) {
    if (code_get_bit(c, i)) // calls get bit to decide on the print
    {
      printf("1");
    } else if (!(code_get_bit(c, i))) {
      printf("0");
    }
  }
  printf("\n");
}

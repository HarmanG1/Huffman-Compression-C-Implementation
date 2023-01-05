#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Constructor for the node
Node *node_create(uint8_t symbol, uint64_t frequency) {
  Node *n = (Node *)malloc(sizeof(Node)); // allocates memory
  // sets the vars with the passed in symbol and frequency
  n->symbol = symbol;
  n->frequency = frequency;
  n->left = NULL; // inits left and right to null
  n->right = NULL;
  return n;
}
// Function clears allocated memeory
void node_delete(Node **n) {
  free(*n);
  *n = NULL; // sets pointer to null after freeing
}
// Joins together two passsed in nodes
Node *node_join(Node *left, Node *right) {
  // creates a node with the parent symbol and combined frequency
  Node *n = node_create('$', (left->frequency + right->frequency));
  n->left = left; // sets the left and right to the passed in ones
  n->right = right;
  return n;
}
// Function to print out nodes
void node_print(Node *n) {
  if (isprint(n->symbol) &&
      (!iscntrl(
          n->symbol))) // checks if it is a contol char and if its printable
  {
    printf("%c : %lu\n", n->symbol, n->frequency);
  } else {
    printf("0x%02" PRIx8 " : %lu\n", n->symbol, n->frequency);
  }
}
// Prints out the comparison between two nodes frequencies
bool node_cmp(Node *n, Node *m) {
  if (n->frequency > m->frequency) {
    return true;
  }
  return false;
}
// Prints out the node's symbol
void node_print_sym(Node *n) {
  if (isprint(n->symbol) && (!iscntrl(n->symbol))) {
    printf("%c", n->symbol);
  } else {
    printf("0x%02" PRIx8, n->symbol);
  }
}

#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <stdint.h>
#include <stdio.h>
// Function builds tree 
Node *build_tree(uint64_t hist[static ALPHABET]) {
  PriorityQueue *q = pq_create(ALPHABET); // creats a pq of size ALPHABET
  for (uint32_t i = 0; i < ALPHABET; i++) // loops through the passed in hist
  {
    if (hist[i]) // if the hist has a set index then it enqueues it in the pq
    {
      Node *hn = node_create(i, hist[i]);
      enqueue(q, hn);
    }
  }

  Node *left, *right, *parent; // sets up the vars to be used in the loop
  while (pq_size(q) > 1) {
    // De and enqueues as expected
    dequeue(q, &left);
    dequeue(q, &right);
    parent = node_join(left, right);
    enqueue(q, parent);
  }

  Node *root;
  dequeue(q, &root); // gets the node from the pq
  pq_delete(&q);     // clears the pq
  return root;       // returns the node
}

// static code var to be used
static Code c = {0};
// Function fills out the code table 
void build_codes(Node *root, Code table[static ALPHABET]) {
  uint8_t b = 0;
  if (root != NULL) // checks if root is null
  {
    if ((root->left == NULL) &&
        (root->right == NULL)) // checks if leaf and sets the table index
    {
      table[root->symbol] = c;
    } else {
      // Pushes and pops the bits
      code_push_bit(&c, 0);
      build_codes(root->left, table);
      code_pop_bit(&c, &b);

      code_push_bit(&c, 1);
      build_codes(root->right, table);
      code_pop_bit(&c, &b);
    }
  }
}
// Writes the tree to the outfile with a
// post order transversal
void dump_tree(int outfile, Node *root) {
  if (root != NULL) {
    dump_tree(outfile,
              root->left); // recursivley calls the function for left and right
    dump_tree(outfile, root->right);

    if ((root->left == NULL) &&
        (root->right == NULL)) // checks if leaf or not and writes accordingly
    {
      uint8_t ll = 'L';
      write_bytes(outfile, &ll, 1);
      write_bytes(outfile, &root->symbol, 1);
    } else {
      uint8_t ii = 'I';
      write_bytes(outfile, &ii, 1);
    }
  }
}
// Function reconstructs the tree given the array tree_dump
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
  Stack *s = stack_create(nbytes);      // creates a stack of the tree size
  Node *left, *right, *root;            // sets up node vars to be used
  for (uint32_t i = 0; i < nbytes; i++) // loops the length of the tree array
  {
    if (tree[i] == 'L') // checks if its a leaf or not and acts accoridngly
    {
      Node *ln = node_create(tree[i + 1], 0); // creats a new node
      stack_push(s, ln);                      // pushes it onto the stack
    } else if (tree[i] == 'I') {
      stack_pop(s,
                &right); // pops the left and right off the stack and passes it
      stack_pop(s, &left);
      Node *p = node_join(left, right); // joins the passed nodes and pushes it
      stack_push(s, p);
    }
  }

  stack_pop(s, &root); // pops the root and returns it
  stack_delete(&s);    // deletes the stack
  return root;
}
// Function deletes the tree with post order transversal
void delete_tree(Node **root) {
  if (*root != NULL) {
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);

    node_delete(root);
  }
}

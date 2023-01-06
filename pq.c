#include "pq.h"
#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


// Struct to create a Priority Queue
struct PriorityQueue {
  uint32_t size;
  uint32_t capacity;
  Node **items;
};
// Function to init a pq
PriorityQueue *pq_create(uint32_t capacity) {
  PriorityQueue *q = (PriorityQueue *)malloc(
      sizeof(PriorityQueue)); // dynamically allocated memeory
  q->size = 0;
  q->capacity = capacity; // sets capacity to the passed in value
  q->items = (Node **)calloc(capacity, sizeof(Node *));
  return q;
}
// Function to delete a pq
void pq_delete(PriorityQueue **q) {
  free((*q)->items);
  (*q)->items = NULL;
  free(*q);
  *q = NULL; // frees the memeory then sets the pointers to null
}
// Function checks if pq is empty
bool pq_empty(PriorityQueue *q) {
  if (q->size == 0) {
    return true;
  }
  return false;
}
// Function checks if pq is full
bool pq_full(PriorityQueue *q) {
  if (q->size == q->capacity) {
    return true;
  }
  return false;
}
// Function returns pq's size
uint32_t pq_size(PriorityQueue *q) { return q->size; }

// Left child function
uint32_t l_child(uint32_t n) { return 2 * n + 1; }

// Right child function
uint32_t r_child(uint32_t n) { return 2 * n + 2; }
// Parent function
uint32_t parent(uint32_t n) { return (n - 1) / 2; }
// Function swaps the passed in values
void swap(uint32_t inone, uint32_t intwo, PriorityQueue *q) {
  Node *tempn = q->items[inone]; // sets a temp node to allow for the swap
  q->items[inone] = q->items[intwo];
  q->items[intwo] = tempn;
}

// Up heap function
void up_heap(PriorityQueue *q) {
  uint32_t n = q->size - 1;
  while (n > 0 && node_cmp(q->items[parent(n)],
                           q->items[n])) // compares the node frequency using
                                         // the function from node.c
  {
    swap(parent(n), n, q);
    n = parent(n);
  }
}

// down heap function
void down_heap(PriorityQueue *q) {
  uint32_t n = 0;
  uint32_t smaller;
  while (l_child(n) < q->size) {
    if (r_child(n) == q->size) {
      smaller = l_child(n);
    } else {
      if (node_cmp(q->items[r_child(n)],
                   q->items[l_child(n)])) // compares the node frequency using
                                          // the function from node.c
      {
        smaller = l_child(n);
      } else {
        smaller = r_child(n);
      }
    }
    if (node_cmp(q->items[smaller],
                 q->items[n])) // compares the node frequency using the function
                               // from node.c
    {
      break;
    }
    swap(n, smaller, q);
    n = smaller;
  }
}
// Function enqueues the passed in node
bool enqueue(PriorityQueue *q, Node *n) {
  if (pq_full(q)) // checks if pq is empty
  {
    return false;
  }
  q->items[pq_size(q)] = n; // adds the node to the pq
  q->size += 1;
  up_heap(q); // calls up_heap to sort it in
  return true;
}
// Function dequeues a node and passes it out
bool dequeue(PriorityQueue *q, Node **n) {
  if (pq_empty(q)) // checks if pq is empty
  {
    return false;
  }
  *n = q->items[0]; // dequeue the node from the pq
  q->size -= 1;
  q->items[0] = q->items[pq_size(q)]; // set the node to the end of the pq
  down_heap(q);                       // calls down heap to sort the changed pq
  return true;
}
// Function to print out the pq calls node_print
void pq_print(PriorityQueue *q) {
  for (uint32_t i = 0; i < pq_size(q); i++) {
    node_print(q->items[i]);
  }
}

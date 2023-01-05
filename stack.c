#include "stack.h"
#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
// Struct for the stack
struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};
// Function to init stacks
Stack *stack_create(uint32_t capacity) {
  Stack *s = (Stack *)malloc(sizeof(Stack)); // allocates memory for the stack
  s->top = 0;
  s->capacity = capacity; // sets the capacity from the passed in var
  s->items = (Node **)calloc(capacity, sizeof(Node *));
  return s;
}
// Function to delete a stack
void stack_delete(Stack **s) {
  free((*s)->items);
  (*s)->items = NULL;
  free(*s);
  *s = NULL; // frees memeory and sets pointers to null
}
// Function checks if stack is empty
bool stack_empty(Stack *s) {
  if (s->top == 0) {
    return true;
  }
  return false;
}
// Function checks if stack is full
bool stack_full(Stack *s) {
  if (s->top == s->capacity) {
    return true;
  }
  return false;
}
// Function returns stack size
uint32_t stack_size(Stack *s) { return s->top; }
// Function pushs a bit off the stack
bool stack_push(Stack *s, Node *n) {
  if (stack_full(s)) // checks if stack is full
  {
    return false;
  }
  s->items[s->top] = n;
  s->top++;
  return true;
}
// Function pops a bit off the stack
bool stack_pop(Stack *s, Node **n) {
  if (stack_empty(s)) // checks if stack is empty
  {
    return false;
  }
  s->top += -1;
  *n = s->items[s->top];
  return true;
}
// function for printing the stack
void stack_print(Stack *s) {
  for (uint32_t i = 0; i < stack_size(s); i++) {
    node_print(s->items[i]);
  }
}

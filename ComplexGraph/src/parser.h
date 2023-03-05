#include <stdio.h>

typedef struct Node *Node;

Node *parser(FILE *file);

complex eval(Node *queue_head, complex variable_value);
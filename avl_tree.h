#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "regex_match.h"

#define MAX_ID_LENGTH 200

#define CMP(s1, s2) (strcmp((s1)->ID, (s2)->ID))
#define CMP_ID(s1, s2) (strcmp((s1), (s2)->ID))

typedef struct stud
{

} stud_t;

typedef struct node
{
    int height;
    struct node *left, *right;

    char ID[MAX_ID_LENGTH];
    int parts_send;
    int minutes_late;
    int *solving_time;
} node_t;


int height(node_t *N);
int max(int a, int b);

node_t *newnode_t(const char *ID);
node_t *right_rotate(node_t *y);
node_t *left_rotate(node_t *x);
int get_balance(node_t *N);
node_t *insert_node(node_t *node, const char *ID);
node_t *minValuenode_t(node_t *node);
void print_node(node_t *node);
void delete_node(node_t *node);
node_t *search_node(node_t *root, char *ID);

#endif
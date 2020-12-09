#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <string.h>

#define MAX_ID_LENGTH 200

#define CMP(s1, s2) (strcmp((s1)->ID, (s2)->ID))

typedef struct stud
{
    char ID[MAX_ID_LENGTH];
    int parts_send;
    int minutes_late;
    int *solving_time;
} stud_t;

typedef struct node
{
    stud_t *stud;
    int bl;
    struct node *left, *right;
} node_t;


void rr_rotation(node_t **p);

void ll_rotation(node_t **p);

void lr_rotation(node_t **p);

void rl_rotation(node_t **p);

void insert(int v, node_t **root, int *h);

void print(node *root);

node_t *search(int value, node_t *root);

void print(node_t *root);

#endif

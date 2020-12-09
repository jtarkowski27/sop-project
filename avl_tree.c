
#include "avl_tree.h"

void rr_rotation(node_t **p)
{
    node_t *x = (*p)->right;
    (*p)->right = x->left;
    x->left = (*p);
    (*p)->bl = (x->bl == -1) ? 0 : -1;
    x->bl = (x->bl == 0) ? 1 : 0;
    (*p) = x;
}

void ll_rotation(node_t **p)
{
    node_t *x = (*p)->left;
    (*p)->left = x->right;
    x->right = (*p);
    (*p)->bl = (x->bl == 1) ? 0 : 1;
    x->bl = (x->bl == 0) ? -1 : 0;
    (*p) = x;
}

void lr_rotation(node_t **p)
{
    rr_rotation(&((*p)->left));
    ll_rotation(p);
}

void rl_rotation(node_t **p)
{
    ll_rotation(&((*p)->right));
    rr_rotation(p);
}

void insert(stud_t *stud, node_t **root, int *h)
{
    node_t *ptr;
    if (!(*root))
    {
        (*root) = (node_t *)malloc(sizeof(node_t));
        (*root)->stud = stud;
        (*root)->left = (*root)->right = NULL;
        (*root)->bl = 0;
        *h = 1;
        return;
    }
    if (CMP(stud, (*root)->stud) == 1)
    {
        insert(stud, &((*root)->left), h);
        if (*h)
        {
            switch ((*root)->bl)
            {
                case 1:
                    ptr = (*root)->left;
                    if (ptr->bl == 1 || ptr->bl == 0)
                        ll_rotation(root);
                    else
                        lr_rotation(root);
                    *h = 0;
                    break;
                case 0:
                    (*root)->bl = 1;
                    break;
                case -1:
                    (*root)->bl = 0;
                    *h = 0;
                    break;
            }
        }
        return;
    }
    if (CMP(stud, (*root)->stud) == -1)
    {
        insert(stud, &((*root)->right), h);
        if (*h) {
            switch ((*root)->bl)
            {
                case 1:
                    (*root)->bl = 0;
                    *h = 0;
                    break;
                case 0:
                    (*root)->bl = 1;
                    break;
                case -1:
                    ptr = (*root)->right;
                    if (ptr->bl == 1 || ptr->bl == 0)
                        rr_rotation(root);
                    else
                        rl_rotation(root);
                    *h = 0;
                    break;
            }
        }
        return;
    }
}

node_t *search(int value, node_t *root)
{
    node_t *p = root;
    while (p && value != p->key)
    {
        if (value < p->key)
            p = p->left;
        else
            p = p->right;
    }
    return p;
}

void print(node_t *root)
{
    if (root)
    {
        print(root->left);
        printf("%s ", root->stud->ID);
        print(root->right);
    }
}
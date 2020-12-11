#include "avl_tree.h"

int height(node_t *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}


node_t *newnode_t(const char *ID)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (!node)
        ERR("malloc");
    strncpy(node->ID, ID, MAX_ARG_LENGTH);
    node->parts_send = 1;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return (node);
}


node_t *right_rotate(node_t *y)
{
    node_t *x = y->left;
    node_t *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}


node_t *left_rotate(node_t *x)
{
    node_t *y = x->right;
    node_t *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}


int get_balance(node_t *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}


node_t *insert_node(node_t *node, const char *ID)
{
    if (node == NULL)
        return (newnode_t(ID));

    if (strcmp(ID, node->ID) == 1)
        node->left = insert_node(node->left, ID);
    else if (strcmp(ID, node->ID) == -1)
        node->right = insert_node(node->right, ID);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = get_balance(node);
    if (balance > 1 && strcmp(ID, node->left->ID) == 1)
        return right_rotate(node);

    if (balance < -1 && strcmp(ID, node->right->ID) == -1)
        return left_rotate(node);

    if (balance > 1 && strcmp(ID, node->left->ID) == -1)
    {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }

    if (balance < -1 && strcmp(ID, node->right->ID) == 1)
    {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }

    return node;
}

node_t *minValuenode_t(node_t *node)
{
    node_t *current = node;

    while (current->left != NULL)
        current = current->left;

    return current;
}

void print_node(node_t *node)
{
    if (node)
    {
        print_node(node->left);
        printf("%s, %d\n", node->ID, node->parts_send);
        print_node(node->right);
    }
}

void delete_node(node_t *node)
{
    if (node)
    {
        print_node(node->left);
        print_node(node->right);
        free(node);
    }
}

node_t *search_node(node_t *root, char *ID)
{
    node_t *p = root;
    while (p && strcmp(ID, p->ID) != 0)
    {
        if (strcmp(ID, p->ID) < 0)
            p = p->left;
        else
            p = p->right;
    }
    return p;
}
#include <iostream>
#include "vertex.h"

void Vertex::add(ListNode * node) volatile
{
    if (head == nullptr)
    {
        head = node;
        return;
    } 

    weight_t w = node->weight();
    if (w < head->weight())
    {
        node->next = head;
        head = node;
        return;
    }

    ListNode *prev_list_node = head;
    ListNode *list_node = head->next;
    while ((list_node != nullptr) && (w > list_node->weight()))
    {
        prev_list_node = list_node;
        list_node = list_node->next;
    }
    prev_list_node->next = node;
    node->next = list_node;

    node = head;
    while (node != nullptr)
        node = node->next;
}

// find the root vetrix
// without path compression
volatile Vertex * Vertex::croot() const volatile
{
    volatile Vertex * root = parent;
    while (root->parent != root)
    {
        root = root->parent;
    }
    return root;
}

// find the root vertix
// and compress path in the tree if posiible
// volatile Vertex * Vertex::root() volatile
// {
//     volatile Vertex * root = croot();
//     if (parent != root)
//     {
//         parent = root;
//     }
//     return root;
// }

void Vertex::print() const volatile
{
    std::cout << "v " << index << ": ";
    if (parent == this)
    {
        std::cout << "root: ";
    }
    else
    {
        std::cout << "node: ";
    }
    ListNode *node = head;
    while (node != nullptr)
    {
        std::cout << "(" << node->start() << ", " << node->end() << ") ";
        node = node->next;
    }
    std::cout << std::endl;
}

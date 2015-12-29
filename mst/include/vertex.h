#ifndef VERTEX_H
#define VERTEX_H
#include <cstdint>

typedef uint32_t index_t;
typedef uint32_t weight_t;

class Edge
{
public:
    index_t s;  // start vertex
    index_t e;  // end vertex
    weight_t w; // weight
    Edge()
        : s(0), e(0), w(0)
        {
        }
    Edge(const index_t start, const index_t & end, const weight_t & weight)
        : s(start), e(end), w(weight)
        {
        }
};

class ListNode
{
public:
    const Edge * edge;
    ListNode * next;   // next node in edge llist
    bool reverse;
    ListNode()
        : edge(nullptr), next(nullptr), reverse(false)
        {
        }

    inline const index_t & start() const { return (reverse)? edge->e : edge->s; }
    inline const index_t & end() const { return (reverse)? edge->s : edge->e; }
    inline const weight_t & weight() const { return edge->w; }
};

class Vertex
{
public:
    ListNode * head;  // head of the edge list
    volatile Vertex * parent;    // root of the vertex tree
    index_t index;
    Vertex()
        : head(nullptr), parent(this), index(0)
        {
        }

    // add a new ListNode with the edge in the sorted linked list
    void add(ListNode * node) volatile;

    // find the root vetrix
    // without path compression
    volatile Vertex * croot() const volatile;

    // find the root vertix
    // and compress path in the tree if posiible
    // volatile Vertex * root() volatile;

    void print() const volatile;
};

#endif /* VERTEX_H */

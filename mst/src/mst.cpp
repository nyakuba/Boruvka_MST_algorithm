#include <iostream>
#include "mst.h"

inline void printComponents(const Vertex * pV, const index_t & NV)
{
    for (index_t i = 0; i < NV; ++i)
    {
        pV[i].print();
    }
}

inline void move_head(ListNode ** src, ListNode ** dst)
{
    // assume, that dst is initialized
    ListNode *tmp = *src;
    *src = (*src)->next;
    (*dst)->next = tmp;
    *dst = (*dst)->next;
}

inline void step(ListNode ** head)
{
    *head = (*head)->next;
}

// merge two components
void merge(Vertex * pV, const index_t & NV, Vertex * root1, Vertex * root2)
{
    // save link to the minimal edge
    ListNode * min_edge = root1->head;

    // merging of the edge list
    ListNode * head1 = root1->head;
    ListNode * head2 = root2->head;

    // delete first edges (component1, component2)
    if (head1 != nullptr)
    {
        Vertex * r = pV[head1->end()].root();
        while (r == root2)
        {
            step(&head1);
            if (head1 == nullptr) break;
            r = pV[head1->end()].root();
        }
    }

    if (head2 != nullptr)
    {
        Vertex * r = pV[head2->end()].root();
        while (r == root1)
        {
            step(&head2);
            if (head2 == nullptr) break;
            r = pV[head2->end()].root();
        }
    }

    ListNode *head = nullptr;
    // now head1 and head2 do not terminate on this two components
    // initialize the head of the resulting list
    if (head1 == nullptr)
    {
        head = head2;
    }
    else if (head2 == nullptr)
    {
        head = head1;
    }
    // head1 and head2 contain elements
    else if (head1->edge->w < head2->edge->w)
    {
        head = head1;
        step(&head1);
    }
    else
    {
        head = head2;
        step(&head2);
    }


    // merge two lists while both of them contain elements
    // do not forget deletion
    ListNode *node = head, *tmp;
    while(head1 != nullptr && head2 != nullptr)
    {
        if (head1->weight() < head2->weight())
        {
            Vertex * r = pV[head1->end()].root();
            if (r != root2)
            {
                move_head(&head1, &node);
            }
            else
            {
                step(&head1);
            }
        }
        else  // head2 is smaller
        {
            Vertex * r = pV[head2->end()].root();
            if (r != root1)
            {
                move_head(&head2, &node);
            }
            else
            {
                step(&head2);
            }
        }
    }

    // remaining elements
    while (head1 != nullptr)
    {
        Vertex * r = pV[head1->end()].root();
        if (r != root2)
        {
            move_head(&head1, &node);
        }
        else
        {
            step(&head1);
        }
    }
    while (head2 != nullptr)
    {
        Vertex * r = pV[head2->end()].root();
        if (r != root1)
        {
            move_head(&head2, &node);
        }
        else
        {
            step(&head2);
        }
    }
    if (node != nullptr)
    {
        node->next = nullptr;
    } 

    // set merged list of edges
    root1->head = head;
    // set min edge
    root2->head = min_edge;  
    min_edge->next = nullptr;
    // set v1 parent of v2
    root2->parent = root1;
}

void MST(
    // in
    const index_t & NV,
    const index_t & NE,
    const Edge    * pE,
    // out
    const Edge   ** pMST
    )
{
    // initialization
    // create NV components
    Vertex *pV = new Vertex[NV];
    for (int32_t i = 0; i < NV; ++i)
    {
        pV[i].index = i;
    }
    ListNode *pN = new ListNode[NE * 2];
    for (int i = 0, j = 0; i < NE; ++i, j+=2)
    {
        pN[j].edge = pE + i; 
        pN[j + 1].edge = pE + i;
        pN[j + 1].reverse = true;
        // add edge to sorted list
        pV[pE[i].s].add(&pN[j]);
        pV[pE[i].e].add(&pN[j+1]);
    }
    
    for (index_t i = 0; i < NV; ++i)
    {
        std::cout << "Iteration number " << i << std::endl;
        printComponents(pV, NV);
        Vertex * root1 = pV[i].root(); // first component
        if (root1->head != nullptr)
        {
            Vertex * end_vertex = pV + root1->head->end();
            Vertex * root2 = end_vertex->root();
            merge(pV, NV, root1, root2);
        }
    }

    index_t i = 0, j = 0;
    while (i < NV)
    {
        if (pV[i].head != nullptr)
        {
            pMST[j] = pV[i].head->edge;
            ++j;
        }    
        ++i;
    }
    
    delete [] pV;
    delete [] pN;
}

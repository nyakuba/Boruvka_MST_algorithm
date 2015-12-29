#include <iostream>
#include <tbb/mutex.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include "mst.h"

// #define DEBUG

#if defined(DEBUG)
tbb::mutex io_mutex;

#define PRINT(msg) \
    io_mutex.lock(); \
    std::cout << msg << std::endl; \
    io_mutex.unlock();

inline void printComponents(const Vertex * pV, const index_t & NV)
{
    io_mutex.lock();
    for (index_t i = 0; i < NV; ++i)
    {
        pV[i].print();
    }
    io_mutex.unlock();
}
#else
#define PRINT(msg)
inline void printComponents(const Vertex * pV, const index_t & NV) { }
#endif


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
        Vertex * r = pV[head1->end()].croot();
        while (r == root2)
        {
            step(&head1);
            if (head1 == nullptr) break;
            r = pV[head1->end()].croot();
        }
    }

    if (head2 != nullptr)
    {
        Vertex * r = pV[head2->end()].croot();
        while (r == root1)
        {
            step(&head2);
            if (head2 == nullptr) break;
            r = pV[head2->end()].croot();
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
            Vertex * r = pV[head1->end()].croot();
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
            Vertex * r = pV[head2->end()].croot();
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
        Vertex * r = pV[head1->end()].croot();
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
        Vertex * r = pV[head2->end()].croot();
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

class ApplyMerge
{
public:
    ApplyMerge()
        { }
    ApplyMerge(const ApplyMerge & a)
        { }
    ~ApplyMerge()
        { }
    void set_context(const index_t &nvertices,
                     const index_t &nedges,
                     Vertex * pVertices,
                     tbb::mutex * pEdgeMutex,
                     tbb::mutex * pVertexMutex)
        {
            NV = nvertices;
            NE = nedges;
            pV = pVertices;
            pEMutex = pEdgeMutex;
            pVMutex = pVertexMutex;
        }
    void operator()(const tbb::blocked_range<index_t> & r) const
        {
            PRINT("Started interval from " << r.begin() << " to " << r.end());
            printComponents(pV, NV);
            bool completed = false;
            while (!completed)
            {
                completed = true;  // assume mst was built
                bool merge_flag = false;  // are we prepared for merge?
                Vertex *v1, *v2, *root1, *root2;
                index_t rindex1, rindex2;
                pEMutex->lock();
                // find and lock two component's roots for merge
                for (index_t vertex_index = 0; vertex_index < NV; ++vertex_index)
                {
                    v1 = pV + vertex_index;
                    root1 = v1->croot();
                    rindex1 = root1 - pV;
                    // try lock. if successfull, return true
                    if (!pVMutex[rindex1].try_lock())
                    {
                        completed = false;
                    }
                    else if (root1->head != nullptr)
                    {
                        completed = false;
                        v2 = pV + root1->head->end();
                        root2 = v2->croot();
                        rindex2 = root2 - pV;
                        if (!pVMutex[rindex2].try_lock())
                        {
                            pVMutex[rindex1].unlock();
                        }
                        else
                        {
                            merge_flag = true;
                            break;
                        }  // if r2 is locked
                    }
                    else
                    {
                        pVMutex[rindex1].unlock();
                    }
                }  // for each vertex in pV
                pEMutex->unlock();
                if (merge_flag)
                {
                    merge(pV, NV, root1, root2);
                    pVMutex[rindex1].unlock();
                    pVMutex[rindex2].unlock();
                }
            }  // while !completed
        } 
private:
    static index_t NV;
    static index_t NE;
    static Vertex * pV;
    static tbb::mutex * pVMutex;
    static tbb::mutex * pEMutex;
};

index_t ApplyMerge::NV = 0;
index_t ApplyMerge::NE = 0;
Vertex * ApplyMerge::pV = nullptr;
tbb::mutex * ApplyMerge::pVMutex = nullptr;
tbb::mutex * ApplyMerge::pEMutex = nullptr;

void MST(
    // in
    const uint32_t & NT,
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
    // create a mutex for aquiring edge
    tbb::mutex aquireEdgeMutex;
    // create an array of mutexes for each vertex
    tbb::mutex * pVertexMutex = new tbb::mutex[NV];
    // create NE * 2 list nodes for edges
    // since our graph is unoriented
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
    
    tbb::task_scheduler_init init(NT);
    ApplyMerge fun;
    fun.set_context(NV, NE, pV, &aquireEdgeMutex, pVertexMutex);
    // number of iterations does not exceede NV-1, since
    // we merge NV components
    // for (index_t i = 0; i < NV; ++i)
    // {
    tbb::parallel_for(tbb::blocked_range<index_t>(0, NT, 1), fun);
    // }

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

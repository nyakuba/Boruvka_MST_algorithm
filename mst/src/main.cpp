#include <iostream>


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

    const index_t & start() const { return (reverse)? edge->e : edge->s; }
    const index_t & end() const { return (reverse)? edge->s : edge->e; }
    const weight_t & weight() const { return edge->w; }
};

class Vertex
{
public:
    ListNode * head;  // head of the edge list
    Vertex * parent;    // root of the vertex tree
    int32_t index;
    Vertex()
        : head(nullptr), parent(this), index(0)
        {
        }

    void add(ListNode * node)
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

    Vertex * root()
        {
            Vertex * root = this;
            while (root->parent != root)
            {
                root = root->parent;
            }
            if (parent != root)
            {
                parent = root;
            }
            return root;
        }

    void print() const
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

};

void printComponents(const Vertex * pV, const index_t & NV)
{
    for (index_t i = 0; i < NV; ++i)
    {
        pV[i].print();
    }
}

void merge(Vertex * pV, const index_t & NV)
{
    Vertex * root1 = pV[0].root(); // first component
    ListNode * min_edge = root1->head;
    Vertex * end_vertex = pV + min_edge->end();
    Vertex * root2 = end_vertex->root(); // another component
    ListNode * head1 = root1->head;
    ListNode * head2 = root2->head;

    // delete first nodes, that come from component1 to component2
    if (head1 != nullptr)
    {
        Vertex * r = pV[head1->end()].root();
        while (r == root2)
        {
            head1 = head1->next;
            if (head1 == nullptr) break;
            r = pV[head1->end()].root();
        }
    }

    if (head2 != nullptr)
    {
        Vertex * r = pV[head2->end()].root();
        while (r == root1)
        {
            head2 = head2->next;
            if (head2 == nullptr) break;
            r = pV[head2->end()].root();
        }
    }

    // now head1 and head2 do not terminate on this two components
    // initialize the head of the resulting list
    ListNode *head;
    if (head1 == nullptr && head2 == nullptr)
    {
        root1->head = nullptr;
        // set min edge
        end_vertex->head = min_edge;  
        min_edge->next = nullptr;
        // set v1 parent of v2
        root2->parent = root1;
        return;
    }
    if (head1 == nullptr)
    {
        head = head2;
    }
    else if (head2 == nullptr)
    {
        head = head1;
    }
    else if (head1->edge->w < head2->edge->w)
    {
        head = head1;
        head1 = head1->next;
    }
    else
    {
        head = head2;
        head2 = head->next;
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
                tmp = head1;
                head1 = head1->next;
                node->next = tmp;
                node = node->next;
            }
            else
            {
                head1 = head1->next;
            }
        }
        else  // head2 is smaller
        {
            Vertex * r = pV[head2->end()].root();
            if (r != root1)
            {
                tmp = head2;
                head2 = head2->next;
                node->next = tmp;
                node = node->next;
            }
            else
            {
                head2 = head2->next;
            }
        }
    }

    // remaining elements
    while (head1 != nullptr)
    {
        Vertex * r = pV[head1->end()].root();
        if (r != root2)
        {
            tmp = head1;
            head1 = head1->next;
            node->next = tmp;
            node = node->next;
        }
        else
        {
            head1 = head1->next;
        }
    }
    while (head2 != nullptr)
    {
        Vertex * r = pV[head2->end()].root();
        if (r != root1)
        {
            tmp = head2;
            head2 = head2->next;
            node->next = tmp;
            node = node->next;
        }
        else
        {
            head2 = head2->next;
        }
    }
    if (node != nullptr)
    {
        node->next = nullptr;
    } 

    // set merged list of edges
    root1->head = head;
    // set min edge
    end_vertex->head = min_edge;  
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
    
    Vertex *v1, *v2;
    for (index_t i = 0; i < NV-1; ++i)
    {
        std::cout << "Iteration number " << i << std::endl;
        printComponents(pV, NV);
        merge(pV, NV);
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

int main(int argc, char *argv[])
{
    // std::ios_base::sync_with_stdio(false);
    uint32_t NV, NE;
    std::cin >> NV >> NE;

    Edge *pEdges = new Edge[NE];
    const Edge **pMinSpanTree = new const Edge*[NV-1];

    uint32_t v1, v2;
    weight_t w;
    std::cout << "Reading from stdin..." << std::endl;
    for (uint32_t i = 0; i < NE; ++i)
    {
        std::cin >> v1 >> v2 >> w;
        if (!std::cin)
        {
            std::cerr << "Error reading stdin." << std::endl;
            return -1;
        }
        pEdges[i] = Edge(v1, v2, w);
    }

    MST(NV, NE, pEdges, pMinSpanTree);

    std::cout << "MST:" << std::endl;
    for (uint32_t i = 0; i < NV-1; ++i)
    {
        std::cout << pMinSpanTree[i]->s << ' ' << pMinSpanTree[i]->e << ' ' << pMinSpanTree[i]->w << std::endl;
    }

    delete [] pEdges;
    delete [] pMinSpanTree;

    return 0;
}

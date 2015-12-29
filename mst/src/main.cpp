#include <iostream>
#include <cstring>
#include <chrono>
#include "mst.h"

//#define PRINT_MST

void printUsage();

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc != 2)
    {
        printUsage();
        return -1;
    }

    const uint32_t NTHREADS = atoi(argv[1]);

    index_t NV, NE;
    std::cin >> NV >> NE;

    Edge *pEdges = new Edge[NE];
    const Edge **pMinSpanTree = new const Edge*[NV-1];
    memset(pMinSpanTree, 0, sizeof(Edge*)*(NV-1));

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

    std::cout << "computing mst" << std::endl;
    std::chrono::high_resolution_clock::time_point start, end;
    start = std::chrono::high_resolution_clock::now();
    MST(NTHREADS, NV, NE, pEdges, pMinSpanTree);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: "
              << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
              << std::endl;

    uint64_t tree_weight = 0;
#ifdef PRINT_MST
    std::cout << "MST:" << std::endl;
#endif
    for (uint32_t i = 0; i < NV-1; ++i)
    {
        if (pMinSpanTree[i] != nullptr)
        {
#ifdef PRINT_MST
            std::cout << pMinSpanTree[i]->s << ' ' << pMinSpanTree[i]->e << ' ' << pMinSpanTree[i]->w << std::endl;
#endif
            tree_weight += pMinSpanTree[i]->w;
        }
    }
    std::cout << "Total weight = " << tree_weight << std::endl;

    delete [] pEdges;
    delete [] pMinSpanTree;

    return 0;
}

void printUsage()
{
    std::cout << "Usage: mst <nthreads>" << std::endl;
}

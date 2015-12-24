#include <iostream>
#include <cstring>
#include "mst.h"

int main(int argc, char *argv[])
{
    // std::ios_base::sync_with_stdio(false);
    uint32_t NV, NE;
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

    MST(NV, NE, pEdges, pMinSpanTree);

    std::cout << "MST:" << std::endl;
    for (uint32_t i = 0; i < NV-1; ++i)
    {
        if (pMinSpanTree[i] != nullptr)
        {
            std::cout << pMinSpanTree[i]->s << ' ' << pMinSpanTree[i]->e << ' ' << pMinSpanTree[i]->w << std::endl;
        }
    }

    delete [] pEdges;
    delete [] pMinSpanTree;

    return 0;
}

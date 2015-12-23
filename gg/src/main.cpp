#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <set>
#include <utility>

typedef std::pair<uint32_t, uint32_t> Edge;

void printUsage();

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    if (argc != 4)
    {
        printUsage();
        return -1;
    }

    const uint32_t NV = atoi(argv[1]);
    const uint32_t NE = atoi(argv[2]);
    const uint32_t MAX_WEIGHT = atoi(argv[3]);

    if (NE > (NV*(NV - 1) / 2))
    {
        std::cerr << "The edge number is too big ( is greater than n(n-1)/2 )." << std::endl;
        return -2;
    }

    std::cout << NV << ' ' << NE << std::endl;

    // std::random_device rd;
    // std::mt19937_64 gen(rd());
    std::mt19937_64 gen;
    std::uniform_int_distribution<uint64_t> vdis(0, NV-1);
    std::uniform_int_distribution<uint64_t> wdis(0, MAX_WEIGHT);
    std::set<Edge, bool(*)(const Edge &, const Edge &)> edges(
        [](const Edge & a, const Edge & b) {
            if (a.first != b.first)
                return a.first < b.first;
            else
                return a.second < b.second;
        }
        );
    int edges_count = 0;
    while (edges_count < NE)
    {
        uint32_t v1 = vdis(gen);
        uint32_t v2 = vdis(gen);
        while (v2 == v1) v2 = vdis(gen);
        if (v1 < v2) std::swap(v1, v2);
        Edge edge(v1, v2);
        if (edges.find(edge) == edges.end())
        {
            edges.insert(edge);
            std::cout << v1 << ' ' << v2 << ' ' << wdis(gen) << std::endl;
            ++edges_count;
        }
    }
    
    return 0;
}

void printUsage()
{
    std::cout << "(G)raph(G)en is a program to generate random unoriented weighted graph." << std::endl
              << "    - Resulting graph will be printed to stdout." << std::endl
              << "    - Graph is defined as the list of edges." << std::endl
              << "    - Graph can have several components." << std::endl
              << "    - Weigths are represented as 32-bit unsigned integers." << std::endl
              << "Usage: gg <num_of_vertices> <num_of_edges> <max_weight>" << std::endl;
}

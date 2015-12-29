#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <set>
#include <utility>

class Edge
{
public:
    uint32_t start;
    uint32_t end;
    uint32_t weight;
    Edge()
        : start(0), end(0), weight(0)
        {
        }
    Edge(const uint32_t & s, const uint32_t & e, const uint32_t & w)
        : start(s), end(e), weight(w)
        {
        }
    Edge(const Edge & e)
        : start(e.start), end(e.end), weight(e.weight)
        {
        }
    ~Edge() { }

    Edge & operator=(const Edge & e)
        {
            start = e.start;
            end = e.end;
            weight = e.weight;
        }
};

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

    if (MAX_WEIGHT < NE)
    {
        std::cerr << "Maximum weinght is less than number of edges. Cannot make graph with different edge weights." << std::endl;
        return -3;
    }

    std::cout << NV << ' ' << NE << std::endl;

    // std::random_device rd;
    // std::mt19937_64 gen(rd());
    std::mt19937_64 gen;
    std::uniform_int_distribution<uint64_t> vdis(0, NV-1);
    std::uniform_int_distribution<uint64_t> wdis(0, MAX_WEIGHT);
    Edge *edges = new Edge[NE];

    for (uint32_t edge_count = 0; edge_count < NE; ++edge_count)
    {
        uint32_t v1 = vdis(gen);
        uint32_t v2 = vdis(gen);
        if (v1 == v2) v1 = (v1 + 1) % NV;
        uint32_t w = wdis(gen);
        bool found = true;
        uint32_t probe = 0;
        while (found)
        {
            found = false;
            for (int i = 0; i < edge_count; ++i)
            {
                if ((edges[i].start == v1 && edges[i].end == v2) ||
                    (edges[i].start == v2 && edges[i].end == v1))
                {
                    if (probe < NV)
                    {
                        if (v1 + 1 == v2)  
                        {
                            v1 = (v1 + 2) % NV;
                        }
                        else
                        {
                            v1 = (v1 + 1) % NV;
                        }
                    }
                    else
                    {
                        if (v2 + 1 == v1)
                        {
                            v2 = (v2 + 2) % NV;
                        }
                        else
                        {
                            v2 = (v2 + 1) % NV;
                        }
                    }
                    ++probe;
                    found = true;
                    break;
                }
                if (edges[i].weight == w)
                {
                    w = (w + 1) % MAX_WEIGHT;
                    found = true;
                    break;
                }
            }
        }
        
        if (v1 < v2) std::swap(v1, v2);
        edges[edge_count] = Edge(v1, v2, w);
    }

    for (int i = 0; i < NE; ++i)
    {
        std::cout << edges[i].start << ' ' << edges[i].end << ' ' << edges[i].weight << std::endl;
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

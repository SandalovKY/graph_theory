#include "graph.hpp"
#include "algorithm.hpp"
#include "graph_file_parser.hpp"
#include "bitset.h"
#include <iostream>
#include <map>
#include <utility>
#include <tuple>
#include <chrono>
#include <fstream>

const int MAX = 500;
 
// Stores the vertices
int store[MAX], n;
 
// Graph
int graph[MAX][MAX];
 
// Degree of the vertices
int d[MAX];

bool is_clique(int b)
{
 
    // Run a loop for all set of edges
    for (int i = 1; i < b; i++) {
        for (int j = i + 1; j < b; j++)
 
            // If any edge is missing
            if (graph[store[i]][store[j]] == 0)
                return false;
    }
    return true;
}

int maxCliques(int i, int l)
{
    // Maximal clique size
    int max_ = 0;
 
    // Check if any vertices from i+1
    // can be inserted
    for (int j = i + 1; j <= n; j++) {
 
        // Add the vertex to store
        store[l] = j;
 
        // If the graph is not a clique of size k then
        // it cannot be a clique by adding another edge
        if (is_clique(l + 1)) {
 
            // Update max
            max_ = std::max(max_, l);
 
            // Check if another edge can be added
            max_ = std::max(max_, maxCliques(j, l + 1));
        }
    }
    return max_;
}
int main(int argc, char ** argv) {
    if(argc != 2)
    {
        std::cerr <<
            "Usage: program <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }

    my::ColoredGraph resGraph;
    my::ColoredGraph otherGraph;
    using bitset_type = myDynamicBitset<>;
    using adjMatr_type = my::BitAdjacencyMatrix<bitset_type>; 
    adjMatr_type resAdjMatr;
    adjMatr_type otherAdjMatr;
    adjMatr_type myTestMatr;
    try
    {
        std::cout << "Start to input col graph files\n";
        auto const jv = my_parser::ParseFile(argv[1]);
        resGraph = my_parser::ReadJsonGraphToAdjList(jv);
        resAdjMatr = my_parser::ReadJsonGraphToAdjMatr<bitset_type>(jv);
        otherAdjMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>("../dsjc500.1.col");
        otherGraph = my_parser::ReadDimacsGraphToAdjList("../dsjc500.1.col");
        // boost::print_graph(otherGraph);
        myTestMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>("../matr.col");
        std::cout << "Finished files reading\n";

        Algorithm alg(otherGraph);
        auto start1 = std::chrono::high_resolution_clock::now();
        alg.coloring();
        auto end1 = std::chrono::high_resolution_clock::now();
        double time1 = std::chrono::duration_cast<std::chrono::seconds>(end1 - start1).count();

        std::cout << "Ant algorithm time: " << time1 << std::endl;

        start1 = std::chrono::high_resolution_clock::now();
        auto colRes = Algorithm<adjMatr_type>::coloring(otherAdjMatr);
        end1 = std::chrono::high_resolution_clock::now();

        time1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();

        std::cout << "Bit Greedy time: " << time1 << std::endl;

        std::cout << "Greedy result: " << colRes.size() << std::endl;

        std::cout << "NotColoredVert:\n";

        std::pair<my::ColoredGraph::vertex_iterator, my::ColoredGraph::vertex_iterator> adjLPair;
        size_t nonColVert{};
        for (adjLPair = boost::vertices(otherGraph); adjLPair.first != adjLPair.second; ++adjLPair.first)
        {
            if (otherGraph[*(adjLPair.first)].GroupId == -1)
            {
                ++nonColVert;
                // std::cout << *(adjLPair.first) << std::endl;
            }
        }
        std::cout << "Non Col Vet: " << nonColVert << std::endl;
    
        auto start = std::chrono::high_resolution_clock::now();
        const auto cliques = Algorithm<adjMatr_type>::maxCliqueFinding(otherAdjMatr);
        auto end = std::chrono::high_resolution_clock::now();
        size_t maxClique{ 0 };
        std::ofstream myFile;
        myFile.open("Results.txt");
        for (const auto& el : cliques)
        {
            if (el.size() > maxClique) maxClique = el.size();
            for (const auto& vert : el)
            {
                myFile << vert + 1 << '\t';
            }
            myFile << '\n';
        }
        myFile << "Num cliques: " << cliques.size() << '\n';
        std::cout << "Max clique size: " << maxClique << '\n';
        double time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        std::cout << "Measured time: " << time << " in microseconds\n";
        myFile.close();
    }
    catch(std::exception const& e)
    {
        std::cerr <<
            "Caught exception: "
            << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "AdjBitMatr:\n";
    auto size = resAdjMatr.getMatrDimSize();
    std::cout << size << '\n';
    for (int i = 0; i < size; ++i)
    {
        std::cout << resAdjMatr[i] << '\n';
    }
    
    return EXIT_SUCCESS;
}
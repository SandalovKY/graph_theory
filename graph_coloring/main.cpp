#include "graph.hpp"
#include "algorithm.hpp"
#include "parse_input_json.hpp"
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
    using bitset_type = myDynamicBitset<>;
    using adjMatr_type = my::BitAdjacencyMatrix<bitset_type>; 
    adjMatr_type resAdjMatr;
    adjMatr_type otherAdjMatr;
    adjMatr_type myTestMatr;
    try
    {
        auto const jv = my_parser::ParseFile(argv[1]);
        resGraph = my_parser::ReadColoredGraphToAdjList(jv);
        resAdjMatr = my_parser::ReadColoredGraphToAdjMatr<bitset_type>(jv);
        std::cout << "Start to input col graph file\n";
        otherAdjMatr = my_parser::ReadGraphToAdjMatr<bitset_type>("../dsjc500.1.col");
        // std::vector<std::vector<size_t>> edgesVector = my_parser::ReadGraphToVector("../dsjc500.1.col");
        myTestMatr = my_parser::ReadGraphToAdjMatr<bitset_type>("../matr.col");
        std::cout << "Finished files reading\n";
        // n = 200;
        // for (int i = 0; i < edgesVector.size(); i++) {
        //     graph[edgesVector[i][0]][edgesVector[i][1]] = 1;
        //     graph[edgesVector[i][1]][edgesVector[i][0]] = 1;
        //     d[edgesVector[i][0]]++;
        //     d[edgesVector[i][1]]++;
        // }
    
        std::cout << "Max cliques: " << maxCliques(0, 1) << '\n';
        //const auto resColors = Algorithm<adjMatr_type>::coloring(otherAdjMatr);
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
        std::cout << "Measured time: " << time << "in microseconds\n";
        myFile.close();
        // std::cout << "Res colors: " << resColors.size() << '\n';
        // for (const auto& el : resColors)
        // {
        //     for (const auto& vert : el)
        //     {
        //         std::cout << vert << '\t';
        //     }
        //     std::cout << '\n';
        // }
    }
    catch(std::exception const& e)
    {
        std::cerr <<
            "Caught exception: "
            << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    using name_map_t = boost::property_map<my::ColoredGraph, boost::vertex_name_t>::type;
    using color_map_t = boost::property_map<my::ColoredGraph, boost::vertex_color_t>::type;
    using colored_graph_descr_t = boost::graph_traits<my::ColoredGraph>::vertex_descriptor;

    using node_pair = std::pair<boost::graph_traits<my::ColoredGraph>::vertex_descriptor, size_t>;

    using my_map_t = std::map<boost::graph_traits<my::ColoredGraph>::vertex_descriptor, size_t>;

    name_map_t mapForNames{};
    color_map_t mapForColors{};

    boost::graph_traits<my::ColoredGraph>::vertex_iterator vi, vi_end;
    boost::graph_traits<my::ColoredGraph>::in_edge_iterator iei, iei_end;
    colored_graph_descr_t descr;
    std::tie(vi, vi_end) = boost::vertices(resGraph);

    size_t color{ 0 };
    my_map_t mapForNodes{};

    for (;vi != vi_end; ++vi)
    {
        descr = *vi;
        mapForNodes.emplace(std::make_pair(descr, 0));
        boost::put(mapForNames, descr, "Vert");
        boost::put(mapForColors, descr, color++);
    }
    std::tie(vi, vi_end) = boost::vertices(resGraph);
    for (;vi != vi_end; ++vi)
    {
        std::cout << boost::get(mapForNames, *vi) << '\t';
        std::cout << boost::get(mapForColors, *vi) << '\n';
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
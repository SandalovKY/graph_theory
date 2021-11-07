#include "graph.hpp"
#include "algorithm.hpp"
#include "parse_input_json.hpp"
#include "bitset.h"
#include <iostream>
#include <map>
#include <utility>
#include <tuple>

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
    try
    {
        auto const jv = my_parser::ParseFile(argv[1]);
        resGraph = my_parser::ReadColoredGraphToAdjList(jv);
        resAdjMatr = my_parser::ReadColoredGraphToAdjMatr<bitset_type>(jv);
        const auto resColors = Algorithm<adjMatr_type>::coloring(resAdjMatr);
        for (const auto& el : resColors)
        {
            for (const auto& vert : el)
            {
                std::cout << vert << '\t';
            }
            std::cout << '\n';
        }
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
        boost::put(mapForNames, descr, "FirstName");
        boost::put(mapForColors, descr, color++);
    }
    std::tie(vi, vi_end) = boost::vertices(resGraph);
    // for (;vi != vi_end; ++vi)
    // {
    //     std::cout << boost::get(mapForNames, *vi) << '\n';
    //     std::cout << boost::get(mapForColors, *vi) << '\n';
    // }
    std::cout << "AdjBitMatr:\n";
    auto size = resAdjMatr.getMatrDimSize();
    std::cout << size << '\n';
    for (int i = 0; i < size; ++i)
    {
        std::cout << resAdjMatr[i] << '\n';
    }
    
    return EXIT_SUCCESS;
}
#include "algorithm.hpp"
#include "parse_input_json.hpp"
#include <iostream>
#include <tuple>
#include "graph.hpp"

int main(int argc, char ** argv) {
    if(argc != 2)
    {
        std::cerr <<
            "Usage: program <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }

    my::ColoredGraph resGraph;
    try
    {
        auto const jv = my_parser::ParseFile( argv[1] );
        resGraph = my_parser::ReadColoredGraph(jv);
    }
    catch(std::exception const& e)
    {
        std::cerr <<
            "Caught exception: "
            << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    boost::graph_traits<my::ColoredGraph>::vertex_iterator vi, vi_end;
    boost::graph_traits<my::ColoredGraph>::in_edge_iterator iei, iei_end;
    boost::graph_traits<my::ColoredGraph>::vertex_descriptor descr;
    std::tie(vi, vi_end) = boost::vertices(resGraph);
    for (;vi != vi_end; ++vi)
    {
        descr = *vi;
        std::tie(iei, iei_end) = boost::in_edges(descr, resGraph);
        for(;iei != iei_end; ++iei)
        {
            std::cout << *iei << '\t';
        }
        std::cout << '\n';
    }
    
    return EXIT_SUCCESS;
}
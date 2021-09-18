#include "algorithm.hpp"
#include "parse_input_json.hpp"
#include <iostream>

int main(int argc, char ** argv) {
    if(argc != 2)
    {
        std::cerr <<
            "Usage: program <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        auto const jv = my_parser::ParseFile( argv[1] );
        auto resGraph = my_parser::ReadColoredGraph(jv);
    }
    catch(std::exception const& e)
    {
        std::cerr <<
            "Caught exception: "
            << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
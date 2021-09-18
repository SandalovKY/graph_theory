#include "algorithm.hpp"
#include "parse_input_json.hpp"
#include <iostream>

int main(int argc, char ** argv) {
    if(argc != 2)
    {
        std::cerr <<
            "Usage: pretty <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        // Parse the file as JSON
        auto const jv = my_parser::parse_file( argv[1] );

        // Now pretty-print the value
        my_parser::pretty_print(std::cout, jv);
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
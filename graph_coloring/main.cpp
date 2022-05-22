#include "graph.hpp"
#include "algorithm.hpp"
#include "segundo_max_clique.hpp"
#include "graph_file_parser.hpp"
#include <iostream>
#include <map>
#include <utility>
#include <tuple>
#include <chrono>
#include <fstream>

int main(int argc, char ** argv) {
    if(argc != 2)
    {
        std::cerr <<
            "Usage: program <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }

    using bitset_type = myDynamicBitset<>;
    using adjMatr_type = my::BitAdjacencyMatrix<bitset_type>; 
    adjMatr_type adjMatr;
    // adjMatr_type myTestMatr;
    try
    {
        std::cout << "Start to input col graph files\n";
        adjMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>(argv[1]);
        // myTestMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>("../files/matr.col");
        std::cout << "Finished files reading\n";

        size_t adjMatrDimSize = adjMatr.getMatrDimSize();
        std::map<size_t, bitset_type> hmodAdjMatr{};
        

        for (int ind = 0; ind < adjMatrDimSize; ++ind)
        {
            myDynamicBitset<> tmp{};
            tmp = adjMatr.getLine(ind);
            hmodAdjMatr.insert({ ind, tmp });
            tmp.set(ind);
            ~tmp;
            tmp.setId(ind);
        }

        SegundoAlgorithm segAlg{};
        auto start1 = std::chrono::high_resolution_clock::now();
        segAlg.runMaxCliqueFinding(hmodAdjMatr);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto& res = segAlg.maxClique;

        auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg time: " << time1 << std::endl;
        std::cout << "Segundo alg res: " << res.size() << std::endl;
        for (const auto& vert: res)
        {
            std::cout << vert << ' ';
        }
        std::cout << std::endl;

        start1 = std::chrono::high_resolution_clock::now();
        segAlg.runMaxCliqueFinding(hmodAdjMatr, true);
        end1 = std::chrono::high_resolution_clock::now();
        res = segAlg.maxClique;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg mod time: " << time1 << std::endl;
        std::cout << "Segundo alg mod res: " << res.size() << std::endl;
        for (const auto& vert: res)
        {
            std::cout << vert << ' ';
        }
        std::cout << std::endl;
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
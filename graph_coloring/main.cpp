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
    adjMatr_type resAdjMatr;
    adjMatr_type otherAdjMatr;
    adjMatr_type myTestMatr;
    try
    {
        std::cout << "Start to input col graph files\n";
        auto const jv = my_parser::ParseFile(argv[1]);
        resAdjMatr = my_parser::ReadJsonGraphToAdjMatr<bitset_type>(jv);
        otherAdjMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>("../matr.col");
        myTestMatr = my_parser::ReadDimacsGraphToAdjMatr<bitset_type>("../matr.col");
        std::cout << "Finished files reading\n";

        size_t adjMatrDimSize = otherAdjMatr.getMatrDimSize();
        std::vector<std::pair<bitset_type, bool>> modAdjMatr{};
        std::map<size_t, std::pair<bitset_type, bool>> hashedLines{};
        std::map<size_t, bitset_type> hmodAdjMatr{};
        

        for (int ind = 0; ind < adjMatrDimSize; ++ind)
        {
            // std::cout << otherAdjMatr.getLine(ind) << std::endl;
            myDynamicBitset tmp{};
            tmp = otherAdjMatr.getLine(ind);
            hmodAdjMatr.insert({ ind, tmp });
            tmp.set(ind);
            ~tmp;
            tmp.setId(ind);
            hashedLines.insert({ ind, { std::move(tmp), false } });
        }

        auto start1 = std::chrono::high_resolution_clock::now();
        auto colRes = Algorithm<adjMatr_type>::coloring(otherAdjMatr);
        auto end1 = std::chrono::high_resolution_clock::now();

        double time1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();

        std::cout << "Bit Greedy time: " << time1 << std::endl;

        std::cout << "Greedy result: " << colRes.size() << std::endl;

        // for (const auto& colGr: colRes)
        // {
        //     for (const auto& vert: colGr)
        //     {
        //         std::cout << vert << ' ';
        //     }
        //     std::cout << '\n';
        // }

        start1 = std::chrono::high_resolution_clock::now();
        auto colResMod = Algorithm<adjMatr_type>::coloring_mod(hashedLines);
        end1 = std::chrono::high_resolution_clock::now();

        time1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();

        std::cout << "Bit Greedy time for mod: " << time1 << std::endl;

        std::cout << "Greedy result for mod: " << colResMod.size() << std::endl;

        // for (const auto& colGr: colResMod)
        // {
        //     for (const auto& vert: colGr)
        //     {
        //         std::cout << vert.getId() << ' ';
        //     }
        //     std::cout << '\n';
        // }

        std::map<size_t, myDynamicBitset<>> inputMap{};
        std::set<size_t> inputSet{ 9999 };



        SegundoAlgorithm segAlg{};
        segAlg.runMaxCliqueFinding(hmodAdjMatr);
        auto& res = segAlg.maxClique;
    
        std::cout << "Segundo alg res: " << res.size() << std::endl;

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
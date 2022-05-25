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

std::set<size_t> getSetBits(SegundoAlgorithm::bitset_type bitset)
{
    std::set<size_t> retSet{};
    size_t dimSize = bitset.getDimSize();
    size_t firstSetBit = bitset.getFirstNonZeroPosition();
    while (firstSetBit < dimSize)
    {
        retSet.insert(firstSetBit);
        bitset.unset(firstSetBit);
        firstSetBit = bitset.getFirstNonZeroPosition();
    }
    return retSet;
}

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
    using adjMatrMap_type = std::map<size_t, myDynamicBitset<>>;
    adjMatr_type adjMatr;
    adjMatr_type reordered;

    adjMatrMap_type usualOrderMap{};
    adjMatrMap_type reorderedMap{};
    // adjMatr_type myTestMatr;
    try
    {
        std::cout << "Start to input col graph files\n";
        my_parser::myParser graphParser(argv[1]);

        auto reordering = graphParser.getMaxCliqueReordering();

        adjMatr = graphParser.adjList2adjMatr<bitset_type>(&reordering);

        usualOrderMap = graphParser.adjList2adjMatrMap();
        reorderedMap = graphParser.adjList2adjMatrMap(&reordering);

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



        SegundoAlgorithm segAlg(usualOrderMap);
        SegundoAlgorithm segAlgReordered(reorderedMap);
        auto start1 = std::chrono::high_resolution_clock::now();
        // segAlg.runMaxCliqueFinding(hmodAdjMatr);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto& res = segAlg.maxClique;

        auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg time: " << time1 << std::endl;
        std::cout << "Results: " << res.size() << std::endl;
        for (const auto& vert: res)
        {
            std::cout << vert << ' ';
        }
        std::cout << "\n-----------------\n";

        start1 = std::chrono::high_resolution_clock::now();
        // segAlg.runMaxCliqueFinding(hmodAdjMatr, true);
        end1 = std::chrono::high_resolution_clock::now();
        res = segAlg.maxClique;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg mod time: " << time1 << std::endl;
        std::cout << "Results: " << res.size() << std::endl;
        for (const auto& vert: res)
        {
            std::cout << vert << ' ';
        }
        std::cout << "\n-----------------\n";

        start1 = std::chrono::high_resolution_clock::now();
        segAlg.runTestAlgorithm();
        end1 = std::chrono::high_resolution_clock::now();
        auto resTest = segAlg.globalMaxClique;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg test time: " << time1 << std::endl;
        std::cout << "Results: " << countSetBits(resTest) << std::endl;
        auto resultsBits = getSetBits(resTest);
        for (const auto& el: resultsBits)
        {
            std::cout << el << ' ';
        }
        std::cout << "\n-----------------\n";

        start1 = std::chrono::high_resolution_clock::now();
        segAlgReordered.runTestAlgorithm();
        end1 = std::chrono::high_resolution_clock::now();
        auto resTestReordered = segAlgReordered.globalMaxClique;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg test reorsered time: " << time1 << std::endl;
        std::cout << "Results: " << countSetBits(resTestReordered) << std::endl;
        auto resultsBitsReordered = getSetBits(resTestReordered);
        for (const auto& el: resultsBitsReordered)
        {
            std::cout << el << ' ';
        }
        std::cout << "\n-----------------\n";

        start1 = std::chrono::high_resolution_clock::now();
        auto resClique = Algorithm<adjMatr_type>::maxCliqueFinding(adjMatr);
        end1 = std::chrono::high_resolution_clock::now();

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Custom max clique greedy algorithm time: " << time1 << std::endl;
        size_t maxCliqueSize{ 0 };
        std::vector<size_t> maxCLique{};
        for (const auto& clique: resClique)
        {
            if (clique.size() > maxCliqueSize)
            {
                maxCliqueSize = clique.size();
                maxCLique = std::move(clique);
            }
        }
        std::cout << "Results: " << maxCliqueSize << std::endl;
        std::sort(maxCLique.begin(), maxCLique.end());
        for (const auto& elem: maxCLique)
        {
            std::cout << elem << ' ';
        }
        std::cout << "\n-----------------\n";
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
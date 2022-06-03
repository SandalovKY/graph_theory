#include "graph.hpp"
#include "algorithm.hpp"
#include "segundo_max_clique.hpp"
#include "parser.hpp"
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
    using adjMatr_type = BitAdjacencyMatrix<bitset_type>; 
    using adjMatrMap_type = std::map<size_t, myDynamicBitset<>>;
    adjMatr_type adjMatrSimple;
    adjMatr_type adjMatrCoreNum;

    adjMatrMap_type simpleOrderMap{};
    adjMatrMap_type coreNumOrderMap{};
    try
    {
        std::cout << "Start to input col graph files\n";
        Parser graphParser(argv[1]);

        auto simpleReordering = graphParser.getSimpleMaxCliqueReordering();
        auto coreNumReordering = graphParser.getCoreNumsMaxCliqueReorderingMod();

        adjMatrSimple = graphParser.adjList2adjMatr<bitset_type>();    // removed reordering due testing of correctness
        adjMatrCoreNum = graphParser.adjList2adjMatr<bitset_type>(&simpleReordering);

        simpleOrderMap = graphParser.adjList2adjMatrMap();   // removed reordering due testing of correctness
        coreNumOrderMap = graphParser.adjList2adjMatrMap(&simpleReordering);

        std::cout << "Finished files reading ---------------------\n";

        SegundoAlgorithm segAlgSimpleReordering(simpleOrderMap);
        SegundoAlgorithm segAlgCoreNumReordering(coreNumOrderMap);

        std::cout << "Start with simple reordering ---------------------\n";

        auto start1 = std::chrono::high_resolution_clock::now();
        segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto& resBitset = segAlgSimpleReordering.globalMaxClique;

        auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg time: " << time1 << std::endl;
        auto resSet = getSetBits(resBitset);
        std::cout << "Results: " << resSet.size() << std::endl;
        for (const auto& vert: resSet)
        {
            std::cout << vert << ' ';
        }
        std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::ModifiedWithInputBitset);
        // end1 = std::chrono::high_resolution_clock::now();
        // auto resMod = segAlgSimpleReordering.globalMaxClique;
        // auto resSetBits = getSetBits(resMod);

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Seg alg time test: " << time1 << std::endl;
        // std::cout << "Results: " << resSetBits.size() << std::endl;
        // for (const auto& vert: resSetBits)
        // {
        //     std::cout << vert << ' ';
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModifiedAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();
        // res = segAlgSimpleReordering.maxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Seg alg boosted time: " << time1 << std::endl;
        // std::cout << "Results: " << res.size() << std::endl;
        // for (const auto& vert: res)
        // {
        //     std::cout << vert << ' ';
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // auto resClique = Algorithm<adjMatr_type>::maxCliqueFinding(adjMatrSimple);
        // end1 = std::chrono::high_resolution_clock::now();

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Custom max clique greedy algorithm time: " << time1 << std::endl;
        // size_t maxCliqueSize{ 0 };
        // std::vector<size_t> maxCLique{};
        // for (const auto& clique: resClique)
        // {
        //     if (clique.size() > maxCliqueSize)
        //     {
        //         maxCliqueSize = clique.size();
        //         maxCLique = std::move(clique);
        //     }
        // }
        // std::cout << "Results: " << maxCliqueSize << std::endl;
        // std::sort(maxCLique.begin(), maxCLique.end());
        // for (const auto& elem: maxCLique)
        // {
        //     std::cout << elem << ' ';
        // }
        // std::cout << "\n-----------------\n";

        std::cout << "Start with core num reordering ---------------------\n";

        start1 = std::chrono::high_resolution_clock::now();
        segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgCoreNumReordering.globalMaxClique;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Seg alg time: " << time1 << std::endl;
        resSet = getSetBits(resBitset);
        std::cout << "Results: " << resSet.size() << std::endl;
        auto usualOrderSer = graphParser.getDefaultOrder(simpleReordering, resSet);
        for (const auto& vert: usualOrderSer)
        {
            std::cout << vert << ' ';
        }
        std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::ModifiedWithInputBitset);
        // end1 = std::chrono::high_resolution_clock::now();
        // resMod = segAlgCoreNumReordering.globalMaxClique;
        // resSetBits = getSetBits(resMod);

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Seg alg time test: " << time1 << std::endl;
        // std::cout << "Results: " << resSetBits.size() << std::endl;
        // for (const auto& vert: resSetBits)
        // {
        //     std::cout << vert << ' ';
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModifiedAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();
        // res = segAlgCoreNumReordering.maxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Seg alg boosted time: " << time1 << std::endl;
        // std::cout << "Results: " << res.size() << std::endl;
        // for (const auto& vert: res)
        // {
        //     std::cout << vert << ' ';
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // resClique = Algorithm<adjMatr_type>::maxCliqueFinding(adjMatrCoreNum);
        // end1 = std::chrono::high_resolution_clock::now();

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Custom max clique greedy algorithm time: " << time1 << std::endl;
        // maxCliqueSize = 0;
        // maxCLique.clear();
        // for (const auto& clique: resClique)
        // {
        //     if (clique.size() > maxCliqueSize)
        //     {
        //         maxCliqueSize = clique.size();
        //         maxCLique = std::move(clique);
        //     }
        // }
        // std::cout << "Results: " << maxCliqueSize << std::endl;
        // std::sort(maxCLique.begin(), maxCLique.end());
        // for (const auto& elem: maxCLique)
        // {
        //     std::cout << elem << ' ';
        // }
        // std::cout << "\n-----------------\n";
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

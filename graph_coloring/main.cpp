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

    using bitset_type = myBitset<>;
    using adjMatr_type = BitAdjacencyMatrix<bitset_type>; 
    using adjMatrMap_type = std::map<size_t, myBitset<>>;
    adjMatr_type adjMatrCoreNum;

    adjMatrMap_type simpleOrderMap{};
    adjMatrMap_type coreNumOrderMap{};
    try
    {
        std::cout << "---------- Start to input col graph files ----------\n";
        Parser graphParser(argv[1]);

        auto simpleReordering = graphParser.getSimpleMaxCliqueReordering();
        auto coreNumReordering = graphParser.getCoreNumsMaxCliqueReordering();


        simpleOrderMap = graphParser.adjList2adjMatrMap(&simpleReordering);
        coreNumOrderMap = graphParser.adjList2adjMatrMap(&coreNumReordering);
        std::map<size_t, boost::dynamic_bitset<> > simpleOrderMapBoost = graphParser.adjList2adjMatrMapBoost(&simpleReordering);

        std::cout << "---------- Finished files reading ----------\n";

        SegundoAlgorithm segAlgSimpleReordering(simpleOrderMap);
        SegundoAlgorithm segAlgCoreNumReordering(coreNumOrderMap);

        std::cout << "---------- Start with simple reordering ----------\n";

        // auto start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        // auto end1 = std::chrono::high_resolution_clock::now();
        // auto& resBitset = segAlgSimpleReordering.globalMaxClique;

        // auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference Segundo algorithm time: " << time1 << std::endl;
        // auto resSet = getSetBits(resBitset);
        // auto defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReferenceAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference segundo algoritm time with preliminary greedy algoritm run: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Modified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModifiedAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time with preliminary greedy algoritm run: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BronKerboschAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();

        // size_t bronKerbosch = segAlgSimpleReordering.maximalKerboshClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Bron Kerbosch alg time: " << time1 << std::endl;
        // std::cout << "Results: " << bronKerbosch << std::endl;
        // std::cout << "-----------------\n";

        // std::cout << "---------- Start with core num reordering ----------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference Segundo algorithm time: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReferenceAlgorithm);
        // end1 = std::chrono::histd::cout << "---------- Start with core num reordering ----------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference Segundo algorithm time: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCouration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference segundo algoritm time with preliminary greedy algoritm run: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Modified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModifiedAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time with preliminary greedy algoritm run: " << time1 << std::endl;
        // resSet = getSetBits(resBitset);
        // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // for (const auto& vert: defOrderSet)
        // // {
        // //     std::cout << vert << ' ';
        // // }

        // std::cout << "-----------------";
        // if (graphParser.provedClique(defOrderSet))
        // {
        //     std::cout << "\nCorrectClique";
        // }
        // std::cout << "\n-----------------\n";

        // // start1 = std::chrono::high_resolution_clock::now();
        // // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BronKerboschAlgorithm);
        // // end1 = std::chrono::high_resolution_clock::now();

        // // bronKerbosch = segAlgCoreNumReordering.maximalKerboshClique;

        // // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // // std::cout << "Bron Kerbosch alg time: " << time1 << std::endl;
        // // std::cout << "Results: " << bronKerbosch << std::endl;
        // // std::cout << "-----------------\n";

        // // start1 = std::chrono::high_resolution_clock::now();
        // // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Heuristic);
        // // end1 = std::chrono::high_resolution_clock::now();
        // // resBitset = segAlgCoreNumReordering.globalMaxClique;

        // // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // // std::cout << "Seg alg time: " << time1 << std::endl;
        // // resSet = getSetBits(resBitset);
        // // defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        // // std::cout << "Results: " << defOrderSet.size() << std::endl;
        // // // for (const auto& vert: defOrderSet)
        // // // {
        // // //     std::cout << vert << ' ';
        // // // }

        // // std::cout << "-----------------";
        // // if (graphParser.provedClique(defOrderSet))
        // // {
        // //     std::cout << "\nCorrectClique";
        // // }
        // // std::cout << "\n-----------------\n";

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

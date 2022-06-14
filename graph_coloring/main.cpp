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
    adjMatrMap_type withoutReordering{};
    try
    {
        std::cout << "---------- Start to input col graph files ----------\n";
        Parser graphParser(argv[1]);

        auto simpleReordering = graphParser.getSimpleMaxCliqueReordering();
        auto coreNumReordering = graphParser.getCoreNumsMaxCliqueReordering();

        withoutReordering = graphParser.adjList2adjMatrMap();
        simpleOrderMap = graphParser.adjList2adjMatrMap(&simpleReordering);
        coreNumOrderMap = graphParser.adjList2adjMatrMap(&coreNumReordering);
        std::map<size_t, boost::dynamic_bitset<> > simpleOrderMapBoost = graphParser.adjList2adjMatrMapBoost(&simpleReordering);

        std::cout << "---------- Finished files reading ----------\n";

        SegundoAlgorithm segAlg(withoutReordering);
        SegundoAlgorithm segAlgSimpleReordering(simpleOrderMap);
        SegundoAlgorithm segAlgCoreNumReordering(coreNumOrderMap);

        std::cout << "---------- Start with simple reordering ----------\n";

        // auto start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Heuristic);
        // auto end1 = std::chrono::high_resolution_clock::now();
        // auto& resBitset = segAlgSimpleReordering.m_globalMaxClique;

        // auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Heuristic time: " << time1 << std::endl;
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

        // segAlgSimpleReordering.m_use_simple_heuristic = true;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::SimpleHeuristic);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Simple heuristic time: " << time1 << std::endl;
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

        // segAlgSimpleReordering.m_use_simple_heuristic = false;

        size_t refTime{ 0 };
        size_t refStepCount{ 0 };

        auto start1 = std::chrono::high_resolution_clock::now();
        segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto& resBitset = segAlgSimpleReordering.m_globalMaxClique;
        refStepCount = segAlgSimpleReordering.m_step_count;

        auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Reference Segundo algorithm time: " << time1 << std::endl;
        refTime = time1;
        auto resSet = getSetBits(resBitset);
        auto defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReference);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.m_globalMaxClique;

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

        // segAlgSimpleReordering.m_use_simple_heuristic = true;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReference);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Reference segundo algoritm time with simple preliminary greedy algoritm run: " << time1 << std::endl;
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

        // segAlgSimpleReordering.m_use_simple_heuristic = false;

        size_t modTime{ 0 };
        size_t modStepCount{ 0 };

        start1 = std::chrono::high_resolution_clock::now();
        segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Modified);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgSimpleReordering.m_globalMaxClique;
        modStepCount = segAlgSimpleReordering.m_step_count;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Modified Segundo algorithm time: " << time1 << std::endl;
        modTime = time1;
        resSet = getSetBits(resBitset);
        defOrderSet = graphParser.getDefaultOrder(simpleReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.m_globalMaxClique;

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

        // segAlgSimpleReordering.m_use_simple_heuristic = true;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgSimpleReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time with simple preliminary greedy algoritm run: " << time1 << std::endl;
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

        // segAlgSimpleReordering.m_use_simple_heuristic = false;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BronKerboschAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();

        // size_t bronKerbosch = segAlgSimpleReordering.maximalKerboshClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Bron Kerbosch alg time: " << time1 << std::endl;
        // std::cout << "Results: " << bronKerbosch << std::endl;
        // std::cout << "-----------------\n";
        
        std::cout << "---------- Start with core num reordering ----------\n";

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Heuristic);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Heuristic time: " << time1 << std::endl;
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
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::SimpleHeuristic);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Simple heuristic time: " << time1 << std::endl;
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

        size_t reorderedRefTime{ 0 };
        size_t reorderedRefNumSteps{ 0 };

        start1 = std::chrono::high_resolution_clock::now();
        segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Reference);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgCoreNumReordering.m_globalMaxClique;
        reorderedRefNumSteps = segAlgCoreNumReordering.m_step_count;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Reference Segundo algorithm time: " << time1 << std::endl;
        reorderedRefTime = time1;
        resSet = getSetBits(resBitset);
        defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        size_t reorderedRefTimeBoosted{ 0 };
        size_t reorderedRefNumStepsBoosted{ 0 };

        start1 = std::chrono::high_resolution_clock::now();
        segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReference);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgCoreNumReordering.m_globalMaxClique;
        reorderedRefNumStepsBoosted = segAlgCoreNumReordering.m_step_count;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Reference segundo algoritm time with preliminary greedy algoritm run: " << time1 << std::endl;
        reorderedRefTimeBoosted = time1;
        resSet = getSetBits(resBitset);
        defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        segAlgCoreNumReordering.m_use_simple_heuristic = true;

        size_t reorderedRefTimeBoostedSimple{ 0 };
        size_t reorderedRefNumStepsBoostedSimple{ 0 };

        start1 = std::chrono::high_resolution_clock::now();
        segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedReference);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgCoreNumReordering.m_globalMaxClique;
        reorderedRefNumStepsBoostedSimple = segAlgCoreNumReordering.m_step_count;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Reference segundo algoritm time with simple preliminary greedy algoritm run: " << time1 << std::endl;
        reorderedRefTimeBoostedSimple = time1;
        resSet = getSetBits(resBitset);
        defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        segAlgCoreNumReordering.m_use_simple_heuristic = false;

        size_t reorderedModTime{ 0 };
        size_t reorderedModNumSteps{ 0 };

        start1 = std::chrono::high_resolution_clock::now();
        segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::Modified);
        end1 = std::chrono::high_resolution_clock::now();
        resBitset = segAlgCoreNumReordering.m_globalMaxClique;
        reorderedModNumSteps = segAlgCoreNumReordering.m_step_count;

        time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        std::cout << "Modified Segundo algorithm time: " << time1 << std::endl;
        reorderedModTime = time1;
        resSet = getSetBits(resBitset);
        defOrderSet = graphParser.getDefaultOrder(coreNumReordering, resSet);
        std::cout << "Results: " << defOrderSet.size() << std::endl;
        // for (const auto& vert: defOrderSet)
        // {
        //     std::cout << vert << ' ';
        // }

        std::cout << "-----------------";
        if (graphParser.provedClique(defOrderSet))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        // size_t reorderedModTimeBoosted{ 0 };
        // size_t reorderedModNumStepsBoosted{ 0 };

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.m_globalMaxClique;
        // reorderedModNumStepsBoosted = segAlgCoreNumReordering.m_step_count;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time with preliminary greedy algoritm run: " << time1 << std::endl;
        // reorderedModTimeBoosted = time1;
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

        // segAlgCoreNumReordering.m_use_simple_heuristic = true;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgCoreNumReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BoostedModified);
        // end1 = std::chrono::high_resolution_clock::now();
        // resBitset = segAlgCoreNumReordering.m_globalMaxClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Modified Segundo algorithm time with simple preliminary greedy algoritm run: " << time1 << std::endl;
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

        // segAlgCoreNumReordering.m_use_simple_heuristic = false;

        // start1 = std::chrono::high_resolution_clock::now();
        // segAlgSimpleReordering.runTestAlgorithm(SegundoAlgorithm::Algorithms::BronKerboschAlgorithm);
        // end1 = std::chrono::high_resolution_clock::now();

        // size_t bronKerbosch = segAlgSimpleReordering.maximalKerboshClique;

        // time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
        // std::cout << "Bron Kerbosch alg time: " << time1 << std::endl;
        // std::cout << "Results: " << bronKerbosch << std::endl;
        // std::cout << "-----------------\n";

        std::ofstream resFile("results.csv", std::ios_base::app);
        if (resFile.is_open())
        {
            resFile << std::string(argv[1]).append(",").append(std::to_string(refTime)).append(",").append(std::to_string(refStepCount))
                .append(",").append(std::to_string(modTime)).append(",").append(std::to_string(modStepCount))
                .append(",").append(std::to_string(reorderedRefTime)).append(",").append(std::to_string(reorderedRefNumSteps))
                .append(",").append(std::to_string(reorderedModTime)).append(",").append(std::to_string(reorderedModNumSteps))
                .append(",").append(std::to_string(reorderedRefTimeBoosted)).append(",").append(std::to_string(reorderedRefNumStepsBoosted))
                .append(",").append(std::to_string(reorderedRefTimeBoostedSimple)).append(",").append(std::to_string(reorderedRefNumStepsBoostedSimple)).append("\n");
            resFile.close();
        }

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

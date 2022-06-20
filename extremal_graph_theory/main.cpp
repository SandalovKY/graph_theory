#include "segundo_max_clique.hpp"
#include "parser.hpp"
#include <iostream>
#include <map>
#include <utility>
#include <chrono>
#include <fstream>

#include <filesystem>


namespace
{
    std::map<std::string, Algorithms> AlgorithmsMap
    {
        { "reference", Algorithms::Reference },
        { "modified", Algorithms::Modified },
        { "heuristic", Algorithms::Heuristic },
        { "reference", Algorithms::BoostedReference },
        { "reference", Algorithms::BoostedModified },
        { "heuristic_boost", Algorithms::HeuristicUsingBoost },
        { "reference_boost", Algorithms::ReferenceUsingBoost }
    };
    
    enum class Order
    {
        DEFAULT_ORDER,
        DEGREE_ORDER,
        CORE_NUM_ORDER
    };

    std::map<std::string, Order> OrderMap
    {
        { "default", Order::DEFAULT_ORDER },
        { "degree", Order::DEGREE_ORDER },
        { "corenum", Order::CORE_NUM_ORDER }
    };

    std::set<size_t> getSetBits(SegundoAlgorithm<myBitset<> >::bitset_type bitset)
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

    std::set<size_t> getSetBits(SegundoAlgorithm<boost::dynamic_bitset<> >::bitset_type bitset)
    {
        std::set<size_t> retSet{};
        size_t dimSize = bitset.size();
        size_t firstSetBit = bitset.find_first();
        while (firstSetBit < dimSize)
        {
            retSet.insert(firstSetBit);
            bitset.set(firstSetBit, false);
            firstSetBit = bitset.find_first();
        }
        return retSet;
    }

    template<typename bitsetType>
    std::pair<size_t, size_t> prepareAlgorithm(Order orderType, Algorithms algoType, const std::string& path2SrcFile)
    {
        std::cout << "---------- Start graph file reading ----------\n";

        Parser graphParser(path2SrcFile);
        Parser::ReorderingMap reorderingMap{};

        using adjMatr_type = std::map<size_t, bitsetType>;

        adjMatr_type srcAdjMatr{};

        switch(orderType)
        {
        case Order::DEFAULT_ORDER:
            srcAdjMatr = graphParser.adjList2adjMatrMap<bitsetType>();
            break;
        case Order::DEGREE_ORDER:
            reorderingMap = graphParser.getSimpleMaxCliqueReordering();
            srcAdjMatr = graphParser.adjList2adjMatrMap<bitsetType>(&reorderingMap);
            break;
        case Order::CORE_NUM_ORDER:
            reorderingMap = graphParser.getCoreNumsMaxCliqueReordering();
            srcAdjMatr = graphParser.adjList2adjMatrMap<bitsetType>(&reorderingMap);
            break;
        default:
            break;
        }

        std::cout << "---------- Ended file reading ----------\n";

        SegundoAlgorithm<bitsetType> algorithm2Run(srcAdjMatr);

        std::cout << "---------- Algorithm started ----------\n";

        auto timeStart = std::chrono::high_resolution_clock::now();
        algorithm2Run.runAlgorithm(algoType);
        auto timeEnd = std::chrono::high_resolution_clock::now();

        std::cout << "---------- Algorithm finished ----------\n";

        auto& retBitsetClique = algorithm2Run.m_globalMaxClique;

        size_t time = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();
        std::cout << "Algorithm time: " << time << std::endl;

        size_t numSteps = algorithm2Run.m_step_count;
        std::cout << "Algorithm steps: " << numSteps << std::endl;

        auto resSetClique = getSetBits(retBitsetClique);
        std::cout << "Results: " << resSetClique.size() << std::endl;

        if (orderType != Order::DEFAULT_ORDER)
        {
            resSetClique = graphParser.getDefaultOrder(reorderingMap, resSetClique);
        }

        std::cout << "-----------------";
        if (graphParser.provedClique(resSetClique))
        {
            std::cout << "\nCorrectClique";
        }
        std::cout << "\n-----------------\n";

        return std::pair(time, numSteps);
    }
}

int main(int argc, char ** argv) {
    if(argc < 7)
    {
        std::cerr <<
            "Usage: Graph [--boost] -a algorithm -r reordering -f <filename>"
            << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        Algorithms algoType{};
        Order orderType{};
        std::string algorithmString{};
        std::string reorderingString{};

        std::filesystem::path path2SrcFile{};

        bool usingBoost{ std::string(argv[1]).compare("--boost") == 0 };

        for (int num = 1; num < argc; ++num)
        {
            if (std::string(argv[num]).compare("-a") == 0)
            {
                algorithmString = argv[num + 1];
                if (usingBoost) algorithmString.append("_boost");
                if (AlgorithmsMap.find(algorithmString) == AlgorithmsMap.end()) throw std::runtime_error("Incorrect algorithm type");
                algoType = AlgorithmsMap[algorithmString];
            }
            if (std::string(argv[num]).compare("-r") == 0)
            {
                reorderingString = argv[num + 1];
                if (OrderMap.find(reorderingString) == OrderMap.end()) throw std::runtime_error("Incorrect reordering type");
                orderType = OrderMap[reorderingString];
            }
            if (std::string(argv[num]).compare("-f") == 0)
            {
                path2SrcFile = argv[num+1];
            }
        }

        std::pair<size_t, size_t> results{};

        if (usingBoost)
        {
            results = prepareAlgorithm<boost::dynamic_bitset<> >(orderType, algoType, path2SrcFile.string());
        }
        else
        {
            results = prepareAlgorithm<myBitset<> >(orderType, algoType, path2SrcFile.string());
        }

        std::string resFileName("results");
        resFileName.append("_").append(reorderingString).append("_").append(algorithmString).append(".csv");

        std::ofstream resFile(resFileName, std::ios_base::app);
        if (resFile.is_open())
        {
            resFile << path2SrcFile.filename().string().append(",").append(std::to_string(results.first)).append(",").append(std::to_string(results.second)).append("\n");
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

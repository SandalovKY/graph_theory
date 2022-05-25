#include "parser.hpp"

Parser::Parser(char const* filename)
{
    std::ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        size_t fileSize{ 0 };
        std::string line;
        std::getline(inputFile, line);
        std::vector<std::string> parsedLine;
        boost::split(parsedLine, line, [](char c){ return c == ' '; });

        while (parsedLine[0] != "p")
        {
            std::getline(inputFile, line);
            boost::split(parsedLine, line, [](char c){ return c == ' '; });
        }

        if (parsedLine.size() < 4) return;
        m_vertexNum = std::stoi(parsedLine[2]);
        m_edgesNum = std::stoi(parsedLine[3]);
        
        std::getline(inputFile, line);
        boost::split(parsedLine, line, [](char c){ return c == ' '; });

        while (parsedLine[0] != "e")
        {
            std::getline(inputFile, line);
            boost::split(parsedLine, line, [](char c){ return c == ' '; });
        }

        while (parsedLine[0] == "e")
        {
            size_t v_first{ static_cast<size_t>(std::stoi(parsedLine[1])) };
            size_t v_second{ static_cast<size_t>(std::stoi(parsedLine[2])) };

            auto findRes = std::find_if(std::begin(m_list), std::end(m_list), [v_first](std::pair<size_t, std::set<size_t>>& element){
                return element.first == v_first - 1;
            });
            if (findRes != std::end(m_list))
            {
                findRes->second.insert(v_second - 1);
            }
            else
            {
                m_list.push_back({ v_first - 1, std::set<size_t>{ v_second - 1 } });
            }

            findRes = std::find_if(std::begin(m_list), std::end(m_list), [v_second](std::pair<size_t, std::set<size_t>>& element){
                return element.first == v_second - 1;
            });
            if (findRes != std::end(m_list))
            {
                findRes->second.insert(v_first - 1);
            }
            else
            {
                m_list.push_back({ v_second - 1, std::set<size_t>{ v_first - 1 } });
            }

            std::getline(inputFile, line);
            boost::split(parsedLine, line, [](char c){ return c == ' '; });
        }
    }
}

typename std::map<size_t, myDynamicBitset<>>
Parser::adjList2adjMatrMap(ReorderingMap* map2Reorder)
{
    std::map<size_t, myDynamicBitset<>> retMap{};
    size_t numBits = m_list.size();
    if (map2Reorder != nullptr)
    {
        for (auto& vertex: m_list)
        {
            size_t currVertId{ (*map2Reorder)[vertex.first] };
            retMap.insert({ currVertId, myDynamicBitset<>(numBits, currVertId) });
            for(const auto& adjacent: vertex.second)
            {
                retMap[currVertId].set((*map2Reorder)[adjacent]);
            }
        }
    }
    else
    {
        for (const auto& vertex: m_list)
        {
            size_t currVertId = vertex.first;
            retMap.insert({ currVertId, myDynamicBitset<>(numBits, currVertId) });
            for(const auto& adjacent: vertex.second)
            {
                retMap[currVertId].set(adjacent);
            }
        }
    }
    return retMap;
}

Parser::ReorderingMap Parser::getMaxCliqueReordering()
{
    ReorderingMap retMap{};

    std::sort(m_list.begin(), m_list.end(), [](std::pair<size_t, std::set<size_t>>& element1, std::pair<size_t, std::set<size_t>>& element2){
        return element1.second.size() > element2.second.size();
    });

    for (size_t ind = 0;ind < m_list.size(); ++ind)
    {
        retMap[m_list[ind].first] = ind;
    }
    return retMap;
}

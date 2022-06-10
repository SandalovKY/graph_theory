#include "parser.hpp"

std::vector<size_t> getCoreNums(std::vector<std::pair<size_t, std::set<size_t> > >& inputAdjList);

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

        if (parsedLine.size() < 4)
        {
            throw std::runtime_error("Incorrect input graph file format");
        }
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

            auto findRes = std::find_if(std::begin(m_list), std::end(m_list), [&](std::pair<size_t, std::set<size_t>>& element){
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

            findRes = std::find_if(std::begin(m_list), std::end(m_list), [&](std::pair<size_t, std::set<size_t>>& element){
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

typename std::map<size_t, boost::dynamic_bitset<>>
Parser::adjList2adjMatrMapBoost(ReorderingMap* map2Reorder)
{
    std::map<size_t, boost::dynamic_bitset<> > retMap{};
    size_t numBits = m_list.size();
    if (map2Reorder != nullptr)
    {
        for (auto& vertex: m_list)
        {
            size_t currVertId{ (*map2Reorder)[vertex.first] };
            retMap.insert({ currVertId, boost::dynamic_bitset<>(numBits) });
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
            retMap.insert({ currVertId, boost::dynamic_bitset<>(numBits) });
            for(const auto& adjacent: vertex.second)
            {
                retMap[currVertId].set(adjacent);
            }
        }
    }
    return retMap;
}

Parser::ReorderingMap Parser::getSimpleMaxCliqueReordering()
{
    ReorderingMap retMap{};

    AdjList localAdjList = m_list;

    std::sort(localAdjList.begin(), localAdjList.end(), [](std::pair<size_t, std::set<size_t>>& element1, std::pair<size_t, std::set<size_t>>& element2){
        return element1.second.size() > element2.second.size();
    });

    for (size_t ind = 0;ind < localAdjList.size(); ++ind)
    {
        retMap[localAdjList[ind].first] = ind;
    }
    return retMap;
}

std::vector<size_t> getCoreNums(std::vector<std::pair<size_t, std::set<size_t> > >& inputAdjList)
{
    size_t numVerts{ inputAdjList.size() };
    std::vector<size_t> retVector;
    retVector.reserve(numVerts);
    std::vector<std::set<size_t>> arrayD(numVerts);
    std::vector<int32_t> currDValues(numVerts);
    for(const auto& vert: inputAdjList)
    {
        arrayD[vert.second.size()].insert(vert.first);
        currDValues[vert.first] = vert.second.size(); 
    }
    size_t k{ 0 };

    for (int ind = 0; ind < numVerts; ++ind)
    {
        size_t iter{ 0 };
        while (arrayD[iter].size() == 0 && iter < arrayD.size()) ++iter;
        if (iter < arrayD.size())
        {
            k = std::max(k, iter);
            retVector.push_back(*arrayD[iter].begin());
            currDValues[retVector.back()] = -1;
            arrayD[iter].erase(arrayD[iter].begin());
            for (const auto& nbhd: inputAdjList[retVector.back()].second)
            {
                int32_t currVal = currDValues[nbhd];
                if (currVal >= 0)
                {
                    --currDValues[nbhd];
                    arrayD[currVal].erase(nbhd);
                    arrayD[currDValues[nbhd]].insert(nbhd);
                }
            }
        }
    }
    return retVector;
}

Parser::ReorderingMap Parser::getCoreNumsMaxCliqueReordering()
{
    ReorderingMap retMap{};
    std::vector<size_t> retVec = getCoreNums(this->m_list);
    for (size_t ind = 0; ind < retVec.size(); ++ind)
    {
        retMap[retVec[retVec.size() - (ind + 1)]] = ind;
    }
    return retMap;
}

std::set<size_t> Parser::getDefaultOrder(ReorderingMap& reorderingMap, std::set<size_t>& srcSet)
{
    std::set<size_t> retSet{};
    for (const auto& vert: srcSet)
    {
        for (const auto& mapEl: reorderingMap)
        {
            if (vert == mapEl.second)
            {
                retSet.insert(mapEl.first);
                break;
            }
        }
    }
    return retSet;
}

bool Parser::provedClique(const std::set<size_t>& clique)
{
    std::map<size_t, std::set<size_t> > localMap(m_list.begin(), m_list.end());
    for (const auto& vert: clique)
    {
        for (const auto& otherVert: clique)
        {
            if (vert != otherVert)
            {
                if (localMap[vert].find(otherVert) == localMap[vert].end()) return false;
            }
        }
    }
    return true;
}

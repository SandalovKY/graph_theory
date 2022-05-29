#include "parser.hpp"

void someDFS(std::map<size_t, std::set<size_t> >& adjLists, size_t startVert, int_fast32_t k, std::vector<bool>& isVisited, std::vector<int_fast32_t>& vDegree);
std::pair<size_t, int_fast32_t> getMinDegreeVert(std::vector<std::pair<size_t, std::set<size_t> > >& array, std::vector<int_fast32_t>& vDegree);
std::pair<size_t, int_fast32_t> getMinDegreeVert(std::vector<int_fast32_t>& vDegree);
std::vector<std::pair<size_t, size_t> > getCoreNums(std::map<size_t, std::set<size_t> > adjLists, size_t startVert, size_t minDegree, std::vector<int_fast32_t>& vDegree);

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

Parser::ReorderingMap Parser::getSimpleMaxCliqueReordering()
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

std::vector<std::pair<size_t, int_fast32_t> > getCoreNums(std::map<size_t, std::set<size_t> > adjLists, size_t startVert, int_fast32_t minDegree, std::vector<int_fast32_t>& vDegree)
{
    std::vector<std::pair<size_t, int_fast32_t> > retCoreNums(adjLists.size());

    for (const auto& vert: adjLists)
    {
        retCoreNums[vert.first] = { vert.first, minDegree };
    }

    bool someVertsInKcore{ true };
    
    while(someVertsInKcore)
    {
        std::vector<bool> isVisited(adjLists.size(), false);
        someDFS(adjLists, startVert, ++minDegree, isVisited, vDegree);
        for (const auto& vert: adjLists)
        {
            // For cases when graph is disconnected
            someDFS(adjLists, vert.first, minDegree, isVisited, vDegree);
        }
        someVertsInKcore = false;
        for (const auto& vert: adjLists)
        {
            if (vDegree[vert.first] >= minDegree)
            {
                someVertsInKcore = true;
                retCoreNums[vert.first].second = minDegree;
            }
        }
        int_fast32_t gotMinDegree{ 0 };
        std::tie(startVert, gotMinDegree) = getMinDegreeVert(vDegree);
        if (startVert == -1 || gotMinDegree == -1) return retCoreNums;
        minDegree = std::max(minDegree, gotMinDegree);
    }
    return retCoreNums;
}

void someDFS(std::map<size_t, std::set<size_t> >& adjLists, size_t startVert, int_fast32_t k, std::vector<bool>& isVisited, std::vector<int_fast32_t>& vDegree)
{
    isVisited[startVert] = true;

    bool vertNotInCore = vDegree[startVert] > 0 && vDegree[startVert] < k;

    for(auto& neighVert: adjLists[startVert])
    {
        if (vertNotInCore)
        {
            // Remove edge
            --vDegree[neighVert];
            --vDegree[startVert];
        }
        if (!isVisited[neighVert])
        {
            someDFS(adjLists, neighVert, k, isVisited, vDegree);
        }
    }
}


std::pair<size_t, int_fast32_t> getMinDegreeVert(std::vector<std::pair<size_t, std::set<size_t> > >& array, std::vector<int_fast32_t>& vDegree)
{
    int_fast32_t minDegree = INT_FAST32_MAX;
    size_t startVert = -1;

    for (const auto& vert: array)
    {
        vDegree[vert.first] = vert.second.size();
        if (vDegree[vert.first] > 0 && vDegree[vert.first] < minDegree)
        {
            minDegree = vDegree[vert.first];
            startVert = vert.first;
        }
    }
    return { startVert, minDegree };
}

std::pair<size_t, int_fast32_t> getMinDegreeVert(std::vector<int_fast32_t>& vDegree)
{
    int_fast32_t minDegree = INT_FAST32_MAX;
    size_t startVert = -1;

    for (size_t ind = 0; ind < vDegree.size(); ++ind)
    {
        if (vDegree[ind] != 0 && vDegree[ind] < minDegree)
        {
            minDegree = vDegree[ind];
            startVert = ind;
        }
    }
    return { startVert, minDegree };
}

Parser::ReorderingMap Parser::getCoreNumsMaxCliqueReordering()
{
    ReorderingMap retMap{};

    size_t coreNum{ 0 };

    int_fast32_t minDegree = INT_FAST32_MAX;
    size_t startVert = -1;

    std::vector<int_fast32_t> vDegree(m_list.size());

    std::tie(startVert, minDegree) = getMinDegreeVert(m_list, vDegree);
    auto coreNums = getCoreNums(std::map<size_t, std::set<size_t> >(m_list.begin(), m_list.end()), startVert, minDegree, vDegree);

    std::sort(coreNums.begin(), coreNums.end(), [&](std::pair<size_t, int_fast32_t>& element1, std::pair<size_t, int_fast32_t>& element2){
        return element1.second > element2.second;
    });

    for (size_t ind = 0;ind < coreNums.size(); ++ind)
    {
        retMap[coreNums[ind].first] = ind;
    }
    return retMap;
}



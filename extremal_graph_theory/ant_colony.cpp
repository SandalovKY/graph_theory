#include "ant_colony.hpp"


AntColony::traits_t AntColony::Ant::doWork(tabu_list_t& tabuMoves, traits_t& globalTraits, ColoredGraph& graph, color_groups_t& colorGroups)
{
    traits_t m_localTraits{};
    for (const auto& el : workingSet)
    {
        auto groupId = graph[el].GroupId;
        if (groupId == -1)
        {
            std::map<size_t, std::set<vert_descr_t>> gfMap;
            std::pair<adj_iter_t, adj_iter_t> adjVertPair;
            std::set<size_t> tabuColors;
            for (adjVertPair = boost::adjacent_vertices(el, graph); adjVertPair.first != adjVertPair.second; ++adjVertPair.first)
            {
                auto adjGroupId = graph[*adjVertPair.first].GroupId;
                if (adjGroupId != -1)
                {
                    if(tabuMoves.find(*adjVertPair.first) != tabuMoves.end())
                    {
                        tabuColors.insert(adjGroupId);
                    }
                    gfMap[adjGroupId].insert(*adjVertPair.first);
                }
            }
            for(int grInd = 0; grInd < m_groups_num; ++grInd)
            {
                if (gfMap.find(grInd) == gfMap.end())
                gfMap[grInd] = std::set<vert_descr_t>();
            }
            for(const auto& col : tabuColors)
            {
                gfMap.erase(col);
            }
            size_t minGfVal{1'000'000};
            for (const auto& gfVecEl : gfMap)
            {
                minGfVal = gfVecEl.second.size() < minGfVal ? gfVecEl.second.size() : minGfVal; 
            }
            std::vector<size_t> minGfGroups;
            for (const auto& gfVecEl : gfMap)
            {
                if (gfVecEl.second.size() == minGfVal) minGfGroups.push_back(gfVecEl.first);
            }
            int64_t rezGroupIndex{ -1 };
            if (minGfGroups.size() == 1)
            {
                rezGroupIndex = minGfGroups[0];
            }
            else if (minGfGroups.size() > 1)
            {
                std::map<size_t, int64_t> tr;
                for(const auto& gfGroup : minGfGroups)
                {
                    tr[gfGroup] = 0;
                    for(const auto& vert : colorGroups[gfGroup])
                    {
                        if (gfMap[gfGroup].find(vert) == gfMap[gfGroup].end())
                        {
                            auto first = vert > el ? el : vert;
                            auto second = vert > el ? vert : el;
                            tr[gfGroup] += globalTraits[std::make_pair(first, second)];
                        }
                    }
                }
                int64_t maxTr{-1};
                for (const auto& trEl : tr)
                {
                    if (maxTr < trEl.second)
                    {
                        maxTr = trEl.second;
                        rezGroupIndex = trEl.first;
                    }
                }
            }

            if (rezGroupIndex != -1)
            {
                for(const auto& adjColEl : gfMap[rezGroupIndex])
                {
                    graph[adjColEl].GroupId = -1;
                    colorGroups[rezGroupIndex].erase(adjColEl);
                }
                graph[el].GroupId = rezGroupIndex;
                size_t colGrSize = colorGroups[rezGroupIndex].size() + 1;
                for(const auto& grColEl : colorGroups[rezGroupIndex])
                {
                    auto first = grColEl > el ? el : grColEl;
                    auto second = grColEl > el ? grColEl : el;
                    m_localTraits[std::make_pair(first, second)] = colGrSize;
                }
                colorGroups[rezGroupIndex].insert(el);
                std::pair<vert_iter_t, vert_iter_t> vertIterPair;
                size_t nonColVert;
                for (vertIterPair = boost::vertices(graph); vertIterPair.first != vertIterPair.second; ++vertIterPair.first)
                {
                    if (graph[*(vertIterPair.first)].GroupId == -1) ++nonColVert;
                }
                tabuMoves[el] = 59;
            }
            break;
        }
    }
    return m_localTraits;
}

AntColony::AntColony(ColoredGraph& graph, int antsCount) : m_colony(antsCount, Ant(-1)), m_numAnts(antsCount), m_graphRef(graph)
{
    for (size_t i = 0; i < m_colony.size(); ++i)
    {
        m_colony[i].setAntIndex(i);
    }

    size_t index{};
    size_t vertNum = m_graphRef[boost::graph_bundle].numProc;
    if (!vertNum || !m_numAnts) return;
    size_t vertPerAnt = vertNum / m_numAnts;

    size_t numVerts{};
    std::pair<vert_iter_t, vert_iter_t> vertPair;
    for (vertPair = boost::vertices(m_graphRef); vertPair.first != vertPair.second; ++vertPair.first)
    {
        m_colony[index].addToWorkingSet(*vertPair.first);
        ++numVerts;
        if (numVerts == vertPerAnt && index < m_colony.size() - 1)
        {
            ++index;
            numVerts = 0;
        }
    }
}

void AntColony::doWork(size_t maxIter)
{
    for(int iter = 0; iter < maxIter; ++iter)
    {
        traits_t allLocalTraits;
        for (int ant = 0; ant < m_colony.size(); ++ant)
        {
            auto localTraits = m_colony[ant].doWork(m_tabuMoves, m_globalTraits, m_graphRef, m_groups);
            for (auto& itTabu : m_tabuMoves)
            {
                --itTabu.second;
            }
            for (auto itTabu = m_tabuMoves.cbegin(); itTabu != m_tabuMoves.cend();)
            {
                if ((*itTabu).second == 0)
                {
                    m_tabuMoves.erase(itTabu++);
                }
                else
                {
                    ++itTabu;
                }
            }
            for(const auto& localTrait : localTraits)
            {
                allLocalTraits[localTrait.first] += localTrait.second;
            }
        }
        for(auto& globalTrait : m_globalTraits)
        {
            m_globalTraits[globalTrait.first] = globalTrait.second * 0.9 + allLocalTraits[globalTrait.first];
        }
    }
}

#include <vector>
#include <utility>
#include <map>
#include <set>

#include <iostream>

namespace my
{

class AntColony
{
private:
    using vert_descr_t = boost::graph_traits<my::ColoredGraph>::vertex_descriptor;
    using edge_descr_t = boost::graph_traits<my::ColoredGraph>::edge_descriptor;
    using vert_iter_t = boost::graph_traits<my::ColoredGraph>::vertex_iterator;
    using adj_iter_t = boost::graph_traits<my::ColoredGraph>::adjacency_iterator;
    using traits_t = std::map<std::pair<vert_descr_t, vert_descr_t>, size_t>;
    using color_groups_t = std::vector<std::set<vert_descr_t>>;
    using tabu_list_t = std::map<vert_descr_t, size_t>;


    color_groups_t m_groups{66};
    traits_t m_globalTraits{};
    tabu_list_t m_tabuMoves{};
    my::ColoredGraph& m_graphRef;

    size_t m_numAnts{};

    class Ant
    {
        using vert_descr_t = boost::graph_traits<my::ColoredGraph>::vertex_descriptor;

        size_t m_groups_num{66};
        int64_t antID{};
        std::set<vert_descr_t> workingSet{};
    public:
        Ant(int64_t index) : antID(index) {}
        void setAntIndex(int64_t index)
        {
            antID = index;
        }
        void addToWorkingSet(vert_descr_t descr)
        {
            workingSet.insert(descr);
        }
        traits_t doWork(tabu_list_t& tabuMoves, traits_t& globalTraits, my::ColoredGraph& graph, color_groups_t& colorGroups)
        {
            // std::cout << "\nAnt: " << antID << '\n';
            traits_t m_localTraits{};
            // el - дескриптор для обрабатываемой вершины
            for (const auto& el : workingSet)
            {
                auto groupId = graph[el].GroupId;
                if (groupId == -1)
                {
                    // std::cout << "El in wl: " << el << std::endl;
                    std::map<size_t, std::set<vert_descr_t>> gfMap;
                    std::pair<adj_iter_t, adj_iter_t> adjVertPair;
                    std::set<size_t> tabuColors;
                    for (adjVertPair = boost::adjacent_vertices(el, graph); adjVertPair.first != adjVertPair.second; ++adjVertPair.first)
                    {
                        // std::cout << "Adj to El: " << *adjVertPair.first << std::endl;
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
                    // std::cout << "GfMap size: " << gfMap.size() << '\n';
                    // std::cout << "ColTabuNum: " << tabuColors.size() << '\n';
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
                        // std::cout << "MinGfGroups: " << minGfGroups.size() << std::endl;
                        std::map<size_t, int64_t> tr;
                        for(const auto& gfGroup : minGfGroups)
                        {
                            // std::cout << "gfGroup: " << gfGroup << std::endl;
                            tr[gfGroup] = 0;
                            for(const auto& vert : colorGroups[gfGroup])
                            {
                                // std::cout << "Inside\n";
                                if (gfMap[gfGroup].find(vert) == gfMap[gfGroup].end())
                                {
                                    auto first = vert > el ? el : vert;
                                    auto second = vert > el ? vert : el;
                                    tr[gfGroup] += globalTraits[std::make_pair(first, second)];
                                }
                            }
                        }
                        // std::cout << "NumTr: " << tr.size() << '\n';
                        int64_t maxTr{-1};
                        for (const auto& trEl : tr)
                        {
                            // std::cout << "maxTr: " << maxTr << " trEl: " << trEl.second << std::endl;
                            if (maxTr < trEl.second)
                            {
                                // std::cout << "Cond done\n";
                                maxTr = trEl.second;
                                rezGroupIndex = trEl.first;
                            }
                        }
                    }

                    if (rezGroupIndex != -1)
                    {
                        // std::cout << "RezGrInd" << rezGroupIndex << std::endl;
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
                        tabuMoves[el] = 30;
                    }
                    break;
                }
            }
            return m_localTraits;
        }
    };

    std::vector<Ant> m_colony{};
public:
    AntColony(my::ColoredGraph& graph, int antsCount = 10) : m_colony(antsCount, Ant(-1)), m_numAnts(antsCount), m_graphRef(graph)
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
            // std::cout << "Ant ind: " << index << "\tVert: " << *vertPair.first << std::endl;
            // std::pair<adj_iter_t, adj_iter_t> adjPair;
            // for (adjPair = boost::adjacent_vertices(*vertPair.first, m_graphRef); adjPair.first != adjPair.second; ++adjPair.first)
            // {
            //     std::cout << "adj to it: " << *adjPair.first << std::endl;
            // }
            m_colony[index].addToWorkingSet(*vertPair.first);
            ++numVerts;
            if (numVerts == vertPerAnt && index < m_colony.size() - 1)
            {
                ++index;
                numVerts = 0;
            }
        }

        // m_globalTraits = std::vector<std::vector<size_t>>{vertNum, std::vector<size_t>{vertNum}};
    }

    void doWork(size_t maxIter)
    {
        for(int iter = 0; iter < maxIter; ++iter)
        {
            // std::cout << "Iter: " << iter << '\n';
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
        for(const auto& group : m_groups)
        {
            std::cout << "Elements: ";
            for(const auto& el : group)
            {
                std::cout << el + 1 << '\t';
            }
            std::cout << std::endl;
        }
    }
};
}

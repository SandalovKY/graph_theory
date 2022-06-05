#include "random_graph_generator.hpp"

#include <fstream>
#include <string>
#include <iostream>

void generateRandomGraph(size_t NOEdges, size_t NOVertices, std::pair<size_t, size_t>* graph)
{
    size_t ind_i{ 0 };
    while (ind_i < NOEdges)
    {
        graph[ind_i].first = rand() % NOVertices + 1;
        graph[ind_i].second = rand() % NOVertices + 1;
        if (graph[ind_i].first == graph[ind_i].second) continue;
        else
        {
            for (size_t ind_j = 0; ind_j < ind_i; ++ind_j)
            {
                if (graph[ind_i].first == graph[ind_j].first && graph[ind_i].second == graph[ind_j].second ||
                    graph[ind_i].first == graph[ind_j].second && graph[ind_i].second == graph[ind_j].first)
                {
                    --ind_i;
                }
            }
            ++ind_i;
        }
    }
}

void printRandomGraph(size_t NOEdges, size_t NOVertices, std::pair<size_t, size_t>* graph, char* name)
{
    std::ofstream graphFile;
    std::string path("../generated_graphs/");
    path.append(name);
    graphFile.open(path);
    if (graphFile.is_open())
    {
        graphFile << "p edge " << NOVertices << ' ' << NOEdges << std::endl;
        for (size_t edge = 0; edge < NOEdges; ++edge)
        {
            graphFile << "e " << graph[edge].first << ' ' << graph[edge].second << std::endl;
        }
        graphFile.close();
    }
    else
    {
        std::cout << "Could not open output file\n";
    }
}

void graphFileGeneration(size_t NOEdges, size_t NOVertices, char* name)
{
    std::pair<size_t, size_t>* graph = new std::pair<size_t, size_t>[NOEdges];
    generateRandomGraph(NOEdges, NOVertices, graph);
    std::cout << "Generation finished\n";
    printRandomGraph(NOEdges, NOVertices, graph, name);
    std::cout << "Printing finished\n";
    delete[](graph);
}

int main(int argc, char** argv)
{
    if (argc == 4)
    {
        size_t NOEdges = std::stoi(argv[2]);
        size_t NOVertices = std::stoi(argv[3]);
        graphFileGeneration(NOEdges, NOVertices, argv[1]);
    }
    else return -1;
    return 0;
}
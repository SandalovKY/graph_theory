#pragma once
#include <unordered_map>
#include <vector>
#include <boost/graph/adjacency_list.hpp>

namespace my
{
using Vertex_color_p = boost::property<boost::vertex_color_t, size_t>;
using ColoredGraph = boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS,
                                                   Vertex_color_p, boost::no_property>;
} // namsepace my
#pragma once
#include <unordered_map>
#include <vector>
#include <boost/graph/adjacency_list.hpp>

namespace my
{
using vertex_color_p = boost::property<boost::vertex_color_t, size_t>;
using edge_index_p = boost::property<boost::edge_index_t, size_t>;
using graph_representation = boost::adjacency_list<boost::vecS, boost::listS, boost::undirectedS,
                                                   edge_index_p, vertex_color_p>;
} // namsepace my
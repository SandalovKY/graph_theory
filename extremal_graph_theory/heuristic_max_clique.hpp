#pragma once

#include <map>
#include <vector>
#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include "bitset.hpp"
#include "segundo_max_clique.hpp"

SegundoAlgorithm<myBitset<> >::bitset_type maxCliqueFindingHeuristic(std::map<size_t, myBitset<>> adjMatr);
SegundoAlgorithm<myBitset<> >::bitset_type maxCliqueFindingHeuristicSimple(std::map<size_t, myBitset<>> adjMatr);
SegundoAlgorithm<boost::dynamic_bitset<> >::bitset_type maxCliqueFindingHeuristicSimpleBoost(std::map<size_t, boost::dynamic_bitset<>> adjMatr);

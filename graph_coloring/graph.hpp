#pragma once
#include <unordered_map>
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <boost/dynamic_bitset.hpp>

namespace my
{
using Vertex_color_p = boost::property<boost::vertex_color_t, size_t>;
using ColoredGraph = boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS,
                                                   boost::property<boost::vertex_name_t,
                                                   std::string, Vertex_color_p>, boost::no_property>;


template <typename NeighboursList>
class BitAdjacencyMatrix
{
public:
    using m_matrix_type = std::vector<NeighboursList>;
    using m_string_type = NeighboursList;
    using m_vertex_num_type = size_t;
    using m_edges_num_type = size_t;
private:
    m_matrix_type m_matrix{};
    m_vertex_num_type m_vertex_num{};
    m_edges_num_type m_edges_num{};
public:
    m_matrix_type& getMatrix()
    {
        return m_matrix;
    }

    NeighboursList& getLine(size_t ind)
    {
        if (ind < 0 || ind >= getMatrDimSize()) throw std::out_of_range("Incorrect index");
        return m_matrix[ind];
    }

    BitAdjacencyMatrix(m_vertex_num_type vertex_num, m_edges_num_type edges_num = 0)
    : m_matrix(vertex_num), m_vertex_num(vertex_num), m_edges_num(edges_num)
    {
        for (auto & vertex : m_matrix)
        {
            vertex = NeighboursList(vertex_num);
        }
    }

    BitAdjacencyMatrix(BitAdjacencyMatrix&& other_matrix)
    : m_matrix(std::move(other_matrix.m_matrix)),
      m_edges_num(other_matrix.m_edges_num),
      m_vertex_num(other_matrix.m_vertex_num)
    {
        other_matrix.m_vertex_num = 0;
        other_matrix.m_edges_num = 0;
    }
    BitAdjacencyMatrix(const BitAdjacencyMatrix& other_matrix)
    : m_matrix(other_matrix.m_matrix),
      m_edges_num(other_matrix.m_edges_num),
      m_vertex_num(other_matrix.m_vertex_num)
    {}

    BitAdjacencyMatrix& operator=(BitAdjacencyMatrix&& other_matrix)
    {
        if (this != &other_matrix)
        {
            m_matrix = std::move(other_matrix.m_matrix);
            m_edges_num = other_matrix.m_edges_num;
            m_vertex_num = other_matrix.m_vertex_num;

            other_matrix.m_vertex_num = 0;
            other_matrix.m_edges_num = 0;
        }
        return *this;
    }
    BitAdjacencyMatrix& operator=(const BitAdjacencyMatrix& other_matrix) = delete;

    BitAdjacencyMatrix(m_matrix_type&& other_matrix)
    : m_matrix(std::move(other_matrix)),
      m_vertex_num(static_cast<m_vertex_num_type>(m_matrix.size()))
    {}
    BitAdjacencyMatrix(const m_matrix_type& other_matrix)
    : m_matrix(other_matrix),
      m_vertex_num(static_cast<m_vertex_num_type>(m_matrix.size()))
    {}
    BitAdjacencyMatrix& operator=(m_matrix_type&& other_matrix)
    {
        m_matrix = std::move(other_matrix);
        m_vertex_num = m_matrix.size();
        return *this;
    }
    BitAdjacencyMatrix& operator=(const m_matrix_type& other_matrix) = delete;
    BitAdjacencyMatrix()
    {}
    NeighboursList& operator[](size_t elem)
    {
        return m_matrix[elem];
    }
    m_vertex_num_type getMatrDimSize()
    {
        return m_vertex_num;
    }
    void setVertexNum(size_t vertexNum) { m_vertex_num = vertexNum; }
    void setEdgesNum(size_t edgesNum) { m_edges_num = edgesNum; }
    ~BitAdjacencyMatrix() = default;
};

} // namsepace my
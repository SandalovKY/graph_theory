#pragma once
#include <boost/json.hpp>
#include <boost/json/stream_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>

#include "graph.hpp"

namespace my_parser
{
namespace json = boost::json;

class Buffer
{
protected:
    void clearBuffer()
    {
        if (m_buffer != nullptr)
        {
            delete[](m_buffer);
            m_buffer = nullptr;
            m_length = 0;
        }
    }
    char* m_buffer{nullptr};
    size_t m_length{};
public:
    Buffer(const Buffer& other) = delete;
    Buffer(Buffer&& other) = delete;
    Buffer& operator=(const Buffer& other) = delete;
    Buffer& operator=(Buffer&& other) = delete;

    Buffer(size_t length)
    {
        if (m_buffer != nullptr) clearBuffer();
        m_buffer = new char[length];
        m_length = length;
    }
    ~Buffer()
    {
        clearBuffer();
    }
    char* getBuffer() const
    {
        return m_buffer;
    }
    size_t getLength() const
    {
        return m_length;
    }
};

json::value
ParseFile(char const* filename)
{
    std::ifstream inputFile(filename, std::ifstream::binary);
    if (inputFile)
    {
        inputFile.seekg(0, inputFile.end);
        const size_t length = inputFile.tellg();
        inputFile.seekg(0, inputFile.beg);

        Buffer buffer(length);
        inputFile.read(buffer.getBuffer(), buffer.getLength());
        if (!inputFile) return{};

        json::error_code err;
        json::stream_parser parser;
        parser.write(buffer.getBuffer(), buffer.getLength(), err);
        if (err) return{};

        parser.finish(err);
        if (err) return{};

        return parser.release();
    }
    return{};
}

my::ColoredGraph
ReadColoredGraphToAdjList(json::value const& jv)
{
    if (jv.kind() != json::kind::object) return my::ColoredGraph();
    
    auto const& obj = jv.get_object();
    if(!obj.empty())
    {
        auto it = obj.begin();
        auto const name = it->key();
        if (name != "Name") return my::ColoredGraph();
        ++it;
        if (it->key() != "Vertices" ||
            it->value().kind() != json::kind::object) return my::ColoredGraph();
        auto const& vertObj = it->value().get_object();
        if (!vertObj.empty())
        {
            std::vector<std::pair<size_t, size_t> > pairList;
            size_t vertNum = 0;
            for ( auto const& vertIt : vertObj )
            {
                size_t vertex = std::stoi(std::string(vertIt.key()));
                ++vertNum;
                if (vertIt.value().kind() != json::kind::array) return my::ColoredGraph();
                auto const& arrVert = vertIt.value().get_array();
                for (auto const& arrVertIt : arrVert)
                {
                    if (arrVertIt.kind() != json::kind::int64) return my::ColoredGraph();
                    pairList.emplace_back(vertex, arrVertIt.get_int64());
                }
            }
            return my::ColoredGraph(pairList.begin(), pairList.end(),
                                vertNum);
        }
    }
    return my::ColoredGraph();
}

template <typename ListType>
typename my::BitAdjacencyMatrix<ListType>
ReadGraphToAdjMatr(char const* filename)
{
    // std::cout << "Inside function\n";
    std::ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        // std::cout << "Opened file\n";
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

        if (parsedLine.size() < 4) return my::BitAdjacencyMatrix<ListType>{};
        my::BitAdjacencyMatrix<ListType> adjMatrRet{ static_cast<typename my::BitAdjacencyMatrix<ListType>::m_vertex_num_type>(std::stoi(parsedLine[2])),
            static_cast<typename my::BitAdjacencyMatrix<ListType>::m_edges_num_type>(std::stoi(parsedLine[3])) };

        // std::cout << adjMatrRet.getMatrDimSize() << std::endl;

        std::getline(inputFile, line);
        boost::split(parsedLine, line, [](char c){ return c == ' '; });

        while (parsedLine[0] == "e")
        {
            size_t v_first{ static_cast<size_t>(std::stoi(parsedLine[1])) };
            size_t v_second{ static_cast<size_t>(std::stoi(parsedLine[2])) };
            adjMatrRet[v_first - 1].set(v_second - 1);
            adjMatrRet[v_second - 1].set(v_first - 1);
            std::getline(inputFile, line);
            boost::split(parsedLine, line, [](char c){ return c == ' '; });
        }

        return adjMatrRet;
    }
    std::cout << "Error while file reading" << std::endl;
    return my::BitAdjacencyMatrix<ListType>{};
}

template <typename ListType>
typename my::BitAdjacencyMatrix<ListType>::m_matrix_type
ReadColoredGraphToAdjMatr(json::value const& jv)
{
    if (jv.kind() != json::kind::object) return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
    auto const& obj = jv.get_object();
    if(!obj.empty())
    {
        auto it = obj.begin();
        auto const name = it->key();
        if (name != "Name") return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
        ++it;
        if (it->key() != "Vertices" ||
            it->value().kind() != json::kind::object) return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
        auto const& vertObj = it->value().get_object();
        if (!vertObj.empty())
        {
            typename my::BitAdjacencyMatrix<ListType>::m_matrix_type adjMatr{};
            for ( auto const& vertIt : vertObj )
            {
                size_t vertex = std::stoi(std::string(vertIt.key()));
                if (vertIt.value().kind() != json::kind::array) return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
                auto const& arrVert = vertIt.value().get_array();
                typename my::BitAdjacencyMatrix<ListType>::m_vertex_num_type nbhdVert = vertObj.size();
                typename my::BitAdjacencyMatrix<ListType>::m_string_type bitStr(nbhdVert);
                for (auto const& arrVertIt : arrVert)
                {
                    if (arrVertIt.kind() != json::kind::int64) return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
                    bitStr.set(arrVertIt.get_int64());
                }
                adjMatr.emplace_back(std::move(bitStr));
            }
            return adjMatr;
        }
    }
    return typename my::BitAdjacencyMatrix<ListType>::m_matrix_type{};
}
}

#pragma once
#include <boost/json.hpp>
#include <boost/json/stream_parser.hpp>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>

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
ParseFile( char const* filename )
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
ReadColoredGraph(json::value const& jv)
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
                    std::cout << vertex << '\t' << arrVertIt.get_int64() << std::endl;
                }
            }

            return my::ColoredGraph(pairList.begin(), pairList.end(),
                                vertNum);
        }
    }
    return my::ColoredGraph();
}
}

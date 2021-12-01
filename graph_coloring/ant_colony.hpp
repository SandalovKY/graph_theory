#include <vector>
#include <utility>
#include <unordered_map>

class Tag
{
private:
public:
    Tag();
    ~Tag();
};

template <typename Graph>
class ant_colony
{
private:
    using vert_descr_t = boost::graph_traits<my::ColoredGraph>::vertex_descriptor;
    using traits_container_t = std::unordered_map<vert_descr_t, std::unordered_map<vert_descr_t, size_t>>;
    static std::vector<std::pair<vert_descr_t, Tag>> colony_tabuList;
    static traits_container_t colony_traits;
    traits_container_t local_traits;
public:
    static void setColonyTraits(Graph graph)
    {
        
    }
    ant_colony();
    void addVertex();
    ~ant_colony();
};

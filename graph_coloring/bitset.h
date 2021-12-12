#include <vector>
#include <cstdint>
#include <iostream>

namespace
{
    const int64_t min_num_blocks = 100;
    const int64_t npos = -1;
}

template <typename BlockType = uint64_t>
class myDynamicBitset
{
public:
    using m_block_type = BlockType;
    using m_block_size_type = uint32_t;
    using m_bit_position_type = int64_t;
    using m_block_iterator_type = typename std::vector<m_block_type>::iterator;
    using m_block_const_iterator_type = typename std::vector<m_block_type>::const_iterator;
private:
    m_bit_position_type m_block_num{};
    m_bit_position_type m_last_set_bit_num{}; 
    m_block_size_type m_block_size{};
    std::vector<m_block_type> m_block_vector{};

    static bool m_block_not_empty(const m_block_type& block)
    {
        return block != m_block_type{ 0 };
    }

    m_block_size_type getLowestBit(const m_block_type& block) const
    {
        return __builtin_ctzll(block);
    }

public:
    myDynamicBitset()
        : m_block_size(sizeof(m_block_type) * 8),
          m_last_set_bit_num(-1),
          m_block_num(0),
          m_block_vector(min_num_blocks, 0)
    {}

    myDynamicBitset(m_bit_position_type numBits)
        : m_block_size(sizeof(m_block_type) * 8),
          m_last_set_bit_num(numBits - 1),
          m_block_num(numBits / (sizeof(m_block_type) * 8) + 1),
          m_block_vector(m_block_num)
    {}

    myDynamicBitset(myDynamicBitset&& other)
        : m_block_vector(std::move(other.m_block_vector)),
          m_last_set_bit_num(other.m_last_set_bit_num),
          m_block_num(other.m_block_num),
          m_block_size(other.m_block_size)
    {
        other.m_last_set_bit_num = -1;
        other.m_block_num = 0;
        other.m_block_size = 0;
    }

    myDynamicBitset(const myDynamicBitset& other)
        : m_block_vector(other.m_block_vector),
          m_last_set_bit_num(other.m_last_set_bit_num),
          m_block_num(other.m_block_num),
          m_block_size(other.m_block_size)
    {}

    void operator=(myDynamicBitset&& other)
    {
        m_block_vector = std::move(other.m_block_vector);

        m_last_set_bit_num = other.m_last_set_bit_num;
        m_block_num = other.m_block_num;
        m_block_size = other.m_block_size;

        other.m_last_set_bit_num = -1;
        other.m_block_num = 0;
        other.m_block_size = 0;
    }

    void operator=(const myDynamicBitset& other)
    {
        m_block_vector = other.m_block_vector;

        m_last_set_bit_num = other.m_last_set_bit_num;
        m_block_num = other.m_block_num;
        m_block_size = other.m_block_size;
    }

    friend bool operator==(const myDynamicBitset& l, const myDynamicBitset& r)
    {
        if (l.m_last_set_bit_num != r.m_last_set_bit_num ||
            l.m_block_num != r.m_block_num) return false;
        for (size_t ind = 0; ind < l.m_block_num; ++ind)
        {
            if (l.m_block_vector[ind] != r.m_block_vector[ind]) return false;
        }
        return true;
    }

    m_bit_position_type num_blocks() const
    {
        return m_block_num;
    }

    bool set(m_bit_position_type bitPos)
    {
        if (m_last_set_bit_num < bitPos) m_last_set_bit_num = bitPos;
        const m_bit_position_type block_num = bitPos / m_block_size + 1;
        const m_block_size_type block_offset = bitPos % m_block_size;

        if (block_num > m_block_vector.size())
        {
            m_block_vector.resize((block_num) * 2);
        }
        m_block_vector[block_num - 1] |= static_cast<m_block_type>(1) << block_offset;

        if (m_block_num < block_num) m_block_num = block_num;
        return true;
    }

    bool unset(m_bit_position_type bitPos)
    {
        if (m_last_set_bit_num < bitPos) return false;
        const m_bit_position_type block_num = bitPos / m_block_size;
        const m_block_size_type block_offset = bitPos % m_block_size;

        if (block_num >= m_block_vector.size()) return false;
        m_block_vector[block_num] &= ~(m_block_type{ 1 } << block_offset);
        return true;
    }

    std::set<size_t> getNeighbours()
    {
        std::set<size_t> ret;
        myDynamicBitset tmp = *this;
        size_t adjVert = tmp.getFirstNonZeroPosition();
        while(adjVert <= tmp.m_last_set_bit_num)
        {
            ret.insert(adjVert);
            tmp.unset(adjVert);
            adjVert = tmp.getFirstNonZeroPosition();
        }
        return ret;
    }
    
    friend size_t countSetBits(const myDynamicBitset& bitset)
    {
        size_t count{ 0 };
        for (const auto& block : bitset.m_block_vector)
        {
            count += __builtin_popcount(block);
        }
        return count;
    }

    m_bit_position_type getFirstNonZeroPosition() const
    {
        m_bit_position_type i = std::distance(m_block_vector.begin(), std::find_if(m_block_vector.begin(), m_block_vector.end(), m_block_not_empty));
        if (i >= num_blocks())
        {
            return m_last_set_bit_num + 1;
        }
        return i * m_block_size + static_cast<m_block_size_type>(getLowestBit(m_block_vector[i]));
    }

    m_block_const_iterator_type getFirstBlockIterator() const
    {
        return m_block_vector.cbegin();
    }

    m_block_const_iterator_type getLastBlockIterator() const
    {
        return m_block_vector.cend();
    }

    myDynamicBitset& operator&=(const myDynamicBitset& other)
    {
        if (this->m_block_num != other.m_block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        for(size_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] &= other.m_block_vector[block];
        }
        return *this;
    }

    friend myDynamicBitset operator&(const myDynamicBitset lhs, const myDynamicBitset& rhs)
    {
        myDynamicBitset tmp{ lhs };
        tmp &= rhs;
        return tmp;
    }

    myDynamicBitset& operator|=(const myDynamicBitset& other)
    {
        if (this->m_block_num != other.block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        for(size_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] |= other.m_block_vector[block];
        }
        return *this;
    }

    friend myDynamicBitset operator|(myDynamicBitset lhs, const myDynamicBitset& rhs)
    {
        myDynamicBitset tmp{ lhs };
        tmp |= rhs;
        return tmp;
    }

    myDynamicBitset& operator^=(const myDynamicBitset& other)
    {
        if (this->m_block_num != other.m_block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        for(size_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] ^= other.m_block_vector[block];
        }
        return *this;
    }

    friend myDynamicBitset operator^(myDynamicBitset lhs, const myDynamicBitset& rhs)
    {
        myDynamicBitset tmp{ lhs };
        tmp ^= rhs;
        return tmp;
    }

    myDynamicBitset& operator~()
    {
        for(size_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] = ~(this->m_block_vector[block]);
        }
        return *this;
    }

    ~myDynamicBitset()
    {}

    friend std::ostream& operator<<(std::ostream& os, const myDynamicBitset& data)
    {
        myDynamicBitset::m_block_type tmpBlockPos{0};
        for (size_t block = 0; block < data.m_block_num; ++block)
        {
            if (data.m_last_set_bit_num / ((block + 1) * data.m_block_size) > 0)
            {
                while (tmpBlockPos < data.m_block_size)
                {
                    os << ((data.m_block_vector[block] & (static_cast<myDynamicBitset::m_block_type>(1) << tmpBlockPos)) >> tmpBlockPos++) << ' ';
                }
                tmpBlockPos = 0;
            }
            else
            {
                size_t lastBits = data.m_last_set_bit_num % ((block + 1) * data.m_block_size);
                while (tmpBlockPos <= lastBits)
                {
                    os << ((data.m_block_vector[block] & (static_cast<myDynamicBitset::m_block_type>(1) << tmpBlockPos)) >> tmpBlockPos++) << ' ';
                }
                tmpBlockPos = 0;
            }
        }
        return os;
    }
};

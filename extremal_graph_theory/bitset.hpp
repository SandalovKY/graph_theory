#pragma once

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <set>
#include <omp.h>

#ifdef _WIN32
#include <intrin.h>
#endif

using block_type = unsigned long long;
namespace
{
    static const int64_t npos = -1;
}

template <typename BlockType = block_type>
class myBitset
{
public:
    using m_block_type = BlockType;
    using m_block_size_type = unsigned long;
    using m_bit_position_type = int32_t;
    using m_block_iterator_type = typename std::vector<m_block_type>::iterator;
    using m_block_const_iterator_type = typename std::vector<m_block_type>::const_iterator;
private:
    static const int64_t min_num_blocks = 100;

    m_bit_position_type m_block_num{};
    m_bit_position_type m_last_set_bit_num{}; 
    m_block_size_type m_block_size{};
    std::vector<m_block_type> m_block_vector{};
    size_t m_dimSize{};

    static bool inline m_block_not_empty(const m_block_type& block)
    {
        return block != m_block_type{};
    }

    static m_block_size_type inline getLowestBit(const m_block_type& block)
    {
#ifdef _WIN32
        m_block_size_type trailingZero{ 0 };
        _BitScanForward64(&trailingZero, block);
        return trailingZero;
#else
        return __builtin_ctzll(block);
#endif
    }

public:
    myBitset()
        : m_block_size(sizeof(m_block_type) * 8),
          m_last_set_bit_num(-1),
          m_block_num(0),
          m_block_vector(min_num_blocks, 0)
    {}

    myBitset(m_bit_position_type numBits)
        : m_block_size(sizeof(m_block_type) * 8),
          m_last_set_bit_num(numBits - 1),
          m_block_num(numBits / (sizeof(m_block_type) * 8) + 1),
          m_block_vector(m_block_num, 0),
          m_dimSize(numBits)
    {}

    myBitset(myBitset&& other)
        : m_block_vector(std::move(other.m_block_vector)),
          m_last_set_bit_num(other.m_last_set_bit_num),
          m_block_num(other.m_block_num),
          m_block_size(other.m_block_size),
          m_dimSize(other.m_dimSize)
    {
        other.m_last_set_bit_num = -1;
        other.m_block_num = 0;
        other.m_block_size = 0;
    }

    myBitset(const myBitset& other)
        : m_block_vector(other.m_block_vector),
          m_last_set_bit_num(other.m_last_set_bit_num),
          m_block_num(other.m_block_num),
          m_block_size(other.m_block_size),
          m_dimSize(other.m_dimSize)
    {}

    size_t getDimSize() const { return m_dimSize; }

    void operator=(myBitset&& other)
    {
        m_block_vector = std::move(other.m_block_vector);

        m_last_set_bit_num = other.m_last_set_bit_num;
        m_block_num = other.m_block_num;
        m_block_size = other.m_block_size;
        m_dimSize = other.m_dimSize;

        other.m_last_set_bit_num = -1;
        other.m_block_num = 0;
        other.m_block_size = 0;
        other.m_dimSize = 0;
    }

    void operator=(const myBitset& other)
    {
        m_block_vector = other.m_block_vector;

        m_last_set_bit_num = other.m_last_set_bit_num;
        m_block_num = other.m_block_num;
        m_block_size = other.m_block_size;
        m_dimSize = other.m_dimSize;
    }

    friend bool operator==(const myBitset& l, const myBitset& r)
    {
        if (l.m_last_set_bit_num != r.m_last_set_bit_num ||
            l.m_block_num != r.m_block_num) return false;
        for (size_t ind = 0; ind < l.m_block_num; ++ind)
        {
            if (l.m_block_vector[ind] != r.m_block_vector[ind]) return false;
        }
        return true;
    }

    bool isEmpty()
    {
        bool isEmpty{ true };
        size_t numBlocks{ m_block_vector.size() };
        size_t ind{ 0 };
        while (isEmpty && ind < numBlocks)
        {
            isEmpty &= m_block_vector[ind] == 0;
        }
        return isEmpty;
    }

    void all2one()
    {
        // #pragma omp parallel for
        for (int64_t ind = 0; ind < m_block_num; ++ind)
        {
            m_block_vector[ind] = -1;
        }
    }
    void all2zero()
    {
        // #pragma omp parallel for
        for (int64_t ind = 0; ind < m_block_num; ++ind)
        {
            m_block_vector[ind] = 0;
        }
    }

    m_bit_position_type numBlocks() const
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

    bool isBitSet(m_bit_position_type bitPos)
    {
        if (m_last_set_bit_num < bitPos) return false;
        const m_bit_position_type block_num = bitPos / m_block_size;
        const m_block_size_type block_offset = bitPos % m_block_size;

        if (block_num >= m_block_vector.size()) return false;
        return (m_block_vector[block_num] & (m_block_type{ 1 } << block_offset)) != 0;
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
        myBitset tmp = *this;
        size_t adjVert = tmp.getFirstNonZeroPosition();
        while(adjVert <= tmp.m_last_set_bit_num)
        {
            ret.insert(adjVert);
            tmp.unset(adjVert);
            adjVert = tmp.getFirstNonZeroPosition();
        }
        return ret;
    }
    
    friend size_t countSetBits(const myBitset& bitset)
    {
        size_t count{ 0 };
        // #pragma omp parallel for reduction(+:count)
        for (int64_t block = 0; block < bitset.m_block_num; ++block)
        {
#ifdef _WIN32
            count += __popcnt64(bitset.m_block_vector[block]);
#else
            count += __builtin_popcountll(bitset.m_block_vector[block]);
#endif
        }
        return count;
    }

    m_bit_position_type getFirstNonZeroPosition() const
    {
        m_bit_position_type i = std::distance(m_block_vector.begin(), std::find_if(m_block_vector.begin(), m_block_vector.end(), m_block_not_empty));
        if (i >= numBlocks())
        {
            return npos;
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

    myBitset& operator&=(const myBitset& other)
    {
        if (this->m_block_num != other.m_block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        // #pragma omp parallel for
        for(int64_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] &= other.m_block_vector[block];
        }
        return *this;
    }

    friend myBitset operator&(myBitset& lhs, const myBitset& rhs)
    {
        myBitset tmp{ lhs };
        tmp &= rhs;
        return tmp;
    }

    myBitset& operator|=(const myBitset& other)
    {
        if (this->m_block_num != other.m_block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        // #pragma omp parallel for
        for(int64_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] |= other.m_block_vector[block];
        }
        return *this;
    }

    friend myBitset operator|(myBitset& lhs, const myBitset& rhs)
    {
        myBitset tmp{ lhs };
        tmp |= rhs;
        return tmp;
    }

    myBitset& operator^=(const myBitset& other)
    {
        if (this->m_block_num != other.m_block_num) throw std::runtime_error("Cannot to operate bitsets with different block nums.");
        // #pragma omp parallel for
        for(int64_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] ^= other.m_block_vector[block];
        }
        return *this;
    }

    friend myBitset operator^(myBitset& lhs, const myBitset& rhs)
    {
        myBitset tmp{ lhs };
        tmp ^= rhs;
        return tmp;
    }

    myBitset& operator~()
    {
        // #pragma omp parallel for
        for(int64_t block = 0; block < this->m_block_num; ++block)
        {
            this->m_block_vector[block] = ~(this->m_block_vector[block]);
        }
        return *this;
    }

    ~myBitset()
    {}

    friend std::ostream& operator<<(std::ostream& os, const myBitset& data)
    {
        myBitset::m_block_type tmpBlockPos{0};
        for (size_t block = 0; block < data.m_block_num; ++block)
        {
            if (data.m_last_set_bit_num / ((block + 1) * data.m_block_size) > 0)
            {
                while (tmpBlockPos < data.m_block_size)
                {
                    os << ((data.m_block_vector[block] & (static_cast<myBitset::m_block_type>(1) << tmpBlockPos)) >> tmpBlockPos++) << ' ';
                }
                tmpBlockPos = 0;
            }
            else
            {
                size_t lastBits = data.m_last_set_bit_num % ((block + 1) * data.m_block_size);
                while (tmpBlockPos <= lastBits)
                {
                    os << ((data.m_block_vector[block] & (static_cast<myBitset::m_block_type>(1) << tmpBlockPos)) >> tmpBlockPos++) << ' ';
                }
                tmpBlockPos = 0;
            }
        }
        return os;
    }
};

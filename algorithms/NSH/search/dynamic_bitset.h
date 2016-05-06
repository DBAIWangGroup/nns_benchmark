#include <limits.h>

class DynamicBitset
{
public:
    /** @param only constructor we use in our code
     * @param the size of the bitset (in bits)
     */
    DynamicBitset(size_t size)
    {
        resize(size);
        reset();
    }

    /** Sets all the bits to 0
     */
    void clear()
    {
        std::fill(bitset_.begin(), bitset_.end(), 0);
    }

    void reset()
    {
        std::fill(bitset_.begin(), bitset_.end(), 0);
    }

    
    void resize(size_t size)
    {
        size_ = size;
        bitset_.resize(size / cell_bit_size_ + 1);
    }
    size_t size() const
    {
        return size_;
    }

	void set(size_t index)
    {
        bitset_[index / cell_bit_size_] |= size_t(1) << (index % cell_bit_size_);
    }

    /** @param check if a bit is set
     * @param index the index of the bit to check
     * @return true if the bit is set
     */
    bool test(size_t index) const
    {
        return (bitset_[index / cell_bit_size_] & (size_t(1) << (index % cell_bit_size_))) != 0;
    }

private:
    std::vector<size_t> bitset_;
    size_t size_;
    static const unsigned int cell_bit_size_ = CHAR_BIT * sizeof(size_t);
};

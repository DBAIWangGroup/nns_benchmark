#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <vector>
/**
 * Seeds the random number generator
 *  @param seed Random seed
 */
inline void seed_random(unsigned int seed)
{
    srand(seed);
}

/*
 * Generates a random double value.
 */
/**
 * Generates a random double value.
 * @param high Upper limit
 * @param low Lower limit
 * @return Random double value
 */
inline double rand_double(double high = 1.0, double low = 0)
{
    return low + ((high-low) * (std::rand() / (RAND_MAX + 1.0)));
}

/**
 * Generates a random integer value.
 * @param high Upper limit
 * @param low Lower limit
 * @return Random integer value
 */
inline int rand_int(int high = RAND_MAX, int low = 0)
{
    return low + (int) ( double(high-low) * (std::rand() / (RAND_MAX + 1.0)));
}


class RandomGenerator
{
public:
    ptrdiff_t operator() (ptrdiff_t i) { return rand_int(i); }
};


/**
 * Random number generator that returns a distinct number from
 * the [0,n) interval each time.
 */
class UniqueRandom
{
    std::vector<int> vals_;
    int size_;
    int counter_;

public:
    /**
     * Constructor.
     * @param n Size of the interval from which to generate
     * @return
     */
    UniqueRandom(int n)
    {
        init(n);
    }

    /**
     * Initializes the number generator.
     * @param n the size of the interval from which to generate random numbers.
     */
    void init(int n)
    {
        static RandomGenerator generator;
        // create and initialize an array of size n
        vals_.resize(n);
        size_ = n;
        for (int i = 0; i < size_; ++i) vals_[i] = i;

        // shuffle the elements in the array
        std::random_shuffle(vals_.begin(), vals_.end(), generator);

        counter_ = 0;
    }

    /**
     * Return a distinct random integer in greater or equal to 0 and less
     * than 'n' on each call. It should be called maximum 'n' times.
     * Returns: a random integer
     */
    int next()
    {
        if (counter_ == size_) {
            return -1;
        }
        else {
            return vals_[counter_++];
        }
    }
};




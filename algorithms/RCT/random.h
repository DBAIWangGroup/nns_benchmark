#ifndef __MTRAND_H
#define __MTRAND_H

class Random {

private:
        //! The size of the state vector.
        static const int n = 624;

        //! A magic number.
        static const int m = 397;

        //! The generator's state vector.
        static unsigned long state[n];

        //! The position in the state vector.
        static int p;

        //! Was the generator intialized?
        static bool init;

        //! Bit twiddling used by the generator.
        inline unsigned long twiddle(const unsigned long& u,
                const unsigned long& v) const {
                return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1)
                                ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
        }

        //! Generate the next state vector.
        void gen_state();

protected:
        //! Hide the copy constructor.
        Random(const Random&) {}

        //! Hide the assignment operator.
        Random& operator=(const Random&) {}

public:
        //! Default constructor.
        Random();

        //! Seed the generator.
        Random(const unsigned long& s);

        //! Seed with an array of values.
        Random(const unsigned long* array, const unsigned long& size);

        //! Seed the generator from an integer.
        Random& seed(const unsigned long& s);

        //! Seed the generator from an array of integers.
        Random& seed(const unsigned long* array, const unsigned long& size);

        //! Make the generator a functor.
        double operator()();

        //! The destructor.
        virtual ~Random() {}

        //! Generate a 32-bit random integer.
        unsigned long integer();

} /* .. */;

#endif /* __MTRAND_H */

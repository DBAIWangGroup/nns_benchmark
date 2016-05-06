#include "random.h"

int		Random::p		= 0;
bool            Random::init	= false;
unsigned long	Random::state[Random::n];

Random::Random() {
    if (!init) {
        seed(5489UL);
    }
    init = true;
}

Random::Random(const unsigned long& s) {
   seed(s);
   init = true;
}

Random::Random(const unsigned long* array, const unsigned long& size) {
    seed(array, size);
    init = true;
}

double Random::operator()() {
    return integer() * (1.0 / 4294967295.0); 
}

unsigned long Random::integer() {
    if (p == n) {
        gen_state();
    }
    unsigned long x = state[p++];
    x ^= (x >> 11);
    x ^= (x << 7) & 0x9D2C5680UL;
    x ^= (x << 15) & 0xEFC60000UL;
    return x ^ (x >> 18);
}

void Random::gen_state() {
    for (int i = 0; i < (n - m); ++i)
        state[i] = state[i + m] ^ twiddle(state[i], state[i + 1]);
    for (int i = n - m; i < (n - 1); ++i)
        state[i] = state[i + m - n] ^ twiddle(state[i], state[i + 1]);
    state[n - 1] = state[m - 1] ^ twiddle(state[n - 1], state[0]);
    p = 0;
}

Random& Random::seed(const unsigned long& s) {
    state[0] = s & 0xFFFFFFFFUL;
    for (int i = 1; i < n; ++i) {
        state[i] = 1812433253UL * (state[i - 1] ^ (state[i - 1] >> 30)) + i;
        state[i] &= 0xFFFFFFFFUL;
    }
    p = n;
    return *this;
}

Random& Random::seed(const unsigned long* array, const unsigned long& size) {
    seed(19650218UL);
    int i = 1, j = 0;
    for (int k = ((n > size) ? n : size); k; --k) {
        state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1664525UL))
                + array[j] + j;
        state[i] &= 0xFFFFFFFFUL;
        ++j;
        j %= size;
        if ((++i) == n) {
            state[0] = state[n - 1];
            i = 1;
        }
    }
    for (int k = n - 1; k; --k) {
        state[i] = (state[i] ^ ((state[i - 1] ^ (state[i - 1] >> 30)) * 1566083941UL)) - i;
        state[i] &= 0xFFFFFFFFUL;
        if ((++i) == n) {
            state[0] = state[n - 1];
            i = 1;
        }
    }
    state[0] = 0x80000000UL;
    p = n;
    return *this;
}

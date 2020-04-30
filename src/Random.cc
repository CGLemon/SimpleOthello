#include "Random.h"

thread_local std::uint64_t Random::m_s[2];

void Random::print_seed() {
	printf("seed : %lu | %lu \n",m_s[0] , m_s[1]);
}


std::uint64_t splitmix64(std::uint64_t z) {
    
    /*
     The parameter detail are from
     https://github.com/lemire/testingRNG/blob/master/source/splitmix64.h
     */

    z += 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}


Random& Random::get_Rng() {
    static thread_local Random s_rng{0};
    return s_rng;
}

Random::Random(std::uint64_t seed) {
    if (seed == 0) {
        size_t thread_id = std::hash<std::thread::id>()(std::this_thread::get_id());
        seed_init(std::uint64_t(thread_id));
    } else {
        seed_init(seed);
    }
}


void Random::seed_init(std::uint64_t default_seed) {
    m_s[0] = splitmix64(default_seed);
    m_s[1] = splitmix64(m_s[0]);
}


std::uint64_t Random::rotl(const uint64_t x, const int k) {
  	return (x << k) | (x >> (64 - k));
}

std::uint64_t Random::gen() {
    /*
    The parameter detail are from
    https://github.com/lemire/testingRNG/blob/master/source/Random.h
    */
    
    const std::uint64_t s0 = m_s[0];
    std::uint64_t s1       = m_s[1];
    const uint64_t result = s0 + s1;

    s1 ^= s0;
    m_s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
    m_s[1] = rotl(s1, 36);

    return result;
}

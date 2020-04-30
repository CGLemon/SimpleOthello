#ifndef RANDON_H_INCLUDE
#define RANDON_H_INCLUDE


#include <cstdint>
#include <limits>
#include <time.h>
#include <random>
#include <thread>

/*
 https://github.com/lemire/testingRNG
 */



class Random {
    
public:
    
    Random() = delete;
    Random(std::uint64_t default_seed = 0);
    
    static Random& get_Rng();
    
    void seed_init(std::uint64_t);
    
    std::uint64_t rotl(const std::uint64_t ,int);
    
    
    std::uint64_t randuint64() {
        return gen();
    }
    
	void print_seed();
    
    using result_type = std::uint64_t;
    constexpr static result_type min() {
        return std::numeric_limits<result_type>::min();
    }
    constexpr static result_type max() {
        return std::numeric_limits<result_type>::max();
    }
    result_type operator()() {
        return gen();
    }
    
private:
    std::uint64_t gen();
    static thread_local std::uint64_t m_s[2];
};




#endif

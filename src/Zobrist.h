
#ifndef ZOBRIST_H_INCLUDE
#define ZOBRIST_H_INCLUDE

#include <random>
#include <array>
#include <vector>

#include "Random.h"
#include "config.h"



class Zobrist {
    
public :

    static constexpr std::uint64_t zobrist_empty 		= 0x1234567887654321;
    static constexpr std::uint64_t zobrist_blacktomove 	= 0xABCDABCDABCDABCD;

    static std::array<std::array<std::uint64_t, NUM_VERTICES>, 4>  zobrist;
    /* Black , White , EMPTY , INVAL*/
    
    static void init_zobrist();
};


#endif

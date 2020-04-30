
#include "Zobrist.h"
#include "config.h"

std::array< std::array< std::uint64_t, NUMVERTICS > , 4 >   Zobrist::zobrist;




void Zobrist::init_zobrist() {
    
    auto rng = Random::get_Rng();

	for (int i = 0; i < 4; i++) {
	    for (int j = 0; j < NUMVERTICS ; j++) {
	        Zobrist::zobrist[i][j] = rng.randuint64();
	    }
	}

}



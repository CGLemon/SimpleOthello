#ifndef EVALUATION_H_INCLUDE
#define EVALUATION_H_INCLUDE

#include <vector>
#include <array>

#include "config.h"
#include "Board.h"
#include "CacheTable.h"
#include "GameState.h"

class Evaluation {
public:

	using Eval = TAResult; 

	static Eval get_score(const Board & board);
	static void get_tablescore(const Board & board, Evaluation::Eval & eval, 
								const std::array<int, NUM_INTERSECTIONS> & table);

private:
	static CacheTable<Eval> m_cache;
	static const std::array<int, NUM_INTERSECTIONS> m_evaltable;
	//static const std::array<int, NUM_INTERSECTIONS> m_evalstabletable;
	static constexpr int winner_value = 9999;
	static constexpr int loser_value = -9999;
	static constexpr int draw_value = 0;
	static constexpr int onelegalvale = 10;
};




#endif

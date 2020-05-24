#include "Evaluation.h"
#include "Board.h"
#include "GameState.h"
#include "Random.h"
#include "CacheTable.h"

#include <memory>
#include <cstdint>
#include <cassert>

CacheTable<Evaluation::Eval> Evaluation::m_cache;

constexpr int Evaluation::winner_value;
constexpr int Evaluation::loser_value;
constexpr int Evaluation::draw_value;

const std::array<int, NUM_INTERSECTIONS> Evaluation::m_evaltable = {
	120, -20,   20,  5,     5,  20, -20, 120,
	-20, -40,  -5,  -5,    -5,  -5, -40, -20,
	 20,  -5,  15,   4,     4,  15,  -5,  20,
	  5,  -5,   4,   3,     3,   4 , -5,   5,

	  5,  -5,   4,   3,     3,   4 , -5,   5,
	 20,  -5,  15,   4,     4,  15,  -5,  20,
	-20, -40,  -5,  -5,    -5,  -5, -40, -20,
	120, -20,   20,  5,     5,  20, -20, 120
};


/*
const std::array<int, NUM_INTERSECTIONS> Evaluation::m_evaltable = {
	120, -20,  20,    20, -20, 120,
	-20, -40,   5,     5, -40, -20,
	 20,   5,   0,     0,   5,  20,

	 20,   5,   0,     0,   5,  20,
	-20, -40,   5,     5, -40, -20,
	120, -20,  20,    20, -20, 120
};
*/
constexpr int Evaluation::onelegalvale;


void Evaluation::get_tablescore(const Board & board, Evaluation::Eval & eval,
									const std::array<int, NUM_INTERSECTIONS> & table) {

	eval.blackscore = 0;
	eval.whitescore = 0;
	const int boardsize = board.get_boardsize();
	const int color = board.get_to_move(); 
	assert(board.get_boardsize() == 8);

	for (int y = 0; y < boardsize; ++y) {
		for (int x = 0; x < boardsize; ++x) {
			const auto vtx = board.get_vertex(x, y);
			const auto idx = board.get_index(x, y);
			if (board.get_state(vtx) == Board::BLACK) {
				eval.blackscore += table[idx];
			} else if (board.get_state(vtx) == Board::WHITE) {
				eval.whitescore += table[idx];
			}
		}
	}
	if (color == Board::BLACK) {
		eval.blackscore += onelegalvale * board.get_numlegalmove(color);
	} else {
		eval.whitescore += onelegalvale * board.get_numlegalmove(color);
	}
}

Evaluation::Eval Evaluation::get_score(const Board & board) {
	assert(board.get_boardsize() == 8);

	auto eval = Eval{};
	for (int sym = 0; sym < Board::NUM_SYMMETRIES ; sym++){
		if(m_cache.lookup(board.calc_hash(sym), eval)){
			return eval;
		}
	}

	if (board.is_gameover()) {
		const auto winner = board.get_winner();
		if (winner == Board::BLACK) {
			eval.blackscore = winner_value;
			eval.whitescore = loser_value;
		} else if (winner == Board::WHITE) {
			eval.blackscore = loser_value;
			eval.whitescore = winner_value;
		} else if (winner == Board::EMPTY){
			eval.blackscore = draw_value;
			eval.whitescore = draw_value;
		}
	} else {
		get_tablescore(board, eval, m_evaltable);
	}
	m_cache.insert(board.get_hash(), eval);

	return eval;
}



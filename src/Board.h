#ifndef BOARD_H_INCLUDE
#define BOARD_H_INCLUDE

#include <array>
#include <cstdint>
#include <cassert>

#include "Zobrist.h"
#include "config.h"



class Board {
public:

	enum vertex_t : char {
 		BLACK = 0, WHITE = 1, EMPTY = 2, INVAL = 3 
	};
	static constexpr int NOVERTEX       = -1;
	static constexpr int PASS           = 0;
	static constexpr int NUM_SYMMETRIES = 8;
	static constexpr int IDENTITY_SYMMETRY = 0;
	static std::array<int,8> m_dirs;
	static std::array<std::array<int ,INTERSECTION>, NUM_SYMMETRIES> m_symmetry_idx_table;

	int get_vertex(const int x, const int y) const;
	int get_index(const int x, const int y) const;
	int idx_to_vtx(const int idx) const;
	std::pair<int, int> get_symmetry(const int x, const int y,
										 const int symmetry);

	void reset_board(int boardsize);
	void init_symmetry();
	void filp_board(const int symmetry);

	void fix_board();
	void set_to_move(const int);
	void exchange_to_move();
	void update_state(const int color, const int vtx);
	void update_stable(const int vtx);

	bool is_gameover() const;
	bool is_legal(const int color, const int vtx) const;
	bool exsit_moves(const int color) const;	
	void reseve(const int color, const int vtx);
	void play_move(const int color, const int vtx);
	void play_move(const int vtx);	

	void area_count();

	void text_display(const int mark) const;
	void print_type(vertex_t) const;
	void print_row(const int count) const;
	void print_hash() const;
	void print_to_move() const;
	void print_stones() const;

	std::uint64_t calc_hash(const int symmetry = IDENTITY_SYMMETRY) const;
	
	int get_last_move() const;
	int get_to_move() const;
	std::uint64_t get_hash() const;
	int get_winner() const;
	int get_state(const int x, const int y) const;
	int get_state(const int vtx) const;
	bool get_stable(const int vtx) const;
	int get_boardsize() const;
	int get_numlegalmove(const int color) const;

private :
	int m_quiet;
	int m_tomove;
	int m_lastmove;	
	std::uint64_t m_hash;

	std::array<vertex_t ,NUMVERTICS> m_state;
	//std::array<bool ,NUMVERTICS> m_stable;

	int m_boardsize;
	int m_numvertics;
	int m_eboardsize;
	int m_intersection;

	int m_black_stones;
	int m_white_stones;
};

inline void Board::update_state(const int color, const int vtx){
	assert(color == EMPTY || color == BLACK || color == WHITE || color == INVAL);
	const int get_color = m_state[vtx];
	m_state[vtx] = static_cast<vertex_t>(color);

	m_hash ^= Zobrist::zobrist[get_color][vtx];
	m_hash ^= Zobrist::zobrist[    color][vtx];
}

inline int Board::get_vertex(const int x, const int y) const{
	assert(x >= 0 && x < m_boardsize);
	assert(y >= 0 && y < m_boardsize);
	return (x+1)+m_eboardsize*(y+1);
}

inline int Board::get_index(const int x, const int y) const {
	assert(x >= 0 && x < m_boardsize);
	assert(y >= 0 && y < m_boardsize);
	return x + m_boardsize*y;
}

inline int Board::idx_to_vtx(const int idx) const {
	assert(idx >= 0 && idx < m_intersection);
	const int x = idx % m_boardsize;
	const int y = idx / m_boardsize;
	return get_vertex(x, y);
}

inline int Board::get_state(const int x, const int y) const {
	return m_state[get_vertex(x, y)];
}

inline int Board::get_state(const int vtx) const {
	return m_state[vtx];
}

#endif

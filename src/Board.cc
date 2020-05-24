#include "Board.h"
#include "Zobrist.h"
#include "config.h"
#include "Utils.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <queue>

std::array<int,8> Board::m_dirs;
constexpr int Board::NUM_SYMMETRIES;
constexpr int Board::IDENTITY_SYMMETRY;
constexpr int Board::PASS;
constexpr int Board::NOVERTEX;
std::array<std::array<int ,NUM_INTERSECTIONS>, Board::NUM_SYMMETRIES> Board::m_symmetry_idx_table;

std::pair<int, int> Board::get_symmetry(const int x, const int y,
										const int symmetry) {
  
    assert(x >= 0 && x < m_boardsize);
    assert(y >= 0 && y < m_boardsize);
    assert(symmetry >= 0 && symmetry < NUM_SYMMETRIES);

	int idx_x = x;
	int idx_y = y;

    if ((symmetry & 4) != 0) {
        std::swap(idx_x, idx_y);
    }

    if ((symmetry & 2) != 0) {
        idx_x = m_boardsize - idx_x - 1;
    }

    if ((symmetry & 1) != 0) {
        idx_y = m_boardsize - idx_y - 1;
    }

    assert(idx_x >= 0 && idx_x < m_boardsize);
    assert(idx_y >= 0 && idx_y < m_boardsize);
    assert(symmetry != IDENTITY_SYMMETRY || (x==idx_x && y==idx_y));

    return {idx_x, idx_y};
}


void Board::reset_board(int boardsize){
	if (boardsize >  BOARD_SIZE) {
		boardsize = BOARD_SIZE;
	} else if (boardsize <= 0){
		boardsize = BOARD_SIZE;
	}
	m_boardsize  = boardsize;
	m_eboardsize = boardsize+1; 	
	m_numvertices = m_eboardsize *m_eboardsize;
	m_intersections = m_boardsize * m_boardsize;
	m_tomove = BLACK;
	m_lastmove = NOVERTEX;	
	m_black_stones = 0;
	m_white_stones = 0; 
	m_quiet = false;

	m_dirs[0] = -1				;m_dirs[4] = -m_eboardsize-1;
	m_dirs[1] =  1				;m_dirs[5] =  m_eboardsize+1;
	m_dirs[2] = -m_eboardsize	;m_dirs[6] =  m_eboardsize-1;
	m_dirs[3] =  m_eboardsize	;m_dirs[7] = -m_eboardsize+1;


	for (auto i = 0 ; i < NUM_VERTICES ; ++i){
		m_state[i] 	= INVAL;
	}


	for (auto y = 0 ; y < m_boardsize ; ++y){
		for (auto x = 0; x < m_boardsize ; ++x){
			const int vertex = get_vertex(x,y);
			m_state[vertex] = EMPTY;
		}
	}

	fix_board();
	init_symmetry();
	area_count();
	m_hash = calc_hash();
}


void Board::fix_board(){

	const int part_center = m_boardsize / 2;
	
	m_state[get_vertex(part_center,part_center)] = WHITE;
	m_state[get_vertex(part_center-1,part_center-1)] = WHITE;
	m_state[get_vertex(part_center-1,part_center)] = BLACK;
	m_state[get_vertex(part_center,part_center-1)] = BLACK;
}

void Board::init_symmetry() {
	for (int sym = 0; sym < NUM_SYMMETRIES; ++sym) {
		for (int idx = 0; idx < m_intersections; ++idx) {
			const int x = idx % m_boardsize;
			const int y = idx / m_boardsize;
			const auto sym_idx = get_symmetry(x, y, sym);
			m_symmetry_idx_table[sym][idx] = get_index(sym_idx.first, sym_idx.second);
		}
	}
}

void Board::filp_board(const int symmetry) {
	const auto tmp_state = m_state;
	for (auto y = 0 ; y < m_boardsize ; ++y){
		for (auto x = 0; x < m_boardsize ; ++x){
			const int vtx = get_vertex(x, y);
			const int idx = get_index(x, y);
			const int sym_idx = m_symmetry_idx_table[symmetry][idx];
			const int sym_vtx = idx_to_vtx(sym_idx);
			m_state[vtx] = tmp_state[sym_vtx];
		}
	}
	m_hash = calc_hash();
}

void Board::set_to_move(const int color){
	assert(color == BLACK || color == WHITE);
	if (color != m_tomove) {
		m_hash ^= Zobrist::zobrist_blacktomove;
	}
	m_tomove = color;
}

void Board::exchange_to_move(){
	m_hash ^= Zobrist::zobrist_blacktomove;
	m_tomove = (!m_tomove);
}

bool Board::is_gameover() const {
	std::vector<int> empty_vertex;
	for (auto y = 0 ; y < m_boardsize ; ++y){
		for (auto x = 0; x < m_boardsize ; ++x){
			const int vtx = get_vertex(x, y);
			if (m_state[vtx] == EMPTY) {
				empty_vertex.emplace_back(vtx);
			}
		}
	}

	if (empty_vertex.empty()) {
		return true;
	}

	for (auto & vtx: empty_vertex) {
		if (is_legal(BLACK, vtx) || is_legal(WHITE, vtx)) {
			return false;
		}
	}

	return true;
}


void Board::update_stable(const int vtx) {
	
}


bool Board::is_legal(const int color, const int vtx) const{
		
	const int opp_color = !color;
	if(m_state[vtx] == EMPTY){	
		for(auto k = 0; k < 8; ++k){
			int avtx = vtx;
			do {
				avtx += m_dirs[k];
			} while (m_state[avtx] == opp_color);
			
			if (avtx != (vtx + m_dirs[k]) && m_state[avtx] == color) {
				return true;
			}
		}
	}
	return false;
}

bool Board::exsit_moves(const int color) const{
	for(auto vtx = 0 ; vtx < m_numvertices; ++vtx){
		if(m_state[vtx] == EMPTY){
			if (is_legal(color, vtx)) {
				return true;
			}
		}
	}
	return false;
}


void Board::reseve(const int color, const int vtx){

	const int opp_color = !color;
	std::vector<int> update_vtx;;

	for(auto k = 0; k < 8; ++k){
		int avtx = vtx;
		int res = 0;
		
		do {
			avtx += m_dirs[k];
			res ++;
		} while(m_state[avtx] == opp_color);
		
		if(res > 1 && m_state[avtx] == color){
			for (int i = 0 ; i < (res-1); ++i){
				avtx -= m_dirs[k];
				update_state(color, avtx);
				update_vtx.emplace_back(avtx);
			}
		}
	}
}


void Board::play_move(const int vtx){
	play_move(m_tomove, vtx);
}

void Board::play_move(const int color, const int vtx){
	
	if (vtx != Board::PASS) {
		if (m_state[vtx] != EMPTY) {
			Utils::myprintf("vtx : %d \n", vtx);
		}
		assert(m_state[vtx] == EMPTY);
		update_state(color, vtx);
		reseve(color, vtx);
		area_count();
	}
	m_lastmove = vtx;
}


void Board::area_count() {
	int black_area = 0;
	int white_area = 0;
	for (auto i = 0 ; i < m_numvertices ; ++i){
		if(m_state[i] == BLACK){	
			black_area++;
		}else if(m_state[i] == WHITE){
			white_area++;
		}
	} 
	m_black_stones = black_area;
	m_white_stones = white_area; 
}	


void Board::print_type(Board::vertex_t vtx_t) const{
	vtx_t == BLACK ? Utils::myprintf("X") : 
	vtx_t == WHITE ? Utils::myprintf("O") :
	vtx_t == EMPTY ? Utils::myprintf(" ") : Utils::myprintf("error");
}

void Board::print_row(const int count) const {
	assert(count >= 1);
	for (int x = 0; x < count; ++x) {
		if (x == 0) {
			Utils::myprintf("+");
		}
		Utils::myprintf("---+");
	}
	Utils::myprintf("\n");
}

void Board::text_display(const int mark) const{
	if (!m_quiet) {
		Utils::myprintf("   ");
		for (char c = 65; c < 65 + m_boardsize; ++c) {Utils::myprintf("  %c ", c);}
		Utils::myprintf("\n");
		for (auto y = 0 ; y < m_boardsize ; ++y){
			Utils::myprintf("   ");
			print_row(m_boardsize);
			if (y+1 < 10) {
				Utils::myprintf(" %d ",y+1);
			} else {
				Utils::myprintf("%d ",y+1);
			}
			Utils::myprintf("|");
			for(auto x = 0; x < m_boardsize ; ++x){
				const int vertex = get_vertex(x,y);
				mark == vertex ? Utils::myprintf("(") : Utils::myprintf(" ");
				if(is_legal(m_tomove, vertex)){
					Utils::myprintf(".");
				}else{
					print_type(m_state[vertex]);
				}
				mark == vertex ? Utils::myprintf(")") : Utils::myprintf(" ");
				Utils::myprintf("|");
			}
			Utils::myprintf("%d",y+1);
			Utils::myprintf("\n");
		}
		Utils::myprintf("   ");
		print_row(m_boardsize);
		Utils::myprintf("   ");
		for (char c = 65; c < 65 + m_boardsize; ++c) {Utils::myprintf("  %c ", c);}
		Utils::myprintf("\n");
		Utils::myprintf("\n");
	}
}
void Board::print_to_move() const {
	m_tomove == BLACK ? Utils::myprintf("black move \n") :
	m_tomove == WHITE ? Utils::myprintf("white move \n") : Utils::myprintf("error \n");
}

void Board::print_hash() const{
	Utils::myprintf("HASH 	     	: %lx \n", m_hash);
	//Utils::myprintf("FILE C  HASH 	: %lx \n", calc_hash(4));
	//Utils::myprintf("FILE XY HASH 	: %lx \n", calc_hash(3));
	//Utils::myprintf("FILE C XY HASH 	: %lx \n", calc_hash(7));
}

void Board::print_stones() const {
	Utils::myprintf("black stones : %d | white stones : %d\n", m_black_stones, m_white_stones);
}

std::uint64_t Board::calc_hash(const int symmetry) const {

	std::uint64_t res = Zobrist::zobrist_empty;
	for (auto y = 0 ; y < m_boardsize ; ++y){
		for (auto x = 0; x < m_boardsize ; ++x){
			const int vtx = get_vertex(x, y);
			const int idx = get_index(x, y);
			const int sym_idx = m_symmetry_idx_table[symmetry][idx];
			const int sym_vtx = idx_to_vtx(sym_idx);
			res ^= Zobrist::zobrist[m_state[vtx]][sym_vtx];
		}
	}
	if (m_tomove == BLACK) {
		res ^= Zobrist::zobrist_blacktomove;
	}

	return res;
}
int Board::get_last_move() const {
	return m_lastmove;
}

int Board::get_to_move() const {
	return m_tomove;
}

std::uint64_t Board::get_hash() const {
	return m_hash;
}

int Board::get_winner() const {
	int winner = EMPTY;
	m_black_stones > m_white_stones ? winner = BLACK :
	m_black_stones < m_white_stones ? winner = WHITE :
									  winner = EMPTY;
	return winner;
}

int Board::get_boardsize() const {
	return m_boardsize;
}

bool Board::get_stable(const int vtx) const {
	//return m_stable[vtx];
	return false;
}

int Board::get_numlegalmove(const int color) const {
	int num = 0;
	for (auto y = 0 ; y < m_boardsize ; ++y){
		for (auto x = 0; x < m_boardsize ; ++x){
			const int vtx = get_vertex(x, y);
			if (m_state[vtx] == EMPTY) {
				if (is_legal(color, vtx)) {
					num++;
				}
			}
		}
	}
	return num;
}

std::pair<int ,int> Board::get_stones() const {
	return {m_black_stones, m_white_stones};
}

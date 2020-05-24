#include "GameState.h"

#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <sstream>

GameState::GameState(){
	init_game(BOARD_SIZE);
}


void GameState::init_game(int boardsize) {
	board.reset_board(boardsize);
	game_history.clear();
	game_history.emplace_back(std::make_shared<Board>(board));
	m_movenum = 0;
	m_gameover = false;
}


void GameState::display() const {
	board.text_display(board.get_last_move());
	board.print_stones();
	board.print_hash();
	board.print_to_move();
}

bool GameState::undo_move() {
	if (m_gameover) {m_gameover = false;}

	assert(m_movenum == game_history.size()-1);
	if (m_movenum == 0) {
		return false;
	}
	
	m_movenum--;
	board = *game_history[m_movenum];
	game_history.pop_back();

	return true;
}

bool GameState::play_move(const int color, const int vtx){
	if (m_gameover) {return false;}
	if (color != Board::WHITE && color != Board::BLACK) {
		return false;
	}

	if (vtx == Board::PASS && board.exsit_moves(color)) {
		return false;
	}

	if(vtx != Board::PASS && !board.is_legal(color, vtx)) {
		return false;
	}
	
	board.set_to_move(color);
	board.play_move(vtx);
	game_history.emplace_back(std::make_shared<Board>(board));
	m_movenum++;
	assert(m_movenum == game_history.size()-1);

	return true;
}


bool GameState::text_play_move(std::string &input){
	if (m_gameover) {return false;}
	std::stringstream txt_move_string(input);
	auto cmd = std::string{};
	auto cmd_count = size_t{0};
	auto color = Board::INVAL;
	auto vtx  = Board::NOVERTEX;

	while(txt_move_string >> cmd){
		cmd_count++;
		if (cmd_count == 1) {
			if(cmd == "b" || cmd == "black"){
				color = Board::BLACK;
			} else if (cmd == "w" || cmd == "white"){
				color = Board::WHITE;
			} else if (cmd == "pass" || cmd == "PASS") {
				vtx = Board::PASS;
				color = (Board::vertex_t)board.get_to_move();
				break;
			} else {
				const auto cmd_size = cmd.size();
				auto number_str = std::string{};
				int x = -1;
				int y = -1;
				if (cmd[0] >= 97 && cmd[0] <= 122) {
					x = (int)cmd[0] - 97;
				} else if (cmd[0] >= 65 && cmd[0] <= 90) {
					x = (int)cmd[0] - 65;
				} else {
					return false;			
				}

				for (int i = 1; i < cmd_size; ++i) {
					if (cmd[i] >= 48 && cmd[i] <= 57) {
						number_str += cmd[i];
					} else {
						return false;			
					}
				}
				y = std::stoi(number_str) - 1;
				vtx = board.get_vertex(x, y);
				color = (Board::vertex_t)board.get_to_move();
				break;			
			}
		} else if (cmd_count == 2) {
			if (cmd == "pass" || cmd == "PASS") {
				vtx = Board::PASS;
			} else {
				const auto cmd_size = cmd.size();
				auto number_str = std::string{};
				int x = -1;
				int y = -1;
				if (cmd[0] >= 97 && cmd[0] <= 122) {
					x = (int)cmd[0] - 97;
				} else if (cmd[0] >= 65 && cmd[0] <= 90) {
					x = (int)cmd[0] - 65;
				} else {
					return false;			
				}

				for (int i = 1; i < cmd_size; ++i) {
					if (cmd[i] >= 48 && cmd[i] <= 57) {
						number_str += cmd[i];
					} else {
						return false;			
					}
				}
				y = std::stoi(number_str) - 1;
				vtx = board.get_vertex(x, y);
			}
		} else {
			return false;
		}
	}
	return play_move(color, vtx);
}

bool GameState::is_gameover() const {
	return board.is_gameover();
}

int GameState::get_winner(bool ret) {
	if (m_gameover) {return m_winner;}
	
	const auto res = board.get_winner();
	if (ret) {
		if (!board.is_gameover()) {
			return Board::INVAL;
		} else {
			m_gameover = true;
			m_winner   = res;
		}
	}
	return res;
}

void GameState::one_resign(const int resign_color) {
	if (m_gameover) {return;}
	m_gameover = true;
	m_winner   = !(resign_color);
}

void GameState::force_countine() {
	m_gameover = false;
}

int GameState::get_movenum() const {
	return m_movenum;
}

std::shared_ptr<const Board> GameState::get_board(const int v) {
	if (v < 0 || v > m_movenum) {
		return nullptr;
	}
	return game_history[v];
}

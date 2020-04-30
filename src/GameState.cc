#include "GameState.h"

#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <sstream>

GameState::GameState(){
	init_game(BOARDSIZE);
}


void GameState::init_game(int boardsize) {
	m_winner = Board::INVAL;
	board.reset_board(boardsize);
	game_history.clear();
	game_history.emplace_back(std::make_shared<Board>(board));
}


void GameState::display() const {
	board.text_display(board.get_last_move());
	board.print_stones();
	board.print_hash();
	board.print_to_move();
}


bool GameState::play_move(const int color, const int vtx){
	
	if (color != Board::WHITE && color != Board::BLACK) {
		return false;
	}

	if (vtx == Board::PASS) {
		if (board.exsit_moves(color)) {
			return false;
		} else {
			board.set_to_move(color);
			game_history.emplace_back(std::make_shared<Board>(board));
			return true;
		}
	}

	if(!board.is_legal(color, vtx)){
		return false;
	}
	
	board.set_to_move(color);
	board.play_move(vtx);
	game_history.emplace_back(std::make_shared<Board>(board));

	return true;
}

bool GameState::text_play_move(std::string &input){

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
			} else {
				if (cmd == "pass") {
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
					color = (Board::vertex_t)board.get_to_move();
					break;
				}			
			}
		} else if (cmd_count == 2) {
			if (cmd == "pass") {
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

int GameState::get_winner() const {
	return board.get_winner();
}

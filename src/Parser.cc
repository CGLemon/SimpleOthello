#include "Parser.h"
#include "Board.h"
#include "GameState.h"

#include <string>
#include <sstream>
#include <fstream>
std::string Parser::simple_thor(GameState & state, bool withpass) {

	std::string thor;
	const int movenum = state.get_movenum();
	std::vector<int> vertex_all;
	for (int m = 1; m <= movenum; m++) {
		auto board_ptr = state.get_board(m);
		int vtx;

		if (board_ptr) {
			vtx = board_ptr->get_last_move();
		} else {
			break;
		}
		if (!withpass && vtx == Board::PASS) {
			continue;
		}

		vertex_all.emplace_back(vtx);
	}

	const auto boardsize = state.board.get_boardsize();
	const auto size = vertex_all.size();
	for (int p = 0; p < size; ++p) {
		thor += parser_vertex(vertex_all[p], boardsize);
		if (p != size-1) {
			thor += " ";
		}
	}

	return thor;
}


bool Parser::save_thor(GameState & state, std::string filename, bool withpass) {

	bool pass;
	auto thor = simple_thor(state);
	std::ofstream file;
	file.open(filename);
	if (file) {
		file << thor << "\n";
		pass = true;
	} else {
		pass = false;
	}
	file.close();
	return pass;
}

void Parser::simple_parser(GameState & state, std::string & thor) {
	const auto boardsize = state.board.get_boardsize();
	std::vector<int> vertex_all;
	std::stringstream thor_input(thor);
	std::string vertex_str;

	while (thor_input >> vertex_str) {
		
		if (vertex_str == "pa") {
			vertex_all.emplace_back(Board::PASS);
			continue;
		}
		
		const int size = vertex_str.size();
		const char x_char = vertex_str[0];
		auto y_str = std::string{};
		int x, y;
		if (size <= 1) {
			return;
		}
		if (x_char >= 'a' && x_char <= 'z') {
			x = x_char - 97;
		} else if (x_char >= 'A' && x_char <= 'Z') {
			x = x_char - 65;
		} else {
			return;
		}

		for (int i = 1; i < size; ++i) {
			if (vertex_str[i] >= '0' || vertex_str[i] <= '9') {
				y_str += vertex_str[i];
			} else {
				return;
			}
		}
		y = std::stoi(y_str) - 1;

		const int vertex = state.board.get_vertex(x, y);
		vertex_all.emplace_back(vertex);
	}


	state.init_game(boardsize);

	for (auto & vtx: vertex_all) {

		int color = state.board.get_to_move();
		while(vtx != Board::PASS && !state.board.exsit_moves(color)) {
			state.play_move(color, Board::PASS);
			state.board.exchange_to_move();
			state.display();
			color = state.board.get_to_move();
		}
		state.play_move(color, vtx);
		state.board.exchange_to_move();
		state.display();
	}
}

bool Parser::load_thor(GameState & state, std::string filename){
	std::ifstream file;
	bool pass;
	auto thor = std::string{};
	file.open(filename);
	if (file && std::getline(file,thor)){
		simple_parser(state, thor);
		pass = true;
	} else {
		pass = false;
	}
	file.close();
	return pass;
}

std::string Parser::parser_vertex(const int vtx, const int boardsize, bool character) {
	if (vtx == Board::NOVERTEX) {
		return std::string{""};
	}

	if (vtx == Board::PASS) {
		if (character) {
			return std::string{"PA"};
		} else {
			return std::string{"pa"};
		}
	}

	
	const int x = vtx % (boardsize+1);
	const int y = vtx / (boardsize+1);
	int separate = 96;
	if (character) {
		separate = 64;
	}
 	char x_char = (x+separate); 
	auto y_str = std::to_string(y);
	auto x_str = std::string{x_char};
	auto out   = x_str + y_str;
	
	return out;
}

#include "Parser.h"
#include "Board.h"
#include "GameState.h"

#include <string>

std::string Parser::simple_pgn(GameState & state) {

	std::string pgn;
	const int movenum = state.get_movenum();
	std::vector<std::pair<int, int>> vertex_pair;
	for (int m = 1; m <= movenum; m+=2) {
		auto board_ptr_1 = state.get_board(m);
		auto board_ptr_2 = state.get_board(m+1);

		int vtx_1;
		int vtx_2;
		if (board_ptr_1) {
			vtx_1 = board_ptr_1->get_last_move();
		} else {
			break;
		}
		if (board_ptr_2) {
			vtx_2 = board_ptr_2->get_last_move();
		} else {
			vtx_2 = Board::NOVERTEX;
		}

		vertex_pair.push_back({vtx_1, vtx_2});
	}

	const auto boardsize = state.board.get_boardsize();
	const auto size = vertex_pair.size();
	for (int p = 0; p < size; ++p) {
		pgn += std::to_string(p+1);
		pgn += ". ";
		pgn += parser_vertex(vertex_pair[p].first, boardsize);
		pgn += " ";
		pgn += parser_vertex(vertex_pair[p].second, boardsize);
		pgn += " ";
	}
	if (!state.is_gameover()) {
		pgn += "*";
	} else {
		const auto stones_pair = state.board.get_stones();
		pgn += std::to_string(stones_pair.first);
		pgn += "-";
		pgn += std::to_string(stones_pair.second);
	}

	return pgn;
}


std::string Parser::parser_vertex(const int vtx, const int boardsize) {
	if (vtx == Board::NOVERTEX) {
		return std::string{""};
	}

	if (vtx == Board::PASS) {
		return std::string{"--"};
	}

	
	const int x = vtx % (boardsize+1);
	const int y = vtx / (boardsize+1);
	char x_char = (x+64); 
	auto y_str = std::to_string(y);
	auto x_str = std::string{x_char};
	auto out   = x_str + y_str;
	
	return out;
}

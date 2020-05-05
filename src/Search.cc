#include "Search.h"
#include "Node.h"
#include "Board.h"
#include "Timer.h"

#include <string>
#include <numeric>
bool Search::prepare_root() {
	auto board = std::make_shared<Board>(root_state.board); 
	if (board -> is_gameover()) {
		return false;
	} 

	root_node = std::make_unique<Node>(*board);
	root_node -> increment();

	return true;
}

void Search::search(const Board & current_board, Node * node, 
						Search::MoveList movelist, int maxdepth, int alpha, int beta) {

	if (current_board.is_gameover()) {
		return;
	}
	
	const int depth = movelist.size();
	if (depth+1 > m_searchdepth) {
		m_searchdepth = depth+1;
	}
	if (depth >= maxdepth) {
		return;
	}
	auto const root_color = root_state.board.get_to_move();
	auto const current_color = current_board.get_to_move();

	movelist.emplace_back(0);
	node -> expand_children(current_board, root_color == Board::BLACK, root_color != current_color);
	const int child_size = node -> get_numchild();
	bool cut = false;
	for (int n = 0; n < child_size; ++n) {
		if (depth+1 >= maxdepth) {
			break;
		}
		if (!m_running) {
			break;
		}

		if (m_TimeController.stop_search()) {
			break;
		}
	
		const auto vtx = node -> get_child_vertex(n);
		if (cut) {
			node -> pruning_child(vtx);
		}

		if (node -> is_child_pruning(vtx)) {
			continue;
		}
		node -> inflate_children(vtx);
		

		auto child = node -> get_child(vtx);
		auto deeper_board = std::make_shared<Board>(current_board);
		deeper_board -> play_move(current_color, vtx);
		deeper_board -> exchange_to_move();
		movelist[depth] = n;
		search(*deeper_board, child, movelist, maxdepth, alpha, beta);

		const int temp_eval = child -> get_eval_value();
		if (current_color == root_color) {   // max node
			// update alpha
			if (temp_eval > alpha) {
				alpha = temp_eval;
			}
			if (temp_eval > beta) {
				cut = true;
			}
		} else {
			// update beta
			if (temp_eval < beta) {
				beta = temp_eval;
			}
			if (temp_eval < alpha) {
				cut = true;
			}
		}

	}
	node -> update_eval_value(current_color == root_color);
}

int Search::get_best_move() {
	return root_node -> get_best_move(true);
}

int Search::think() {
	m_searchdepth = 0;
	m_running = true;

	if (!prepare_root()) {
		return Board::NOVERTEX;
	}
	int edge_count = root_node -> get_edge_count();

	m_TimeController.set_time(m_search_seconds);
	m_TimeController.start_clock();

	MoveList movelist = {};
	int maxdepth = movelist.size() + m_basicdepth;
	
	
	while (m_running) {
		if (m_TimeController.stop_search()) {
			printf("Timeout\n");
			m_running = false;
			break;
		}
		
		printf("Searching to depth : %d.... ", maxdepth);
		auto deeper_board = std::make_shared<Board>(root_state.board);
		auto node = root_node -> get_node();
		search(*deeper_board, node, movelist, maxdepth);
		printf("Search complete!\n");

		const int vtx = get_best_move();
		const auto search_edge_count =  root_node -> get_edge_count();
		if (search_edge_count == edge_count) {
			printf("All node is expended!\n");
			m_running = false;
			break;
		} else {
			edge_count = search_edge_count;
		}

		if (node -> get_eval_value() > 9999  || node -> get_eval_value() < -9999) {
			printf("Find ending way !\n");
			m_running = false;
			break;
		}
		maxdepth = m_searchdepth+1;
		if (m_maxdepth < maxdepth) {
			printf("stop !\n");
			m_running = false;
			break;
		}
	}
	
	root_node -> inflate_allchildren();

	auto const root_color = root_state.board.get_to_move();
	root_node -> update_eval_value(true);

	print_move_eval(false);
	print_node_count(false);
	const int best_move = get_best_move();
	printf("using time : %f seconds\n", m_TimeController.get_during());
	printf("computer move = ");
	parser_vertex(best_move, false);
	printf("\n");	

	return best_move;
}

void Search::print_move_eval(bool quiet) const {
	const int root_child_size = root_node -> get_numchild();	
	for (int n = 0; n < root_child_size; ++n) {
		const auto vtx = root_node -> get_child_vertex(n);
		if (root_node -> is_child_pruning(vtx)) {
			continue;
		}
		auto child = root_node -> get_child(vtx);
		const auto color = root_state.board.get_to_move();
		int val = child -> get_eval_value();

		parser_vertex(vtx, quiet);
		if (!quiet) {
			printf(" : %d scores \n", val);
		}
	}
}

void Search::print_node_count(bool quiet) const {
	if (!quiet) {
		const auto count = root_node -> get_node_count();
		printf("generate %d nodes, %d uninflated nodes\n", count.first, count.second);
	}
} 

void Search::clear() {
	root_node -> clear_count();
}

void Search::set_time(const int seconds) {
	if (seconds <= 0) {
		return;
	}
	m_search_seconds = seconds;
}

void Search::set_maxdepth(const int maxdepth) {
	if (maxdepth <= 0) {
		return;
	}
	m_maxdepth = maxdepth;
}

void Search::set_basicdepth(const int basicdepth) {
	if (basicdepth <= 0) {
		return;
	}
	m_basicdepth = basicdepth;
}

void Search::parser_vertex(const int vtx, bool quiet) const {

	if (vtx == Board::NOVERTEX) {
		printf("erro");
		return;
	}

	if (vtx == Board::PASS) {
		if (!quiet) {
			printf("PASS");
		}
		return;
	}

	const int boardsize = root_state.board.get_boardsize();
	const int x = vtx % (boardsize+1);
	const int y = vtx / (boardsize+1);
	auto y_str = std::to_string(y);
	if (!quiet) {
		printf("%c%s", (x+64), y_str.c_str());
	}
}

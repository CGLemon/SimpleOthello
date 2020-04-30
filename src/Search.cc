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
						Search::MoveList movelist, int maxdeep, int alpha, int beta) {

	if (current_board.is_gameover()) {
		return;
	}
	
	const int deep = movelist.size();
	if (deep+1 > m_searchdeep) {
		m_searchdeep = deep+1;
	}
	if (deep >= maxdeep) {
		return;
	}
	auto const root_color = root_state.board.get_to_move();
	auto const current_color = current_board.get_to_move();

	movelist.emplace_back(0);
	node -> extend_children(current_board, root_color == Board::BLACK, root_color != current_color);
	const int child_size = node -> get_numchild();
	bool cut = false;
	for (int n = 0; n < child_size; ++n) {
		if (deep+1 >= maxdeep) {
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
		movelist[deep] = n;
		search(*deeper_board, child, movelist, maxdeep, alpha, beta);

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
	m_searchdeep = 0;

	if (!prepare_root()) {
		return Board::NOVERTEX;
	}
	int edge_count = root_node -> get_edge_count();

	m_TimeController.set_time(10.f);
	m_TimeController.start_clock();
	 
	int alpha = std::numeric_limits<int>::min();
	int beta = std::numeric_limits<int>::max();
	MoveList movelist = {};
	int maxdeep = m_maxdeep;
	
	
	while (!m_TimeController.stop_search()) {
		
		printf("Searching to depth : %d.... ", maxdeep);
		auto deeper_board = std::make_shared<Board>(root_state.board);
		auto node = root_node -> get_node();
		search(*deeper_board, node, movelist, maxdeep, alpha, beta);
		printf("Search complete!\n");
		const int vtx = get_best_move();
		const auto search_edge_count =  root_node -> get_edge_count();
		if (search_edge_count == edge_count) {
			printf("All node is expended!\n");
			break;
		} else {
			edge_count = search_edge_count;
		}

		if (node -> get_eval_value() > 1000  || node -> get_eval_value() < -1000) {
			printf("Find ending way !\n");
			break;
		}
		maxdeep+=1;
	}
	auto const root_color = root_state.board.get_to_move();
	root_node -> update_eval_value(true);

	print_move_eval(false);
	print_node_count(false);
	const int best_move = get_best_move();
	printf("using time : %f seconds\n", m_TimeController.get_during());
	return best_move;
}

void Search::print_move_eval(bool quiet) const {
	const int root_child_size = root_node -> get_numchild();	
	const int boardsize = root_state.board.get_boardsize();
	for (int n = 0; n < root_child_size; ++n) {
		const auto vtx = root_node -> get_child_vertex(n);
		if (root_node -> is_child_pruning(vtx)) {
			continue;
		}
		auto child = root_node -> get_child(vtx);
		const auto color = root_state.board.get_to_move();
		int val = child -> get_eval_value();

		const int x = vtx % (boardsize+1);
		const int y = vtx / (boardsize+1);
		auto y_str = std::to_string(y);
		if (!quiet) {
			printf("%c%s : %d scores \n", (x+64), y_str.c_str(), val);
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

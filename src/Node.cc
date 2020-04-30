#include "Node.h"
#include "Evaluation.h"
#include "Board.h"

#include <memory>
#include <algorithm>
#include <iterator>
#include <numeric>

constexpr std::uint64_t Edge::INVAL;
constexpr std::uint64_t Edge::UNINFLATED;
constexpr std::uint64_t Edge::POINTER;

Edge::Edge(const int vertex, const int value) {
	m_pointer = UNINFLATED;
	m_vertex  = vertex;
	m_node_value = value;
}

void Edge::increment() const {
	tree::edge_count++;
	tree::total_edge_count++;
}
void Edge::decrement() const {
	tree::edge_count--;
}

void Edge::inflate() {
	if (is_uninflate()) {
		auto v = new Node(m_vertex, m_node_value);
		m_pointer = reinterpret_cast<std::uint64_t>(v) | POINTER;
		decrement();
		read_ptr(m_pointer) -> increment();
	}
}

bool Edge::is_inval() const {
	return (m_pointer & 3ULL) == INVAL;
}

bool Edge::is_uninflate() const {
	return (m_pointer & 3ULL) == UNINFLATED;
}

bool Edge::is_pointer() const {
	return (m_pointer & 3ULL) == POINTER;
}

void Edge::set_inval() {
	m_pointer |= INVAL;
}

Node * Edge::get_node() const {
	if (is_pointer()) { 
		return read_ptr(m_pointer);
	} else {
		return nullptr;
	}
}

int Edge::get_vertex() const {
	return m_vertex;
}

int Edge::get_node_value() const {
	return m_node_value;
}

int Edge::get_eval_value() const {
	if (is_pointer()) {
		return read_ptr(m_pointer) -> get_eval_value();
	} else {
		return m_node_value;
	}
}

Node::Node(Board & board) {
	if (!board.is_gameover()) {
		const auto color = board.get_to_move(); 
		const auto eval = Evaluation::get_score(board);
		const int node_value = eval.blackscore - eval.whitescore;
		m_node_value = node_value;
		m_eval_value = node_value;
	}
}

Node::Node(const int vertex, const int value) {
	m_vertex 	 = vertex;
	m_node_value = value;
	m_eval_value = value;
}

void Node::extend_children(const Board & board, bool is_black, bool reverse) {
	if (status == EXPANDING || status == EXPANDED) {
		return;
	}
	status = EXPANDING;

	const int boardsize = board.get_boardsize();
	const auto color = board.get_to_move(); 
	std::vector<std::pair<int, int>> node_list;
	for (int y = 0; y < boardsize; ++y) {
		for (int x = 0; x < boardsize; ++x) {
			const auto vtx = board.get_vertex(x, y);
			if (board.is_legal(color, vtx)) {
				auto smi_board = std::make_shared<Board>(board);
				smi_board -> play_move(color, vtx);
				const auto eval = Evaluation::get_score(*smi_board);
				if (is_black) {
					const int node_value = eval.blackscore - eval.whitescore;
					node_list.emplace_back(node_value, vtx);
				} else {
					const int node_value = eval.whitescore - eval.blackscore;
					node_list.emplace_back(node_value, vtx);
				}
			}
		}
	} 
	if (node_list.empty()) {
		if (!board.is_gameover()) {
			const auto eval = Evaluation::get_score(board);
			if (is_black) {
				const int node_value = eval.blackscore - eval.whitescore;
				node_list.emplace_back(node_value, Board::PASS);
			} else {
				const int node_value = eval.whitescore - eval.blackscore;
				node_list.emplace_back(node_value, Board::PASS);
			}
		}
	}
	link_noodlist(node_list, reverse);
	status = EXPANDED;
}


void Node::link_noodlist(std::vector<std::pair<int, int>> & node_list, bool reverse) {
	
	if (node_list.empty()) {
		return;
	}
	std::stable_sort(rbegin(node_list), rend(node_list));
	const auto size = node_list.size();
	//int first_value;
	for (auto n = 0; n < size; ++n) {
		int id;
		if (reverse) {
			id = size - 1 - n;
		} else {
			id = n;
		}
		const int value = node_list[id].second;
		//if (n == 0) {
		//	first_value = value;
		//}
		//if (first_value > value) {
		//	if (first_value - value > 200) {
		//		continue;
		//	}
		//} else {
		//	if (first_value - value < -200) {
		//		continue;
		//	}
		//} 

		m_numchilden++;
		m_children.emplace_back(value, node_list[id].first);
		m_children[m_numchilden-1].increment();
	}
	m_children.reserve(m_numchilden);
}

void Node::inflate_allchildren() {
	for (auto &child : m_children) {
		child.inflate();
	}
}
void Node::inflate_children(const int vtx) {
	for (auto &child : m_children) {
		if (child.get_vertex() == vtx) {
			child.inflate();
			return;
		}
	}
}

void Node::pruning_child(const int vtx) {
	for (auto &child : m_children) {
		if (child.get_vertex() == vtx) {
			child.set_inval();
			return;
		}
	} 
}

bool Node::is_child_pruning(const int vtx) const{
	for (auto &child : m_children) {
		if (child.get_vertex() == vtx) {
			return child.is_inval();
		}
	} 
	return true;
}

int Node::get_eval_value() const {
	return m_eval_value;
}

void Node::update_eval_value(bool max) {
	if (status != EXPANDED) {
		return;
	}

	if (m_children.empty()) {
		return;
	}

	if (max) {
		int max_val = std::numeric_limits<int>::min();
		int vtx;
		for (const auto &child : m_children) {
			const int child_value = child.get_eval_value();
			if (max_val < child_value) {
				vtx = child.get_vertex();
				max_val = child_value;
			}
		}
		m_eval_value = max_val;
		m_bestmove = vtx;
	} else {
		int min_val = std::numeric_limits<int>::max();
		int vtx;
		for (const auto &child : m_children) {
			const int child_value = child.get_eval_value();
			if (min_val > child_value) {
				vtx = child.get_vertex();
				min_val = child_value;
			}
		}
		m_eval_value = min_val;
		m_bestmove = vtx;
	}
}
int Node::get_child_vertex(const int id) const {
	if (id <= m_numchilden-1 && m_numchilden != 0) {
		return m_children[id].get_vertex();
	}
	return Board::NOVERTEX;
}
Node * Node::get_child(const int vtx) const {

	for (auto &child : m_children) {
		if (child.get_vertex() == vtx) {
			return child.get_node();
		}
	} 
	return nullptr;
}

int Node::get_numchild() const {
	return m_numchilden;
}

bool Node::has_child() const {
	return m_numchilden != 0;
}

int Node::get_best_move(bool max) const {

	int vtx = Board::NOVERTEX;
	if (max) {
		int max_val = std::numeric_limits<int>::min();
		for (const auto &child : m_children) {
			const int child_value = child.get_eval_value();
			if (max_val < child_value) {
				vtx = child.get_vertex();
				max_val = child_value;
			}
		}
	} else {
		int min_val = std::numeric_limits<int>::max();
		for (const auto &child : m_children) {
			const int child_value = child.get_eval_value();
			if (min_val > child_value) {
				vtx = child.get_vertex();
				min_val = child_value;
			}
		}
	}
	return vtx;
}

std::pair<int, int> Node::get_node_count() const {
	return {tree::node_count, tree::edge_count};
}

int Node::get_edge_count() const {
	return tree::total_edge_count;
}

void Node::clear_count() {
	tree::edge_count = 0;
	tree::node_count = 0;
	tree::total_edge_count = 0;
}

Node * Node::get_node() {
	return this;
}

void Node::increment() const {
	tree::node_count++;
}

void Node::decrement() const {
	tree::node_count--;
}




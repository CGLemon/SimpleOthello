#include "Node.h"
#include "Evaluation.h"
#include "Board.h"

#include <memory>
#include <algorithm>
#include <iterator>
#include <numeric>

constexpr std::uint64_t Edge::INVALID;
constexpr std::uint64_t Edge::UNINFLATED;
constexpr std::uint64_t Edge::POINTER;


std::atomic<size_t> Edge::total_edge_count{0};
std::atomic<size_t> Edge::edge_count{0};
std::atomic<size_t> Node::node_count{0};


Edge::Edge(const int vertex, const int value) {
	m_pointer = UNINFLATED;
	m_vertex  = vertex;
	m_node_value = value;
}

Edge::Edge(Edge&& n) {
    auto nv = std::atomic_exchange(&n.m_pointer, INVALID);
    auto v = std::atomic_exchange(&m_pointer, nv);
    assert(v == INVALID);
}

void Edge::increment() const {
	edge_count++;
	total_edge_count++;
}
void Edge::decrement() const {
	edge_count--;
}

void Edge::inflate(Node * parent) {
	while (true) {
		if (!is_uninflate()) {
			return;
		}
		auto v1 = m_pointer.load();
		auto v2 = reinterpret_cast<std::uint64_t> (
					new Node(m_vertex, m_node_value, parent)) | POINTER;
		

		bool success = m_pointer.compare_exchange_strong(v1, v2);
		if (success) {
			decrement();
			read_ptr(m_pointer.load()) -> increment();
	        return;
	    } else {
	        delete read_ptr(v2);
	    }
	}
}

bool Edge::is_inval() const {
	return (m_pointer.load() & 3ULL) == INVALID;
}

bool Edge::is_uninflate() const {
	return (m_pointer.load() & 3ULL) == UNINFLATED;
}

bool Edge::is_pointer() const {
	return (m_pointer.load() & 3ULL) == POINTER;
}

void Edge::set_inval() {
	m_pointer |= INVALID;
}

Node * Edge::get_node() const {
	if (is_pointer()) { 
		return read_ptr(m_pointer.load());
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
		return read_ptr(m_pointer.load()) -> get_eval_value();
	} else {
		return m_node_value;
	}
}

Node::Node(Board & board, Node * parent) {
	if (!board.is_gameover()) {
		const auto color = board.get_to_move(); 
		const auto eval = Evaluation::get_score(board);
		const int node_value = eval.blackscore - eval.whitescore;
		m_node_value = node_value;
		m_eval_value = node_value;
	}
	m_parentnode = parent;
}

Node::Node(const int vertex, const int value, Node * parent) {
	m_vertex 	 = vertex;
	m_node_value = value;
	m_eval_value = value;
	m_parentnode = parent;
}

void Node::expand_children(const Board & board, bool is_black, bool reverse) {
	if (m_expand_state != ExpandState::INITIAL) {
		return;
	}
	if (!acquire_expanding()) {
		return;
	}

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
	expand_done();
}


void Node::link_noodlist(std::vector<std::pair<int, int>> & node_list, bool reverse) {
	
	if (node_list.empty()) {
		return;
	}
	std::stable_sort(rbegin(node_list), rend(node_list));
	const auto size = node_list.size();
	m_children.reserve(size);

	for (auto n = 0; n < size; ++n) {
		int id;
		if (reverse) {
			id = size - 1 - n;
		} else {
			id = n;
		}

		m_numchilden++;
		m_children.emplace_back(node_list[id].second, node_list[id].first);
		m_children[m_numchilden-1].increment();
	}
	m_children.reserve(m_numchilden);
}

void Node::inflate_allchildren() {
	for (auto &child : m_children) {
		child.inflate(this);
	}
}
void Node::inflate_children(const int vtx) {
	for (auto &child : m_children) {
		if (child.get_vertex() == vtx) {
			child.inflate(this);
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
	if (!is_expanded()) {
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

	//wait_expanded();
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
	return {node_count.load(), Edge::edge_count.load()};
}

int Node::get_edge_count() const {
	return Edge::total_edge_count.load();
}

void Node::clear_count() {
	Edge::edge_count = 0;
	node_count = 0;
	Edge::total_edge_count = 0;
}

Node * Node::get_node() {
	return this;
}

Node * Node::get_parent() const {
	return m_parentnode;
}

void Node::increment() const {
	node_count++;
}

void Node::decrement() const {
	node_count--;
}


bool Node::acquire_expanding() {
    auto initial	 = ExpandState::INITIAL;
    auto expecting   = ExpandState::EXPANDING;
    return m_expand_state.compare_exchange_strong(initial, expecting);
}

void Node::expand_done() {
    auto v = m_expand_state.exchange(ExpandState::EXPANDED);
    assert(v == ExpandState::EXPANDING);
}
void Node::expand_cancel() {
    auto v = m_expand_state.exchange(ExpandState::INITIAL);
    assert(v == ExpandState::EXPANDING);
}
void Node::wait_expanded() {
    while (m_expand_state.load() == ExpandState::EXPANDING) {}
    auto v = m_expand_state.load();
    assert(v == ExpandState::EXPANDED);
}

bool Node::is_expanded() const {
	return m_expand_state.load() == ExpandState::EXPANDED;
}

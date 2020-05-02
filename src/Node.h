#ifndef NODE_H_INCLUDE
#define NODE_H_INCLUDE

#include <atomic>
#include <vector>

#include "Board.h"
#include "CacheTable.h"
#include "Evaluation.h"

class Node;


class Edge {
public:
	static constexpr std::uint64_t UNINFLATED = 0x1;
	static constexpr std::uint64_t POINTER 	  = 0x2;
	static constexpr std::uint64_t INVALID    = 0x3;


	static std::atomic<size_t> total_edge_count;
	static std::atomic<size_t> edge_count;

	Edge(const int vertex, const int value);

	void set_inval();
	void inflate(Node *);
	Node * read_ptr(std::uint64_t v) const;
	
	bool is_inval() const;
	bool is_uninflate() const;
	bool is_pointer() const;

	int get_vertex() const;
	int get_node_value() const;
	int get_eval_value() const;
	Node * get_node() const;	

	void increment() const;
	void decrement() const;

	Edge(const Edge&) = delete;
    Edge(Edge&& n);
private:
	mutable std::atomic<std::uint64_t> m_pointer;
	int m_vertex;
	int m_node_value;

};



inline Node * Edge::read_ptr(std::uint64_t v) const {
    return reinterpret_cast<Node*>(v & ~(0x3ULL));
}


class Node {
public:
	enum class ExpandState : std::uint8_t {
		INITIAL   = 0,
		EXPANDING = 1,
		EXPANDED  = 2
	};

	static std::atomic<size_t> node_count;

	Node(const int vertex, const int value, Node * parent = nullptr);
	Node(Board & board, Node * parent = nullptr);
	Node() = delete;
	Node(const Node&) = delete;
    ~Node() = default;

	void link_noodlist(std::vector<std::pair<int, int>> & node_list, bool reverse);
	void expand_children(const Board & board, bool is_black, bool reverse);
	void inflate_allchildren();
	void inflate_children(const int vtx);

	int get_eval_value() const;
	void update_eval_value(bool max);

	int get_numchild() const;
	bool has_child() const;
	bool is_child_pruning(const int vtx) const;
	void pruning_child(const int vtx);


	int get_child_vertex(const int id) const;
	int get_best_move(bool max) const;

	Node * get_child(const int vtx) const;
	static void clear_count();
	std::pair<int, int> get_node_count() const;
	int get_edge_count() const;

	Node * get_node();
	void increment() const;
	void decrement() const;


	bool acquire_expanding();
	void expand_done();
	void expand_cancel();
	void wait_expanded();
	bool is_expanded() const;
	
	Node * get_parent() const;

private:
	std::vector<Edge> m_children;
	int m_vertex;
	int m_node_value;

	int m_bestmove;
	int m_eval_value;

	int m_numchilden{0};	
	
	Node * m_parentnode;
	std::atomic<ExpandState> m_expand_state{ExpandState::INITIAL};
};


#endif

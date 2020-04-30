#ifndef NODE_H_INCLUDE
#define NODE_H_INCLUDE

#include <vector>

#include "Board.h"
#include "CacheTable.h"
#include "Evaluation.h"

class Node;
namespace tree {
	static size_t total_edge_count = 0;
	static size_t edge_count = 0;
	static size_t node_count = 0;
}

class Edge {
public:
	static constexpr std::uint64_t UNINFLATED = 0x1;
	static constexpr std::uint64_t POINTER 	  = 0x2;
	static constexpr std::uint64_t INVAL      = 0x3;

	Edge(const int vertex, const int value);

	void set_inval();
	void inflate();
	Node * read_ptr(std::uint64_t v) const;
	
	bool is_inval() const;
	bool is_uninflate() const;
	bool is_pointer() const;

	int get_vertex() const;
	int get_node_value() const;
	int get_eval_value() const;
	Node *  get_node() const;	

	void increment() const;
	void decrement() const;
private:
	std::uint64_t m_pointer;
	int m_vertex;
	int m_node_value;
	
};

inline Node * Edge::read_ptr(std::uint64_t v) const {
    return reinterpret_cast<Node*>(v & ~(0x3ULL));
}


class Node {
public:
	enum ExpandState {
		INITIAL = 0,
        EXPANDING,
        EXPANDED
	};

	Node(const int vertex, const int value);
	Node(Board & board);

	void link_noodlist(std::vector<std::pair<int, int>> & node_list, bool reverse);
	void extend_children(const Board & board, bool is_black, bool reverse);
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
	void clear_count();
	std::pair<int, int> get_node_count() const;
	int get_edge_count() const;

	Node * get_node();
	void increment() const;
	void decrement() const;

private:
	std::vector<Edge> m_children;
	int m_vertex;
	int m_node_value;

	int m_bestmove;
	int m_eval_value;

	int m_numchilden{0};	

	ExpandState status{INITIAL};
};


#endif

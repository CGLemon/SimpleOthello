#ifndef CACHETABLE_H_INCLUDE
#define CACHETABLE_H_INCLUDE

#include <array>
#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>

class CacheTable {
public:
	struct EvalResult {
		EvalResult() : blackscore(0), whitescore(0) {}
		int blackscore;
		int whitescore;
	};

	CacheTable(size_t size = MAX_CACHE_COUNT) : m_hits(0), m_lookups(0), m_inserts(0) { resize(size); }

	bool lookup(std::uint64_t hash, EvalResult & result);
	void insert(std::uint64_t hash, const EvalResult & result);
	void resize(size_t size);


	void dump_stats();

   
    size_t get_estimated_size();

private:


	static constexpr size_t MAX_CACHE_COUNT = 150000;

    static constexpr size_t MIN_CACHE_COUNT = 6000;

	static constexpr size_t ENTRY_SIZE =
		      sizeof(EvalResult)
		    + sizeof(std::uint64_t)
		    + sizeof(std::unique_ptr<EvalResult>);


	std::mutex m_mutex;

    size_t m_size;

    int m_hits;
    int m_lookups;
    int m_inserts;

	struct Entry {
        Entry(const EvalResult& r)
            : result(r) {}
        EvalResult result;
    };


    std::unordered_map<std::uint64_t, std::unique_ptr<const Entry>> m_cache;
    
    std::deque<size_t> m_order;
	
};


#endif

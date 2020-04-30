#include "CacheTable.h"

#include <functional>
#include <memory>

constexpr size_t CacheTable::MAX_CACHE_COUNT;
constexpr size_t CacheTable::MIN_CACHE_COUNT;
constexpr size_t CacheTable::ENTRY_SIZE;

bool CacheTable::lookup(std::uint64_t hash, EvalResult & result) {

	std::lock_guard<std::mutex> lock(m_mutex);
    ++m_lookups;

    auto iter = m_cache.find(hash);
    if (iter == m_cache.end()) {
        return false;
	}
    const auto& entry = iter->second;


    ++m_hits;
    result = entry->result;
    return true;
}


void CacheTable::insert(std::uint64_t hash, const EvalResult & result) {

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_cache.find(hash) != m_cache.end()) {
        return;  
    }

    m_cache.emplace(hash, std::make_unique<Entry>(result));
    m_order.push_back(hash);
    ++m_inserts;

    if (m_order.size() > m_size) {
        m_cache.erase(m_order.front());
        m_order.pop_front();
    }
}

void CacheTable::resize(size_t size) {

	m_size = ( 
			size > CacheTable::MAX_CACHE_COUNT ? CacheTable::MAX_CACHE_COUNT :
			size < CacheTable::MIN_CACHE_COUNT ? CacheTable::MIN_CACHE_COUNT : size
	 );

    while (m_order.size() > m_size) {
        m_cache.erase(m_order.front());
        m_order.pop_front();
    }
}

void CacheTable::dump_stats() {
    printf(
        "NNCache: %d/%d hits/lookups = %.1f%% hitrate, %d inserts, %lu size\n",
        m_hits, m_lookups, 100. * m_hits / (m_lookups + 1),
        m_inserts, m_cache.size());
}

size_t CacheTable::get_estimated_size() {
    return m_order.size() * CacheTable::ENTRY_SIZE;
}

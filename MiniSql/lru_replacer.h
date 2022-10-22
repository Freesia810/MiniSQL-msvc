#ifndef MINISQL_LRU_REPLACER_H
#define MINISQL_LRU_REPLACER_H

#include <list>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <map>

#include "replacer.h"
#include "config.h"


/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
class LRUReplacer : public Replacer {
public:
	/**
	 * Create a new LRUReplacer.
	 * @param num_pages the maximum number of pages the LRUReplacer will be required to store
	 */
	explicit LRUReplacer(size_t num_pages);

	/**
	 * Destroys the LRUReplacer.
	 */
	~LRUReplacer() override;

	bool Victim(frame_id_t* frame_id) override;

	void Pin(frame_id_t frame_id) override;

	void Unpin(frame_id_t frame_id) override;

	size_t Size() override;

private:
	// add your own private member variables here
	size_t capacity;//可以放置的最大容量
	std::list <frame_id_t> lru_list_;//存放能够被移出的frame_id
	std::unordered_map <frame_id_t, std::list<frame_id_t>::iterator> lru_hash;//哈希表
};

#endif  // MINISQL_LRU_REPLACER_H

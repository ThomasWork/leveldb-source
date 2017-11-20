// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <vector>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "port/port.h"

namespace leveldb {

class Arena {
    public:
	Arena();
	~Arena();
	
	// Return a pointer to a newly allocated memory block of "bytes" bytes.
	char* Allocate(size_t bytes);
	
	// Allocate memory with the normal alignment guarantees provided by malloc
	char* AllocateAligned(size_t bytes);
	
	// Return an estimate of the total memory usage of data allocated
	// by the arena.
	
	size_t MemoryUsage() const {
		return reinterpret_cast<uintptr_t>(memory_usage_.NoBarrier_Load());
	}
	
	private:
	char* AllocateFallback(size_t bytes);
	char* AllocateNewBlock(size_t block_bytes);
	
	// Allocation state
	char* alloc_ptr_;
	size_t alloc_bytes_remaining_;
	
	//Array of new[] allocated memory blocks
	std::vector<char*> blocks_;
	
	// Total memory usage of the arena.
	port::AtomicPointer memory_usage_;//arena的 总的内存使用量
	
	// No copying allowed
	Arena(const Arena&);
	void operator=(const Arena&);
};

inline char* Arena::Allocate(size_t bytes) {
    // The semantics of what to return are a bit messy if we allow
	// 0-byte allocations, so we disallow them here (we dont't need
	// them for our internal use).
	assert(bytes > 0);
	if (bytes <= alloc_bytes_remaining_) {
	    char* result = alloc_ptr_;
		alloc_ptr += bytes;
		alloc_bytes_remaining_ -= bytes;
		return result;
	}
	return AllocateFallback(bytes);
}

}; // namespace leveldb

#endif // STORAGE_LEVEL_UTIL_ARENA_H_
2015103435 徐志明 男 东风7栋225 15600925853 xzm@ruc.edu.cn xuzhiming_1 1164076705 2015信息硕 安徽省淮南市 无
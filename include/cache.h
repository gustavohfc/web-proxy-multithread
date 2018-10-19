
#ifndef CACHE_H
#define CACHE_H

#include <unistd.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "connection.h"
#include "log.h"

#define CACHE_SIZE 1000  // in number of CacheType's
#define CACHE_PATH "./cache"

// typedef struct cache_struct
// {
// 	size_t key;
// 	std::vector<char> data;
// }CacheType;

// class Cache
// {
// private:
// 	std::array<CacheType, CACHE_SIZE> cache;

// public:
// 	Cache();
// 	~Cache();

// 	bool find(size_t key);
// 	HTTPMessage get(size_t key);
// 	void set(size_t key, HTTPMessage data);
// };

void getResponseMessage(Connection& connection);

#endif  // CACHE_H
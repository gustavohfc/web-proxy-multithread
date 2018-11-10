
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

void getResponseMessage(Connection& connection);

void saveToCache(std::string filename, std::vector<char> data);

#endif  // CACHE_H
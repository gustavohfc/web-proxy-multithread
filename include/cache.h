
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

//#define CACHE_SIZE 1000  // in number of CacheType's
//#define CACHE_PATH "./cache"

struct CachedPage {
    std::vector<char> data;
};

class Cache {
   private:
    std::map<std::string, CachedPage> pages;

    void saveToCache(CachedPage& cachedPage, std::vector<char> data);

   public:
    void getResponseMessage(Connection &connection);
};

#endif  // CACHE_H
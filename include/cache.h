
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

class CachedPage {
   public:
    std::vector<char> data;

    pthread_mutex_t pageReadMutex;

    CachedPage() {
        pthread_mutex_init(&pageReadMutex, NULL);
    }

    ~CachedPage() {
        pthread_mutex_destroy(&pageReadMutex);
    }
};

class Cache {
   private:
    std::map<std::string, CachedPage> pages;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    void saveToCache(CachedPage& cachedPage, std::vector<char> data);

   public:
    void getResponseMessage(Connection& connection);
};

#endif  // CACHE_H
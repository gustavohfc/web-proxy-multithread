
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

class CachedPage {
   public:
    std::vector<char> data;
    bool tryingToUpdate = false;
    int nReading = 0;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t startUpdateCond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t updateCompleteCond = PTHREAD_COND_INITIALIZER;
};

class Cache {
   private:
    std::map<std::string, CachedPage> pages;
    pthread_mutex_t pagesMutex = PTHREAD_MUTEX_INITIALIZER;

    void saveToCache(CachedPage& cachedPage, std::vector<char> data);
    CachedPage& getLockedCachedPage(std::string path);
    void unlockCachedPage(CachedPage &cachedPage);
    void updateCachedPage(CachedPage& cachedPage, std::vector<char> data);

   public:
    void getResponseMessage(Connection& connection);
};

#endif  // CACHE_H
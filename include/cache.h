
#ifndef CACHE_H
#define CACHE_H

#include <functional>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "connection.h"

#define CACHE_PATH /cache

void getResponseMessage(Connection& connection);

#endif // CACHE_H
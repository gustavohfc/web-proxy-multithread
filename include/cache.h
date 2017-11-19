/*!
 * \file cache.h
 * \author Gustavo Henrique Fernandes Carvalho
 */

#ifndef CACHE_H
#define CACHE_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string>

#include "connection.h"

class Cache
{
private:

public:
    void getResponseMessage(Connection& connection);
};

#endif // CACHE_H
#ifndef _UTILS_H_
#define _UTILS_H_

#include <netinet/in.h>

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif
ssize_t nread (int fd, void *data, size_t len);
ssize_t timed_nread (int fd, void *buffer, size_t len, int millis);

ssize_t nwrite (int fd, const void *data, size_t len);

int socket_listen (int socktype, int port);
int get_host_address (const char *host, int port, struct sockaddr_in *addr);

#ifdef __cplusplus
}
#endif

#endif

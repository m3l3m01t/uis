#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <linux/ioctl.h>  
#include <sys/types.h>  
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

#include "netutils.h"
#include "debug.h"

ssize_t nread (int fd, void *buffer, size_t len) {
	ssize_t n = 0;

	if (len == 0) {
		LOG_DEBUG ("invalid len %ld", (long)len);
		return 0;
	}
	while ((size_t)n < len) {
		ssize_t ret = read (fd, (uint8_t *)buffer + n, len - n);
		if (ret > 0) {
			n += ret;
		} else if (ret == 0) {
			return n > 0 ? n : -1;
		} else if (errno == EINTR) {
			continue;
		} else if (errno == EAGAIN) {
			break;
		} else {
			LOG_DEBUG ("read error: %d", errno);
			return -1;
		}
	}

	return n;
}

ssize_t timed_nread (int fd, void *buffer, size_t len, int millis) {
	ssize_t ret = 0;
	fd_set rfds;
	struct timeval tv;

	ret = nread (fd, buffer, len);
	if (ret != 0) {
		return ret;
	}

	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

	gettimeofday (&tv, NULL);
	tv.tv_usec += (long)1000 * millis;
	tv.tv_sec += tv.tv_usec / 1000000;
	tv.tv_usec %= 1000000;

	ret = select (fd + 1, &rfds, NULL, NULL, &tv);
	if (ret == 1) {
		ret = nread (fd, buffer, len);
		return ret;
	} else if (ret == -1 && errno == EAGAIN) {
		return 0;
	}

	return ret;
}


ssize_t nwrite (int fd, const void *data, size_t len) {
	size_t n = 0;

	while (n < len) {
		int ret = write (fd, (uint8_t *)data + n, len - n);

		if (ret == -1) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EAGAIN) {
				break;
			} else {
				LOG_DEBUG ("write error: %d", errno);
				return -1;
			}
		} else
			n += ret;
	}

	return n;
}

int socket_listen (int socktype, int port) {
	int sock;

	int flag= 1;
	struct sockaddr_in sas1; 

	sock=socket(AF_INET, socktype, 0);
	if (sock == -1) {
		LOG_ERROR ("error creating server socket:%d", errno);
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof (flag)) != 0) {
		LOG_ERROR ("setsockopt failed:%d", errno);
		close (sock);
		return -1;
	}

	sas1.sin_family=AF_INET;
	sas1.sin_addr.s_addr=htonl(INADDR_ANY);
	sas1.sin_port=htons(port);

	if (bind(sock, (struct sockaddr *)&sas1, sizeof(sas1)) != 0) {
		LOG_ERROR ("Error binding server socket:%d", errno);
		close (sock);
		return -1;
	}

	if (socktype == SOCK_STREAM) {
		if (listen(sock, 4) != 0) {;
			LOG_ERROR ("Error listening:%d", errno);
			close (sock);
			return -1;
		}
	}

	return sock;
}

int get_host_address (const char *host, int port, struct sockaddr_in *sockaddr) {
	struct addrinfo *res = NULL, *p;

	char service [64];
	snprintf (service, sizeof (service), "%d", port);

	if (getaddrinfo(host, service, NULL, &res) || res == NULL) {
		LOG_ERROR ("cannot translate %s:%d to valid address: %d", host, port, errno);
		return -1;
	}

	for (p = res; p ; p = p->ai_next) {
		if (p->ai_addrlen == sizeof (struct sockaddr_in)) {
			memcpy (sockaddr, p->ai_addr, p->ai_addrlen);
			break;
		}

		res = res->ai_next;
	}
	freeaddrinfo (res);

	if (p == NULL) {
		LOG_ERROR ("cannot find valid result returned by getaddrinfo");
		return -1;
	}

	return 0;
}

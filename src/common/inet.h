/* include stuff for internet */

#ifndef _WIN32

# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>

# define closesocket close
# define set_blocking(sok) fcntl(sok, F_SETFL, 0)
# define set_nonblocking(sok) fcntl(sok, F_SETFL, O_NONBLOCK)
# define would_block() (errno == EAGAIN || errno == EWOULDBLOCK)
# define sock_error() (errno)

#else

#ifdef USE_IPV6
# include <winsock2.h>
# include <ws2tcpip.h>
# include <tpipv6.h>
#else
# include <winsock.h>
#endif

#define ECONNRESET			WSAECONNRESET
#define ETIMEDOUT			WSAETIMEDOUT
#define EISCONN				WSAEISCONN

#define set_block_state(sok, state)		{		\
		unsigned long val = state;				\
		ioctlsocket(sok, FIONBIO, &val);		\
		}

#define set_blocking(sok)		set_block_state(sok, 0)
#define set_nonblocking(sok)	set_block_state(sok, 1)

#define would_block()		(WSAGetLastError() == WSAEWOULDBLOCK)
#define sock_error			WSAGetLastError

#endif
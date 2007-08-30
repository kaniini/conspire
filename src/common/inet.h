/* include stuff for internet */

#ifdef WANTSOCKET
#include <sys/types.h>
#include <sys/socket.h>
#endif
#ifdef WANTARPA
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#ifdef WANTDNS
#include <netdb.h>
#endif
#define closesocket close
#define set_blocking(sok) fcntl(sok, F_SETFL, 0)
#define set_nonblocking(sok) fcntl(sok, F_SETFL, O_NONBLOCK)
#define would_block() (errno == EAGAIN || errno == EWOULDBLOCK)
#define sock_error() (errno)


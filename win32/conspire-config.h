#define USE_GMODULE
#define USE_PLUGIN

#define PACKAGE_NAME "conspire"
#define PACKAGE_VERSION "1.0-alpha1"

#define XCHATLIBDIR "."
#define XCHATSHAREDIR "."

#ifndef USE_IPV6
# define socklen_t int
#endif

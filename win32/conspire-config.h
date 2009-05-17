#define USE_GMODULE
#define USE_PLUGIN

#define PACKAGE_NAME "conspire"
#define PACKAGE_VERSION "1.0-alpha1"

#define CONSPIRE_LIBDIR "."
#define CONSPIRE_SHAREDIR "."

#define XCHATLIBDIR CONSPIRE_LIBDIR
#define XCHATSHAREDIR CONSPIRE_SHAREDIR

#define HAVE_SNPRINTF
#define HAVE_VSNPRINTF

#define SHARED_SUFFIX	".dll"

#ifndef USE_IPV6
# define socklen_t int
#endif

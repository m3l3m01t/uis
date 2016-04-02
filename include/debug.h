#ifndef _H_DEBUG
#define _H_DEBUG

#include <stdlib.h>

#ifdef WIN32
#define snprintf _snprintf
#endif

#ifdef __cplusplus
#define ON_PANIC() ::abort()
#else
#define ON_PANIC() abort()
#endif

#ifdef __GNUC__
#define ATTR_PACKED __attribute__ ((__packed__))
#endif

enum {
  LOG_UNKNOWN,
  LOG_DEFAULT,
  LOG_VERBOSE,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL,
};

#ifdef __GNUC__
#define FUNC_NAME __PRETTY_FUNCTION__
#else
#define FUNC_NAME __FUNCTION__
#endif

#include <android/log.h>

#define SYS_LOG(type, format, ...) __android_log_print(ANDROID_##type, FUNC_NAME, format, ## __VA_ARGS__)

#define LOG_INFO(format, ...)  SYS_LOG(LOG_INFO, "%d:"format, __LINE__, ## __VA_ARGS__)
#define LOG_WARN(format, ...)  SYS_LOG(LOG_WARN, "%d:"format, __LINE__, ## __VA_ARGS__)
#define LOG_ERROR(format, ...) SYS_LOG(LOG_ERROR, "%d:"format, __LINE__, ## __VA_ARGS__)
#define LOG_F(format, ...)     {SYS_LOG(LOG_FATAL, "%d:"format, __LINE__, ## __VA_ARGS__); ON_PANIC();}
#define LOG_DEBUG(format, ...) SYS_LOG(LOG_DEBUG, "%d:"format, __LINE__, ## __VA_ARGS__)

#define ASSERT(x) if (!(x)) { \
    LOG_F( "ASSERT %s failed", #x); \
	ON_PANIC();\
}

#define PANIC(format, ...) {                \
    LOG_F(format, ## __VA_ARGS__);     \
    ON_PANIC();                             \
}

#define PANIC_ON(x) if ((x)) {                      \
    LOG_F("%s panic %s\n", __FUNCTION__, #x);  \
    ON_PANIC();                                     \
}

#define DBGLEVEL 1000

#define DBG(level, format, ...) {               \
    if (level <= DBGLEVEL) {                    \
        LOG_DEBUG(format, ## __VA_ARGS__); \
    }                                           \
}


#endif // _H_DEBUG


#include "multi_timer.h"

/*
 * Platform-specific entry point
 */
#ifdef __unix__
#include "multi_timer_unix.c"

#elif __APPLE__
// TODO: OS X platform layer

#elif _WIN32
// TODO: Windows platform layer

#endif

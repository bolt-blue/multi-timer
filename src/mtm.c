/*
 * Platform-specific entry point
 */
#ifdef __unix__
// TODO: Do we need to split between linux and BSD?
#include "mtm_unix.c"

#elif __APPLE__
// TODO: OS X platform layer

#elif _WIN32
// TODO: Windows platform layer
#endif

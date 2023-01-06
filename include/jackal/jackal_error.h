#ifndef JACKAL_ERROR_H
#define JACKAL_ERROR_H

#include <jackal/jackal_types.h>

// normal colors
#define $R "\x1b[31m"
#define $G "\x1b[32m"
#define $Y "\x1b[33m"
#define $B "\x1b[34m"
#define $M "\x1b[35m"
#define $C "\x1b[36m"

// bold colors
#define $RB "\x1b[1;31m"
#define $GB "\x1b[1;32m"
#define $YB "\x1b[1;33m"
#define $BB "\x1b[1;34m"
#define $MB "\x1b[1;35m"
#define $CB "\x1b[1;36m"

// reset
#define $E "\x1b[0m"

#define ENABLE_COLOR 1

#if ENABLE_COLOR
  #define jkl_error(c_, f_, ...) \
    do { \
      fprintf(stderr, $R "[" c_ "]: " $E $RB f_ $E "\n", ##__VA_ARGS__); \
      exit(1); \
    } while (0);

  #define jkl_warn(c_, f_, ...) \
    do { \
      fprintf(stderr, $Y "[" c_ "]: " $E f_ "\n", ##__VA_ARGS__); \
    } while (0);

  #define jkl_log(c_, f_, ...) \
    do { \
      fprintf(stderr, $G "[" c_ "]: " $E f_ "\n", ##__VA_ARGS__); \
    } while (0);

  #define jkl_note(c_, f_, ...) \
    do { \
      fprintf(stderr, $CB "[" c_ "]: " $E $C f_ $E "\n", ##__VA_ARGS__); \
    } while (0);
#else
  #define jkl_error(c_, f_, ...) \
    do { \
      fprintf(stderr, "[" c_ "]: " f_ "\n", ##__VA_ARGS__); \
      exit(1); \
    } while (0);

  #define jkl_warn(c_, f_, ...) \
    do { \
      fprintf(stderr, "[" c_ "]: " f_ "\n", ##__VA_ARGS__); \
    } while (0);

  #define jkl_log(c_, f_, ...) \
    do { \
      fprintf(stderr, "[" c_ "]: " f_ "\n", ##__VA_ARGS__); \
    } while (0);

  #define jkl_note(c_, f_, ...) \
    do { \
      fprintf(stderr, "[" c_ "]: " f_ "\n", ##__VA_ARGS__); \
    } while (0);
#endif

#define jkl_debug(w_) \
  do { \
    fprintf(stdout, $M "debuging at %s:%d" $E "\n", __FILE__, __LINE__); \
    char c = getc(stdin); \
    if (c == 'c') { \
      fprintf(stdout, $M "Continuing..." $E "\n"); \
    } \
    if (c == 'w') { \
      w_; \
    } \
    if (c == 'q') { \
      fprintf(stdout, $M "Quitting..." $E "\n"); \
      exit(1); \
    } \
  } while (0);

#endif

#ifndef JKL_NO_CHECK_H
#define JKL_NO_CHECK_H

/*
 * Minimal, dependency-free stand-in for the subset of the "Check" unit-test
 * API used by this test suite. Enabled with -DJKL_NO_CHECK when libcheck is
 * not available. The real Check framework is used by default (see Makefile).
 */

#include <stdio.h>
#include <stdlib.h>

#define CK_NORMAL 0

/* Single shared failure counter: defined once (in main.c via
 * JKL_NO_CHECK_MAIN), referenced everywhere else. */
#ifdef JKL_NO_CHECK_MAIN
int jkl_test_failures = 0;
#else
extern int jkl_test_failures;
#endif

#define START_TEST(name) static void name(void) {
#define END_TEST }

#define ck_assert_int_eq(a, b) \
  do { \
    long _a = (long)(a), _b = (long)(b); \
    if (_a != _b) { \
      jkl_test_failures++; \
      fprintf(stderr, "  FAIL %s:%d: %s (%ld) != %s (%ld)\n", \
              __FILE__, __LINE__, #a, _a, #b, _b); \
    } \
  } while (0)

#define ck_assert_ptr_null(p) \
  do { \
    if ((p) != NULL) { \
      jkl_test_failures++; \
      fprintf(stderr, "  FAIL %s:%d: %s is not NULL\n", __FILE__, __LINE__, #p); \
    } \
  } while (0)

typedef void (*jkl_test_fn)(void);

typedef struct {
  const char *name;
  jkl_test_fn fns[256];
  int n;
} TCase;

typedef struct {
  const char *name;
  TCase *tcs[64];
  int n;
} Suite;

typedef struct {
  Suite *s;
} SRunner;

static inline Suite *suite_create(const char *name)
{
  Suite *s = (Suite *)calloc(1, sizeof(Suite));
  s->name = name;
  return s;
}

static inline TCase *tcase_create(const char *name)
{
  TCase *tc = (TCase *)calloc(1, sizeof(TCase));
  tc->name = name;
  return tc;
}

static inline void tcase_add_test(TCase *tc, jkl_test_fn fn)
{
  tc->fns[tc->n++] = fn;
}

static inline void suite_add_tcase(Suite *s, TCase *tc)
{
  s->tcs[s->n++] = tc;
}

static inline SRunner *srunner_create(Suite *s)
{
  SRunner *r = (SRunner *)calloc(1, sizeof(SRunner));
  r->s = s;
  return r;
}

static inline void srunner_run_all(SRunner *r, int mode)
{
  (void)mode;
  for (int i = 0; i < r->s->n; i++) {
    for (int j = 0; j < r->s->tcs[i]->n; j++) {
      r->s->tcs[i]->fns[j]();
    }
  }
}

static inline int srunner_ntests_failed(SRunner *r)
{
  (void)r;
  return jkl_test_failures;
}

static inline void srunner_free(SRunner *r)
{
  (void)r;   /* small allocations intentionally leaked at exit */
}

#endif

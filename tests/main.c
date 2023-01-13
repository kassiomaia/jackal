#include <jackal.h>
#include <check.h>

extern Suite *jkl_compiler_suite();

int main()
{
  Suite *s = suite_create("jkl_compiler");

  SRunner *compiler_runner = srunner_create(jkl_compiler_suite());
  srunner_run_all(compiler_runner, CK_NORMAL);

  int number_failed = srunner_ntests_failed(compiler_runner);
  srunner_free(compiler_runner);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

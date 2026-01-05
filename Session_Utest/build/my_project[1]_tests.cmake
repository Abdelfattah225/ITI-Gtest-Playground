add_test( HelloTest.BasicAssertions /home/abdo/ITI/07-gtest/build/my_project [==[--gtest_filter=HelloTest.BasicAssertions]==] --gtest_also_run_disabled_tests)
set_tests_properties( HelloTest.BasicAssertions PROPERTIES WORKING_DIRECTORY /home/abdo/ITI/07-gtest/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( my_project_TESTS HelloTest.BasicAssertions)

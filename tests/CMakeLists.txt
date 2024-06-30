file(REAL_PATH ./examples.test EXAMPLES_TEST)
file(REAL_PATH ../rere.py RERE)

add_test(
  NAME ExamplesTest
  COMMAND python3 ${RERE} replay ${EXAMPLES_TEST}
)

add_custom_target(
  record
  COMMENT "Re-record outdated tests"
  COMMAND python3 ${RERE} record ${EXAMPLES_TEST}
)

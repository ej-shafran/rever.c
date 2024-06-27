if ENABLE_TESTS
TEST_LOG_COMPILER = $(PYTHON) $(top_srcdir)/rere.py replay

TESTS = examples.test

record-local: $(TESTS)
	for test in $(TESTS) ; do $(PYTHON) $(top_srcdir)/rere.py record $$test ; done
endif

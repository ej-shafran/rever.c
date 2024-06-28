AC_PREREQ([2.71])

AC_INIT([reverc], [1.0], [ejshafran@hilma.tech])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--enable-tests], [Enable tests (default is no)])],
    [enable_tests=yes], [enable_tests=no])

# Checks for programs.
AC_PROG_CC

AS_IF([test "x$enable_tests" = "xyes"], [
    AM_PATH_PYTHON([3.0])
    AM_CONDITIONAL([ENABLE_TESTS], [true])
    AM_EXTRA_RECURSIVE_TARGETS([record])
], [
    AM_CONDITIONAL([ENABLE_TESTS], [false])
])

# Checks for libraries.
AC_SEARCH_LIBS([sqrt], [m])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT

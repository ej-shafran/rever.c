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
AC_PATH_X
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([fcntl.h inttypes.h mach/mach.h malloc.h stdint.h strings.h sys/ioctl.h sys/param.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_CHECK_FUNCS([strtoul])
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit clock_gettime floor ftruncate gettimeofday memchr memmove memset munmap pow regcomp select setenv setlocale sqrt strcasecmp strchr strcspn strdup strerror strndup strpbrk strrchr strspn strstr strtol strtoul])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])

AC_OUTPUT

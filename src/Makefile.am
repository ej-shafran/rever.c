bin_PROGRAMS = reverc-cli reverc

reverc_cli_SOURCES = cli.c reverc.c

reverc_SOURCES = gui.c reverc.c
reverc_LDADD = $(top_srcdir)/raylib/src/libraylib.a
reverc_CFLAGS = -I$(top_srcdir)/raylib/src/
reverc_CFLAGS += -DINCLUDE_RAYLIB
reverc_CFLAGS += -D'REVERC_WARNING(...)=TraceLog(LOG_WARNING, __VA_ARGS__)'

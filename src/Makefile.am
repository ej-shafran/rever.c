bin_PROGRAMS = reverc-cli reverc

reverc_cli_SOURCES = cli.c reverc.c

AM_LDFLAGS = -lm
reverc_SOURCES = gui.c reverc.c
reverc_LDADD = $(top_srcdir)/raylib/src/libraylib.a
reverc_CFLAGS = -I$(top_srcdir)/raylib/src/

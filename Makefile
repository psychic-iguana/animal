# Animal - makefile
#
# System Dependencies - edit as appropriate for your machine
#

# Unix System 5.3

CC= gcc
CFLAGS= -O
LIBS= -lcurses

# BSD
#
#CFLAGS= -O -DBOGUS_GET -DBOGUS_BOX
#LIBS= -lcurses -ltermcap
#

# Unix System 5.2 (possibly other less advanced SysV ports)
#
#CFLAGS= -O -DBOGUS_GET -DBOGUS_BOX
#LIBS= -lcurses
#

LDFLAGS= -O
OBJS= an.o scr.o
SRC= an.c scr.c
EXE= animal

$(EXE): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXE) $(LIBS)

shar:
	xshar -v README animal.6 Makefile $(SRC) example > $(EXE).shar

man:
	nroff -man animal.6 > $(EXE).man

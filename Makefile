# Animal - makefile
#
# System Dependencies - edit as appropriate for your machine
#

# Unix System 5.3

CFLAGS := -O2
LIBS= -lcurses
LDFLAGS :=
OBJS := an.o scr.o
EXE := animal

$(EXE): $(OBJS) $(EXE).man
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXE) $(LIBS)

$(EXE).man:
	nroff -man animal.6 > $(EXE).man

clean:
	rm -f $(OBJS) $(EXE) $(EXE).man

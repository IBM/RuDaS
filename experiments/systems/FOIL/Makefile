# Vanilla makefile for distribution
# You may need to set local c compiler options

CFLAGS = -g
 
.SUFFIXES: .o .c .l .ln

.c.o:	Makefile defns.i extern.i
#	lint -c $<
	cc $(CFLAGS) -c $<

.c.ln:
	lint -c $<

SRC =	global.c main.c input.c output.c state.c\
	literal.c evaluatelit.c search.c determinate.c order.c\
	join.c utility.c finddef.c interpret.c prune.c constants.c

OBJ =	global.o main.o input.o output.o state.o\
	literal.o evaluatelit.o search.o determinate.o order.o\
	join.o utility.o finddef.o interpret.o prune.o constants.o

LINT =	global.ln main.ln input.ln output.ln state.ln\
	literal.ln evaluatelit.ln search.ln determinate.ln order.ln\
	join.ln utility.ln finddef.ln interpret.ln prune.ln constants.ln


foil:   $(OBJ) Makefile
#	lint -x $(LINT) -lm >,nittygritty
	cc -o foil6 $(OBJ) -lm


foilgt:  $(SRC) defns.i Makefile
	cat defns.i $(SRC) >.temp
	egrep -v '"defns.i"|"extern.i"' .temp >foilgt.c
	cc -O3 -o foil6 foilgt.c -lm
	rm .temp foilgt.c


$(OBJ): defns.i extern.i

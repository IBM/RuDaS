############################################################################
#
#			PROGOL Makefile
#
############################################################################

.SUFFIXES: .pl .cmp
LDFLAGS= -lm # -O2
CFLAGS=  -O2 # -O2 -g  -pg -f68881
CC= gcc 
OBJECTS= main.o plg.o command.o init.o hash.o func.o set.o \
	item.o list.o utils.o alloc.o terms.o clause.o lib.o \
	integrat.o cost.o bits.o reduce.o control.o sat.o sample.o \
	interp.o deduce.o xy.o subsume.o chi.o
SOURCE= main.c plg.c command.c init.c hash.c func.c set.c \
	item.c list.c utils.c alloc.c terms.c clause.c lib.c \
	integrat.c cost.c bits.c reduce.c control.c sat.c sample.c \
	interp.c deduce.c xy.c subsume.c chi.c

progol: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o progol $(LDFLAGS)
mem:
	touch item.c init.c plg.c
	make progol
count:
	wc $(SOURCE)
clean:
	 -rm -f *.o

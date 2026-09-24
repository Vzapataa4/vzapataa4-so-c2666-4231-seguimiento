.phony: clean

SRCS=reg_eventos.c servidor4.c tuberias_nombradas.c
OBJS=$(SRCS:.c=.o)
EXECS=reg_eventos servidor4

all: ${EXECS}

reg_eventos: reg_eventos.o
	${CC} -o $@ $<

servidor4: servidor4.o tuberias_nombradas.o
	${CC} -o $@ $^

servidor4.o: servidor4.c tuberias_nombradas.h servidor.h

tuberias_nombradas.o: tuberias_nombradas.c tuberias_nombradas.h

clean:
	rm -f ${OBJS}
	rm -f ${EXECS}
	rm -f *~

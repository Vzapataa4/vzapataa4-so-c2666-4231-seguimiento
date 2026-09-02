.phony: clean

SRCS=manejar-senal.c captura-senal.c manejadores.c manejar-senal2.c senales.c
OBJS=$(SRCS:.c=.o)
EXECS=manejar-senal captura-senal manejar-senal2 senales

all: ${EXECS}

manejar-senal: manejar-senal.o
	${CC} -o $@ $<

captura-senal: captura-senal.o manejadores.o
	${CC} -o $@ $^

captura-senal.o: captura-senal.c manejadores.h

manejadores.o: manejadores.c manejadores.h

manejar-senal2: manejar-senal2.o
	${CC} -o $@ $<

senales: senales.o
	${CC} -o $@ $<

clean:
	rm -f ${OBJS}
	rm -f ${EXECS}
	rm -f *~

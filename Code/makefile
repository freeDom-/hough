IDIR=include
ODIR=obj
CC=gcc
CFLAGS=-I$(IDIR) -std=c11 -g
LIBS=-lSDL2 -lSDL2_image -lm

_DEPS=grayscaler.h gauss.h canny.h hough.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o grayscaler.o gauss.o canny.o hough.o
_CONVOBJ=convert.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
OMPOBJ=$(patsubst %,$(ODIR)/omp_%,$(_OBJ))
CONVOBJ=$(patsubst %,$(ODIR)/omp_%,$(_CONVOBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)

$(ODIR)/omp_%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS) -fopenmp

hough: $(OBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) $(LIBS)

hough_omp: $(OMPOBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) -fopenmp $(LIBS)

convert: $(CONVOBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) $(LIBS)

all: hough hough_omp convert

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o hough hough_omp convert ../img/gen/*.png ../img/gen/*.dat

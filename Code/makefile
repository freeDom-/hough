IDIR=include
ODIR=obj
CC=gcc
CFLAGS=-I$(IDIR) -std=c11 -g
LIBS=-lSDL2 -lSDL2_image -lm

_DEPS=grayscaler.h gauss.h canny.h hough.h dimensions.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o grayscaler.o gauss.o canny.o hough.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
OMPOBJ=$(patsubst %,$(ODIR)/omp_%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)

$(ODIR)/omp_%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS) -fopenmp

hough: $(OBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) $(LIBS)

hough_omp: $(OMPOBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) -fopenmp $(LIBS)

all: hough hough_omp

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o hough hough_omp ../img/gen/*.png

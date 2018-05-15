IDIR=include
ODIR=obj
CC=gcc
CFLAGS=-I$(IDIR) -std=c11 -fopenmp
LIBS=-lSDL2 -lSDL2_image -lm

_DEPS=grayscaler.h gauss.h canny.h hough.h dimensions.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ=main.o grayscaler.o gauss.o canny.o hough.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)

hough: $(OBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o hough ../img/grayscale.png ../img/gauss.png ../img/sobel.png ../img/hough.png

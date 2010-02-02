main.out: driver.cpp ReadImage.o ReadImageHeader.o WriteImage.o image.h image.o curses_io_V13.h
	g++ -lncurses -g -o main.out image.o ReadImage.o ReadImageHeader.o WriteImage.o driver.cpp

ReadImage.o: image.h ReadImage.cpp
	g++ -c ReadImage.cpp

ReadImageHeader.o: image.h ReadImageHeader.cpp
	g++ -c ReadImageHeader.cpp

WriteImage.o: image.h WriteImage.cpp
	g++ -c WriteImage.cpp

image.o: image.h image.cpp
	g++ -c image.cpp

clean:
	rm *.o main.out

.PHONY: clean


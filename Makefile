main.out: driver.cpp image.o cubicSpline.o imageIO.o curses_io_V13.h comp_curses.h
	g++ -lncurses -g -o main.out image.o imageIO.o cubicSpline.o driver.cpp

cubicSpline.o: cubicSpline.cpp cubicSpline.h
	g++ -c -g cubicSpline.cpp

imageIO.o: imageIO.h imageIO.cpp image.h image.cpp
	g++ -c -g imageIO.cpp image.cpp

image.o: image.h image.cpp
	g++ -c -g image.cpp

clean:
	rm *.o main.out

.PHONY: clean


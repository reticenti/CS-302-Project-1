main.out: driver.o image.o cubicSpline.o imageIO.o comp_curses.o
	g++ -lncurses -g -o main.out driver.o image.o imageIO.o cubicSpline.o comp_curses.o

driver.o: driver.cpp image.h comp_curses.h cubicSpline.h imageIO.h
	g++ -c -lncurses -g driver.cpp

comp_curses.o: comp_curses.cpp comp_curses.h
	g++ -c -lncurses -g comp_curses.cpp

cubicSpline.o: cubicSpline.cpp cubicSpline.h
	g++ -c -g cubicSpline.cpp

imageIO.o: imageIO.h imageIO.cpp image.h image.cpp
	g++ -c -g imageIO.cpp image.cpp

image.o: image.h image.cpp
	g++ -c -g image.cpp

clean:
	rm *.o main.out

.PHONY: clean


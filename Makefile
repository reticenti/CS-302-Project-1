main.out: driver.o cubicSpline.o imageIO.o comp_curses.o rgb.o RegionType.o
	g++ -lncurses -g -o main.out driver.o imageIO.o cubicSpline.o comp_curses.o rgb.o RegionType.o

driver.o: driver.cpp image.h comp_curses.h cubicSpline.h imageIO.h queue.h list.h sortedList.h RegionType.h
	g++ -c -lncurses -g driver.cpp

RegionType.o: RegionType.h RegionType.cpp list.h
	g++ -c -g RegionType.cpp

comp_curses.o: comp_curses.cpp comp_curses.h
	g++ -c -lncurses -g comp_curses.cpp

cubicSpline.o: cubicSpline.cpp cubicSpline.h
	g++ -c -g cubicSpline.cpp

imageIO.o: imageIO.h imageIO.cpp image.h
	g++ -c -g imageIO.cpp

rgb.o: rgb.cpp rgb.h
	g++ -c -g rgb.cpp

clean:
	rm *.o main.out

.PHONY: clean


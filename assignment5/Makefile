part1.out : part1.cpp ../queue.h binaryTree.h user.h part2.out
	g++ -g -o part1.out part1.cpp

part2.out : part2.cpp heap.h pqueue.h U_PQType.h
	g++ -g -o part2.out part2.cpp

clean:
	rm part1.out part2.out documentation.toc documentation.log documentation.aux documentation.out

.PHONY: clean


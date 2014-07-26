FLAGS=-g -Wall -pedantic -std=c++11

collide.o: collide.hpp collide.cpp
	g++ $(FLAGS) -c collide.cpp -o collide.o

test_collide: collide.o test.cpp
	g++ $(FLAGS) test.cpp collide.o -lboost_unit_test_framework -o test_collide

tests.ok: test_collide
	./test_collide && touch tests.ok

collide: collide.o main.cpp tests.ok
	g++ $(FLAGS) main.cpp collide.o -o collide

clean:
	rm -f collide.o test_collide collide tests.ok

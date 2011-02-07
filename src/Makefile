all: cirth

ast.o: ast.h ast.cpp common.h
	clang++ -isystem /home/jt/dev/boost_1_45_0 -c ast.cpp

parser.o: parser.cpp parser.h common.h ast.h
	clang++ -isystem /home/jt/dev/boost_1_45_0 -c parser.cpp

main.o: main.cpp common.h parser.h ast.h
	clang++ -isystem /home/jt/dev/boost_1_45_0 -c main.cpp

cirth: main.o parser.o ast.o
	clang++ -isystem /home/jt/dev/boost_1_45_0 -o cirth main.o ast.o parser.o

#test: parser.cpp ast.h common.h run_tests.cpp
#	clang

clean:
	rm -f *.o cirth

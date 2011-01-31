all: parser

parser: src/parser.cpp src/ast.h src/common.h
	clang++ -isystem /home/jt/dev/boost_1_45_0 -o src/parser src/parser.cpp

clean:
	rm -f src/*.o src/parser

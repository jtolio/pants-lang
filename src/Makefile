CPP = clang++
CFLAGS = -isystem /home/jt/dev/boost_1_45_0

all: cirth

assets.h assets.cpp: assets/* generate_assets.py
	./generate_assets.py assets

ast.o: ast.h ast.cpp common.h
	$(CPP) $(CFLAGS) -c ast.cpp

parser.o: parser.cpp parser.h common.h ast.h
	$(CPP) $(CFLAGS) -c parser.cpp

wrap.o: wrap.cpp wrap.h common.h ir.h
	$(CPP) $(CFLAGS) -c wrap.cpp

assets.o: assets.h assets.cpp

compile.o: compile.cpp common.h cps.h assets.h ir.h ast.h
	$(CPP) $(CFLAGS) -c compile.cpp

assets.o: assets.h assets.cpp
	$(CPP) $(CFLAGS) -c assets.cpp

main.o: main.cpp common.h parser.h ast.h wrap.h ir.h cps.h assets.h compile.h
	$(CPP) $(CFLAGS) -c main.cpp

ir.o: ir.cpp ir.h ast.h common.h
	$(CPP) $(CFLAGS) -c ir.cpp

cps.o: cps.cpp cps.h common.h ir.h
	$(CPP) $(CFLAGS) -c cps.cpp

run_tests.o: run_tests.cpp common.h parser.h ast.h wrap.h ir.h cps.h compile.h
	$(CPP) $(CFLAGS) -c run_tests.cpp

cirth: main.o parser.o ast.o wrap.o ir.o cps.o compile.o assets.o
	$(CPP) $(CFLAGS) -o cirth main.o ast.o parser.o wrap.o ir.o cps.o compile.o assets.o

run_tests: run_tests.o parser.o ast.o wrap.o ir.o cps.o compile.o assets.o
	$(CPP) $(CFLAGS) -o run_tests run_tests.o ast.o parser.o wrap.o ir.o cps.o compile.o assets.o -lcppunit -ldl

test: run_tests
	./run_tests

clean:
	rm -f *.o cirth run_tests assets.h assets.cpp

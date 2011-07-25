CPP = g++
CFLAGS = -isystem /home/jt/dev/boost_1_45_0

all: pants

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

main.o: main.cpp common.h parser.h ast.h wrap.h ir.h cps.h assets.h compile.h optimize.h
	$(CPP) $(CFLAGS) -c main.cpp

ir.o: ir.cpp ir.h ast.h common.h
	$(CPP) $(CFLAGS) -c ir.cpp

cps.o: cps.cpp cps.h common.h ir.h
	$(CPP) $(CFLAGS) -c cps.cpp

optimize.o: optimize.cpp optimize.h cps.h ir.h common.h
	$(CPP) $(CFLAGS) -c optimize.cpp

run_tests.o: run_tests.cpp common.h parser.h ast.h wrap.h ir.h cps.h compile.h optimize.h
	$(CPP) $(CFLAGS) -c run_tests.cpp

pants: main.o parser.o ast.o wrap.o ir.o cps.o compile.o assets.o optimize.o
	$(CPP) $(CFLAGS) -o pants main.o ast.o parser.o wrap.o ir.o cps.o compile.o assets.o optimize.o -ldl

run_tests: run_tests.o parser.o ast.o wrap.o ir.o cps.o compile.o assets.o optimize.o
	$(CPP) $(CFLAGS) -o run_tests run_tests.o ast.o parser.o wrap.o ir.o cps.o compile.o assets.o optimize.o -lcppunit -ldl

test: run_tests
	./run_tests

clean:
	rm -f *.o pants run_tests assets.h assets.cpp

CPP = clang++
CFLAGS = -isystem /home/jt/dev/boost_1_45_0

all: cirth

ast.o: ast.h ast.cpp common.h
	$(CPP) $(CFLAGS) -c ast.cpp

parser.o: parser.cpp parser.h common.h ast.h
	$(CPP) $(CFLAGS) -c parser.cpp

wrap.o: wrap.cpp wrap.h common.h ir.h
	$(CPP) $(CFLAGS) -c wrap.cpp

main.o: main.cpp common.h parser.h ast.h wrap.h ir.h cps_transform.h
	$(CPP) $(CFLAGS) -c main.cpp

ir.o: ir.cpp ir.h ast.h common.h
	$(CPP) $(CFLAGS) -c ir.cpp

cps_transform.o: cps_transform.cpp cps_transform.h common.h ir.h
	$(CPP) $(CFLAGS) -c cps_transform.cpp

run_tests.o: run_tests.cpp common.h parser.h ast.h wrap.h ir.h cps_transform.h
	$(CPP) $(CFLAGS) -c run_tests.cpp

cirth: main.o parser.o ast.o wrap.o ir.o cps_transform.o
	$(CPP) $(CFLAGS) -o cirth main.o ast.o parser.o wrap.o ir.o cps_transform.o

test: run_tests.o parser.o ast.o wrap.o ir.o cps_transform.o
	$(CPP) $(CFLAGS) -o run_tests run_tests.o ast.o parser.o wrap.o ir.o cps_transform.o -lcppunit -ldl
	./run_tests

clean:
	rm -f *.o cirth run_tests

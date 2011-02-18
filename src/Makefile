CPP = clang++
CFLAGS = -isystem /home/jt/dev/boost_1_45_0

all: cirth

ast.o: ast.h ast.cpp common.h
	$(CPP) $(CFLAGS) -c ast.cpp

parser.o: parser.cpp parser.h common.h ast.h
	$(CPP) $(CFLAGS) -c parser.cpp

wrap.o: wrap.cpp wrap.h common.h ast.h
	$(CPP) $(CFLAGS) -c wrap.cpp

main.o: main.cpp common.h parser.h ast.h
	$(CPP) $(CFLAGS) -c main.cpp

pre_cps_ir.o: pre_cps_ir.cpp pre_cps_ir.h ast.h common.h
	$(CPP) $(CFLAGS) -c pre_cps_ir.cpp

run_tests.o: run_tests.cpp common.h parser.h ast.h
	$(CPP) $(CFLAGS) -c run_tests.cpp

cirth: main.o parser.o ast.o wrap.o pre_cps_ir.o
	$(CPP) $(CFLAGS) -o cirth main.o ast.o parser.o wrap.o pre_cps_ir.o

test: run_tests.o parser.o ast.o wrap.o pre_cps_ir.o
	$(CPP) $(CFLAGS) -o run_tests run_tests.o ast.o parser.o wrap.o pre_cps_ir.o -lcppunit -ldl
	./run_tests

clean:
	rm -f *.o cirth run_tests

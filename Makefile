all: cirth

cirth:
	cd src && make cirth

test: cirth
	cd src && make test
	cd tests && ../tools/run_tests.py

clean:
	cd src && make clean

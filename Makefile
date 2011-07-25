all: pants

pants:
	cd src && make pants

test: pants
	cd src && make test
	cd tests && ../tools/run_tests.py

clean:
	cd src && make clean

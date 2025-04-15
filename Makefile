default: ringmaster

ringmaster: src/ringmaster.c src/functions.c
	gcc -o ringmaster src/ringmaster.c src/functions.c

grade: 
	python3 test/grader.py ./ringmaster test-cases


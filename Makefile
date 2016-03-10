wordc: wordc.c
	gcc -o wordc wordc.c -std=c99
clean:
	rm -f wordc.c wordc

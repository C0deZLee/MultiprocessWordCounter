wordc: wordc_mp.c
	gcc -o wordc wordc_mp.c -std=c99
clean:
	rm -f wordc_mp.c wordc

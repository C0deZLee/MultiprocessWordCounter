CS311 Project2. Authors: Jesse Rong (jbr5205) Sec 001 and Xiang(Steve) Li (xpl5016) Sec 001

This program is for the CMPSC 311 Project 2. It's a word counter that uses processes.
This program is written in C.


## Discription
*	Our C program uses a linked list of structures to organize words. The main function tokenizes the words in the file and counts them with functions. The parent process creates the children and the tokens are divided equally among the number of processes. Once the children are done with the tokens, they use pipes to transfer the word and word count seperately to the parent.  

## Difficulties: 
* We were not very familiar with pipes. Learning more about pipes, read(), write(), and file descriptors took a lot of time. 
* It took some time to realize that for file descriptors, you would need something similar to a 2D array. And that it should be like fd[number of processes][0 or 1] instead of fd[0 or 1][number of processes].
* For read(), Steve had a difficult time finding the buffer was too small for taking data from the pipe.
* Some times the free() function caused errors and we don't know why. 

##System calls/Library functions
* stdio.h
* stdlib.h
* sys/types.h
* ctype.h
* string.h
* unistd.h
* sys/time.h

##Interesting design choices
* We used a linked list of structures to hold the words and word count. Since project 1 used linked lists, we knew how to count and sort the words. It was easier to organize compared with passing aaround a file. 

## Performance results:
* For wordc, the average runtime  on my computer is
  * Pangur Ban:     996.15 ms
  * Hamlet:         655039.3 ms
  * Arabian nights:  28229117.71 ms
* For wordc-mp n = 2
	* Pangur Ban:   741   ms
	* Hamlet:    158626 ms
	* Arabian nights: 28604866.69 ms
* For wordc-mp n = 4
	* Pangur Ban:   971  ms
	* Hamlet: 111883 ms
	* Arabian nights:  ms
* For wordc-mp n = 8
	* Pangur Ban:  1494.25    ms
	* Hamlet: 84051  ms
	* Arabian nights:  ms

For wordc-mp n = 2, it was faster. For n=4, it was closer to the speed of wordc. But for n=8, it was slower. 

## Sources used 
The links from the internet and StackOverflow we used:

* http://www.gdsw.at/languages/c/programming-bbrown/c_075.htm
* https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html
* http://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
* http://codewiki.wikidot.com/c:system-calls:read
* http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

## Usage
For C:

```
$ make
$ ./wordc-mp input-textfile output-countfile output-runtime n
```



## License
MIT License

CS311 Project2. Authors: Jesse Rong (jbr5205) Sec 001 and Xiang(Steve) Li (xpl5016) Sec 001

This program is for the CMPSC 311 Project 2. It's a word counter that uses processes.
This program is written in C.


## Discription
*	Our C program: wordc-mp.c is --- lines long. 
*	It uses a linked list of structures to organize words. The main function tokenizes the words in the file and counts them with functions. The parent process creates the children and the tokens are divided equally among the number of processes. Once the children are done with the tokens, they use pipes to transfer the word and word count seperately to the parent.  
* Difficulties: 

## Performance results:
* For wordc, the average runtime on my computer is
  * Pangur Ban:  ms
  * Hamlet:  ms
  * Arabian nights:  ms
* For wordc-mp
	* Pangur Ban:  ms
	* Hamlet: 	ms
	* Arabian nights:  ms

## Source
The links from google and StackOverflow we used are listed below:

* http://www.gdsw.at/languages/c/programming-bbrown/c_075.htm
* https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/pipe.html
* 
* 
* 
* 

## Usage
For C:

```
$ make
$ ./wordc-mp input-textfile output-countfile output-runtime n
```



## License
MIT License

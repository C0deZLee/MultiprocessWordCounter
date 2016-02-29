
# CS311 Project2
This program is for the CMPSC 311 Project 1. It's a word counter.

This program is written in both C and Bash.

## Author
Jesse Rong (jbr5205) Sec 001

Xiang(Steve) Li (xpl5016) Sec 001

## Discription
*	Our C program: wordc.c is 172 lines long. It will atomically insert the words to right position when the function add_to_list is called.
*	Our shell program: wordc.sh is 32 lines long. It first checks for an incorrect number of arguments. Then it makes all spaces into new lines, everything lowercase, and removes all non letters and digits. Awk is used to sort the words.
* For writing wordcount, shell was much simpler to use than C. Our C program has over 100 lines compared to around 30 lines in shell. We had to find many functions for C, while we used relatively few for shell. While shell had a very simple interface, we didn't need many lines of code. In contrast, C had an interface with more options, but we needed to write a lot of code for it.
* Difficulties: We were not used to using C to code. Xiang encountered difficulties in keeping track of points and memory allocation. There were multiple errors with memory leaks and it was very frustrating trying to debug. The fact that malloc can't have an argument of sizeof of string was a problem. Jesse was new to UNIX, so he faced some learning curves about how streams worked and just general understanding. Jesse used Google a lot to understand bash. 
* Also, a lot of students needed help at office hours, so it was somewhat hard to get the TA to spend enough time for everyone. 

## Performance results:
* For C, the average runtime on my computer is
  * Pangur Ban: 212.39 ms
  * Hamlet: 560584.37 ms
  * Arabian nights: 24229117.71 ms
* For Bash
	* Pangur Ban: 0.06169564165 s
	* Hamlet: 0.1865099514	s
	* Arabian nights: 1.002866459 s

## Source
We got a lot of help from google and StackOverflow. The links we used are listed below:

* http://stackoverflow.com/questions/18568706/checking-number-of-arguments-bash-script
* http://stackoverflow.com/questions/28628667/word-count-using-awk
* http://stackoverflow.com/questions/7990750/escape-single-quote
* http://www.grymoire.com/Unix/Sed.html#uh-1
* http://stackoverflow.com/questions/9630203/what-does-do-in-bash-aka-hash-dollar-sign-pound-dollar-sign
* http://www.thegeekstuff.com/2010/06/bash-if-statement-examples/
There is another link from StackOverflow in which helped with our C program’s lowercase and punctuation removal, but I lost it.

## Usage
For C:

```
$ make
$ ./wordc input-textfile output-countfile output-runtime
```

For Bash:

```
$ ./wordc.sh input-textfile output-countfile output-runtime
```

## License
MIT License

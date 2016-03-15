
//see https://www.youtube.com/watch?v=7bNMkvcOKlc, http://tldp.org/LDP/lpg/node11.html
//http://stackoverflow.com/questions/2784500/how-to-send-a-simple-string-between-two-programs-using-pipes
//http://linux.die.net/man/3/open
//http://stackoverflow.com/questions/878494/multiple-child-process-reading-from-a-stream

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	//fd = file descriptor 
	
	int fd[2];
	char buffer[20];
	int n, i, j, status;
	int TotalNumberofWords;

	n = atoi(argv[1]);//number of children - 1
    pid_t pid[n];
	int PartialNumberofWords = TotalNumberofWords / n;

	//int fd = open(Input_file, O_RDONLY);//O_RDONLY for reading only
		//open() connects file and file descriptor

	if (pipe(fd) == -1){//if pipe() returns -1, there's an error 
		perror("pipe");
		exit(1);
	}

	for (i = 1; i < n-1; i++){//from textbook
		if((pid[i] = fork())<= 0)//creates all children
		break;
	}

	char wordsToSort[n];
	for (i = 1; i < n - 1; i++){
	//this will seperate the inputfile into different parts
	}


	for (i = 1; i < n-1; i++){
		if (pid[i] == 0){
			//child code
			wordsToSort = TotalNumberofWords - n*PartialNumberofWords;

			for (j = 0; j< PartialNumberofWords; j++){
				fscanf(inputFile, "%d", &arrayPartialNumberofWords[j]);
				//sort here
			}

			exit(0);
		}
	}

	/*The parent needs to empty the pipe before wait. 
	This is because the pipe can only fit 8kb. 
	If the pipe was full, but there was still data to be input, 
	there would be deadlock. */

	while (wait(&status)>0); //waits for all children

	if(pid != 0){
		//parent process
		for (i = 1; i < n; i++){
			read(fd[0], buffer, sizeof(buffer));//0 is for reading.
			printf("buffer: %s\n", buffer);
		}
	}

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


/* structure definition */
typedef struct word_count {
    char *word;
    int count;
    struct word_count *last;
    struct word_count *next;
} word_count;

/* Global variable */
word_count *HEAD = NULL;
word_count *TAIL = NULL;

/* reate a new list if we don't have a list */
int create_list(char *val) {
    struct word_count *curr = (word_count*) malloc (sizeof(word_count));
    curr->word = val, curr->count = 1, curr->last = NULL, curr->next = NULL;

    HEAD = curr;
    TAIL = curr;

    return 0;
}

/* add a new node to the RIGHT POSITION of the list */
int add_to_list(char *val) {
    if (HEAD == NULL)
        return (create_list(val));

    struct word_count *curr = (word_count*) malloc (sizeof(word_count));
    curr->word = val, curr->count = 1, curr->last = NULL, curr->next = NULL;

    for(struct word_count *search=HEAD; search!=NULL; search=search->next){            // start at the first element
        if (strcmp (curr->word, search->word) < 0){                                    // put front
            curr->next = search;
            curr->last = search->last;
            if(search->last != NULL) {
                search->last->next = curr;
                search->last = curr;
            }
            else {
                search->last = curr;
                HEAD = curr;
            }
            return 0;
        }

        if(strcmp (curr->word, search->word)>0 &&
           (search->next == NULL || strcmp(curr->word, search->next->word)<0)){  // put behind
            curr->next = search->next;
            curr->last = search;
            if(search->next != NULL) {
                search->next->last = curr;
                search->next = curr;
            }
            else {
                search->next = curr;
                TAIL = curr;
            }
            return 0;
        }
    }
    return -1;
}

/* search the word in current list */
int search_in_list(char *val) {

    if (HEAD == NULL)
        return (create_list(val));

    for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next) {// search from the HEAD
        if (strcmp(curr->word, val) == 0) {
            curr->count++;
            return 0;
        }
    }
    add_to_list(val);                                                     // not in list then add to list

    return 0;
}

/* Remove punctuation and make all lowercase */
char *word_format(char *raw_str) {

    char *formatted = (char *) malloc(strlen(raw_str)+1), *final = formatted;

    while (*raw_str) {
        if (ispunct(*raw_str))                                           // skip it
            raw_str++;
        else if (isupper(*raw_str))
            *formatted++ = tolower(*raw_str++);                          // make it lowercase
        else
            *formatted++ = *raw_str++;                                   // increment both pointers and copying
    }
    return final;
}

/* print the result into files */
void print_result(char *result_filename, char *runtime_filename, long runtime){
    FILE *fp;                                                           // create the file pointer
    fp = fopen(result_filename, "w+");                                  // print the result into result_filename
    for(struct word_count *curr = HEAD;curr != NULL;curr = curr->next)
        fprintf(fp,"%s, %d\n", curr->word, curr->count);

    fp = fopen(runtime_filename, "w+");                                 // print the runtime into runtime_filename
    fprintf(fp,"The total runtime is %ld\n-------------------", runtime);
    fclose(fp);                                                         // close the file
}

/* main function start here */
int main(int argc, char *argv[]) {
/*TEMP:if ((argc == 2 && (argv[1] == "--help" || argv[1] == "-h")) || argc == 1){  // help
        printf("./wordc-mp input-textfile output-countfile output-runtime num-of-processes\n");
        exit(1);
    }
    if (argc != 5){
        printf("Expected 4 arguments, given %d", argc-1);
        exit(1);
    }*/

    struct timeval start, end;                                         // set the start time
    gettimeofday(&start, NULL);

    //TEMP:FILE *fp = fopen(argv[1], "r");                             // open the given file
    FILE *fp = fopen("b.txt", "r");
    if (fp == NULL) {                                                  // exit if failed to open file
        perror("Failed to open file");
        exit(1);
    }

    //This finds out the number of bytes in the file. size is the number of total bytes.

    /*TEMP: fseek(argv[1], 0, SEEK_END);   // seek to end of file. Taken from http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
    int size = ftell(argv[1]);             // get current file pointer
    fseek(argv[1], 0, SEEK_SET);           // seek back to beginning of file
                                           // proceed with allocating memory and reading the file
    */

    fseek(fp, 0L, SEEK_END);                                           // seek to end of file. Taken from http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
    int size = (int) ftell(fp) + 1;                                    // get current file pointer
    fseek(fp, 0L, SEEK_SET);                                           // seek back to beginning of file

    char *raw_str = (char *) malloc (128 * sizeof(char)),              // save the unformatted word
            **tokenized_file = (char **) malloc (size * sizeof(char*));   //<--Would this be better? A: yes, it is! and btw this is the right format

	int total_num_of_words = 0,                                        // get the total length of the file
		status,                                                        // the status automatically points to the exit position of child process
		//TEMP: partial_num_of_words = total_num_of_words/(int)argv[4];// get the partial size for every child process
		partial_num_of_words;

	int **fd;//this is an array of pointers. Every process has fd[0-2]. A way to identify pipes is to have fd[0-2][0-n]

	fd = (int**)malloc(4 * sizeof(int*));//Allocates space for 4 pointers
	//TEMP: fd = (int**)malloc( argv[4] * sizeof(int*));//Allocates space for 4 pointers
	for (int i = 0; i < 4 - 1; i++)
	{
		fd[i] = (int*)malloc(sizeof(int));//2?
		pipe(fd[i]);
	}
	/*
	TEMP: for (int i = 0; i < argv[4]; i++)
	{
		fd[i] = (int*)malloc(2 * sizeof(int));
		pipe(fd[i]);
	}*/

    pid_t pid;                                                         // set up multi-process


    while(fscanf(fp, "%s", raw_str) != EOF) {                          // tokenize the file and save it to tokenized_file
        tokenized_file[total_num_of_words] = word_format(raw_str);     // format the word
        total_num_of_words++;                                          // get the number of total words in file
    }
    partial_num_of_words = total_num_of_words/3;



    for (int i = 1; i <= 3; i++) {
        //TEMP:for (int i=1; i<=(int)argv[4]; i++) {                   // the 4th arg is number of processes
		pid = fork();                                                  // create new process

        if (pid == 0) {                                                // This is a CHILD process
            for (int j = partial_num_of_words*i; j < partial_num_of_words*(i + 1); j++) {
                search_in_list(tokenized_file[j]);
            }
            printf("This is a CHILD process, %d \n", i);
            // TODO: pipe the result to parent
			close(fd[0][i]); //close read end
            // Steve: the sorted list is not a file, it is a linked list, and each process has one (they are independent),
            //        you can access the linked list by HEAD->next, HEAD->word
            //        one possible solution is send the HEAD pointer through pipe

			/*
			 for (struct word_count *curr = HEAD; curr->next != NULL; curr = curr->next){
				write(fd[1][i-1], curr , sizeof(curr)); 
			 }
			*/
			 
			for (struct word_count *curr = HEAD; curr->next != NULL; curr = curr->next){
				char num[70] = curr->count + '0';    //By adding '0' a number becomes a char http://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
				                                     //There should be no number with more than 70 digits
				/*char *num;
				sprintf(num, "%d", curr->count);//takes count, makes into a char, sends it to num
				This is just another way to make int into char*/

				write(fd[0][i], curr->word, strlen(curr->word));      //sends word
				write(fd[0][i], num, strlen(num));                    //sends number
			}
			 
			 close(fd[1][i]); //close write end
				 //when read looks like word1\0 5\0 so read stops every time when it hits null

            exit(0);                                                   // exit the child process since it done all work
        }

        else if (pid < 0) {                                            // not a child, not a parent, then error
            printf("Failed to create new processes");
            exit(1);
        }
    }

    if (pid > 0) {                                                     // This is the PARENT process
        for (int j=0; j<partial_num_of_words; j++) {                   // first, parent sort first part, which is tokenized_file[0] - tokenized_file[partial_num_of_words]
            search_in_list(tokenized_file[j]);
        }

        while (wait(&status)>0);                                       // then wait all children done their work
        printf("This is the PARENT process\n");
        // TODO: get the result from child
		int bytesRead;
		char buffer[10000*sizeof(HEAD)];                   //must be able to hold the nodes passed
		close(fd[1][0]); //close write end of parent
        for (int i = 1; i <= 3; i++) {
			//read itself is blocking
            //TEMP:for (int i=1; i<=(int)argv[4]; i++) {                   // the 4th arg is number of processes
			if ((bytesRead = read(fd[0][0], buffer, strlen(buffer))) >= 0){ //size is the bytes of the file.
				//read returns the number of bytes read. It returns 0 at end-of-file
				char word, number;
				word = buffer[0];
				number = buffer[1];
				word_count newWord;
				newWord->w
					add_to_list(newWord);
				//reads a number of bytes from the file associated with fd and places the characters read into buffer
			}
        }
		close(fd[0][0]); //close read end of parent
        // TODO: put them together
        // TODO: sort
        // TODO: print result
    }


    gettimeofday(&end, NULL);                                          // get the total runtime
    long runtime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

    //TEMP:print_result(argv[2], argv[3], runtime);                    // print the result

    fclose(fp);                                                        // close the file
    //TEMP: printf("Done. Total runtime: %ld\nThe result is in %s, and the runtime is in %s\n", runtime, argv[2], argv[3]);
    printf("runtime %ld", runtime);

    //free(raw_str);                                                   // Deallocates the allocations
    //free(*tokenized_file);                                           // caused crash for some reason
	/*
	TEMP: for (int i = 0; i < argv[4]; i++)
	{
	free(fd[i]) 
	}*/
	//free(*fd);

    return 0;
}

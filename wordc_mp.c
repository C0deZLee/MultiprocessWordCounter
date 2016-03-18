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
int TOTAL_PROCESS_NUM = 4;

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

    struct word_count *s_val = (word_count*) malloc (sizeof(word_count));
    s_val->word = val, s_val->count = 1, s_val->last = NULL, s_val->next = NULL;        // create new struct

    for(struct word_count *curr=HEAD; curr!=NULL; curr=curr->next) {                    // start at the first element
        if (strcmp (s_val->word, curr->word) < 0) {                                     // s_val<curr, insert front
            s_val->next = curr;
            s_val->last = curr->last;
            if(curr->last != NULL) {
                curr->last->next = s_val;
                curr->last = s_val;
            }
            else {
                curr->last = s_val;
                HEAD = s_val;
            }
            return 0;
        }

        else if(strcmp (s_val->word, curr->word)>0 &&
           (curr->next == NULL || strcmp(s_val->word, curr->next->word)<0)) {       // s_val>curr, insert behind
            s_val->next = curr->next;
            s_val->last = curr;
            if(curr->next != NULL) {
                curr->next->last = s_val;
                curr->next = s_val;
            }
            else {
                curr->next = s_val;
                TAIL = s_val;
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

/* merge the lists together*/
int merge_list(word_count *s_val){
    for(struct word_count *curr = HEAD; curr != NULL; curr = curr->next){
        if(strcmp (s_val->word, curr->word) == 0) {                          // val == curr
            curr->count += s_val->count;
        }
        else if(strcmp (s_val->word, curr->word) < 0) {                     // val < curr
            s_val->next = curr;
            s_val->last = curr->last;
            if(curr->last != NULL) {
                curr->last->next = s_val;
                curr->last = s_val;
            }
            else {
                curr->last = s_val;
                HEAD = s_val;
            }
            return 0;
        }

        else if(strcmp (s_val->word, curr->word)>0 &&
                (curr->next == NULL || strcmp(s_val->word, curr->next->word)<0)) {       // s_val>curr, insert behind
            s_val->next = curr->next;
            s_val->last = curr;
            if(curr->next != NULL) {
                curr->next->last = s_val;
                curr->next = s_val;
            }
            else {
                curr->next = s_val;
                TAIL = s_val;
            }
            return 0;
        }
    }
}

/* Remove punctuation and make all lowercase */
char *word_format(char *raw_str) {

    char *formatted = (char *) malloc(strlen(raw_str)+1), *final = formatted;

    while (*raw_str) {
        if (ispunct(*raw_str))                                           // skip it
            raw_str++;
        else *formatted++ = (char) (isupper(*raw_str) ? tolower(*raw_str++) : *raw_str++);// increment both pointers and copying
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
         **tokenized_file = (char **) malloc (size * sizeof(char*));   // save the file into a string array

	int total_num_of_words = 0,                                        // get the total length of the file
		status,                                                        // the status automatically points to the exit position of child process
		partial_num_of_words;                                          // get the partial size for every child process

	int **fd;                                                          // this is an array of pointers. Every process has fd[0-2]. A way to identify pipes is to have fd[0-2][0-n]
    fd = (int**)malloc(TOTAL_PROCESS_NUM * sizeof(int*));              // allocates space for needed pointers

	//TEMP: fd = (int**)malloc( argv[4] * sizeof(int*));// allocates space for needed pointers

	for (int i = 0; i < TOTAL_PROCESS_NUM - 1; i++) {
		fd[i] = (int*)malloc(2 * sizeof(int));
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
	partial_num_of_words = total_num_of_words / TOTAL_PROCESS_NUM;
	//TEMP: partial_num_of_words = total_num_of_words / (int)argv[4];


	for (int i = 1; i <= TOTAL_PROCESS_NUM - 1; i++) {                 //the number of children is TOTAL_PROCESS_NUM - 1
        //TEMP:for (int i=1; i<=(int)argv[4] -1; i++) {                // the 4th arg is number of processes

		pid = fork();                                                  // create new process

        if (pid == 0) {                                                // This is a CHILD process
            for (int j = partial_num_of_words*i; j < partial_num_of_words*(i + 1); j++) {
                search_in_list(tokenized_file[j]);
            }

            printf("This is a CHILD process, %d \n", i);
			close(fd[i][0]); //close read end
			 
			for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next){
                write(fd[i][0], curr, strlen(curr));
                //char *num = (char *) (curr->count + '0');    //By adding '0' a number becomes a char http://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
				                                                        //There should be no number with more than 70 digits


				//write(fd[i][0], curr->word, strlen(curr->word));      //sends word
				//write(fd[i][0], num, strlen(num));                    //sends number
			}
			 write(fd[i][0], NULL, sizeof(NULL));
			 close(fd[i][1]); //close write end
				 //when read looks like word1\0 5\0 so read stops every time when it hits null
            exit(0);                                                   // exit the child process since it done all work
        }



        else if (pid < 0) {                                            // not a child, not a parent, then error
            printf("Failed to create new processes");
            exit(1);
        }
    }
    // here below is parent
    for (int j=0; j<partial_num_of_words; j++) {                   // first, parent sort first part, which is tokenized_file[0] - tokenized_file[partial_num_of_words]
        search_in_list(tokenized_file[j]);
    }

    printf("This is the PARENT process\n");
    struct word_count *buffer = (word_count *) malloc(sizeof(word_count));//must be able to hold the nodes passed
    close(fd[0][1]);                                               //close write end of parent
    for (int i = 1; i <= TOTAL_PROCESS_NUM-1; i++) {
        //TEMP:for (int i=1; i<=(int)argv[4]; i++) {                   // the 4th arg is number of processes
        read(fd[i][0], buffer, strlen(buffer));

            //read returns the number of bytes read. It returns 0 at end-of-file
            printf("testing message: bytes_read is %d, buffer is %s", buffer->count);
//                buffer[bytes_read] = 0;
//				char* word, number;
//				word = buffer[0];
//				number = buffer[1];
//				struct word_count *new_word = (word_count*) malloc (sizeof(word_count));
//				new_word->word = (char *) word;
//				new_word->count = (int) number - '0';//change back to int
//                merge_list(new_word);
            //reads a number of bytes from the file associated with fd and places the characters read into buffer

    }
    close(fd[0][0]); //close read end of parent
    // TODO: print result
    gettimeofday(&end, NULL);                                          // get the total runtime
    long runtime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

    //TEMP:print_result(argv[2], argv[3], runtime);                    // print the result

    fclose(fp);                                                        // close the file
    //TEMP: printf("Done. Total runtime: %ld\nThe result is in %s, and the runtime is in %s\n", runtime, argv[2], argv[3]);
    printf("runtime %ld", runtime);

    //free(raw_str);                                                   // Deallocates the allocations
    //free(*tokenized_file);                                           // caused crash for some reason
	/*
	TEMP: for (int i = 0; i < argv[4] - 1; i++)
	{
	free(fd[i]) 
	}*/
	//free(*fd);

    return 0;
}

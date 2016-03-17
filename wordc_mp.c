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
            partial_num_of_words,
            fd[2][3];                                                    // fd = file descriptor. Every process gets one.
    //TEMP: fd[2][argv[4]];                                        // fd[0] is for reading. fd[1] is for writing.
    //Steve: ^^^ This WON'T work because the array size must be assigned at compile time

    pid_t pid;                                                         // set up multi-process


    while(fscanf(fp, "%s", raw_str) != EOF) {                          // tokenize the file and save it to tokenized_file
        tokenized_file[total_num_of_words] = word_format(raw_str);     // format the word
        total_num_of_words++;                                          // get the number of total words in file
    }
    partial_num_of_words = total_num_of_words/3;
    for (int i = 1; i <= (3); i++) {                                 // sets up pipes
        pipe(fd + i);                  //Steve: What's this for? fd is a pointer.. and this line of code caused a SIGNAL ABORT ERROR.. idk why
    }
    /*TEMP: for (int i = 1; i < (argv[4] - 1); i++){                   //Why argv[4] - 1? For n total processes, we need n-1 pipes
        pipe(fd + i);
    }*/


    for (int i = 1; i <= 3; i++) {
        //TEMP:for (int i=1; i<=(int)argv[4]; i++) {                   // the 4th arg is number of processes
        pid = fork();                                                  // create new process

        if (pid == 0) {                                                // This is a CHILD process
            for (int j = partial_num_of_words*i; j < partial_num_of_words*(i + 1); j++) {
                search_in_list(tokenized_file[j]);
            }
            printf("This is a CHILD process, %d \n", i);
            // TODO: pipe the result to parent
            // Steve: the sorted list is not a file, it is a linked list, and each process has one (they are independent),
            //        you can access the linked list by HEAD->next, HEAD->word
            //        one possiable solution is send the HEAD pointer through pipe

            // write(fd[0][i], PartoftheSortedTextFile, size); //I don't know what to put for this
            // The problem with this is it looks at the entire textfile and puts it into one pipe. We need to give parts of the textfile to different pipes
            // write(fd[0][i], "b.txt", size);
            // TEMP: write(fd[0][i], ..., size); //size is the bytes of the entire file.
            // TODO: make size just the part of file
            // places the characters read from file into buffer

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

        //Is this too big? -Jesse A: the largest file has about 140,000 words, you can consider that....
        char buffer[100000];                                           //must be able to hold the number of words passed

        for (int i = 1; i <= 3; i++) {
            //TEMP:for (int i=1; i<=(int)argv[4]; i++) {                   // the 4th arg is number of processes
            //read(fd[0][i],buffer,size); //size is the bytes of the entire file.
            //TODO: make size just the part of file
            //TODO: reads a number of bytes from the file associated with fd and places the characters read into buffer
            //TODO: Returns the number of bytes read. Returns 0 at end-of-file

        }
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

    //free(raw_str); //I don't know if this will work -Jesse           // Deallocates the allocations
    //free(*tokenized_file);                                           // caused crash for some reason

    return 0;
}

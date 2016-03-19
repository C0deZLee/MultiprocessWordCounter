#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

// 1 is for write

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
int TOTAL_PROCESS_NUM = 8;

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
int merge_list(word_count *s_val) {
    if (s_val->count != -1) {
        for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next) {
            if (strcmp(s_val->word, curr->word) == 0) {                          // val == curr
                curr->count += s_val->count;
            }
            else if (strcmp(s_val->word, curr->word) < 0) {                     // val < curr
                s_val->next = curr;
                s_val->last = curr->last;
                if (curr->last != NULL) {
                    curr->last->next = s_val;
                    curr->last = s_val;
                }
                else {
                    curr->last = s_val;
                    HEAD = s_val;
                }
                return 0;
            }

            else if (strcmp(s_val->word, curr->word) > 0 &&
                     (curr->next == NULL ||
                      strcmp(s_val->word, curr->next->word) < 0)) {       // s_val>curr, insert behind
                s_val->next = curr->next;
                s_val->last = curr;
                if (curr->next != NULL) {
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
void print_result(char *result_filename, char *runtime_filename, long runtime, char **file, int w){
    FILE *fp;                                                           // create the file pointer
    fp = fopen(result_filename, "w+");                                  // print the result into result_filename

    for(int i=0; i<w; i++) search_in_list(file[i]);
    for(struct word_count *curr = HEAD;curr != NULL;curr = curr->next)
        fprintf(fp,"%s, %d\n", curr->word, curr->count);

    fp = fopen(runtime_filename, "w+");                                 // print the runtime into runtime_filename
    fprintf(fp,"The total runtime is %ld\n-------------------", runtime);
    fclose(fp);                                                         // close the file
}

/* main function start here */
int main(int argc, char *argv[]) {
    if ((argc == 2 && (argv[1] == "--help" || argv[1] == "-h")) || argc == 1){  // help
        printf("./wordc-mp input-textfile output-countfile output-runtime num-of-processes\n");
        exit(1);
    }
    if (argc != 5){
        printf("Expected 4 arguments, given %d", argc-1);
        exit(1);
    }

    struct timeval start, end;                                         // set the start time
    gettimeofday(&start, NULL);

    FILE *fp = fopen(argv[1], "r");                             // open the given file
    if (fp == NULL) {                                                  // exit if failed to open file
        perror("Failed to open file");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);                                           // seek to end of file.
    int size = (int) ftell(fp) + 1;                                    // get the total size of the file
    fseek(fp, 0L, SEEK_SET);                                           // seek back to beginning of file

    char *raw_str = (char *) malloc (128 * sizeof(char)),              // save the unformatted word
         **tokenized_file = (char **) malloc (size * sizeof(char*));   // save the file into a string array

	int total_num_of_words = 0;                                        // get the total length of the file

	int **fd;                                                          // this is an array of pointers. Every process has fd[0-2]. A way to identify pipes is to have fd[0-2][0-n]
    fd = (int**)malloc(TOTAL_PROCESS_NUM * sizeof(int*));              // allocates space for needed pointers

	for (int i = 0; i < TOTAL_PROCESS_NUM; i++) {                      // create pipe from 0 to TOTAL_PROCESS_NUM-1
		fd[i] = (int*)malloc(2 * sizeof(int));
		pipe(fd[i]);
	}

    pid_t pid;                                                         // set up multi-process

    while(fscanf(fp, "%s", raw_str) != EOF) {                          // tokenize the file and save it to tokenized_file
        tokenized_file[total_num_of_words] = word_format(raw_str);     // format the word
        total_num_of_words++;                                          // get the number of total words in file
    }
    int partial_num_of_words = total_num_of_words / TOTAL_PROCESS_NUM;

	for (int i = 1; i < TOTAL_PROCESS_NUM; i++) {                      //the number of children is 1 to TOTAL_PROCESS_NUM-1
		pid = fork();                                                  // create new process
        if (pid == 0) {                                                // This is a CHILD process
            close(fd[i][0]);                                           // close read end (no need)
            for (int j = partial_num_of_words*i; j < partial_num_of_words*(i + 1); j++)
                search_in_list(tokenized_file[j]);                     // each child search its own part


			for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next)
                write(fd[i][1], curr, 1000);                           // pipe the data to parent


            struct word_count *finish_signal = (word_count*) malloc (sizeof(word_count));
            finish_signal->word = "__", finish_signal->count = -1, finish_signal->last = NULL, finish_signal->next = NULL;

            write(fd[i][1], finish_signal, 1000);

            close(fd[i][1]); //close write end
            exit(0);                                                   // exit the child process since it done all work
        }

        else if(pid >0 && i==1){
            for (int j=0; j<partial_num_of_words; j++)                 // parent sort first part, which is tokenized_file[0] - tokenized_file[partial_num_of_words]
                search_in_list(tokenized_file[j]);
        }

        else if (pid < 0) {                                            // not a child, not a parent, then error
            printf("Failed to create new processes");
            exit(1);
        }
    }

    struct word_count *buffer = (word_count *) malloc (sizeof(word_count));
    close(fd[0][1]);                                                  // close write end of parent
    for (int i = 1; i < TOTAL_PROCESS_NUM; i++) {                     // child process is 1 to total-1
        read(fd[i][0], buffer, 1000);

        while(buffer->count != -1){
            buffer->last = NULL, buffer->next = NULL;
            merge_list(buffer);
            read(fd[i][0], buffer, 1000);
        }
    }

    HEAD=NULL, TAIL=NULL;                                              // free the pointer
    close(fd[0][0]);                                                   // close read end of parent

    gettimeofday(&end, NULL);                                          // get the total runtime

    long runtime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    printf("Done. Total runtime: %ld\nThe result is in %s, and the runtime is in %s\n", runtime, argv[2], argv[3]);
    print_result(argv[2], argv[3],runtime,tokenized_file,total_num_of_words);

    fclose(fp);                                                        // close the file


    return 0;
}

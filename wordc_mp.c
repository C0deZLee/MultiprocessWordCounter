//
//
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <stdbool.h>

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

    for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next) {          // search from the HEAD
        if (strcmp(curr->word, val) == 0) {
            curr->count++;
            return 0;
        }
    }
    add_to_list(val);                                                                // not in list then add to list

    return 0;
}

/* Remove punctuation and make all lowercase */
char *word_format(char *raw_str) {

    char *formatted = (char *) malloc(strlen(raw_str)+1), *final = formatted;

    while (*raw_str) {
        if (ispunct(*raw_str)) {                                             // skip it
            raw_str++;
        }
        else if (isupper(*raw_str)) {
            *formatted++ = tolower(*raw_str++);                              // make it lowercase
        }
        else {
            *formatted++ = *raw_str++;                                       // increment both pointers and copying
        }
    }
    return final;
}

/* print the result into files */
void print_result(char *result_filename, char *runtime_filename, long runtime){
    FILE *fp;                                                                   // create the file pointer
    fp = fopen(result_filename, "w+");                                          // print the result into result_filename
    for(struct word_count *curr = HEAD;curr != NULL;curr = curr->next)
        fprintf(fp,"%s, %d\n", curr->word, curr->count);

    fp = fopen(runtime_filename, "w+");                                         // print the runtime into runtime_filename
    fprintf(fp,"The total runtime is %ld\n-------------------", runtime);
    // close the file
    fclose(fp);
}

/* main function start here */
int main(int argc, char *argv[]) {
    if (argc != 5){
        printf("Expected 4 arguments, given %d", argc-1);
        return -1;
    }

    struct timeval start, end;                                          // set the start time
    gettimeofday(&start, NULL);


    char *raw_str = (char *) malloc (128 * sizeof(char)),              // read file
            *formatted_str = (char *) malloc (128 * sizeof(char));
    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    char *file = (char *) malloc (100000 * sizeof (char));  // set up a string to save the whole file
    int word_num = 0;                                       // get the total length of the file

    while(fscanf(fp, "%s", raw_str) != EOF) {               // save the file into string
        file[word_num] = word_format(raw_str);
        word_num++;
    }

    pid_t child_pid;                                    // set up multi-process

    for (int i=0; i<argv[4]; i++) {                     // the fourth arg points out how many process we need (according to the project description)

        int fragment_size = word_num/(int)argv[4];      // get the fragment size for every child process
        child_pid = fork();                             // create new process

        if (child_pid > 0) {                            // This is a PARENT process

            // get all mini part from child process
            // put them together
            // sort it

        }
        else if (child_pid == 0) {                      // This is a CHILD process
            char* fragment = ;
            // get
        }

        else printf("Failed to create new processes");
    }



    gettimeofday(&end, NULL);                           // get the total runtime
    long runtime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

    print_result(argv[2], argv[3], runtime);            // print the result

    fclose(fp);                                         // close the file
    printf("Done. Total runtime: %ld\nThe result is in %s, and the runtime is in %s\n", runtime, argv[2], argv[3]);

    return 0;
}

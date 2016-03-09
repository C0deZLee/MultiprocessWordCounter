//
// Created by Li Xiang on 2/13/16.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <stdbool.h>

// structure definition
typedef struct word_count {
    char *word;
    int count;
    struct word_count *last;
    struct word_count *next;
} word_count;

// Global variable
word_count *HEAD = NULL;
word_count *TAIL = NULL;

// create a new list if we don't have a list
int create_list(char *val) {
    struct word_count *curr = (word_count*) malloc (sizeof(word_count));
    curr->word = val, curr->count = 1, curr->last = NULL, curr->next = NULL;

    HEAD = curr;
    TAIL = curr;

    return 0;
}

// add a new node to the RIGHT POSITION of the list
int add_to_list(char *val) {
    if (HEAD == NULL)
        return (create_list(val));

    struct word_count *curr = (word_count*) malloc (sizeof(word_count));
    curr->word = val, curr->count = 1, curr->last = NULL, curr->next = NULL;


    // start at the first element
    for(struct word_count *search=HEAD; search!=NULL; search=search->next){

        // put front
        if (strcmp (curr->word, search->word) < 0){
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

        // put behind
        if(strcmp (curr->word, search->word)>0 && (search->next == NULL ||strcmp(curr->word, search->next->word)<0)){
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

// search in list
int search_in_list(char *val) {
    // no list then create one
    if (HEAD == NULL)
        return (create_list(val));
    // search from the HEAD
    for (struct word_count *curr = HEAD; curr != NULL; curr = curr->next) {
        if (strcmp(curr->word, val) == 0) {
            curr->count++;
            return 0;
        }
    }
    // not in list then add to list
    add_to_list(val);

    return 0;
}

//Remove punctuation and make all lowercase
char *word_format(char *raw_str) {

    char *formatted = (char *) malloc(strlen(raw_str)+1), *final = formatted;

    while (*raw_str) {
        if (ispunct(*raw_str)) {
            // skip it
            raw_str++;
        }
        else if (isupper(*raw_str)) {
            // make it lowercase
            *formatted++ = tolower(*raw_str++);
        }
        else {
            // increment both pointers and copying
            *formatted++ = *raw_str++;
        }
    }
    return final;
}

//print the result into files
void print_result(char *result_filename, char *runtime_filename, long runtime){
    // write file
    FILE *fp;
    // print the result into result_filename
    fp = fopen(result_filename, "w+");
    for(struct word_count *curr = HEAD;curr != NULL;curr = curr->next)
        fprintf(fp,"%s, %d\n", curr->word, curr->count);
    // print the runtime into runtime_filename
    fp = fopen(runtime_filename, "w+");
    fprintf(fp,"The total runtime is %ld\n-------------------", runtime);
    // close the file
    fclose(fp);
}

// main funtion start here
int main(int argc, char *argv[]) {
    if (argc != 4){
        printf("Expected 3 arguments, given %d", argc);
    }
    else{
        // set the start time
        struct timeval start, end;
        gettimeofday(&start, NULL);

        char *raw_str = (char *) malloc (512 * sizeof(char)), *formatted_str = (char *) malloc (512 * sizeof(char));
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("Failed to open file");
            exit(1);
        }

        // read words from the file until reach the end of file
        while (fscanf(fp, "%s", raw_str) != EOF) {
            formatted_str = word_format(raw_str);
            search_in_list(formatted_str);
        }

        // get the total runtime
        gettimeofday(&end, NULL);
        long runtime = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

        // print the result
        print_result(argv[2], argv[3], runtime);

        // close the file
        fclose(fp);
        printf("Done. Total runtime: %ld\nThe result is in %s, and the runtime is in %s\n", runtime, argv[2], argv[3]);
    }
    return 0;
}

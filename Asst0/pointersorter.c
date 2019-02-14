#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//layout of BST structure
struct BST{
    char* word;
    struct BST* left;
    struct BST* right;
};

//global variable pointing to root node of BST
struct BST* head;

//Compares two strings and determines which comes alphabetically first of the two strings according to assignment specifications
int compareString(char* str1, char* str2){
    int s1 = strlen(str1);
    int s2 = strlen(str2);
    int i = 0;
    //iterate until we get to character at the same position of each word that isn't equal
    while (str1[i] == str2[i]) i++;
    //if i is equal to the length of s1 and s2, it means both strings are equal
    if (i == s1 && i == s2) return 0;
    //if one of words has reached end, that is smaller
    if (i == s1 ^ i == s2) return (i == s1) ? -1 : 1;
    //if exclusively either str1 or str2 is a capital then the string with a capital comes after the string without at position i
    if (str1[i] >= 97 ^ str2[i] >= 97) return (str1[i] >= 97 && str2[i] < 97) ? -1 : 1;
    //if the character at str1[i] is larger than the character at str2[i] return 1 meaning str1 comes after str2 alphabetically
    return str1[i] >= str2[i] ? 1 : -1;
}

//prints all elements of BST In-Order
//frees up memory in heap after element has been printed
void printBST(struct BST* ptr){
    if (ptr == NULL){
        printf("No words found");
        return;
    }
    if (ptr -> left != NULL) printBST(ptr -> left);
    printf("%s\n", ptr -> word);
    free(ptr);
    if (ptr -> right != NULL) printBST(ptr -> right);
}

//Adds substrings into BST in appropriate order
//Recursively traverses through tree to find appropriate location
struct BST* addToBST(char* word, struct BST* ptr){
    if (ptr == NULL){
        ptr = (struct BST*) malloc(sizeof(struct BST));
        ptr -> word = word;
        ptr -> left = NULL;
        ptr -> right = NULL;
        if (head == NULL) head = ptr;
    }
    else if (compareString(word, ptr -> word) > 0) {
//        printf("%s is greater than %s\n", word, ptr -> word);
        ptr -> left = addToBST(word, ptr -> left);
    }
    else{
  //      printf("%s is less than %s\n", word, ptr -> word);
        ptr -> right = addToBST(word, ptr -> right);
    } 
    return ptr;
}

//Parses input string to extract appropriate substrings and store into BST
void getWords(char* input, char* words){
    int startIndex = 0;
    char* word;
    int len;
    head = NULL;
    int i;
    for (i = 0; i <= strlen(input); i++){
        if (!isalpha(input[i]) || i == strlen(input)){
            len = i - startIndex;
            word = (char*) calloc(len, 1);
            word = strncpy(word,input + startIndex, len);
//            if (isalpha(word[0])) printf("%s\n",word);
            startIndex = i + 1;
            if (isalpha(word[0])) addToBST(word, head);
            if (i == strlen(input)) break;
        }
    }
}

//Main method that starts program and takes in all arguments
//Also handles errors in input arguments
int main(int argc, char** argv){
    if (argv[1] == NULL) printf("Not enough arguments entered");
    else{
        if (argv[2] != NULL) printf("Too many arguments entered. If you would like to enter multiple words, be sure to surround argument in quotes.\nOnly using following argument:\n");
        printf("Input: %s\n", argv[1]);
        int strLen = strlen(argv[1]);
        char* input = argv[1];
        char words[strlen(input)];
        getWords(input,words);
        printBST(head);
        return 1;
    }
}
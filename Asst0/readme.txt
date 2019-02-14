Overview:
    This program aims to extract substrings from an input argument string, sorts them into reverse alphabetical order, and prints these values accordingly. The substrings are sorted using a custom comparison algorithm and are then placed into a binary search tree. These substrings are extracted by iterating through the input string and locating any non-alphabetic characters. The substring begins following the last non-alphabetic character and ends at the next non-alphabetic character found. The program then allocates some memory to store the substring into a character array, and proceeds to insert it into the tree after making the appropriate comparisons. Once the program has reached the end of the input string, the substrings are then printed to the console in reverse alphabetical order, with capital letters having larger values than, i.e. coming alphabetically after, lower case letters.

Libraries:
    This program required the use of several libraries, as listed below. The libraries were crucial in order to gain access to the appropriate and most ideal methods that are built into the C programming language

    #include <stdio.h>
        Standard input and output library that allowed for tasks such as printing to console

    #include <stdlib.h>
        Necessary to provide dynamic memory allocation abilities to programming, providing use of functions such as malloc, calloc and free to allocate and free up memory used by strings (character arrays) and structures

    #include <string.h>
        Provided multiple functionalities regarding strings, such as strlen to determine length of string, strncpy to retrieve substrings of the input string

    #include <ctype.h>
        Allowed program to determine whether individual character was a letter or non-alphabetic character using isalpha method

    
Objects:
    struct BST
        Structure designed to serve as binary search tree in order to sort all appropriate words into desired order

Global Variables:
    struct BST* head
        pointer to the head or root note of binary search tree in order

Methods:
    int compareString(char* str1, char* str2)
        Compares two strings to determine which string appears first lexicographically. Functions similar to pre-built strcmp function within <string.h> library, however strcmp utilizes ascii values and order to determine ranking relative to each other. As per requirements of assignment, this does not suffice, as capital letters are supposed to be positioned after lower case letters, whereas, according to ascii table, capital letters hold lower values, thus appearing before lower case letters, thus not fulfilling requirements of this assignment and requiring manual comparison method

    void printBST(struct BST* ptr)
        Iterates through binary search tree after being called from main with head node of BST as parameter. Displays all values in desired order, reverse alphabetical order with capital values printing out before lower case values

    struct BST* addToBST(char* word, struct BST* ptr)
        Method that places each word into appropriate location as per desired order of assignment into binary search tree.

    void getWords(char* input, char* words)
        Parses input string and extracts substrings that satisfy assignment requirements. Appropriate substrings are separated by non-alphabetic characters, determined by isalpha function from <string.h> library.

    int main(int argc, char** argv)
        Main function used to start program and retrieve input arguments to be passed into other methods to parse input and print out appropriate substrings in reverse alphabetical order.
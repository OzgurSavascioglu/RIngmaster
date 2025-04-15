#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

//
// Created by ozgur on 06.04.2024.
//

//returns the next WORD in the string after a given index
char* next(const char* str, int index) {
    int i = index;

    //findx the index of the last whitespaca
    while (str[i] == ' ') {
        i++;
    }


    int j = i + 1;
    //finds the index of first whitespace after the start of the word
    while (str[j] != ' ') {
        j++;
    }

    //allocate memory
    char *nextStr = (char *) malloc((j - i) * sizeof(char));
    int k = 0;
    for (;i < j; i++) {
        nextStr[k] = *(str+i);
        k++;
    }

    return nextStr;
};

//removes the extra whitespaces
char* removeWhites(const char* str) {
    char *newStr = (char *) malloc((strlen(str)) * sizeof(char));
    int wCount=0;//count of extra whites
    int k=0;//count of clean string elements
    int doubleWhite=1;//marks extra white

    for(int i = 0;i<strlen(str);i++){
        if(str[i] == ' ') {
            //if the white is not extra, keep it
            if(doubleWhite != 1){
                newStr[k]=str[i];
                k++;
                doubleWhite = 1;//make doubleWhite 1
            }
            else
                wCount++;//increase the extra white count
        }

        //break after new line
        else if(str[i] == '\n')
            break;

        //copy all the non-whites
        else {
            newStr[k] = str[i];
            k++;
            doubleWhite = -1;
        }
    }


    int size=strlen(str)-wCount-1;

    //check the last character
    if(newStr[size-1]==' ')
        size--;

    //copy the string to the return string
    char *retStr = (char *) malloc((size+1) * sizeof(char));
    for(int i=0;i<size;i++){
        retStr[i]=newStr[i];
    }
    retStr[size] = '\0';

    //free the allocated memory
    free(newStr);

    return retStr;
};

//counts the words in given (clean) string: means no extra whitespace
int wordCount(const char* str) {
    int wordCount=1;

    for(int i = 0;i<strlen(str);i++){
        if(str[i] == ' ')
            wordCount++;
    }

    return wordCount;
};

//find the index of a given substring in a string
int findIndex(const char* str, const char* keyword,int index){
    for(int i=index; i<strlen(str);i++){
        int j=0;

        while(j<strlen(keyword)){
            if(str[i+j]!=keyword[j])
                break;
            j++;
        }

        if(j==strlen(keyword)){
            return i;
        }
    }
    return -1;
};

//returns the max of two integers
int max(int x, int y){
    return (x >= y) ? x : y;
};

//returns the min of two integers
int min(int x, int y){
    return (x <= y) ? x : y;
};

int findSplitAnd(char* str){

    if(strstr(str, " and ") == NULL)
        return -1;

    int index;
    int begin=0;

    index=findIndex(str," and ", begin);

    if(strstr(str, " at ") != NULL) {
        return index;
    }

    else {
        char * ch= (char *)malloc(strlen(str) * sizeof(char));
        strcpy(ch,&str[strlen(" and ")]+index);

        char * newCH=next(ch, 0);

        if(isdigit((int)newCH[0])){
            char *subStr = str + index+strlen(" and ");
            index = index+findSplitAnd(subStr);
        }
    }
    return index;
};


int actionSplitAnd(char* str){

    if(strstr(str, " and ") == NULL)
        return -1;

    int index;
    int begin=0;

    index=findIndex(str," and ", begin);

    char * ch= (char *)malloc(strlen(str) * sizeof(char));
    strcpy(ch,&str[strlen(" and ")]+index);

    char * newCH=next(ch, 0);

    if(isdigit((int)newCH[0])){
        char *subStr = str + index+strlen(" and ");
        index = index+findSplitAnd(subStr);
    }

    return index;
};

int andCount(char* str){
    char *newStr;
    int count=0;
    newStr=strdup(str);
    while(strstr(newStr, " and ") != NULL){
        newStr=newStr+findIndex(newStr," and ", 0)+strlen(" and ");
        count++;
    }

    return count;
};

//skips the extra whitespaces
int skipWSpaces(const char* str, int index) {
    int i = index;

    while (str[i] == ' ') {
        i++;
    }
    return i;
};

//create a substring with given indexes
char* subStr(const char* str, int begin,int end) {
    int i = begin;
    int k = 0;
    char * substr= (char *)malloc((end-begin)* sizeof(char));
    while (i<end) {
        substr[k]=str[i];
        i++;
        k++;
    }
    return substr;
};

//checks if given text contains action keywords
int containsAction(char* str) {
    int index = -1;

    if (strstr(str, " buy ") != NULL)
        index= 1;
   else if (strstr(str, " sell ") != NULL)
        index= 1;
    else if (strstr(str, " go to ") != NULL)
        index= 1;

    return index;
};

//checks if given text contains condiiton keywords
int containsCondition(char* str) {
    int index = -1;

    if (strstr(str, " has ") != NULL)
        index= 1;

    else if (strstr(str, " at ") != NULL)
        index= 1;

    return index;
};

//return the start index of the last condition in a string
int conditionBegins(const char* str, int currentIndex){
    int change=0;

    if(strstr(str, " has less than ") != NULL) {
        int temp=currentIndex;

        if(currentIndex==-1)
            currentIndex++;
        currentIndex = max(currentIndex,findIndex(str, " has less than ", currentIndex));
        if(currentIndex!=temp)
            change=1;
    }

    if(strstr(str, " has more than ") != NULL) {
        int temp=currentIndex;
        if(currentIndex==-1)
            currentIndex++;
        currentIndex = max(currentIndex,findIndex(str, " has more than ", currentIndex));

        if(currentIndex!=temp)
            change=1;
    }

    if(strstr(str, " has ") != NULL) {
        int temp=currentIndex;
        if(currentIndex==-1)
            currentIndex++;
        currentIndex = max(currentIndex, findIndex(str, " has ", currentIndex));
        if(currentIndex!=temp)
            change=1;
    }

    if(strstr(str, " at ") != NULL) {
        int temp=currentIndex;
        if(currentIndex==-1)
            currentIndex++;
        currentIndex = max(currentIndex, findIndex(str, " at ", currentIndex));
        if(currentIndex!=temp)
            change=1;
    }

    if(currentIndex != -1 && change!=0)
        currentIndex=conditionBegins(str, currentIndex+1);

    return currentIndex;
};

//check if the block after an if also includes action keywords
int complexBlock(const char* str){
    if(strstr(str, " buy ") != NULL || strstr(str, " sell ") != NULL || strstr(str, " go to ") != NULL)
        return 1;
    else
        return 0;
};

//check if a given string equals to any of the keywords
int containsKeyword(char* str) {
    int index = -1;

    if (strcmp(str,"sell")==0)
        index= 1;

    else if (strcmp(str,"buy")==0)
        index= 1;

    else if (strcmp(str,"go")==0)
        index= 1;

    else if (strcmp(str,"to")==0)
        index= 1;

    else if (strcmp(str,"from")==0)
        index= 1;

    else if (strcmp(str,"and")==0)
        index= 1;

    else if (strcmp(str,"at")==0)
        index= 1;

    else if (strcmp(str,"has")==0)
        index= 1;

    else if (strcmp(str,"if")==0)
        index= 1;

    else if (strcmp(str,"less")==0)
        index= 1;

    else if (strcmp(str,"more")==0)
        index= 1;

    else if (strcmp(str,"than")==0)
        index= 1;

    else if (strcmp(str,"exit")==0)
        index= 1;

    else if (strcmp(str,"where")==0)
        index= 1;

    else if (strcmp(str,"total")==0)
        index= 1;

    else if (strcmp(str,"who")==0)
        index= 1;

    else if (strcmp(str,"NOBODY")==0)
        index= 1;

    else if (strcmp(str,"NOTHING")==0)
        index= 1;

    else if (strcmp(str,"NOWHERE")==0)
        index= 1;

    return index;
};

//create a substring of the part between given indexes
char* safeSplit(char* str, int begin, int end){
    char *retStr=(char *) malloc((end-begin) * sizeof(char));

    int k=0;
    for(int i=begin;i<end-1;i++){
        retStr[k]=str[i];
        k++;
    }

    retStr[end-begin-1] = '\0';

    return retStr;
};

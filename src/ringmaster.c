#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdbool.h>

//
// Created by ozgur on 06.04.2024.
//

//stores the details of a subject entity
struct Subject{
    char* text;//subject name
    char* location;//subject location
    char** items;//item names
    int* amount;//item amounts
    int totalItems;//count of distinct items (including the previously owned)
};

//lists that stores the entities in the book
struct Subject* subjectList[16000];
char* itemList[16000];
char* locationList[16000];

//existing entity counts
int subjectCount=0;
int itemCount=0;
int locationCount=0;

//temporary lists to avoid duplicate entities in complex sentences
char* tempSubjects[1024];
char* tempItems[1024];
char* tempLocations[1024];

//temporary entity counts
int tempSubjectCount=0;
int tempItemCount=0;
int tempLocationCount=0;

//mentioned entity count (within the same sentence)
int elementCounter=0;

//stores the details of an action in a sentence
struct Action{
    int index;//beginning of action in the sentence
    char type;//B: buy, F: buy from, S: sell, T: sell to, G: go to
    int secondIndex;// used for buy from and sell to
    int andIndex;//and that marks the end of the sentence
};

//stores the details of a condition in a sentence
struct Condition{
    int index;//the beginning of the condition in the sentence
    char type;//A: at, H: has, L: has less than, M: has more than
    int andIndex;//and that marks the end of the sentence
};

//checks if the given subject array contains the subject with given text
int containsSubject(struct Subject *myList[], char* str, int lastIndex){
    for(int i=0;i<lastIndex;i++)
        if(strcmp(myList[i]->text,str)==0)
            return 1;
    return -1;//if does not contain
};

//checks if the given string array contains the string
int containsStr(char* list[], char* str, int lastIndex){
    for(int i=0;i<lastIndex;i++)
        if(strcmp(list[i],str)==0)
            return 1;
    return -1;//if does not contain
};

//returns the index of the subject with given text
int indexSubject(struct Subject *myList[], char* str, int lastIndex){
    for(int i=0;i<lastIndex;i++)
        if(strcmp(myList[i]->text,str)==0)
            return i;
    return -1;//if does not contain
};

//returns the index of the givne string
int indexStr(char* list[], char* str, int lastIndex){
    for(int i=0;i<lastIndex;i++)
        if(strcmp(list[i],str)==0)
            return i;
    return -1;//if does not contain
};


//stores the details of a sentence and connected sentences in a doubly linked list structure
struct Sentence{
    char* text;//sentence text
    char type;//S: action sentence, C: condition sentence, A: AND, H: head, L: last
    char detail;//stores the action or condition type of the sentence
    int actIndex;//index of the action
    int secActIndex;// used for buy from and sell to
    struct Sentence* previous;//previous sentence
    struct Sentence* next;//next sentence
};

//used to create AND parts in the doubly linked list
void newSentence(char* str, struct Sentence* last,char type) {
    struct Sentence *newSentence = (struct Sentence *) malloc(sizeof(struct Sentence));
    newSentence->text = strdup(str);
    newSentence->type = type;
    last->previous->next=newSentence;
    newSentence->previous=last->previous;
    last->previous=newSentence;
    newSentence->next = last;
};

//used to create action and condition sentences the doubly linked list
void mainSentence(char* str, struct Sentence* last, char type, char detail, int actIndex, int secActIndex) {
    struct Sentence *newSentence = (struct Sentence *) malloc(sizeof(struct Sentence));

    newSentence->text = strdup(str);
    newSentence->type = type;
    newSentence->detail=detail;

    last->previous->next=newSentence;

    newSentence->previous=last->previous;
    last->previous=newSentence;
    newSentence->next = last;

    newSentence->actIndex=actIndex;
    newSentence->secActIndex=secActIndex;
};

//used to create subjects
void newSubject(char* str){
    //subjectList[subjectCount] marks the first available slot in the subjectList
    //create the new subject by populating that slot
    subjectList[subjectCount]->text = strdup(str);//initialize the subject name
    //allocate memory
    subjectList[subjectCount]->location= (char *)malloc(1024 * sizeof(char));
    subjectList[subjectCount]->amount=(int *)malloc(1024 * sizeof(int));
    subjectList[subjectCount]->items= (char **)malloc(1024 * sizeof(char*));
    subjectList[subjectCount]->totalItems=0;//initialize the totalItems as 0
    subjectList[subjectCount]->location="NOWHERE"; //initialize the beginning location as "NOWHERE"

    //allocate memory for the elements of the items array
    for (int i = 0; i < 1024; i++) {
        subjectList[subjectCount]->items[i] = (char*) malloc(1024 * sizeof(char));
    }

    subjectCount++;//increase the subject count
};

//stores the details of a sentence and connected sentences in a doubly linked list structure
struct Block{
    char* text;//block text
    char type; //S: action block, C: condition block, A: AND, I: IF, U: Unknown,H: head, L: last
    struct Block* previous; //previous block
    struct Block* next; //next block
    struct Sentence* head; //starting sentence
    struct Sentence* last; //end sentence
};

//used to create blocks
void newBlock(char* str, struct Block* last,char type) {
    struct Block *myBlock = (struct Block *) malloc(sizeof(struct Block));//allocate memory

    //initialize the values
    myBlock->text = strdup(str);
    myBlock->type = type;
    last->previous->next=myBlock;
    myBlock->previous=last->previous;
    last->previous=myBlock;
    myBlock->next = last;
};

//FUNCTIONS IN THE functions.c file
int findIndex(const char* str, const char* keyword,int index);

char* next(const char*, int);

int andCount(char* str);

int skipWSpaces(const char* str, int index);

char* removeWhites(const char* str);

int wordCount(const char* str);

char* subStr(const char* str, int begin,int end);

char* safeSplit(char* str, int begin, int end);

int max(int x, int y);

int min(int x, int y);

int conditionBegins(const char* str, int currentIndex);

int complexBlock(const char* str);

int containsAction(char* str);

int containsCondition(char* str);

int containsKeyword(char* str);

int findSplitAnd(char* str);

int actionSplitAnd(char* str);

//checks the validity of the location
int locationCheckEach(char* str, char* list[]){
    //checks if the location mentioned as a different entity in the same sentence
    if(containsStr(list, str, elementCounter) == 1)
        return -1;
    //checks if the location defined as a different entity in the book
    if(containsStr(itemList, str, itemCount) == 1 || containsSubject(subjectList, str, subjectCount) == 1){
        return -1;
    }

    //checks if the location defined as a different entity in the given line
    if(containsStr(tempItems, str, tempItemCount) == 1 || containsStr(tempSubjects, str, tempSubjectCount) == 1){
        return -1;
    }

    //checks if the location contains a keyword
    if(containsKeyword(str)==1)
        return -1;

    //checks if the location contains an invalid character
    for(int i=0;i<strlen(str);i++) {
        if (!isalpha(str[i]) && str[i] != '_') {
            return -1;
        }
    }

    //add the location to the mentioned list for this sentence
    list[elementCounter]=strdup(str);
    (elementCounter)=elementCounter+1;

    //add the location to the tempLOcation list for this line
    tempLocations[tempLocationCount]=strdup(str);
    (tempLocationCount)=tempLocationCount+1;

    return 1;
};

//checks the validity of the location
int questionLocationCheck(char* str){
    if(strlen(str)==0)
        return -1;

    //checks if the location defined as a different entity in the book
    if(containsStr(itemList, str, itemCount) == 1 || containsSubject(subjectList, str, subjectCount) == 1){
        return -1;
    }

    //checks if the location contains a keyword
    if(containsKeyword(str)==1)
        return -1;

    //checks if the location contains an invalid character
    for(int i=0;i<strlen(str);i++)
        if (!isalpha(str[i]) && str[i] != '_')
            return -1;

    return 1;
};

//checks the validity of the subject
int subjectCheckEach(char* str){

    //checks if the subject defined as a different entity in the book
    if(containsStr(itemList, str, itemCount) == 1 || containsStr(locationList, str, locationCount) == 1){
        return -1;
    }

    //checks if the location defined as a different entity in the given line
    if(containsStr(tempItems, str, tempItemCount) == 1 || containsStr(tempLocations, str, tempLocationCount) == 1){
        return -1;
    }

    //checks if the subject contains a keyword
    if(containsKeyword(str)==1){
        return -1;
    }

    //checks if the subject contains an invalid character
    for(int i=0;i<strlen(str);i++)
        if(!isalpha(str[i]) && str[i]!='_') {
            return -1;
        }

    return 1;
};

//checks the validity of the subject
int questionSubjectCheck(char* str){

    //checks if the subject defined as a different entity in the book
    if(containsStr(itemList, str, itemCount) == 1 || containsStr(locationList, str, locationCount) == 1){
        return -1;
    }

    //checks if the subject contains a keyword
    if(containsKeyword(str)==1) {
        return -1;
    }

    //checks if the subject contains an invalid character
    for(int i=0;i<strlen(str);i++)
        if(!isalpha(str[i]) && str[i]!='_') {
            return -1;
        }
    return 1;
};

//checks the validity of the item
int itemCheckEach(char* str){

    //checks the validity of the world count (it should be 2)
    if(wordCount(str)!=2) {
        if(str[(int) strlen(str)]==' ')
        return -1;
    }

    //split the amount and itemName
    int index= findIndex(str," ", 0);
    char* amount=safeSplit(str,0,index+1);
    char* remaining=safeSplit(str,index+1,(int)strlen(str)+1);

    //checks if the itemName mentioned as a different entity in the book
    if(containsSubject(subjectList, remaining, subjectCount) == 1 || containsStr(locationList, remaining, locationCount) == 1){
        return -1;
    }

    //checks if the itemName mentioned as a different entity in the given line
    if(containsStr(tempSubjects, remaining, tempSubjectCount) == 1 || containsStr(tempLocations, remaining, tempLocationCount) == 1){
        return -1;
    }

    //checks if the itemName contains a keyword
    if(containsKeyword(remaining)==1)
        return -1;

    //checks if the amount is valid
    for(int i=0;i<strlen(amount);i++)
        if(!isdigit(amount[i]))
            return -1;

    //checks if the itemName contains an invalid character
    for(int i=0;i<strlen(remaining);i++)
        if(!isalpha(remaining[i]) && remaining[i]!='_')
            return -1;

    return 1;
};


//checks the validity of the item in questions
int questionItemCheck(char *str, char *newlyAdded[], int sCount) {

    //checks if the itemName mentioned as a different entity in the book
    if(containsSubject(subjectList, str, subjectCount) == 1 || containsStr(locationList, str, locationCount) == 1){
        return -1;
    }

    if(containsStr(newlyAdded, str, sCount) == 1)
        return -1;

    //checks if the itemName contains a keyword
    if(containsKeyword(str)==1)
        return -1;

    //checks if the itemName contains an invalid character
    for(int i=0;i<strlen(str);i++)
        if(!isalpha(str[i]) && str[i]!='_')
            return -1;

    return 1;
};




//checks the validity of a subject that appears as the seller in the buy from or buyer in the sell to
int otherSubjectCheck(char* str, char* list[]){
    //checks if the subject mentioned as a different entity in the same sentence
    if(containsStr(list, str, elementCounter) == 1)
        return -1;

    //checks the validity of the subject
    if(subjectCheckEach(str)==-1)
        return -1;

    //add the subject to the mentioned list for this sentence
    list[elementCounter]= strdup(str);
    (elementCounter)=elementCounter+1;

    //add the subject to the tempSubject list for this line
    tempSubjects[tempSubjectCount]= strdup(str);
    (tempSubjectCount)=tempSubjectCount+1;

    return 1;
};

//extract the subjects from the string and checks the validity of the subjects by calling relevant functions
int subjectSplitAndCheck(char* str, char* subjects[]){
    char* substr;
    substr= strdup(str);// copy the string

    //split the string by " and "
    while(strstr(substr, " and ") != NULL){
        int index= findIndex(substr," and ", 0);
        char* current=safeSplit(substr,0,index+1);// extract the part before the " and "

        //checks if the subject mentioned as a different entity in the same sentence
        if(containsStr(subjects, current, elementCounter) == 1)
            return -1;

        //move the pointer after the " and "
        substr=substr+findIndex(substr," and ", 0)+strlen(" and ");

        //call the function that checks the validity of the subject
        if(subjectCheckEach(current)==-1)
            return -1;

        //add the subject to the mentioned list for this sentence
        subjects[elementCounter]= strdup(current);
        (elementCounter)=elementCounter+1;

        //add the subject to the tempSubjects list for this line
        tempSubjects[tempSubjectCount]= strdup(current);
        (tempSubjectCount)=tempSubjectCount+1;

    }

    //THE PART AFTER THE LAST AND
    //checks if the subject mentioned as a different entity in the same sentence
    if(containsStr(subjects, substr, elementCounter) == 1)
        return -1;

    //call the function that checks the validity of the subject
    if(subjectCheckEach(substr)==-1)
        return -1;

    //add the subject to the mentioned list for this sentence
    subjects[elementCounter]= strdup(substr);
    (elementCounter)=elementCounter+1;

    //add the subject to the tempSubjects list for this line
    tempSubjects[tempSubjectCount]= strdup(substr);
    (tempSubjectCount)=tempSubjectCount+1;

    return 1;
};

//extract the items from the string and checks the validity of the items by calling relevant functions
int itemSplitAndCheck(char* str, char* list[]){
    char* substr;
    substr= strdup(str);// copy the string

    //split the string by " and "
    while(strstr(substr, " and ") != NULL){
        int index= findIndex(substr," and ", 0);
        char* current=safeSplit(substr,0,index+1);// extract the part before the " and "

        //call the function that checks the validity of the item
        if(itemCheckEach(current)==-1)
            return -1;

        int nameIndex= findIndex(current," ", 0);
        char* name=safeSplit(substr,nameIndex+1,(int)strlen(current)+1);//extract the item name

        //checks if the item mentioned as a different entity in the same sentence
        if(containsStr(list, name, elementCounter) == 1)
            return -1;

        //move the pointer after the " and "
        substr=substr+findIndex(substr," and ", 0)+strlen(" and ");

        //add the itemName to the mentioned list for this sentence
        list[elementCounter]= strdup(name);
        (elementCounter)=elementCounter+1;

        //add the itemName to the tempItems list for this line
        tempItems[tempItemCount]= strdup(name);
        (tempItemCount)=tempItemCount+1;
    }

    //THE PART AFTER THE LAST AND
    int nameIndex= findIndex(substr," ", 0);
    char* lastName=safeSplit(substr,nameIndex+1,(int)strlen(substr)+1);//extract the item name

    //checks if the item mentioned as a different entity in the same sentence
    if(containsStr(list, lastName, elementCounter) == 1)
        return -1;

    //call the function that checks the validity of the item
    if(itemCheckEach(substr)==-1)
        return -1;

    //add the itemName to the mentioned list for this sentence
    list[elementCounter]= strdup(lastName);
    (elementCounter)=elementCounter+1;

    //add the itemName to the tempItems list for this line
    tempItems[tempItemCount]= strdup(lastName);
    (tempItemCount)=tempItemCount+1;

    return 1;
};

//splits The Elements of the Action Entity to see the validity, if valid the result will be used in processing the sentences in the second part
int actionEntitySplit(struct Sentence* sentence){
    char* strTemp;
    strTemp= strdup(sentence->text);// copy the sentence text string

    //Switch by action type
    switch (sentence->detail) {
        case 'B': {//Buy action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex+1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = (sentence->actIndex)+(int)strlen(" buy ");
            int end = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,begin,end);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024,sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'F': {//Buy from action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex+1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = (sentence->actIndex)+(int)strlen(" buy ");
            int end = sentence->secActIndex+1;
            char *item=safeSplit(strTemp,begin,end);

            //extract the seller subject part
            int beginOther = (sentence->secActIndex) + (int)strlen(" from ");
            int endOther = (int)strlen(strTemp)+1;
            char *other=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 2;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024,sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1) {
                return -1;
            }

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1) {
                return -1;
            }

            //check the validity of the item part
            if(otherSubjectCheck(other, newlyAdded)==-1) {
                return -1;
            }

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'S': {//Sell action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = sentence->actIndex+(int)strlen(" sell ");
            int end = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,begin,end);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024,sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'T': {//Sell to action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex+1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = sentence->actIndex+(int)strlen(" sell ");
            int end = sentence->secActIndex+1;
            char *item=safeSplit(strTemp,begin,end);

            //extract the buyer subject part
            int beginOther = sentence->secActIndex + (int) strlen(" to ");
            int endOther = (int)strlen(strTemp)+1;
            char *other=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 2;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024,sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //check the validity of the buyer subject part
            if(otherSubjectCheck(other, newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'G': {//Go to action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the location part
            int beginOther = sentence->actIndex + (int) strlen(" go to ");
            int endOther = (int)strlen(strTemp)+1;
            char *place=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 2;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024,sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the location part
            if(locationCheckEach(place, newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }
            break;
        }
    }
    return 1;
};

//splits The Elements of the Condition Entity to see the validity, if valid the result will be used in processing the conditions in the second part
int conditionEntitySplit(struct Sentence* sentence){
    char* strTemp;
    strTemp= strdup(sentence->text);// copy the sentence text string

    //Switch by condition type
    switch (sentence->detail) {
        case 'A': {//At condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the action part
            int beginOther = sentence->actIndex + (int) strlen(" at ");
            int endOther = (int)strlen(strTemp)+1;
            char *place=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = malloc(1024 * sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the location part
            if(locationCheckEach(place, newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'H': {//Has condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = malloc(1024 * sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'M': {//Has more than condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has more than ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = malloc(1024 * sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'L': {//Has less than condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has less than ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned entities in the sentence
            elementCounter=0;
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];

            //allocate the memory for the mentioned entities array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = malloc(1024 * sizeof(char));
            }

            //check the validity of the subject part
            if(subjectSplitAndCheck(subject, newlyAdded)==-1)
                return -1;

            //check the validity of the item part
            if(itemSplitAndCheck(item,newlyAdded)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }
    }
    return 1;
};


//function calls the relevant entity split functions by the type of sentence, returns -1 if there is an invalid sentence
int entitySplitter(struct Sentence* sentence){
    int returnValue=1;

    if(sentence->type=='S'){
        returnValue=actionEntitySplit(sentence);//splits the action sentence and returns validity
    }

    else if(sentence->type=='C'){
        returnValue=conditionEntitySplit(sentence);//splits the condition sentence and returns validity
    }

    return returnValue;
};

//determine the type of function in case of type buy and sell to avoid buy/buy from and sell/sell to confusion
struct Action* actType (const char* str, int index){
    //allocate memory for the return value
    struct Action *action = (struct Action *) calloc(1,sizeof(struct Action));
    index=index+1;//skip the whitespace
    action->index=index;//assign the index to the action

    //make necessary calculations to find the action with minimum index after given index value
    //possible FROM case check
    if(strstr(str+index, " from ") != NULL) {
        action->index = findIndex(str, " from ", index);
        action->type='F';
    }

    //possible TO case check
    if(strstr(str+index, " to ") != NULL) {
        int temp=action->index;
        if(action->type=='F')
            action->index = min(action->index, findIndex(str, " to ", index));
        else
            action->index = findIndex(str, " to ", index);

        if(action->index != temp)
            action->type='T';
    }

    //possible BUY case check
    if(strstr(str+index, " buy ") != NULL) {
        int temp=action->index;

        action->index = min(action->index, findIndex(str, " buy ", index));
        if(action->index != temp)
            action->type='B';
    }

    //possible SELL case check
    if(strstr(str+index, " sell ") != NULL) {
        int temp=action->index;

        if(action->index == -1) {
            action->index = findIndex(str, " sell ", index);
        }

        else
            action->index = min(action->index, findIndex(str, " sell ", index));

        if(action->index != temp)
            action->type='S';

    }

    //possible GO TO case check
    if(strstr(str+index, " go to ") != NULL) {
        int temp=action->index;

        if(action->index == -1) {
            action->index = findIndex(str, " go to ", index);
        }
        else
            action->index = min(action->index, findIndex(str, " go to ", index));

        if(action->index != temp)
            action->type='G';

    }
    return action;//returns the action with minimum index
};

//return the first action in the given text
struct Action* firstAction(char* str) {
    //allocate memory for the return value
    struct Action *action = (struct Action *) calloc(1,sizeof(struct Action));
    //initialize indexes as -1
    action->index = -1;
    action->andIndex = -1;

    //make necessary calculations to find the action with minimum index value
    //possible BUY case check
    if (strstr(str, " buy ") != NULL) {
        int temp = action->index;

        action->index = findIndex(str, " buy ", 0);
        if (action->index != temp)
            action->type = 'B';
    }

    //possible SELL case check
    if (strstr(str, " sell ") != NULL) {
        int temp = action->index;

        if (action->index == -1) {
            action->index = findIndex(str, " sell ", 0);
        } else
            action->index = min(action->index, findIndex(str, " sell ", 0));

        if (action->index != temp)
            action->type = 'S';
    }

    //possible GO TO case check
    if (strstr(str, " go to ") != NULL) {
        int temp = action->index;

        if (action->index == -1) {
            action->index = findIndex(str, " go to ", 0);
        } else
            action->index = min(action->index, findIndex(str, " go to ", 0));

        if (action->index != temp)
            action->type = 'G';
    }

    //IF FROM COMES BEFORE THE OTHER KEYWORDS, MAKE INVALID
    if (strstr(str, " from ") != NULL) {
        int temp = action->index;

        action->index = min(action->index, findIndex(str, " from ", 0));

        if (action->index != temp) {
            action->index = -1;
            return action;
        }
    }

    //IF TO COMES BEFORE THE OTHER KEYWORDS, MAKE INVALID
    if (strstr(str, " to ") != NULL) {
        int temp = action->index;

        action->index = min(action->index, findIndex(str, " to ", 0));

        if (action->index != temp) {
            action->index = -1;
            return action;
        }
    }

    //control possible BUY FROM action type
    if (action->type == 'B') {
        struct Action *secondPart = (struct Action *) malloc(sizeof(struct Action));
        secondPart = actType(str, action->index);
        if (secondPart->type == 'F') {
            action->type = 'F';
            action->secondIndex = secondPart->index;
        }
    }

    //control possible SELL TO action type
    if (action->type == 'S') {
        struct Action *secondPart = (struct Action *) malloc(sizeof(struct Action));
        secondPart = actType(str, action->index);
        if (secondPart->type == 'T') {
            action->type = 'T';
            action->secondIndex = secondPart->index;
        }
    }

    //FIND THE SPLITTING AND DELIMITER INDEX, WHICH MARKS THE START OF THE NEXT SENTENCE IF IT EXISTS
    if (containsAction(str + action->index + 1) == 1) {
        if (action->type == 'T' || action->type == 'F')
            action->andIndex = findIndex(str, " and ", action->secondIndex);

        else if (action->type == 'B') {
            //char *subStr = (char *) malloc((strlen(str)-action->index-strlen(" and "))*sizeof(char));
            char *subStr = str + action->index + strlen(" buy ");
            action->andIndex = action->index + strlen(" buy ") + actionSplitAnd(subStr);
        }
        else
            action->andIndex = findIndex(str, " and ", action->index);
    }

    return action;
};

//return the first condition in the given text
struct Condition* firstCondition(char* str) {
    //allocate memory for the return value
    struct Condition *condition = (struct Condition *) malloc(sizeof(struct Condition));
    //initialize indexes as -1 and type as U (unknown)
    condition->index = -1;
    condition->andIndex = -1;
    condition->type = 'U';

    //make necessary calculations to find the condition with minimum index value
    //possible HAS case check
    if (strstr(str, " has ") != NULL) {
        condition->index = findIndex(str, " has ", 0);
        condition->type = 'H';
    }

    //possible AT case check
    if (strstr(str, " at ") != NULL) {
        if (condition->type == 'U') {
            condition->index = findIndex(str, " at ", 0);
            condition->type = 'A';
        } else if (condition->type == 'H') {
            int temp = condition->index;
            condition->index = min(temp, findIndex(str, " at ", 0));
            if (temp != condition->index) {
                condition->type = 'A';
            }
        }
    }

    //possible HAS LESS THAN/HAS MORE THAN case checks
    if(condition->type == 'H') {

        if (strstr(str, " has less than ") != NULL) {
            if(findIndex(str, " has less than ", 0)==condition->index)
                condition->type = 'L';
        }

        if (strstr(str, " has more than ") != NULL) {
            if(findIndex(str, " has more than ", 0)==condition->index)
                condition->type = 'M';
        }

    }

    //FIND THE SPLITTING AND DELIMITER INDEX, WHICH MARKS THE START OF THE NEXT SENTENCE IF IT EXISTS
    if(containsCondition(str + condition->index + 4)!=-1) {
        if (condition->type == 'A')
            condition->andIndex = findIndex(str, " and ", condition->index);

        else {
            char *subStr = str + condition->index + (int) strlen(" has ");
            condition->andIndex = condition->index + (int) strlen(" has ") + actionSplitAnd(subStr);
        }
    }
    return condition;//return the condition
};

//split the ACTION blocks (S Type) to the sentences and check the validity
int actionSplitter(const char* str, struct Block* block){
    char* strTemp;
    strTemp=strdup(str);// copy the string

    //checks if the string contains an action
    if(containsAction(strTemp)==-1) {
        return -1;
    }

    struct Action *pAction;
    pAction=firstAction(strTemp);//get the first action

    //allocate the memory for the HEAD and LAST sentences
    block->head=(struct Sentence *) malloc(sizeof(struct Sentence));
    block->last=(struct Sentence *) malloc(sizeof(struct Sentence));

    //link the head and last and initialize their types
    block->head->next=block->last;
    block->last->previous=block->head;
    block->head->type='H';
    block->last->type='L';

    //check if action is valid
    if(pAction->index==-1)
        return -1;

    //split the action sentences by "and"
    while(pAction->andIndex!=-1){
        char *substr = strTemp+pAction->andIndex+strlen(" and ");; // pointer for the address of the " and "
        char *text=(char *)malloc((pAction->andIndex+1) * sizeof(char));//create the string to copy text before " and "
        strncpy(text, strTemp, pAction->andIndex);//copy the text before " and "

        strTemp=strdup(substr);//copy the string after and to strTemp

        //create the sentence
        mainSentence(text, block->last, 'S',pAction->type, pAction->index, pAction->secondIndex);
        newSentence("AND", block->last, 'A');//create the AND part

        if(strlen(strTemp)==0){
            return -1;
        }

        //checks if strTemp has action
        if(containsAction(strTemp)!=-1)
            pAction=firstAction(strTemp);//get the next action
    }

    //create the sentence for the remaining part
    mainSentence(strTemp, block->last, 'S',pAction->type,pAction->index, pAction->secondIndex);

    //set the current sentence as head->next
    struct Sentence *current = block->head->next;

    //split sentences to the entities and checks the validity of each sentence part
    while(current->next!=NULL){
        int temp=entitySplitter(current);
        if(temp==-1)
            return -1;

        if(current->next->type=='L')
            break;

        current = current->next;
    }

    return 1;
};

//split the CONDITION blocks (C Type) to the sentences and check the validity
int conditionSplitter(const char* str, struct Block* block){
    char* strTemp;
    strTemp= strdup(str);// copy the string

    //checks if the string contains a condition
    if(containsCondition(strTemp)==-1) {
        return -1;
    }

    struct Condition *pCondition;
    pCondition=firstCondition(strTemp);//get the first condition

    //allocate the memory for the HEAD and LAST sentences
    block->head=(struct Sentence *) malloc(sizeof(struct Sentence));
    block->last=(struct Sentence *) malloc(sizeof(struct Sentence));

    //link the head and last and initialize their types
    block->head->next=block->last;
    block->last->previous=block->head;
    block->head->type='H';
    block->last->type='L';

    //split the condition sentences by "and"
    while(pCondition->andIndex!=-1){
        char *substr = strTemp+pCondition->andIndex+(int)strlen(" and "); // pointer for the address of the " and "
        char *text=(char *)malloc((pCondition->andIndex+1) * sizeof(char));//create the string to copy text before and
        strncpy(text, strTemp, pCondition->andIndex);//copy the text before and

        strTemp= strdup(substr);//copy the string after " and " to strTemp

        //create the sentence
        mainSentence(text, block->last, 'C',pCondition->type,pCondition->index, -1);
        newSentence("AND", block->last, 'A');//create the AND part


        if(strlen(strTemp)==0){
            return -1;
        }

        //checks if strTemp has condition
        if(containsCondition(strTemp)!=-1)
            pCondition=firstCondition(strTemp);
    }

    //create the sentence for the remaining part
    mainSentence(strTemp, block->last, 'C',pCondition->type, pCondition->index, -1);

    //set the current sentence as head->next
    struct Sentence *current = block->head->next;

    //split sentences to the entities and checks the validity of each sentence part
    while(current->next!=NULL){
        int temp=entitySplitter(current);
        if(temp==-1)
            return -1;

        if(current->next->type=='L')
            break;

        current = current->next;
    }

    return 1;
};

//function calls the relevant sentence split functions by the type of block, returns -1 if there is an invalid sentence
int sentenceSplitter(struct Block* block){
    int returnValue=1;

    if(block->type=='S'){
        returnValue=actionSplitter(block->text, block);//splits the action blocks and returns validity
    }

    else if(block->type=='C'){
        returnValue=conditionSplitter(block->text, block);//splits the condition blocks and returns validity
    }

    return returnValue;
};

//function that creates the subject entities and transaction list
int createSubjects(char *str, char *subList[]) {
    char* substr;
    substr= strdup(str);// copy the string
    int sCount=0;//initialize count of subjects in the string to 0

    //split the string by " and "
    while(strstr(substr, " and ") != NULL){
        int index= findIndex(substr," and ", 0);
        char* current=safeSplit(substr,0,index+1);// extract the part before the " and "

        //create the subject if it has not been defined before
        if(containsSubject(subjectList, current, subjectCount) != 1)
            newSubject(current);

        substr=substr+findIndex(substr," and ", 0)+strlen(" and ");//move the pointer after the and

        //add the subject to the newly added list, that will be used in transactions
        subList[sCount]= strdup(current);
        sCount++;
    }

    //create the subject if it has not been defined before
    if(containsSubject(subjectList, substr, subjectCount) != 1)
        newSubject(substr);

    //add the subject to the newly added list, that will be used in transactions
    subList[sCount]= strdup(substr);
    sCount++;

    return sCount;
}

//function that creates the subject entities and list for the question calculations
int checkAndCreateSubjects(char *str, char *subList[]) {
    char* substr;
    substr= strdup(str);// copy the string
    int sCount=0;//initialize count of subjects in the string to 0

    while(strstr(substr, " and ") != NULL){
        int index= findIndex(substr," and ", 0);
        char* current=safeSplit(substr,0,index+1);// extract the part before the " and "

        //checks if the subject mentioned as a different entity in the same sentence
        if(containsStr(subList, current, sCount) == 1)
            return -1;

        //check if it has been defined before
        if(containsSubject(subjectList, current, subjectCount) != 1) {
            //check the validity of the subject
            if (questionSubjectCheck(current)==-1)
                return -1;
            else
                newSubject(current);//if valid create the subject
        }

        substr=substr+findIndex(substr," and ", 0)+strlen(" and ");//move the pointer after the and

        //add the subject to the newly added list, that will be used in question answer calculations
        subList[sCount]= strdup(current);
        sCount++;
    }

    //checks if the subject mentioned as a different entity in the same sentence
    if(containsStr(subList, substr, sCount) == 1)
        return -1;

    //check if it has been defined before
    if(containsSubject(subjectList, substr, subjectCount) != 1) {
        //check the validity of the subject
        if (questionSubjectCheck(substr)==-1)
            return -1;
        else
            newSubject(substr);//if valid create the subject
    }

    //add the subject to the newly added list, that will be used in transactions
    subList[sCount]= strdup(substr);
    sCount++;

    return sCount;
}

//function that creates the item entities
int createItems(char *str, char **addedItems, int* amounts) {
    char* substr;
    substr= strdup(str);// copy the string
    int iCount=0;//initialize count of items in the string to 0

    while(strstr(substr, " and ") != NULL){
        int index= findIndex(substr," and ", 0);
        char* current=safeSplit(substr,0,index+1);// extract the part before the " and "

        //extract the AMOUNT part
        int amountIndex= findIndex(current," ", 0);
        char* amountStr=safeSplit(current,0,amountIndex+1);
        //extract the itemName part
        char* remaining=safeSplit(current,amountIndex+1,(int)strlen(current)+1);
        int numericAmount=0;//create the amount int value and initialize as 0

        //calculate the numeric Value of the amount string
        int digit=1;
        int length=(int)strlen(amountStr);
        for(int i=length-1;i>=0;i--) {
            numericAmount += ((amountStr[i]-'0')*digit);
            digit*=10;
        }

        //create the item if it has not been defined before
        if(containsStr(itemList, remaining, itemCount) != 1){
            itemList[itemCount]=strdup(remaining);
            itemCount++;
        }

        substr=substr+findIndex(substr," and ", 0)+strlen(" and ");//move the pointer after the and

        //add the item and amount to the newly added lists, that will be used in transactions
        addedItems[iCount]=strdup(remaining);
        amounts[iCount]=numericAmount;
        iCount++;
    }

    //THE PART AFTER THE LAST AND
    //extract the AMOUNT part
    int amountIndex= findIndex(substr," ", 0);
    char* amountStr=safeSplit(substr,0,amountIndex+1);
    //extract the itemName part
    char* remaining=safeSplit(substr,amountIndex+1,(int)strlen(substr)+1);
    int numericAmount=0;//create the amount int value and initialize as 0

    //calculate the numeric Value of the amount string
    int digit=1;
    int length=(int)strlen(amountStr);
    for(int i=length-1;i>=0;i--) {
        numericAmount += ((amountStr[i]-'0')*digit);
        digit*=10;
    }

    //create the item if it has not been defined before
    if(containsStr(itemList, remaining, itemCount) != 1){
        itemList[itemCount]=strdup(remaining);
        itemCount++;
    }

    //add the item and amount to the newly added lists, that will be used in transactions
    addedItems[iCount]=strdup(remaining);
    amounts[iCount]=numericAmount;
    iCount++;

    return iCount;
}

//condition check function for the HAS condition, returns 1 if condition correct, else returns less than 1
int hasCheck(char **sellers, int sCount, char **items, const int itemAmounts[],int iCount) {
    int returnValue=1;

    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){

            //find the subject index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);

            if(itemAmounts[k]>0){
                //check whether the item exist
                if(exists==-1)
                    returnValue--;

                //check if amount is equal to the given number
                else{
                    int enough= subjectList[sIndex]->amount[exists];
                    if(enough!=itemAmounts[k])
                        returnValue--;
                 }
            }
        }
    }
    return returnValue;
};

//condition check function for the HAS LESS THAN condition, returns 1 if condition correct, else returns less than 1
int hasLessThanCheck(char **sellers, int sCount, char **items, const int itemAmounts[],int iCount) {
    int returnValue=1;

    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){
            //find the subject index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);

            //check whether the item exist
            if(exists!=-1){
                int enough= subjectList[sIndex]->amount[exists];
                //check if amount is less than the given number
                if(enough>=itemAmounts[k])
                    returnValue--;
            }
        }
    }
    return returnValue;
};

//condition check function for the HAS MORE THAN condition, returns 1 if condition correct, else returns less than 1
int hasMoreThanCheck(char **sellers, int sCount, char **items, const int itemAmounts[],int iCount) {
    int returnValue=1;

    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){
            //find the subject index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);

            //check if item exist in the inventory
            if(exists==-1)
                returnValue--;

            //check if amount is more than the given number
            else{
                int enough= subjectList[sIndex]->amount[exists];
                if(enough<=itemAmounts[k])
                    returnValue--;
            }
        }
    }
    return returnValue;
};

//checks if there is enough item for the transaction in the inventory of the sellers, else returns less than 1
int checkTheInventory(char **sellers, int sCount, char **items, const int itemAmounts[],int iCount) {
    int returnValue=1;

    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){
            //find the seller index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);

            //check if item exist in the inventory
            if(exists==-1)
                returnValue--;

            //check of amount is enough
            else{
                int enough= subjectList[sIndex]->amount[exists];
                if(enough<itemAmounts[k])
                    returnValue--;
            }
        }
    }
    return returnValue;
};

//makes the SELL TO calculations and change the inventories of the subjects accordingly
void sellTransaction(char **sellers, int sCount, char **items, const int *itemAmounts, int iCount, char *buyer) {
    int bIndex=indexSubject(subjectList,buyer,subjectCount);

    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){
            //find the seller index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);

            //decrease the amount in the sellers inventory
            subjectList[sIndex]->amount[exists]=subjectList[sIndex]->amount[exists]- itemAmounts[k];

            //check if buyer has the same item before
            int buyerHas=indexStr(subjectList[bIndex]->items, items[k],subjectList[bIndex]->totalItems);

            //add the item amount to the pre-existing slot in the inventory if buyer has item item before
            if(buyerHas!=-1) {
                subjectList[bIndex]->amount[buyerHas] = subjectList[bIndex]->amount[buyerHas] + itemAmounts[k];
            }

            //else add the itemName to the inventory and set the amount
            else
            {
                subjectList[bIndex]->items[subjectList[bIndex]->totalItems]= strdup(items[k]);
                subjectList[bIndex]->amount[subjectList[bIndex]->totalItems]=itemAmounts[k];
                subjectList[bIndex]->totalItems++;
            }
        }
    }
};

//makes SELL TO calculations and call relevant functions
void sellToProcessor(char *sellers[], int sCount, char *itemStr, char *items[], int* amounts, char *buyer) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    int avail=checkTheInventory(sellers, sCount, items, amounts,iCount);//check the inventory availability

    //if available make the transaction
    if(avail==1){
        sellTransaction(sellers, sCount, items, amounts,iCount,buyer);
    }

}

//makes the SELL calculations and change the inventories of the subjects accordingly
void simpleSellTransaction(char **sellers, int sCount, char **items, const int *itemAmounts, int iCount) {
    for(int i=0;i<sCount;i++){
        for(int k=0;k<iCount;k++){
            //find the seller index
            int sIndex=indexSubject(subjectList,sellers[i],subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[sIndex]->items, items[k],subjectList[sIndex]->totalItems);
            //decrease the amount in the sellers inventory
            subjectList[sIndex]->amount[exists]=subjectList[sIndex]->amount[exists]- itemAmounts[k];
        }
    }
}

//makes SELL calculations and call relevant functions
void simpleSellProcessor(char *sellers[], int sCount, char *itemStr, char *items[], int* amounts) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    int avail=checkTheInventory(sellers, sCount, items, amounts,iCount);//check the inventory availability

    //if available make the transaction
    if(avail==1){
        simpleSellTransaction(sellers, sCount, items, amounts,iCount);
    }
}

//checks if there is enough item for the transaction in the inventory of the seller, else returns less than 1
int singleInventoryCheck(char *seller, char **items, const int itemAmounts[],int iCount, int bCount) {
    int returnValue=1;

    for(int i=0;i<iCount;i++){
        //find the seller index
        int sIndex=indexSubject(subjectList,seller,subjectCount);
        //find the inventory index for the item
        int exists=indexStr(subjectList[sIndex]->items, items[i],subjectList[sIndex]->totalItems);

        //check if item exist in the inventory
        if(exists==-1)
            returnValue--;

        //check if the amount is enough
        else{
            int enough= subjectList[sIndex]->amount[exists];
            if(enough<itemAmounts[i]*bCount)
                returnValue--;
        }
    }
    return returnValue;
}

//makes the BUY FROM calculations and change the inventories of the subjects accordingly
void buyFromTransaction(char **buyers, int bCount, char **items, const int *itemAmounts, int iCount, char *seller) {
    int sellerIndex=indexSubject(subjectList, seller, subjectCount);//find the seller index

    for(int i=0;i<bCount;i++){
        for(int k=0;k<iCount;k++){
            //find the item index in the sellers inventory
            int sellerHas=indexStr(subjectList[sellerIndex]->items, items[k], subjectList[sellerIndex]->totalItems);
            //decrease the amount in the sellers inventory
            subjectList[sellerIndex]->amount[sellerHas]= subjectList[sellerIndex]->amount[sellerHas] - itemAmounts[k];

            //find the buyer index
            int buyerIndex=indexSubject(subjectList, buyers[i], subjectCount);
            //check if buyer has the same item before
            int exists=indexStr(subjectList[buyerIndex]->items, items[k], subjectList[buyerIndex]->totalItems);

            //add the item amount to the pre-existing slot in the inventory if buyer has item item before
            if(exists != -1)
                subjectList[buyerIndex]->amount[exists]= subjectList[buyerIndex]->amount[exists] + itemAmounts[k];

            //else add the itemName to the inventory and set the amount
            else
            {
                subjectList[buyerIndex]->items[subjectList[buyerIndex]->totalItems]= strdup(items[k]);
                subjectList[buyerIndex]->amount[subjectList[buyerIndex]->totalItems]=itemAmounts[k];
                subjectList[buyerIndex]->totalItems++;
            }
        }
    }
}

//makes BUY FROM calculations and call relevant functions
void buyFromProcessor(char *buyers[], int bCount, char *itemStr, char *items[], int* amounts, char *seller) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    int avail=singleInventoryCheck(seller, items, amounts,iCount,bCount);//check the inventory availability

    //if available make the transaction
    if(avail==1){
        buyFromTransaction(buyers, bCount, items, amounts, iCount, seller);
    }
};

//makes the BUY calculations and change the inventories of the subjects accordingly
void simpleBuyTransaction(char **buyers, int bCount, char **items, const int *itemAmounts, int iCount) {
    for(int i=0;i<bCount;i++){
        for(int k=0;k<iCount;k++){
            //find the seller index
            int buyerIndex=indexSubject(subjectList, buyers[i], subjectCount);
            //find the inventory index for the item
            int exists=indexStr(subjectList[buyerIndex]->items, items[k], subjectList[buyerIndex]->totalItems);

            //add the item amount to the pre-existing slot in the inventory if buyer has item item before
            if(exists != -1)
                subjectList[buyerIndex]->amount[exists]= subjectList[buyerIndex]->amount[exists] + itemAmounts[k];

            //else add the itemName to the inventory and set the amount
            else
            {
                subjectList[buyerIndex]->items[subjectList[buyerIndex]->totalItems]= strdup(items[k]);
                subjectList[buyerIndex]->amount[subjectList[buyerIndex]->totalItems]=itemAmounts[k];
                subjectList[buyerIndex]->totalItems++;
            }
        }
    }
};

//makes BUY calculations and call relevant functions
void simpleBuyProcessor(char *buyers[], int bCount, char *itemStr, char *items[], int* amounts) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    simpleBuyTransaction(buyers, bCount, items, amounts,iCount);//make the transaction
};

//processes the Action (S type) sentences and call the functions to make necessary changes accordingly
void sentenceProcessor(struct Sentence* sentence){
    char* strTemp;
    strTemp= strdup(sentence->text);// copy the sentence text string

    //skip the AND type sentences
    if (sentence->type=='A')
        return;

    //Switch by action type
    switch (sentence->detail) {
        case 'B': {//Buy action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex+1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = (sentence->actIndex)+(int)strlen(" buy ");
            int end = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,begin,end);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int bCount=createSubjects(subject, newlyAdded);// create the subjecs and return the subject count
            simpleBuyProcessor(newlyAdded, bCount, item, addedItems, amounts);//makes the buy transaction

            //free the memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }

        case 'F': {//Buy from action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = (sentence->actIndex)+(int)strlen(" buy ");
            int end = sentence->secActIndex+1;
            char *item=safeSplit(strTemp,begin,end);

            //extract the seller subject part
            int beginOther = (sentence->secActIndex) + (int)strlen(" from ");
            int endOther = (int)strlen(strTemp)+1;
            char *other=safeSplit(strTemp,beginOther,endOther);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int bCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            // create the seller subject if it does not exist
            if(containsSubject(subjectList,other,subjectCount)!=1)
                newSubject(other);

            //makes the buy from transaction
            buyFromProcessor(newlyAdded, bCount, item, addedItems,amounts, other);

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }

        case 'S': {//Sell action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = sentence->actIndex+(int)strlen(" sell ");
            int end = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,begin,end);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count
            simpleSellProcessor(newlyAdded, sCount, item, addedItems,amounts);//makes the sell transaction

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }
        case 'T': {//Sell to action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex+1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int begin = sentence->actIndex+(int)strlen(" sell ");
            int end = sentence->secActIndex+1;
            char *item=safeSplit(strTemp,begin,end);

            //extract the buyer subject part
            int beginOther = sentence->secActIndex + (int) strlen(" to ");
            int endOther = (int)strlen(strTemp)+1;
            char *other=safeSplit(strTemp,beginOther,endOther);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            // create the buyer subject if it does not exist
            if(containsSubject(subjectList,other,subjectCount)!=1)
                newSubject(other);

            //makes the sell to transaction
            sellToProcessor(newlyAdded, sCount, item, addedItems,amounts, other);

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }

        case 'G': {//Go to action
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the location part
            int beginOther = sentence->actIndex + (int) strlen(" go to ");
            int endOther = (int)strlen(strTemp)+1;
            char *place=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the subjects in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);

            //create the location if it does not exists
            if(containsStr(locationList, place, locationCount) != 1) {
                locationList[locationCount] = strdup(place);
                locationCount++;
            }

            //change the locations of the subjects
            for(int i=0;i<sCount;i++) {
                int sIndex=indexSubject(subjectList,newlyAdded[i],subjectCount);
                subjectList[sIndex]->location= strdup(place);
            }

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }
    }
};

//makes HAS condition calculations and call relevant functions
int hasProcessor(char *sellers[], int sCount, char *itemStr, char *items[], int* amounts) {
        int iCount=createItems(itemStr,items, amounts);//creates the item entities
        int temp=hasCheck(sellers, sCount, items, amounts,iCount);//check the inventory

        if(temp<=0)
            return -1;
        else
            return 1;
}

//makes HAS LESS THAN condition calculations and call relevant functions
int hasLessProcessor(char *sellers[], int sCount, char *itemStr, char *items[], int* amounts) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    int temp=hasLessThanCheck(sellers, sCount, items, amounts,iCount);//check the inventory

    if(temp<=0)
        return -1;
    else
        return 1;
}

//makes HAS MORE THAN condition calculations and call relevant functions
int hasMoreProcessor(char *sellers[], int sCount, char *itemStr, char *items[], int* amounts) {
    int iCount=createItems(itemStr,items, amounts);//creates the item entities
    int temp=hasMoreThanCheck(sellers, sCount, items, amounts,iCount);//check the inventory

    if(temp<=0)
        return -1;
    else
        return 1;
}

//processes the Condition (C type) sentences and call the functions to make necessary changes accordingly
int conditionProcessor(struct Sentence *sentence) {
    char* strTemp;
    strTemp= strdup(sentence->text);// copy the sentence text string

    //Switch by condition type
    switch (sentence->detail) {
        case 'A': {//At condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the action part
            int beginOther = sentence->actIndex + (int) strlen(" at ");
            int endOther = (int)strlen(strTemp)+1;
            char *place=safeSplit(strTemp,beginOther,endOther);

            //create the array to track the mentioned subjects in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            //create the location if it does not exist in the list
            if(containsStr(locationList, place, locationCount) != 1) {
                locationList[locationCount] = strdup(place);
                locationCount++;
            }

            //check if the subject is at the given location
            for(int i=0;i<sCount;i++) {
                int sIndex=indexSubject(subjectList,newlyAdded[i],subjectCount);
                if(strcmp(subjectList[sIndex]->location,strdup(place))!=0)
                    return -1;
            }

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            break;
        }

        case 'H': {//Has condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(subject);
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            //checks the HAS condition
            if(hasProcessor(newlyAdded, sCount, item, addedItems,amounts)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }

        case 'M': {//Has more than condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has more than ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            //checks the HAS MORE THAN condition
            if(hasMoreProcessor(newlyAdded, sCount, item, addedItems,amounts)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }

        case 'L': {//Has less than condition
            //extract the subject part
            char *subject = (char *) malloc((sentence->actIndex + 1) * sizeof(char));
            for(int i=0;i<sentence->actIndex;i++){
                subject[i]=strTemp[i];
            }
            subject[sentence->actIndex] = '\0';

            //extract the item part
            int beginOther = sentence->actIndex + (int) strlen(" has less than ");
            int endOther = (int)strlen(strTemp)+1;
            char *item=safeSplit(strTemp,beginOther,endOther);

            //create the arrays to track the mentioned entities in the sentence
            int size=wordCount(strTemp) - 1;
            char* newlyAdded[size];//for subjects
            int sizeItem=wordCount(item);
            char* addedItems[sizeItem];//for itemNames
            int amounts[sizeItem];//for itemAmounts

            //allocate the memory for the mentioned subjects array
            for (int i = 0; i < size; i++) {
                newlyAdded[i] = (char*)calloc(1024 , sizeof(char));
            }

            //allocate the memory for the mentioned items array
            for (int i = 0; i < sizeItem; i++) {
                addedItems[i] = (char*)calloc(1024 , sizeof(char));
            }

            int sCount=createSubjects(subject, newlyAdded);// create the subjects and return the subject count

            //checks the HAS LESS THAN condition
            if(hasLessProcessor(newlyAdded, sCount, item, addedItems,amounts)==-1)
                return -1;

            //free the allocated memory
            for (int i = 0; i < size; i++) {
                free(newlyAdded[i]);
            }

            //free the allocated memory
            for (int i = 0; i < sizeItem; i++) {
                free(addedItems[i]);
            }

            break;
        }
    }
    return 1;
}

//processes the Blocks
int blockProcessor(struct Block* block){
    int returnValue=0;

    //check the block type and proceeed accordingly
    if (block->type=='S') {// case Action (S type)
        struct Sentence *current = block->head->next;//select the first sentence as current

        //unconditional actions: last block in a complex line or a line consist single block
        if (block->next->type == 'L') {

            while(current->next!=NULL){
                sentenceProcessor(current);// process the sentence
                current = current->next;//get the next sentence
            }
            returnValue = 1;
        }

        //case Action before an IF (conditional actions)
        else if (block->next->type == 'I') {
            int temp=0;
            current = block->next->next->head->next;//get the condition

            //process the condition block
            while(current->next!=NULL){

                if(conditionProcessor(current)==-1){
                    temp=-1;//set temp as -1 if there is an unmet condition
                    break;
                }

                if(current->next->type=='L')
                    break;
                current = current->next;//get the next condition
            }

            //check if the conditions are met
            if(temp>-1) {
                current = block->head->next;//process the sentences

                while(current->next!=NULL){
                    sentenceProcessor(current);// process the sentence
                    current = current->next;//get the next sentence
                }
            }

            returnValue = 4;//return 4 in case of conditional sentence
        }
    }

    return returnValue;
};

//function that splits the input line to the BLOCKS
int blockSplitter(const char* str){
    char* strTemp;
    strTemp=strdup(str);// copy the input text string
    int typeCounter=0;

    //allocate the memory for the HEAD and LAST blocks
    struct Block *head=(struct Block *) malloc(sizeof(struct Block));
    struct Block *last=(struct Block *) malloc(sizeof(struct Block));

    //link the head and last and initialize their types
    head->next=last;
    last->previous=head;
    head->type='H';
    last->type='L';

    //if input does not contain IF create a single block from the input
    if(strstr(strTemp, " if ") == NULL)
        newBlock(strTemp,last,'S');

    else {//SPLIT PARAGRAPH BY IF
        while (strstr(strTemp, " if ") != NULL) {

            char *substr = strstr(strTemp, " if "); //pointer for the address of the IF

            int index = findIndex(strTemp, " if ", 0);//the index of the IF
            char *blockText = (char *) malloc(index * sizeof(char));//the string to copy text before IF
            strncpy(blockText, strTemp, index);//copy the text before IF

            substr += strlen(" if "); //move pointer after the IF

            if (typeCounter == 0)
                newBlock(blockText, last, 'S');//the part before the IF

            else newBlock(blockText, last, 'C');//the part after IF, this part may further splitted if necessary

            typeCounter++;

            newBlock("IF", last, 'I');//create the IF block

            strTemp=strdup(substr);//cop the part after IF
        }

        //THE PART AFTER THE LAST IF

        if (typeCounter == 0)
            newBlock(strTemp, last, 'S');
        else
            newBlock(strTemp, last, 'C');//the part after IF, this part may further splitted if necessary

        //Set the current Block to the beginning
        struct Block *current = head->next;

        //process the blocks to analyze further split needs
        while (current->next != NULL) {
            //if the part after it (C type) contains actions (complex block), split the block again
            if (current->type == 'C' && complexBlock(current->text) == 1) {
                //find the condition index
                int begin = conditionBegins(current->text, -1);
                //int end = conditionEnds(current->text, -1);

                //check if there is a condition
                if(begin==-1)
                    return -1;

                char *subAnd = current->text + begin;//move pointer after the last condition
                int andIndex = findSplitAnd(subAnd);//find the index of the AND that splits condition and action
                char *blockText = (char *) malloc((begin + andIndex + 1) * sizeof(char));//create the string to copy text before AND
                strncpy(blockText, current->text, andIndex + begin);//copy the text before AND
                current->text = blockText;//set the text of the current as the part before and
                //set the next as current->next
                struct Block *next = current->next;

                //create the AND type block
                newBlock("AND", next, 'A');
                current = current->next;
                next = current->next;

                //create the ACTION (S type) Block
                newBlock(subAnd + andIndex + strlen(" and "), next, 'S');
                current = current->next;
            }

            if(current->next->type=='L')
                break;
            current = current->next;
        }
    }

    //set the current block as the beginning block
    struct Block *current = head->next;

    //check if the block structure is valid
    while(current->next!=NULL){
        switch (current->type){
            case 'S'://Action (S type) check
                if ((current->previous->type == 'S' || current->previous->type == 'I' ||
                     current->previous->type == 'C') ||
                    ((current->next->type == 'A') || (current->next->type == 'C'))) {
                    return -1;
                }
            break;
            case 'I'://IF check
                if (current->previous->type != 'S' || current->next->type != 'C') {
                    return -1;
                }
            break;
            case 'C'://Condition (C type) check
                if (current->previous->type != 'I' || (current->next->type != 'A' && current->next->type != 'L'))
                    return -1;
            break;
            case 'A'://AND check
                if (current->previous->type != 'C' || current->next->type != 'S')
                    return -1;
            break;
        }

        current = current->next;
        if(current->type=='L')
            break;
    }

    //set the current block as the beginning block
    current = head->next;

    //split each block to the sentences
    while(current->next!=NULL){
        int temp=sentenceSplitter(current);//splits the block and returns validity
        if(temp==-1)
            return -1;

        if(current->next->type=='L')
            break;

        current = current->next;
    }

    //set the current block as the beginning block
    current = head->next;

    //process the valid actions and conditions
    while(current->next!=NULL){
        blockProcessor(current);//Process the block

        if(current->next->type=='L')
            break;
        current = current->next;

    }

    return 1;
};

//the function that makes the calculations for the Inventory inquiry question
int totalInventoryQuestion(char *subject) {
    if(questionSubjectCheck(subject)==-1)
        return -1;

    //find the subject index
    int sIndex=indexSubject(subjectList,subject,subjectCount);

    //return 0 if there is no subject
    if(sIndex==-1) {
        if(containsSubject(subjectList, subject, subjectCount) != 1)
            newSubject(subject);
        return 0;
    }

    int distinctItems=subjectList[sIndex]->totalItems;//distinct item types in the inventory
    int enoughCount=0;

    //find the items in the inventory
    for(int i=0;i<distinctItems;i++) {
        int total = subjectList[sIndex]->amount[i];
        if(total >0) {
            if(enoughCount > 0) {
                printf( " and ");//if it is not the first item, print an and before the item details
                fflush(stdout);
            }
            enoughCount++;
            char* itemName=strdup(subjectList[sIndex]->items[i]);
            printf( "%d %s", total, itemName); //print the amount and name
            fflush(stdout);
        }
    }

    if(enoughCount == 0)
        return 0;

    printf( "\n");
    fflush(stdout);

    return enoughCount;
}

//the function that makes the calculations for the Inventory inquiry question
int whereQuestion(char *subject) {

    //checks if the subject is valid
    if(questionSubjectCheck(subject)==-1) {
        return -1;
    }

    //find the subject index
    int sIndex=indexSubject(subjectList,subject,subjectCount);

    //return 0 if there is no subject
    if(sIndex==-1) {
        if(containsSubject(subjectList, subject, subjectCount) != 1)
            newSubject(subject);
        return 0;
    }

    //find the subject location
    char* locationIs=subjectList[sIndex]->location;

    //return zero if location is NOWHERE
    if(strcmp(locationIs,"NOWHERE")==0)
        return 0;
    
    printf("%s \n", locationIs);//print the location
    fflush(stdout);

    return 1;
}

//calculations for Quantity asking (total ... ?) question
int totalQuantityCalculator(char **subjects, int sCount, char *item) {
    int returnValue=0;

    //calculate for eaxh subject
    for(int i=0;i<sCount;i++){
        //find the subject index
        int sIndex=indexSubject(subjectList,subjects[i],subjectCount);
        //find the item index
        int exists=indexStr(subjectList[sIndex]->items, item,subjectList[sIndex]->totalItems);

        //if item exists add the amount to the return value
        if(exists!=-1)
            returnValue+=subjectList[sIndex]->amount[exists];

    }
    return returnValue;
};

//calls relevant functions for the Quantity asking (total ... ?) question
int totalQuantityQuestion(char *subjects[], int sCount, char *item) {
    //check if the item is mentioned in the book
    if(containsStr(itemList, item, itemCount) != 1){
        itemList[itemCount]=strdup(item);//if not mentioned create the item
        itemCount++;
        return 0;
    }

    int total=totalQuantityCalculator(subjects, sCount, item);// calculate total

    if(total==0)
        return 0;
    else if (total==-1)
        return -1;

    return total;
}

int questionProcessor(char* str){
    //find the index of the question mark
    int questionInd= findIndex(str, " ?",0);
    //check the validity of the part after " ?'
    for(int i=questionInd+2;i<strlen(str);i++){
        if(str[i]!='\n' && str[i]!=' ')
            return -1;
    }

    //extract the part before " ?"
    char *textPart=(char *) malloc((questionInd+2) * sizeof(char));
    for(int i=0;i<questionInd+1;i++){
        textPart[i]=str[i];
    }
    textPart[questionInd+1] = '\0';

    //check the question type
    if(strstr(str, " total ?") != NULL) {//Inventory inquiry type (total ?)
        int totalIndex= findIndex(str, " total ",0);
        //extract the subject part
        char *subject=(char *) malloc((totalIndex+1) * sizeof(char));
        for(int i=0;i<totalIndex;i++){
            subject[i]=textPart[i];
        }
        subject[totalIndex] = '\0';

        int total=totalInventoryQuestion(subject);//calculate the total

        //if total is zero print NOTHING
        if(total==0) {
            printf( "NOTHING\n");
            fflush(stdout);
        }

        else if(total==-1)
            return -1;
    }

    else if(strstr(str, "who at ") != NULL) {//who at type
        //find the index of the question
        int begin = (int)strlen("who at ");
        int end = (int)strlen(textPart);
        //extract the location part
        char *place=(char *) malloc((end-begin) * sizeof(char));
        int k=0;
        for(int i=begin;i<end-1;i++){
            place[k]=textPart[i];
            k++;
        }
        place[end-begin-1] = '\0';

        int avail=questionLocationCheck(place);//check the locations validity

        if (avail==-1)
            return -1;

        //check if location exist before
        if(containsStr(locationList, place, locationCount) != 1) {
            locationList[locationCount] = strdup(place);//if new, add to the list
            locationCount++;
        }

        //find and print the subjects in the location
        int total=0;
        for(int i=0;i<subjectCount;i++) {
            if (strcmp(subjectList[i]->location, place) == 0) {
                if(total!=0)
                    printf( " and ");
                printf( "%s", subjectList[i]->text);
                fflush(stdout);
                total++;
            }
        }

        //if total is zero print NOBODY
        if(total==0) {
            printf("NOBODY\n");
            fflush(stdout);
        }

        else
            printf( "\n");
    }

    else if(strstr(str, " where ?") != NULL) {//where type
        int totalIndex= findIndex(textPart, " where ",0);
        //extract the subject part
        char *subject=(char *) malloc((totalIndex+1) * sizeof(char));
        for(int i=0;i<totalIndex;i++){
            subject[i]=textPart[i];
        }
        subject[totalIndex] = '\0';

        int place=whereQuestion(subject);//find the location

        //if place is not given print NOWHERE
        if(place==0) {
            printf( "NOWHERE\n");
            fflush(stdout);
        }

        else if(place==-1)
            return -1;
    }

    else if(strstr(str, " total ") != NULL) {//Quantity asking type
        int totalIndex= findIndex(textPart, " total ",0);
        //extract the subject part
        char *subject=(char *) malloc((totalIndex+1) * sizeof(char));
        for(int i=0;i<totalIndex;i++){
            subject[i]=textPart[i];
        }
        subject[totalIndex] = '\0';

        //extract the item part
        int begin = totalIndex+(int)strlen(" total ");
        int end = (int)strlen(textPart);
        char *item=(char *) malloc((end-begin) * sizeof(char));
        int k=0;
        for(int i=begin;i<end-1;i++){
            item[k]=textPart[i];
            k++;
        }
        item[end-begin-1] = '\0';

        //create the arrays to track the mentioned subjects in the sentence
        int size=wordCount(subject);
        char* newlyAdded[size];

        //allocate the memory for the mentioned subjects array
        for (int i = 0; i < size; i++) {
            newlyAdded[i] = (char*) calloc (1024 , sizeof(char));
        }

        int sCount=checkAndCreateSubjects(subject, newlyAdded);// create the subjecs and return the subject count

        if(sCount==-1)
            return -1;

        int iCheck= questionItemCheck(item, newlyAdded, sCount);//check te item and return the result

        if(iCheck==-1)
            return -1;

        int total= totalQuantityQuestion(newlyAdded, sCount, item);//calculate the total quantity

        printf( "%d\n",total);
        fflush(stdout);

        if(total==-1)
            return -1;

        //check if it is a pre-existing item
        if(containsStr(itemList, item, itemCount) != 1){
            itemList[itemCount]=strdup(item);//if not, create the item
            itemCount++;
        }
    }

    //if question sentence does not include above types return -1
    else
        return -1;

    return 1;
};

int main() {
    char line[1025]; //input string

    //allocate memory for the subject, item and location lists
    for (int i = 0; i < 16000; i++) {
        subjectList[i] = (struct Subject * ) calloc (1 , sizeof(struct Subject));
        itemList[i] = (char*) calloc (1024, sizeof(char));
        locationList[i] = (char*) calloc (1024,  sizeof(char));
    }

    //start to receive input
    while (1) {
        printf(">> ");
        fflush(stdout);

        if (fgets(line, 1025, stdin) == NULL) {
            break;
        }

        //Check if the input is a question
        if(strstr(line, " ?") != NULL){

            int check=questionProcessor(removeWhites(line));//remove extra whitespaces and process the question

            //check the validity of the question
            if(check==-1){
                printf("INVALID \n" );
                fflush(stdout);
                continue;
            }
        }

        //check if input is exit command
        else if (strcmp(line, "exit\n") == 0) {

            break;
        }

        else {
            //allocate the memory for the temp lists (used to track the entities created in this line)
            for (int i = 0; i < 1024; i++) {
                tempSubjects[i] = (char*) calloc (1024,  sizeof(char));
                tempItems[i] = (char*) calloc (1024,  sizeof(char));
                tempLocations[i] = (char*) calloc (1024,  sizeof(char));
            }

            //remove extra whitespaces, call block processor, which splits the line into blocks and sentences
            //block splitter also will call the relevant functions to process the actions and conditions
            int check=blockSplitter(removeWhites(line));

            //check the validity of the sentence
            if(check==-1){
                printf("INVALID \n" );
                fflush(stdout);

                //free the allocated memory
                for (int i = 0; i < 1024; i++) {
                    free(tempSubjects[i]);
                    free(tempItems[i]);
                    free(tempLocations[i]);
                }
                continue;
            }

            else {
                printf("OK \n");
                fflush(stdout);
            }

            //free the allocated memory
            for (int i = 0; i < 1024; i++) {
                free(tempSubjects[i]);
                free(tempItems[i]);
                free(tempLocations[i]);
            }

        }

        fflush(stdout);
    }

    //free the allocated memory
    for (int i = 0; i < 1024; i++) {
        free(subjectList[i]);
        free(itemList[i]);
        free(locationList[i]);
    }

    return 0;
}
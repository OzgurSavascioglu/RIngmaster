# Ringmaster
CMPE 230 Spring 2024 Project 1
Purpose
The project aims to create a language interpreter for the book writers that will check the grammatical correctness of the sentences according to the constructed language rules and keep track of the character inventories. The interpreter will analyze question type and sentence type input and terminate when receiving the exit command.
Our constructed language rules have three main entity types (subject, item, and location) and two main sentence-type inputs one includes actions and the other one includes conditions. The project also allows writers to create complex sentences that are a combination of these two types. 
The constructed language allows only the stated question (total …, total, who at, and where), action (buy, buy from, sell, sell to, and go to), and condition (at, has, has less than, and has more than) types in the project description but it is possible to improve the model if needs be. 
Design and Implementation
The C programming language is used for this project. For the implementation part, we have defined two main tasks for each input. The first one (Split and Check) is to check the validity of the input and produce invalid output if that is the case. The second task (Process) is to make the necessary changes in our data and produce the output. 
In the sentence-type inputs, we strictly separate these two processes since the possibility of complex and consequential sentences creates risks regarding the alterations in the data. We need to make sure that the data will stay the same if the input is invalid or there is an unmet condition in the conditional sentences.
Since the question-type inputs do not include sequential or complex sentences, the input is checked and processed consecutively. 
For the sentence-type inputs, the main calls the function that splits the data to the Blocks which are Struct entities with doubly-linked list data structures, and checks the correctness of the general Block structure. Then the Blocks are split into sentences which are also struct entities with doubly-linked list data structure. In this part validity of the sentence structure is checked during the split. Lastly, sentences are analyzed one by one and the validity of each is checked during this analysis. We also determine the details about actions and conditions in the inputs during this process and keep actions and conditions stored as Struct entities. 
After the completion of the first task, the process-type functions are called by the BlockSplitter for each block. These functions check the conditions if there are any and make the necessary changes in the data tables.
We used arrays to create the tables for different entity types. Locations and Items are elements of corresponding string arrays. On the other hand, we created a dedicated Struct for the Subject as it also includes details regarding location, inventory item names, and item amount. Then we created a Subject struct array to store and access all the subjects in the book.
Challenges
The main challenge was to deal with the memory issues related to copying and splitting of the strings. In the first tests, we have experienced inconsistencies in the output of the program. Understanding the reason for the inconsistencies was not easy in the first place since we are not that familiar with the memory structure of the C.  
After debugging we understand that the problem stems from two main reasons. Firstly, copying the string with strcpy() function in some cases. Secondly, the missing ‘\0’ characters at the end of the splitted strings with a function that we created. For this part, we used Valgrind to better understand the memory leaks.
To address the issue, we have removed all the strcpy() functions and replaced with strdup() and revised our string Split function (safeSplit()).
Usage of the program and Examples of Input/Output
The program starts with the ./ringmaster command in the terminal. The user will enter the inputs and receive the outputs from the terminal.  
The project has a MakeFile file that will automatically update the ringmaster file as the linked files are updated.
Below you can find some examples of expected inputs and outputs of the program. 
EXAMPLE 1
>>     Frodo    go    to Bree    if Sauron has 0 ring 
OK 
>> Sam  go to Rivendell and        Gandalf go to Shire if Frodo at Bree and Sauron has less than     3324324 key and Gimli buy 2 axe and        Legolas buy 1 bow if Frodo at Bree and Sam buy 7 carrot if Sam at   Rivendell 
OK 
>> Gandalf    where     ?
Shire 
>> Shadowfang buy 37 carrot and 33   onion and 7 carpet and 9 key if Gandalf at Shire
OK 
>> Shadowfang total ?
37 carrot and 33 onion and 7 carpet and 9 key
>> Elrond buy 37 carrot and 33   onion and 7 carpet from Shadowfang and Shadowfang go to Rohan if Merry has less than 1 ring and Gandalf at Shire
OK 
>> Frodo buy Bree
INVALID 
>> Elrond total ?
37 carrot and 33 onion and 7 carpet
>> Frodo go to carpet
INVALID 
>> Witch_King sell 2 ring to Sauron if Frodo at Witch_King
INVALID 
>> Balrog buy 55 key if Gandalf at Shire
OK 
>> Sauron buy 2 key from Balrog if Sam at Rivendell and Legolas go to Mordor and Shadowfang buy 3 carrot and Gimli buy 2 carrot from Shadowfang if Elrond has more than 2 onion and Shadowfang at Rohan and Sauron buy 3 key
OK 
>> Sauron total ?
5 key
>> Frodo buy 2 Legolas from Sauron
INVALID 
>> Legolas go to Mordor
OK 
>> Sauron buy 9 ring from Witch_King if Legolas at Mordor
OK 
>> Sauron total ?
5 key
>> Witch_King total ring ?
0
>> Sauron and Witch_King total key ? 
5
>> exit
EXAMPLE 2
>> Frodo buy 3 key
OK 
>> Sam and Gimli buy 2 key from Frodo
OK 
>> Sam total ?
NOTHING
>> Frodo total ?
3 key
>> Sam and Gimli buy 1 key from Frodo
OK 
>> Frodo total ?
1 key
>> exit
EXAMPLE 3
>> Galadriel sell 1 ring
OK 
>> ring buy 3 key
INVALID 
>> Galadriel go to Rivendell and Rivendell buy 1 carrot    
INVALID 
>> Galadriel and Galadriel buy 1 carrot
INVALID 
>> Galadriel buy 1 carrot from Galadriel
INVALID 
>> Galadriel buy 1 carrot and 2 carrot  
INVALID 
>> exit
EXAMPLE 4
>> Frodo go to Rivendell and buy 1 ring and 2 bread if Sam at Shire
INVALID 
>> who at Shire and Sam go to Mordor if Frodo has 1 ring
INVALID 
>> Frodo total ring ?
0
>> Frodo buy 3 map and go to Rivendell if Frodo total ring is more than 0
INVALID 
>> who at Mordor ?
NOBODY
>> exit

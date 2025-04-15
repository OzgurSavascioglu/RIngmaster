# CMPE 230 - HW1

**Course:** Spring 2024

## Purpose

The project aims to create a language interpreter for book writers that checks the grammatical correctness of sentences according to constructed language rules and keeps track of character inventories. The interpreter will analyze the type of question and sentence input and terminate when receiving the exit command.

Our constructed language rules consist of three main entity types (subject, item, and location) and two primary sentence-type inputs, one for actions and another for conditions. The project also supports writers in creating complex sentences that combine these two types.

The constructed language only allows specific question types (`total …`, `total`, `who at`, `where`), action types (`buy`, `buy from`, `sell`, `sell to`, `go to`), and condition types (`at`, `has`, `has less than`, and `has more than`). It is possible to improve the model if necessary.

## Design and Implementation

The project is implemented in C programming language. We defined two main tasks for each input:

1. **Split and Check**: This task checks the validity of the input and produces an invalid output if necessary.
2. **Process**: This task makes the necessary changes to the data and produces the output.

For sentence-type inputs, we strictly separate these two processes to ensure that invalid inputs or unmet conditions do not alter the data. The question-type inputs, which don't contain sequential or complex sentences, are checked and processed consecutively.

For sentence-type inputs, the main program calls a function that splits the data into blocks (struct entities with doubly-linked list data structures). Each block's structure is then validated. The blocks are further split into sentences, which are also struct entities with doubly-linked list data structures. We validate each sentence during this process and determine details about actions and conditions in the input. Actions and conditions are stored as struct entities.

After completing the Split and Check task, the process-type functions are called for each block. These functions check conditions and update the corresponding data tables.

We used arrays to create tables for different entity types. Locations and items are stored in corresponding string arrays. We also created a dedicated struct for the subject, which contains details about the location, inventory item names, and amounts. A Subject struct array stores and accesses all subjects in the book.

## Challenges

The main challenge faced was dealing with memory issues related to copying and splitting strings. Initially, we experienced inconsistencies in the output. Identifying the cause of these inconsistencies was difficult, as we weren't familiar with C's memory structure.

After debugging, we discovered two main issues:
1. Copying strings using the `strcpy()` function.
2. Missing `\0` characters at the end of split strings.

To resolve these issues, we replaced `strcpy()` with `strdup()` and revised our string splitting function (`safeSplit()`).

## Usage of the Program and Examples of Input/Output

The program starts by running the `./ringmaster` command in the terminal. The user enters inputs and receives corresponding outputs in the terminal. The project includes a `MakeFile` that automatically updates the `ringmaster` file as the linked files are updated.

### Example Usage

```bash
>> Frodo go to Bree if Sauron has 0 ring
OK
>> Sam go to Rivendell and Gandalf go to Shire if Frodo at Bree and Sauron has less than 3324324 key and Gimli buy 2 axe and Legolas buy 1 bow if Frodo at Bree and Sam buy 7 carrot if Sam at Rivendell
OK
>> Gandalf where ?
Shire
>> Shadowfang buy 37 carrot and 33 onion and 7 carpet and 9 key if Gandalf at Shire
OK
>> Shadowfang total ?
37 carrot and 33 onion and 7 carpet and 9 key
>> Elrond buy 37 carrot and 33 onion and 7 carpet from Shadowfang and Shadowfang go to Rohan if Merry has less than 1 ring and Gandalf at Shire
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


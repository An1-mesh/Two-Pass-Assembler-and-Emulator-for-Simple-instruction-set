# Two-Pass-Assembler-and-Emulator-for-Simple-instruction-set

Program format and structure:
=============================
1. The assembler:

    * with two data structures to store respectively labels and mnemonic table with expected operands.
    * uses a single routine for both passes.
    * detects label errors.
    * consistent and sensible formatting with sensible program structure. 
    * sensible variable, function & type names with explanatory comments.
    * advanced listing file.
    * assembles test programs.
    * can implement and demonstrate the instruction SET.

2. The emulator :

    * with three data structures to store respectively memory, mnemonic table with expected operands.
    * loads object file. 
    * by giving the instruction "-dump", the program can produce memory dump after execution.
    * by giving the instruction like "-trace", the program prints the result of program executed.
    * detects errant programs.

Testing:
=============================

1. The assembler:

I have tested  five examples shown as below. The errors only occurred in the file test2.asm and 
the program immediately stopped on the second pass. 

The others were tested without any errors detected and produced three output files, listing file, 
error log file and machine readable object file, for each example. 

After comparing the listing files and object file of memcpy.asm , memcpy.l, memcpy.o, triangle.asm, triangle.l, triangle.o 
with the implemented result given on the assignment, both of them show no difference 
from those provided
 

1)
#Input: asm test1.asm
#Output: 
a. test1.l 
b. test1.log (without any error messages)
c. test1.o

2)
#Input:asm test2.asm
#Output: 
a. test2.l 
b. test2.log (without error messages shown as below but also in log file)
c. test2.o

error messages:
ERROR: A duplicate label was found on line 4
ERROR: There is a non-numerical value on line 6
ERROR: A numerical value was expected on line 7
ERROR: A numerical value was not expected on line 8
ERROR: Only one numerical value was expected on line 9
ERROR: A bogus label name was found on line 10
ERROR: An unknown mnemonic was found on line 11
ERROR: An unknown mnemonic was found on line 12
ERROR: A non existant label was found on line 5

3)
#Input:asm test3.asm
#Output: 
a. test3.l 
b. test3.log (without any error messages)
c. test3.o


4)
#Input:asm memcpy.asm
#Output: 
a. memcpy.l 
b. memcpy.log (without any error messages)
c. memcpy.o

5)
#Input:asm triangle.asm
#Output: 
a. triangle.l 
b. triangle.log (without any error messages)
c. triangle.o

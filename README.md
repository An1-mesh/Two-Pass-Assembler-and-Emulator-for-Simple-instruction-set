# Two-Pass-Assembler-and-Emulator-for-Simple-instruction-set

Program format and structure:
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

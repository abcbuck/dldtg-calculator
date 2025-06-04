# General

A brute-force solver to help with NAND gate minimization challenges in [Digital Logic Design - The Game](https://asteriskman7.github.io/dldtg/).\
Only works for circuits with few gates. Doesn't use multithreading.

# Usage

Watch your RAM when you run this program. Memory doesn't fill too quickly but the program doesn't do any memory checks. Close the program when your RAM is full. You likely won't get any result in the foreseeable future.

Open from command line to avoid loss of output.

Then enter the outputs to optimize for. Outputs are given as a truth table of the form

```
A B C Y  Z  ...
0 0 0 y0 z0 ...
0 0 1 y1 z1 ...
0 1 0 y2 z2 ...
0 1 1 y3 z3 ...
1 0 0 y4 z4 ...
1 0 1 y5 z5 ...
1 1 0 y6 z6 ...
1 1 1 y7 z7 ...
```

Enter the truth table as the strings `y0y1y2y3y4y5y6y7`, `z0z1z2z3z4z5z6z7`, ... . A truth table is valid if the length of the entered strings is consistent, a power of two and none of the outputs are equal to any of the inputs. Duplicate outputs are treated as one output.

## Example output

```
Please enter the number of desired truth tables: 2
Please enter desired truth table 1: 00001000
Please enter desired truth table 2: 11111110
Number of inputs: 3

Initializing... done.
Creating additional circuits of size 1...2...3...4...5...6...7...
Found solution of size 7!!
00001000
00001010
00001111
00110011
01010101
11001101
11110101
11110111
11111010
11111110

Netlist definition:
DEF FUNCTION_NAME
  PORT IN A # 00001111
  PORT IN B # 00110011
  PORT IN C # 01010101
  PORT OUT Y # 00001000
  PORT OUT Z # 11111110

  NET N1
  NET N2
  NET N3
  NET N4
  NET N5

  INST I1 NAND A C N1 # 11111010
  INST I2 NAND B N1 N2 # 11001101
  INST I3 NAND A N1 N3 # 11110101
  INST I4 NAND N3 N3 N4 # 00001010
  INST I5 NAND N2 N4 N5 # 11110111
  INST I6 NAND N5 N5 Y # 00001000
  INST I7 NAND B N2 Z # 11111110
ENDDEF
```

### Explanation

The program is looking for a size-optimal solution to this truth table:
```
A B C  Y Z
0 0 0  0 1
0 0 1  0 1
0 1 0  0 1
0 1 1  0 1
1 0 0  1 1
1 0 1  0 1
1 1 0  0 1
1 1 1  0 0
```

where A, B, C are the inputs and Y, Z are the outputs.

It incrementally checks all circuits of a certain size until it finds a circuit that fulfills the given truth table (*Note*: That might never happen because the search space is very large).

When a solution is found, all truth value configurations that are part of the corresponding circuit as the output of some NAND gate are printed to screen.

Then, in addition, the program creates a netlist from this set of truth value configurations, providing comments on which configurations belong to which nets and gates.
All variable names in the netlist are set to default values as shown.

# Further information

If you have further questions, please check the [Issues](https://github.com/abcbuck/dldtg-calculator/issues) page for similar questions and open an issue with the 'question' label if your question hasn't been asked before. I'll reply when I can.\
Feel free to fork this project and improve on it. Let me know if you do!

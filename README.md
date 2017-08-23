# SudokuSATSolver

Converts sudo puzzle into Conjunctive Normal Form (CNF) for use in SAT solvers.

For example 
```
....
12..
3...
...1
```
Will be converted to 
```
p cnf 64 254
17 0
22 0
35 0
61 0
1 2 3 4 0
5 6 7 8 0
9 10 11 12 0
13 14 15 16 0
25 26 27 28 0
29 30 31 32 0
37 38 39 40 0
.
.
.
etc
```

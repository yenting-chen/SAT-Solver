## SAT Solver
An implementation of Davis–Putnam–Logemann–Loveland (DPLL) algorithm for solving boolean satisfiability problem (SAT), implemented using C++ and DIMACS CNF parser by MiniSat.

## Features
### 2-literal Watching
To do/undo variable assignment efficiently, the solver maintains, for each variable, a list of occurrence locations in clauses which are watched, i.e. a list of pointers to literal in clause. It also maintains a table recording changes of those pointers due to variable assignment.

Upon variable assignment, the lists are changed accordingly and checked to see if there is any conflict or unit clause under the current assignment.

### Non-Chronological Backtracking
1-UIP cut and conflict clause is found by repeatedly resolving the current cut and a clause, and a simulation of variable assignment is done on the learned clause to update the list and table mentioned above.

### Heuristic
Variables are sorted in decreasing order of number of occurrences in clauses. The ordering is used to assign true/false values to variables, and the values assigned are randomized by using rand() and a static seed.

### Random Restart
If the solver runs for more than 10 seconds, it is aborted and a new solver is created.

All previous results are discarded except for the learned clauses.

The variable ordering used in assignment heuristic is recalculated, based on both the original and learned clauses.

### Clause Learning Policy
It is sometimes beneficial to discard a learned clause that is too long. So in this implementation, 5 solvers are created, numbered from 1 to 5, and the i-th solver discards a learned clause if the clause size is more than max⁡(5, n*i/5).

They take turns running for 0.5 seconds, until one of them finds a satisfying assignment.

## How to Run
```
$ make
$ ./yasat <input.cnf>
```

## Benchmark
### Environment
Windows 8.1 Enterprise 64-bit, i7-6700k, 16GB ram. Compiled with –/O2 optimization.

### Test Instances
uniform random-3-sat problems from http://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

### Result
Record maximum execution time among five problem instances for each category.
150 variables with 645 clauses(sat): <0.07 second

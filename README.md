## SAT Solver
An implementation of Davis–Putnam–Logemann–Loveland (DPLL) algorithm for solving boolean satisfiability problem (SAT), implemented using C++ and DIMACS CNF parser by MiniSat.

## Features
### 2-literal Watching
To do/undo variable assignment efficiently, the solver maintains, for each variable, a list of occurrence locations in clauses which are watched, i.e. a list of pointers to literal in clause. It also maintains a table recording changes of those pointers due to variable assignment.

Upon variable assignment, the lists are changed accordingly and checked to see if there is any conflict or unit clause under the current assignment.

### Non-Chronological Backtracking
1-UIP cut and conflict clause is found by repeatedly resolving the current cut and a clause, and simulation of variable assignments is done on the learned clause to update the list and table mentioned above.

### Heuristic - Variable Ordering
Variables are sorted in decreasing order of number of occurrences in clauses. The ordering is used to assign true/false values to variables, and the values assigned are randomized by using rand() and a static seed.

### Random Restart
If the solver runs for more than 100 seconds, it is aborted and a new solver is created.

All previous results are discarded except for the learned clauses.

The variable ordering used in assignment heuristic is recalculated, based on both the original and learned clauses.

### Clause Learning Policy
It is sometimes beneficial to discard a learned clause that is too long. So in this implementation, 5 solvers are created, numbered from 1 to 5, and the i-th solver discards a learned clause if the clause size is more than max⁡(5, n*i/5).

They take turns running for 10 seconds, until one of them finds a satisfying assignment.

## How to Run
Construct a SAT problem in DIMACS CNF format.

For example, a text file with the following content:
```
```

Compile solver source code.
```
$ make
```

Run the solver.
```
$ ./yasat sample.cnf
```
The solver will read from `sample.cnf` and write a satisfying assignment to `sample.sat`, which looks like the following:
```
```
If the SAT problem has no solution, the solver outputs `s UNSATISFIABLE`.

Remove solver and temporary files.
```
$ make clean
```

## Benchmark
### Environment
Windows 8.1 Enterprise 64-bit, i7-6700k, 16GB RAM

Microsoft Visual C++ 2019

### Test Instances
Instances from [SATLIB - Benchmark Problems](http://www.cs.ubc.ca/~hoos/SATLIB/benchm.html).

### Result
#### Uniform Random-3-SAT - 150 variables, 645 clauses, Satisfiable
Instance Name | Time (s)
:-:|:-:
uf150-01.cnf | 0.01
uf150-02.cnf | 0.04
uf150-03.cnf | 0.49

#### Uniform Random-3-SAT - 150 variables, 645 clauses, Unsatisfiable
Instance Name | Time (s)
:-:|:-:
uuf150-01.cnf | 4.04
uuf150-02.cnf | 3.18
uuf150-03.cnf | 2.96

#### Uniform Random-3-SAT - 175 variables, 753 clauses, Satisfiable
Instance Name | Time (s)
:-:|:-:
uf175-01.cnf | 0.9
uf175-02.cnf | 15.0
uf175-03.cnf | 1.1

#### Uniform Random-3-SAT - 175 variables, 753 clauses, Unsatisfiable
Instance Name | Time (s)
:-:|:-:
uuf175-01.cnf | 102.8
uuf175-02.cnf | 2459.0
uuf175-03.cnf | 53.8

# Data structures and algorithms - project
## Exercise 3

### This is an exercise, which may (and likely does) contain errors!

Time analysis for AVL and RB trees.
To generate `csv` file with necessary data you can use `generate_random`.
The main analysis is conducted within `zad3`.
In `config.ini` file you can specify `database` filename, how many integers from `database` you need to load and `results` filename. Each line after that specifies operations.

## Operations:
Everything is divided by spaces.
First, specify type tree, so either `AVL` or `RB`.
Next, specify operation type: `CREATE`, `INSERT`, `DELETE` or `SEARCH`.
Then specify instances sizes, min and max.
Lastly specify step size and how many repetitions you need.

So, examplary input for creation of AVL tree, sizes from 1000 to 5000, with step of 100 and 10 repetitions would be:
```
AVL CREATE 1000 5000 100 10
```
# Project

This project is a Frog interpreter written ic C, plus a chess game and an Emacs-like editor for Frog.

# Frog
Frog is a toy-language language that is very similar to Python in semantic, but has a C-like syntax.
This language is a toy developed in a school setting, it is not intended to be used.

Frog has default libraries and data structures:
* string
* dictionaries
* set
* tuples
* iterators
* objects (OOP)
* multithreading
* graphic library
* mouse and keyboard control library

# Example

```
function factorial(i)
{
    if (i <= 0)
        return 1;

    res = 1;

    foreach(v : range(2, i + 1))
        res *= v;
}
```

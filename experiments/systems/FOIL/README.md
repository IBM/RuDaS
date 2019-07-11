# How to obtain FOIL

* Source code downloadable at <https://www.rulequest.com/Personal/>

# How to run FOIL script

./foil6 -n -v0 < even.d

## Sintax:

### File: 
 extension .d

### Types: 

* N: integers, 
* C: continuous. 
* E: elements, 
* L: lists (example: [123] )
  
### Examples facts

If the predicate has * in front is not a target.

There have the following format:

p1(type)
arg
.
p2(type,type)
arg1,arg2
arg1,arg2
arg1,arg2
arg1,arg2
.
p2(type)
arg
arg
arg
.
    
### Test facts (optional)
Test fact are optional and have the following format
    pred [no variables]
    example: +
    example: -
    .

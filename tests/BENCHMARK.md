### BASELINES

## HARD
median: 2.4830 ms
minimum: 1.5992 ms
ns/node: 178.3

## EASY 
median: 0.0036 ms
minimum: 0.0022 ms
ns/node: 76.75

### FIRST UPGRADE 
After the first profiling, popcount was consuming 17seconds
When changed the compiler options, so the compiler used native
instructions, reduced to almost-0 

## HARD
median: 1.7131 ms  
minimum: 1.0902 ms
ns/node: 124

## EASY
median: 0.0016 ms
minimum: 0.0014 ms
ns/node: 24 

### SECOND UPGRADE
The second upgrade made the cell search algorithm a bit faster
Instead of the MRV going throug the whole board, it sored a mask
with the empty cells, so insted of going through the array, it 
makes bitwise ops, and apply the same method.
Also, the return conditions in clear and modify cell were eliminated
as they were unused and added operations and branches to the code

## HARD
- median: 0.6500 ms
- minimum: 0.6264 ms
- ns/node: 47.1

## EASY
- median: 0.0010 ms
- minimum: 0.0009 ms
- ns/node: 20.7
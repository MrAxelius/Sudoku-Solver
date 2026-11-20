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

### New benchmark -- Second Upgrade 
id	nodes	median ms	min ms	    P90 ms	    cv %	ns/node
#01	13811	0.623547	0.602300	0.660374	3.65	45.1
#02	4876	0.179316	0.174826	0.194886	4.46	36.8
#03	654	    0.020887	0.020516	0.023145	5.48	31.9
#04	213	    0.006537	0.006364	0.006988	4.04	30.7
#05	1517	0.054670	0.053557	0.058702	4.03	36.0
#06	79  	0.002724	0.002676	0.002992	5.23	34.5
#07	220	    0.006750	0.006334	0.007221	4.66	30.7
#08	96	    0.003324	0.003227	0.003648	5.23	34.6
#09	293	    0.007872	0.007669	0.008491	5.33	26.9
#10	175	    0.007255	0.007036	0.007670	3.32	41.5
#11	255	    0.007264	0.007106	0.008016	5.89	28.5
#12	504	    0.015290	0.014972	0.016228	4.49	30.3
SET	22693	0.9354				                        41.2
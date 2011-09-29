This is a simple command line program to find how long it takes for bounded rule 30 cellular automata to cycle.

to run, simply execute  ./cellular 15

where you replace 15 with the number of cells you want.  The automation starts with a single cell enabled, and all others disabled.

I wrote this because I was curious about the chaos of rule 30 (wikipedia if confused).
I was curious how long it would keep producing unique states if I restricted it to a finite length circle.

The way I implement it is pretty cool.  I use a hash set to keep track of all previous states.

However, I don't want to consider a state to be different if it is just a
rotation of a previous state. (otherwise I would spend a lot of time just
counting the same loop over and over.) In order to do this, I defined a
rotation invariant equality operator (just try all possible rotations and see
if you can get the two states to match up) and a rotation invariant hash
function (xor the hash functions of all possible rotations of the input
together).  I use these for the hash set.

I am guessing that this program is useless theoretically, as there is probably
a way to calculate these numbers without so much computation



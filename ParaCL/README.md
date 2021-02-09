# ParaCL


## Levels

#### LVL1 (done):


##### Algorithm:


#### LVL2 (in dev):
The peculiarity of the ParaCL language is that everything, 
including the function, is an object.

The func keyword is used to inject a function, followed by the argument names and scope

a = 1;                       \
foo = func (x) {x * 42; }    \
b = foo (a);                 \
print b; // on screen 42

Arguments in this syntax are passed by value, not by reference. 
Do not think about passing by link yet, link semantics in the piglet is monstrous, 
inhuman and is a subject of higher levels

The result of the function is the last expression inside the scope

bar = func () {   \
x = 5;            \
y = 10;           \
x + y;            \
}; // semicolon is possible here but not required

t = bar (); // t == 15

Explicit return is also possible

buz = func (x) { \
if (x! = 5)      \
return x;        \
y = 10;          \
x + y;           \
}

z = buz (6); // z == 6

Just like a function, returning a value from any scope works

t = {x = 5; y = 10; x + y; } // t == 15

Functions differ from blocks in that they can be called multiple times.

The function can have a name, if needed (for example, for recursion) 
then it is written separated by a colon

fact = func (x): factorial { \
res = 1;                     \
if (x> 0)                    \
res = factorial (x - 1);     \
res;                         \
}

The function name is always available in all scopes, 
the name of the variable in which it was written is only in the scope in which it lives

##### Algorithm:
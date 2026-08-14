% Variable listing works in script mode and REPL.

vars;
A = [1 2; 3 4];
x = det(A);
v = [1:3];
vars;
who;
del x;
vars;
clean;
vars;

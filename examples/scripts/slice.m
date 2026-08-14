% Matrix and vector slicing.

A = [1 2 3; 4 5 6; 7 8 9];

col1 = A[:, 1]
block = A[1:2, 2:3]
row2 = A[2, 1:3]
a21 = A[2, 1]
rev_col = A[3:-1:1, 3]

v = [10 20 30 40];
v_mid = v[2:3]
v_one = v[4]

u = [10; 20; 30; 40];
u_mid = u[2:3]

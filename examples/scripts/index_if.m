% Indexing, shape/size, and if/elif/else demo.

A = [1 2; 3 4];
v = [10:10:30];
u = [4; 5; 6];

a12 = A(1, 2)
v3 = v(3)
u2 = u(2)
shape_A = shape(A)
rows_A = size(A, 1)
cols_A = size(A, 2)

if A(1, 2) == 2
    branch = 10;
elif missing_variable > 0
    branch = 20;
else
    branch = 30;
end
branch

if det(A) > 0
    sign = 1;
elif det(A) < 0
    sign = -1;
else
    sign = 0;
end
sign

if 0
    fallback = 1;
elif 0
    fallback = 2;
else
    fallback = 3;
end
fallback

total = 0;
for k = 1:3
    if k == 2
        total = total + 20;
    else
        total = total + k;
    end
end
total

% Matrix Hub script mode demo.
% Semicolon suppresses console output; print(...) always writes output.

A = [1 2; 3 4];
B = eye(2);
C = A * B + [10 0; 0 10];

print(C);

A_inv = inv(A);
identity_check = A * A_inv

d = det(A)
t = tr(A)
n = norm(A, 2)

v = [1; 2; 3];
D = diag(v)

T = A'

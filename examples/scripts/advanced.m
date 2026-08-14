% Matrix Hub script mode: nested functions and a simple loop.

A = [1 2; 3 4];
X = zeros(2, 2);

for k = 1:3
    X = X + k * A;
end

print(X);

B = inv(A + eye(2)) * transpose(A)
r = rank(B)
fro_norm = norm(B, FRO)

top_left = cut(X, 1, 1, 1, 1)
mask = logic_equal(B, 0)

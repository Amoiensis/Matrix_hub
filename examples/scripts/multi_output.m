% Multi-output functions keep related matrix results under one function name.

A = [4 1; 2 3];

[Q, R] = qr(A);
qr_shape = shape(Q) + shape(R)

[D, V] = eigen(A);
eigen_shapes = shape(D) + shape(V)

[lambda, max_vec] = eigen_max(A);
lambda
max_vec_shape = shape(max_vec)

[U, S, W] = svd(A);
svd_shapes = shape(U) + shape(S) + shape(W)

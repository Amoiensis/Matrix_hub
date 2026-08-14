% Multi-output transformation functions.

A = [4 1 2; 2 5 1; 1 0 3];

[Tu, U] = uptri(A);
uptri_shapes = shape(Tu) + shape(U)

[Tl, L] = lowtri(A);
lowtri_shapes = shape(Tl) + shape(L)

[Td_left, D, Td_right] = diatri(A);
diatri_shapes = shape(Td_left) + shape(D) + shape(Td_right)

x = [3; 4; 0];
[H, y] = householder(x);
householder_shapes = shape(H) + shape(y)

[Qh, Hessen] = hessenberg(A);
hessenberg_shapes = shape(Qh) + shape(Hessen)

% Compatibility names can also be used in multi-output assignment.
[Tu2, U2] = uptri_trans(A);
[Tl2, L2] = lowtri_trans(A);
compat_shapes = shape(Tu2) + shape(U2) + shape(Tl2) + shape(L2)

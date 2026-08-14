% 1x1 matrix results are promoted to scalar-like values in mixed expressions.

a = [1 2 3];
b = [1; 2; 4];

left_scalar = a * b
outer = b * a
scaled = (b * a) * (a * b)
scaled_left = (a * b) * (b * a)

\ Source: Google OR-Tools MIP array example
\ URL: https://developers.google.com/optimization/mip/mip_var_array
\ Variables x1..x5 are nonnegative integers.

Maximize
 obj: 7 x1 + 8 x2 + 2 x3 + 9 x4 + 6 x5
Subject To
 c1: 5 x1 + 7 x2 + 9 x3 + 2 x4 + x5 <= 250
 c2: 18 x1 + 4 x2 - 9 x3 + 10 x4 + 12 x5 <= 285
 c3: 4 x1 + 7 x2 + 3 x3 + 8 x4 + 5 x5 <= 211
 c4: 5 x1 + 13 x2 + 16 x3 + 3 x4 - 7 x5 <= 315
Bounds
 0 <= x1
 0 <= x2
 0 <= x3
 0 <= x4
 0 <= x5
Generals
 x1 x2 x3 x4 x5
End

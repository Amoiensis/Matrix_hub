\ Source: Google OR-Tools MIP example
\ URL: https://developers.google.com/optimization/mip/mip_example
\ Expected optimum: objective = 23, x = 3, y = 2

Maximize
 obj: x + 10 y
Subject To
 c0: x + 7 y <= 17.5
 c1: x <= 3.5
Bounds
 0 <= x
 0 <= y
Generals
 x y
End

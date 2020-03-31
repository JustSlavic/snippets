import numpy as np
from math import *

points = []

R = np.arange(-1, 1.01, 0.05)

i = 1

for x in R:
    for y in R:
        if x**2 + y**2 <= 1.001:
            points.append((x, y))

with open('data/init2.data', 'w') as f:
    f.write(str(len(points)) + '\n')

    for p in points:
        f.write(str(p[0]) + ' ' + str(p[1]) + ' 0.3\n')

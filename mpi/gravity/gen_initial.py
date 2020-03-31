from math import *
from random import uniform

def main():
    n = 200
    a = 14

    G = 10

    M = 400000

    with open('data/random.init', 'w') as f:
        f.write(str(n) + '\n')

        f.write('0 0 0 0 ' + str(M) + '\n')

        for i in range(n):
            x = uniform(-a, a)
            y = uniform(-a, a)
            d = sqrt(x**2 + y**2)
            f.write(str(x) + ' ' + \
                    str(y) + ' ' + \
                    str(-y*sqrt(G*M/d)/d) + ' ' + \
                    str( x*sqrt(G*M/d)/d) + ' ' + \
                    '1\n')

if __name__ == '__main__':
    main()

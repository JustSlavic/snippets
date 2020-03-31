import matplotlib.pyplot as plt
from math import *
import numpy as np

class Figure:
    def draw(self, color = 'black'):
        plt.plot([p[0] for p in self.points], 
                 [p[1] for p in self.points], color)

    def apply(self, A):
        self.points = [np.dot(x, A) for x in self.points]

class ClosedFigure(Figure):
    def draw(self, color = 'black'):
        plt.plot([p[0] for p in self.points] + [self.points[0][0]], 
                 [p[1] for p in self.points] + [self.points[0][1]], color)        

class Line(Figure):
    def __init__(self, p1 = (0,0), p2 = (1,1)):
        self.points = [np.array(p1 + (1,)), np.array(p2 + (1,))]

class Ellipse(ClosedFigure):
    def __init__(self, a = 1, b = 1, n = 100):
        T = np.arange(0, 2*pi, 2*pi/(n - 1))
        xs = [a*cos(t) for t in T]
        ys = [b*sin(t) for t in T]
        pairs = zip(xs, ys)
        self.points = [np.array(p + (1,)) for p in pairs]

class Circle(Ellipse):
    def __init__(self, radius = 1, n = 100):
        Ellipse.__init__(self, a = radius, b = radius)
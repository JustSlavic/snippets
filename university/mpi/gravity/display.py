import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, writers
import numpy as np
from config_animation import config

def setup_axes(points):
    fig, ax = plt.subplots()

    x_min = np.min([p[0] for p in points])
    x_max = np.max([p[0] for p in points])
    y_min = np.min([p[1] for p in points])
    y_max = np.max([p[1] for p in points])

    ax.set_xlim(x_min - 1, x_max + 1)
    ax.set_ylim(y_min - 1, y_max + 1)

    ax.set_xlim(-15, 15)
    ax.set_ylim(-15, 15)

    ax.grid(True)

    return fig, ax


def main():
    with open(config['timeline']) as f:
        n = int(f.readline())

        points = []
        for line in f:
            x = line.split(' ')
            points.append([float(x[0]), float(x[1])])

        fig, ax = setup_axes(points)
        xdata, ydata = [], []
        ln, = plt.plot([], [], '.', animated=True, markersize=5)

        def update(frame):
            xdata = []
            ydata = []
            for k in range(n):
                xdata.append(points[n*frame+k][0])
                ydata.append(points[n*frame+k][1])
            ln.set_data(xdata, ydata)
            return ln,

        ani = FuncAnimation(fig, update, frames=range(len(points)//n),
                        blit=True)

        if (config['save_video'] == True):

            writer = writers['ffmpeg'](fps=config['fps'], 
                metadata=dict(artist='Me'), 
                bitrate=config['bitrate'])

            ani.save(config['output'], writer=writer)
        elif (config['trace'] == True):
            plt.scatter([p[0] for p in points], [p[1] for p in points], s=2)
            plt.show()
        else:
            plt.show()



if __name__ == '__main__':
    main()
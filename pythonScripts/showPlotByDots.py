import matplotlib.pyplot as plt
import numpy as np

# Data for plotting
t = np.arange(0.0, 2.0, 0.01)
s = 1 + np.sin(2 * np.pi * t)

class Plot:
    def __init__(self, xs, ys, color, label) -> None:
        self.xs    = xs
        self.ys    = ys
        self.color = color
        self.label = label

    def addDot(self, x, y) -> None:
        self.xs.append(x)
        self.ys.append(y)

    def draw(self, ax) -> None:
        ax.plot(self.xs, self.ys, color=self.color, label=self.label)

fig, ax = plt.subplots()
plots = []
with open("pythonScripts/plotPoints.txt", "r") as file:
    plot = Plot([], [], "", "")
    isFirst = True
    for line in file.readlines():
        if len(line) <= 1 or line == "\n":
            if len(plot.xs) != 0:
                plots.append(plot)
                #print(plot.xs, plot.ys, plot.label, plot.color)
            isFirst = True
            continue

        if isFirst:
            color, label = line.split()
            plot = Plot([], [], color, label)
            isFirst = False
            continue

        x, y = map(float, line.split())
        plot.addDot(x, y)

    plots.append(plot)

    file.close()

# print("xs : ", xs)
# print("ys : ", ys)
# ax.plot(xs, ys)

for plot in plots:
    plot.draw(ax)
plt.legend()

ax.set(xlabel='x', ylabel='y',
       title='biba boba buba baba')
ax.grid()

fig.savefig("pythonScripts/plot.png")
plt.show()

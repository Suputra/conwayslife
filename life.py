# /// script
# requires-python = ">=3.12"
# dependencies = [
#     "matplotlib",
#     "scipy"
# ]
# ///
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
import numpy as np
import scipy.signal as sig
import time

GRIDSIZE = 100

NEIGHBORS = np.array([
    [1, 1, 1],
    [1, 0, 1],
    [1, 1, 1]
])

grid = np.zeros((GRIDSIZE, GRIDSIZE))
timeflow = False

def gridselect(event):
    # Only act if the click is inside the plot area
    if not event.inaxes:
        return
    x = round(event.xdata)
    y = round(event.ydata)
    
    # Toggle the cell value
    grid[y, x] = 1 - grid[y, x]
    
    # Update only the data instead of recreating the image
    img.set_array(grid)
    plt.draw()
    print(f"set {x} {y}")


def pauseplay(event):
    global timeflow
    timeflow = not timeflow
    print(f"time is {timeflow}")

# some initialization
fig, ax = plt.subplots()
img = ax.imshow(grid, cmap='binary', origin='lower')
fig.canvas.mpl_connect('button_press_event', gridselect)

axpauseplay = fig.add_axes([0.0, 0.0, 0.2, 0.075])
pauseplaybutton = Button(axpauseplay, 'Pause/Play')
pauseplaybutton.on_clicked(pauseplay)

def tick():
    global grid  # Need to declare global since we're reassigning
    counts = sig.convolve2d(grid, NEIGHBORS, boundary="wrap", mode="same")
    new_grid = np.zeros_like(grid)  # Create new grid to avoid modifying while calculating
    new_grid[(counts == 3) | ((grid == 1) & (counts == 2))] = 1
    grid = new_grid
    
    # Update only the data
    img.set_array(grid)
    fig.canvas.draw_idle()
    print("tick")

# main loop
plt.ion()  # Turn on interactive mode
plt.show()
while True:
    if timeflow:
        tick()

    plt.pause(0.1)  # Add small delay and handles GUI events

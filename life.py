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
import matplotlib.animation as animation

GRIDSIZE = 50  # Made smaller for better visibility

NEIGHBORS = np.array([
    [1, 1, 1],
    [1, 0, 1],
    [1, 1, 1]
])

# Initialize grid with some cells alive
grid = np.zeros((GRIDSIZE, GRIDSIZE))
timeflow = False

def gridselect(event):
    # Only act if the click is inside the main plot area
    if event.inaxes != ax:  # Check if click is specifically on the main axes
        return
    x = round(event.xdata)
    y = round(event.ydata)
    
    # Make sure we're within grid bounds
    if 0 <= x < GRIDSIZE and 0 <= y < GRIDSIZE:
        grid[y, x] = 1 - grid[y, x]
        img.set_array(grid)
        fig.canvas.draw()

def pauseplay(event):
    global timeflow
    timeflow = not timeflow

# Set up figure
fig, ax = plt.subplots(figsize=(8, 8))
img = ax.imshow(grid, cmap='binary', interpolation='nearest', vmin=0, vmax=1)
fig.canvas.mpl_connect('button_press_event', gridselect)
axpauseplay = fig.add_axes([0.0, 0.0, 0.2, 0.075])
pauseplaybutton = Button(axpauseplay, 'Pause/Play')
pauseplaybutton.on_clicked(pauseplay)

def tick(frame=None):
    global grid
    if not timeflow:
        return [img]
    counts = sig.convolve2d(grid, NEIGHBORS, boundary="wrap", mode="same")
    new_grid = np.zeros_like(grid)
    new_grid[(counts == 3) | ((grid == 1) & (counts == 2))] = 1
    grid = new_grid
    print("ticking")
    img.set_array(grid)
    return [img]

# Set up animation
ani = animation.FuncAnimation(fig, tick, interval=200, blit=True)
plt.show()

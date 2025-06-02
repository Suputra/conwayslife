#!/usr/bin/env -S uv run --script 
# /// script
# requires-python = ">=3.12"
# dependencies = ["matplotlib", "scipy"]
# ///
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.widgets import Button
from matplotlib.animation import FuncAnimation
from scipy.signal import convolve2d

NEIGHBORS = np.array([
    [1, 1, 1],
    [1, 0, 1],
    [1, 1, 1]
])
class ConwaysLife:
    def __init__(self, size=50):
        self.size = size
        self.grid = np.zeros((size, size))
        self.timeflow = False
        
        self.fig, self.ax = plt.subplots(figsize=(8, 8))
        self.img = self.ax.imshow(self.grid, cmap='binary', vmin=0, vmax=1)
        self.fig.canvas.mpl_connect('button_press_event', self.grid_select)
        self.button = Button(self.fig.add_axes([0, 0, 0.2, 0.075]), 'Play/Pause')
        self.button.on_clicked(lambda _: self.toggle_timeflow())
        
    def grid_select(self, event):
        if event.inaxes == self.ax:
            x, y = map(round, (event.xdata, event.ydata))
            self.grid[y, x] = 1 - self.grid[y, x]  
            self.img.set_array(self.grid)
            self.fig.canvas.draw()
            
    def toggle_timeflow(self):
        self.timeflow = not self.timeflow
        
    def tick(self, frame=None):
        if self.timeflow:
            neighbors = convolve2d(self.grid, NEIGHBORS, mode='same', boundary='wrap')
            self.grid = (((neighbors == 3) | ((self.grid == 1) & (neighbors == 2)))).astype(int)
            self.img.set_array(self.grid)
        return [self.img]
        
    def run(self):
        self.anim = FuncAnimation(self.fig, self.tick, interval=100, blit=True, save_count=100)
        plt.show()

if __name__ == '__main__':
    ConwaysLife().run()
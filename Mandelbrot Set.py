# Visualize Mandelbrot corner with zooming and panning + window check

import matplotlib.pyplot as plt 
import numpy as np
import random
import time

def mandelbrot(c, max_iter):
    """
    Check if a complex number c is in the Mandelbrot set, 
    based on maximum number of iterations max_iter.
    """
    z = 0
    for n in range(max_iter):
        if abs(z) > 2: # If z escapes to infinity
            return n  
        z = z*z + c # Iterate z
    return max_iter # c is likely in the set if no escape

def mandelbrot_set(xmin, xmax, ymin, ymax, width, height, max_iter):
    """
    Make a Mandelbrot set image within xmin, xmax, ymin, ymax  
    using a grid of width by height.
    """
    x_values = np.linspace(xmin, xmax, width)
    y_values = np.linspace(ymin, ymax, height)
    
    mandelbrot_array = np.empty((width, height)) # To store set 
    
    for i in range(width):
       for j in range(height):
          mandelbrot_array[i, j] = mandelbrot(complex(x_values[i], y_values[j]), max_iter)
           
    return mandelbrot_array

def random_zoom(xmin, xmax, ymin, ymax, zoom_factor):
    """ 
    Get random coordinates for zooming in,
    zoomed in by zoom_factor.
    """
    x_range = xmax - xmin
    y_range = ymax - ymin
    
    x_zoom_range = x_range / zoom_factor 
    y_zoom_range = y_range / zoom_factor

    x_center = random.uniform(xmin, xmax - x_zoom_range)
    y_center = random.uniform(ymin, ymax - y_zoom_range)

    x_min = x_center
    x_max = x_center + x_zoom_range
    y_min = y_center
    y_max = y_center + y_zoom_range
    
    return x_min, x_max, y_min, y_max
    

# Initialize plot and parameters
plt.ion()  
fig, ax = plt.subplots()
ax.axis('off') 

plt.subplots_adjust(left=0, right=1, top=1, bottom=0)

width, height = 1000, 800
max_iter = 100
xmin, xmax, ymin, ymax = -2, 1, -1.5, 1.5 
zoom = 2

for i in range(10):
    
    # Show Mandelbrot set
    mandelbrot_image = mandelbrot_set(xmin, xmax, ymin, ymax, width, height, max_iter)  
    ax.imshow(mandelbrot_image.T, extent=[xmin, xmax, ymin, ymax], cmap='hot')
    plt.draw()
    plt.pause(0.25)
    
    # Zoom into random location
    xmin, xmax, ymin, ymax = random_zoom(xmin, xmax, ymin, ymax, zoom) 
    
    time.sleep(1)
    
plt.ioff()
plt.close()
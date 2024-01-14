# mandelbrot-visualizer
Simple Python program to visualize the Mandelbrot set uses Matplotlib. Generates the set and zooms and pans

# Mandelbrot Set Visualizer

This is a basic Python program to visualize the famous Mandelbrot set using Matplotlib.

## Usage

To run the program, simply execute `mandelbrot.py`. This will open an interactive Matplotlib window showing the Mandelbrot set. 

You can zoom in and pan around the set using the mouse and toolbar controls in the Matplotlib window. The program will continuously update the view with more detail as you zoom.

The `mandelbrot.py` file contains the main implementation. The key functions are:

- `mandelbrot` - Determines if a point is in the Mandelbrot set 
- `mandelbrot_set` - Renders the set as an image
- `random_zoom` - Generates random coordinates to zoom into

## Requirements

- Matplotlib
- NumPy

Install requirements using `pip install matplotlib numpy`

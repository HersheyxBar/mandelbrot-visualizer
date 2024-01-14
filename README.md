# mandelbrot-visualizer

Simple Python program to visualize the Mandelbrot set uses Matplotlib. Generates the set and zooms and pans
![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/58051d51-93c8-46c6-ba5b-93e621caed73)

![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/2b94f6b2-73ed-4bb2-89da-df20a303c546)

![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/e906f1bf-9003-496f-81a0-09355c6b087b)

![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/24d76e1e-f293-43b6-b616-af24d9ebc0c0)

![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/d7de23dc-804f-42a3-a9bd-061b07238820)

![image](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/d807235b-da5d-4fdc-9fee-e9c02098102a)


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




# Mandelbrot Visualizer

A dual-language (Python and C) application for rendering and exploring the Mandelbrot set, with capabilities for automatic zooming and panning to visually interesting points.

## Overview

This project includes:

- **Python Implementation:** Uses Matplotlib for visualization, suited for educational purposes or quick visualizations.
- **C Implementation:** Utilizes SDL2 for rendering, optimized for performance with OpenMP for multi-threading.

Compile the C version with:
```bash
gcc -o mandelbrot main.c -lSDL2 -lm -fopenmp -O3
```

## Features

- **Automatic Zooming:** The program autonomously zooms into randomly selected, aesthetically pleasing parts of the Mandelbrot set.
- **Interactive Visualization:** Provides an interactive environment where users can explore the intricacies of the Mandelbrot set.
- **High Performance:** The C version leverages parallel processing for faster computation of complex set geometries.

## Visual Examples

Here are several snapshots showcasing the depth and beauty of the Mandelbrot set as rendered by this application:

![Mandelbrot Set 1](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/58051d51-93c8-46c6-ba5b-93e621caed73)

![Mandelbrot Set 2](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/2b94f6b2-73ed-4bb2-89da-df20a303c546)

![Mandelbrot Set 3](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/e906f1bf-9003-496f-81a0-09355c6b087b)

![Mandelbrot Set 4](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/24d76e1e-f293-43b6-b616-af24d9ebc0c0)

![Mandelbrot Set 5](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/d7de23dc-804f-42a3-a9bd-061b07238820)

![Mandelbrot Set 6](https://github.com/HersheyxBar/mandelbrot-visualizer/assets/35160750/d807235b-da5d-4fdc-9fee-e9c02098102a)

## Usage

### Python
Run the Python script by executing:
```bash
python mandelbrot.py
```
This will open an interactive Matplotlib window where you can view and explore the Mandelbrot set.

### C
Compile the C program with the provided command:
```bash
gcc -o mandelbrot main.c -lSDL2 -lm -fopenmp -O3
```
Run the compiled program:
```bash
./mandelbrot
```

## Functions in `mandelbrot.py`

- `mandelbrot`: Calculates if a point in the complex plane is in the Mandelbrot set.
- `mandelbrot_set`: Generates an image representation of the set.
- `random_zoom`: Performs a zoom operation to a randomly chosen part of the set.

## Requirements

### Python
- Matplotlib
- NumPy

Install these with:
```bash
pip install matplotlib numpy
```

### C
- SDL2 library (for rendering)
- OpenMP (for parallel computing)

Ensure SDL2 is installed on your system before compiling the C version.

## Installation

### Python
After installing the required libraries, you can run the script directly.

### C
Install SDL2 and ensure your development environment supports OpenMP before compilation.

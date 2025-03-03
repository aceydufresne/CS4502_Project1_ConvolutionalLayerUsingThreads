
# Project Title: Convolutional Image Processing

## Overview

This project focuses on implementing threads and concurrent programming techniques to find an outline of an user inputted object/image. The program is written in C, and the project utilizes the STBI library, which is used for loading/reading images.
 The project prompts the user for a file pathway for an image that is already in the project directory. The program then passes this image to a function which condenses the three color channels, RGB, into one float value. The program then creates a
 grid, by splitting the now modified greyscale image into four quadrants. Four threads are created that process each quandrant, comparing it to a threshold value. If the pixel value, that the thread is currently scanning, has a higher value than the
 threshold variable, that pixel value is kept in that quadrant's image. The finished function, compiles all the seperate quadrants back into a single image, and saves that image into the project directory. In the future, I plan to update the program,
 so that the pixels which have a higher value than the threshold variable are set to black, and any pixel with a lower value is set to white. This should give the edges of an object. Mutex locks were implemented to prevent deadlock, and the finished
 method was updated to limit resource sharing between the process.

## Build and Run Instructions

### Building the Program

1. **Ensure Dependencies are Installed:**
   - **GCC:** Install via:
     ```bash
     sudo apt update
     sudo apt install gcc
     ```
   - **Pthreads:** Usually included with GCC on Linux.
   - **Math Library:** Ensure you have the standard math library (linked with `-lm`).
   - **STB Libraries:** The project uses `stb_image.h` and `stb_image_write.h`. Make sure these files are included in the `include/` directory.

2. **Compile the Code:**
   Open a terminal in the project directory and run:
   ```bash
   gcc -g ConvolutionalLayer.c -o hello -lpthread -lm


/*
    mdbl.cpp - Main header file
    Part of mandelbrot viewer made in C++ using SFML library
    Copyright (C) 2021 Miles MJ Jamon

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

#ifndef MDBL_LOCK
#define MDBL_LOCK

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <atomic>

#if defined(unix) || defined(__unix) || defined(__unix__)
#include <X11/Xlib.h>
#endif

// determines the number of cpu cores to be used for drawing
const_init < unsigned int > MAX_DRAWER_THREADS;
//#define MAX_THREADS 4

/// cWidth and cHeight ---> The width and the height of the canvas, respectively.
double cWidth = 400, cHeight = 400;
/// zoom and zoomy ---> The width and the height of the zoombox, respectively
double zoom = 100, zoomy = 100;
/// panX andd panY ---> The size of adjusted pan use for moving across the coordinates
double panX = 0, panY = 0;
/// ticks ---> The total iterations achieved (from the first thread up to the last thread)
long ticks = 0;

/// x and y ---> represents the top left coordinate of the complex plane
/// (since from the first writing, I don't know that a std::complex class exists
double x = -2.0, y = 2.0;
/// xmax and ymax ---> represents the bottom right coordinate of the complex plane
double xmax = 2.0, ymax = -2.0;
/// cx and cy ---> represents the coordinate respective to the canvas width and height
float cx = 0, cy = 0;
/// iterations and maxIterations ---> iterations is used with maxIterations
/// it represents whether the given number explodes, maxI. is used for telling it
int iterations = 0, maxIterations = 50;

/// mousex and mousey ---> used for storing the temporary position of mouse inside the canvas
int mousex = 0, mousey = 0;

/// sx and sy ---> this are the scaled version of the current coordinate of the canvas
/// to the current range, either from x to xmax, to y to ymax
double sx = 0, sy = 0;
/// i and j is used for looping
double i = 0, j = 0;

/// rsquare, isquare and zsquare ---> used for temporary storage
double rsquare = 0, isquare = 0, zsquare = 0;
/// the constant log of 2 (base 10?)
double mlog2 = log((double) 2);

// mdbl_data is used to represent the 
// details inside the mandelbrot draw function
struct mdbl_data {
  int i;
  float cx_start_thread, cx_end_thread;
  int maxIterations;
  double x, y, sx, sy, cWidth, cHeight, xmax, ymax;
};

/// My own collections of colors (based from Ultra Fractal default colors of mandelbrot set
rgb palleteOptimized[50] = {
  rgb(0, 7, 100),
  rgb(3, 15, 108),
  rgb(5, 23, 116),
  rgb(8, 31, 125),
  rgb(10, 39, 133),
  rgb(13, 47, 141),
  rgb(15, 55, 149),
  rgb(18, 63, 158),
  rgb(20, 71, 166),
  rgb(23, 79, 174),
  rgb(26, 87, 182),
  rgb(28, 95, 191),
  rgb(31, 103, 199),
  rgb(40, 113, 205),
  rgb(57, 125, 209),
  rgb(73, 137, 213),
  rgb(89, 148, 218),
  rgb(106, 160, 222),
  rgb(122, 172, 226),
  rgb(139, 184, 230),
  rgb(155, 196, 234),
  rgb(171, 208, 238),
  rgb(188, 219, 243),
  rgb(204, 231, 247),
  rgb(221, 243, 251),
  rgb(237, 255, 255),
  rgb(238, 248, 235),
  rgb(240, 241, 214),
  rgb(241, 235, 194),
  rgb(243, 228, 173),
  rgb(244, 221, 153),
  rgb(246, 214, 133),
  rgb(247, 207, 112),
  rgb(249, 201, 92),
  rgb(250, 194, 71),
  rgb(251, 187, 51),
  rgb(253, 180, 31),
  rgb(254, 173, 10),
  rgb(245, 163, 0),
  rgb(224, 150, 0),
  rgb(204, 136, 0),
  rgb(184, 123, 0),
  rgb(163, 110, 0),
  rgb(143, 96, 0),
  rgb(122, 83, 0),
  rgb(102, 69, 0),
  rgb(82, 56, 0),
  rgb(61, 42, 0),
  rgb(41, 29, 0),
  rgb(20, 15, 0)
};

sf::Font font;
std::stringstream ss;

sf::RectangleShape * ZOOMBOX;
sf::Text * TEXT;

// split_canvas holds the most important data in the program,
// the mandelbrot's rendering, which is saved into sf::VertexArray
resource_t * split_canvas;


sf::Mutex mutex;
sf::Mutex renderLock;

std::atomic<bool> stopThreads(false);
std::atomic<bool> isThreadsWorking(false);
std::atomic<int> theThreadsDone(0);
std::atomic<int> progress(0);
// pool will be used for passing workloads to threads
thread_pool<std::function<void(mdbl_data)>, mdbl_data> pool;

// internal representation of bmpfile
rgb * storageBMP;
// the output bmpfile
TinyBitmapOut bmpOutput;
// check whether to slow down
bool isPaused = false;
// determines whether to shutdown the rendering thread
std::atomic<bool> closeThreadRenderer(false);

bool showText = true;
std::ifstream File("config.txt");

#endif
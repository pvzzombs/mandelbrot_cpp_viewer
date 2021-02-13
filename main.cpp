/*
    main.cpp - Main source file
    Part of mandelbrot viewer made in C++ using SFML library
    Copyright (C) 2020 Miles MJ Jamon

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

#include <Windows.h>
#include <iostream>
#include "TinyBitmapOut.hpp"
#include "pool.hpp"
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

/**
 * An implementation of Hue, Saturation and Value color class
 */
class hsv {
  public:
    rgb rgbs;
  int h, s, v, r, g, b;
  hsv();
  hsv(int, int, int);
  double f(int num) {
    double k = (num + h * 360 / (255 * 60)) % 6;
    double lowest;

    if (k < (4 - k) && k < 1) {
      lowest = k;
    } else if ((4 - k) < k && (4 - k) < 1) {
      lowest = (4 - k);
    } else if (1 < k && 1 < (4 - k)) {
      lowest = 1;
    } else {

    }

    double max;
    if (lowest > 0) {
      max = lowest;
    } else {
      max = 0;
    }
    double mix = (v / 255) - (v / 255) * (s / 255) * max;

    return mix;
  }
  rgb toRGBF() {
    rgbs.r = (int)(255 * f(5)) % 256;
    rgbs.g = (int)(255 * f(3)) % 256;
    rgbs.b = (int)(255 * f(1)) % 256;

    return rgbs;
  }
};

hsv::hsv() {
  h = 0;
  s = 0;
  v = 0;
}

hsv::hsv(int hue, int saturation, int value) {
  h = hue;
  s = saturation;
  v = value;
}

sf::VertexArray rt_array[10];
unsigned int rt_array_current = 0;

/**
 * A simple class for keeping track of vertex
 * collection in an sf::VertexArray array
 */
class resource_t {
  public:

    unsigned int index = 0;

  /*                   constructor                    */
  /* define and initialize*/
  resource_t() {

  }

  resource_t(unsigned int width, unsigned int height) {
    //create the canvas
    index = rt_array_current;
    ++rt_array_current;
    rt_array[index] = sf::VertexArray(sf::Points, width * height);
  }~resource_t() {
    //std::cout << "Destroyed...\n";
  }
};

/**
 * An implementation of Hue, Saturation and Value color class
 */
template < typename T_const_init >
  class const_init {
    private:
      bool is_self_empty = true;
    T_const_init self;
    public:
      const_init(): is_self_empty(true) {

      }
    const_init(T_const_init some_data) {
      if (is_self_empty && std::is_copy_assignable < T_const_init > ::value && std::is_copy_assignable < T_const_init > ::value) {
        is_self_empty = false;
        self = some_data;
      } else {
        std::cerr << "const_init Error: Invalid data. Proceeding is dangerous...\n";
      }
    }

    const_init(const const_init & some_data) {
      if (is_self_empty) {
        is_self_empty = false;
        self = some_data.self;
      }
    }

    const_init & operator = (const const_init & some_data) {
      if (is_self_empty) {
        is_self_empty = false;
        self = some_data.self;
      }
      return *this;
    }

    T_const_init return_value() {
      return self;
    }

    T_const_init * return_value_pointer() {
      return &self;
    }

  };

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

/// The functions and utilities
rgb optimizedSmoothColor(double mu);
rgb smoothColor(int iterations_t, int maxIterations_t, double i_t, double j_t,
  double rsquare_t, double isquare_t, double zsquare_t, double sx_t, double sy_t);

void draw(mdbl_data);
void prepare_canvas(int);
void thread_split(int);
void renderingThread(sf::RenderWindow * window);
void refresh();
void zoomIn();
void zoomOut();

sf::Font font;
std::stringstream ss;

sf::RectangleShape * ZOOMBOX;
sf::Text * TEXT;
//std::vector <resource_t> split_canvas(5);
resource_t * split_canvas;

//std::vector<std::thread> threadsQueue;
sf::Mutex mutex;
sf::Mutex renderLock;
//sf::Lock lock;
std::atomic < bool > stopThreads(false);
std::atomic < bool > isThreadsWorking(false);
std::atomic < int > theThreadsDone(0);
std::atomic < int > progress(0);
thread_pool < std:: function < void(mdbl_data) > , mdbl_data > pool;

rgb * storageBMP;
TinyBitmapOut bmpOutput;
bool isPaused = false;
std::atomic < bool > closeThreadRenderer(false);
//std::atomic<int> drawCanvas(0);
bool showText = true;
std::ifstream File("config.txt");

void stopRenderingThreads() {
  if (isThreadsWorking.load()) {
    stopThreads = true;
  }
  progress = 0;

  renderLock.lock();

  pool.kill_jobs();

  renderLock.unlock();

  theThreadsDone = 0;
  isThreadsWorking = false;
  stopThreads = false;

}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
  //get the number of CPU CORES
  unsigned int determinator_of_cpu_cores = std::thread::hardware_concurrency();
  MAX_DRAWER_THREADS = (determinator_of_cpu_cores < 1) ? 1 : determinator_of_cpu_cores;

  split_canvas = new resource_t[MAX_DRAWER_THREADS.return_value()];

  //lets check if the file config.txt is open
  if (File.is_open()) {
    std::string parsedLine = "";
    //read only once
    std::getline(File, parsedLine);
    //store the size
    cWidth = atoi(parsedLine.c_str());

    std::getline(File, parsedLine);

    cHeight = atoi(parsedLine.c_str());
    //close the File
    File.close();
  }
  //store the const first
  cWidth = (cWidth > 2000 || cWidth < 100) ? 400 : cWidth;
  cHeight = (cHeight > 2000 || cHeight < 100) ? 400 : cHeight;
  ///std::cout << "Width: " << cWidth << " Height: " << cHeight << "\n";
  zoom = floor(zoomy * (cWidth / cHeight));
  xmax = (cWidth / cHeight) > 1 ? 2.0 * (cWidth / cHeight) + 2.0 : 2.0;
  ///std::cout << " This xmax: " << xmax << "\n";

  double pan = ((x + ((xmax - x) / cWidth) * (1 - 0)) - x);
  panX = pan * floor(cWidth / 4);
  panY = pan * floor(cHeight / 4);

  //change the size of the storage
  storageBMP = new(std::nothrow) rgb[(unsigned int)(cWidth * cHeight)];

  //operate for the storage of bmp file
  bmpOutput.change_settings(cWidth, cHeight, "output.bmp");

  //create the canvas
  sf::RenderTexture canvas;
  canvas.create(cWidth, cHeight);

  //convert rendered texture to texture
  const sf::Texture & texture = canvas.getTexture();

  //convert texture to sprite
  sf::Sprite mandelbrot(texture);

  //create rendering window
  sf::RenderWindow window(sf::VideoMode((unsigned int) cWidth, (unsigned int) cHeight), "Mandelbrot Set", sf::Style::Close);

  //set it to inactive
  window.setActive(false);

  //set limit of framerate for about 30-60 fps
  //window.setFramerateLimit(60);

  //create a pixel using rectangle shape
  sf::RectangleShape pixel(sf::Vector2f(1, 1));

  //create a zoom shape (used as a magnifier)
  sf::RectangleShape zoomBox(sf::Vector2f(zoom, zoomy));
  zoomBox.setFillColor(sf::Color::Transparent);
  zoomBox.setOutlineThickness(1);
  zoomBox.setOutlineColor(sf::Color::Red);

  //load a font
  if (!font.loadFromFile("verdana.ttf")) {
    exit(1);
  }

  //create a text
  sf::Text text;
  text.setFont(font);
  text.setString("...");
  text.setFillColor(sf::Color::White);
  text.setOutlineColor(sf::Color::Black);
  text.setOutlineThickness(1.0f);
  text.setPosition(1, 1);
  text.setCharacterSize(16);

  //store important objects to be able to use outside
  TEXT = & text;
  ZOOMBOX = & zoomBox;

  //create context
  sf::Context contxt;

  std::thread thread_drawer(renderingThread, & window);

  //prepare all the sprites
  prepare_canvas(MAX_DRAWER_THREADS.return_value());

  // split thread into the number we wanted
  thread_split(MAX_DRAWER_THREADS.return_value());

  //prevent resize when programmatically size is set
  bool ignoreResize = false;

  //while window is open
  while (window.isOpen()) {
    //check to finish drawing
    //      if(cx < cWidth){
    //        draw();
    //      }
    //inside the main while loop
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        stopRenderingThreads();
        pool.shutdown();
        closeThreadRenderer = true;
        thread_drawer.join();
        delete[] split_canvas;
        delete[] storageBMP;
        window.close();
      }

      if (event.type == sf::Event::Resized) {
        if (!ignoreResize) {
          ignoreResize = true;
          window.setSize(sf::Vector2u(cWidth, cHeight));
        } else {
          ignoreResize = false;
        }
      }

      if (event.type == sf::Event::MouseButtonPressed) {
        if (isThreadsWorking.load()) {
          stopRenderingThreads();
          continue;
        }

        //left click
        if (event.mouseButton.button == sf::Mouse::Left) {
          mousex = event.mouseButton.x;
          mousey = event.mouseButton.y;
          zoomIn();
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        //right click
        if (event.mouseButton.button == sf::Mouse::Right) {
          mousex = event.mouseButton.x;
          mousey = event.mouseButton.y;
          zoomOut();
          thread_split(MAX_DRAWER_THREADS.return_value());
        }
      }

      if (event.type == sf::Event::MouseMoved) {
        zoomBox.setPosition(float(event.mouseMove.x), float(event.mouseMove.y));
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Left) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          x -= panX;
          xmax -= panX;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::Right) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          x += panX;
          xmax += panX;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::Down) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          y -= panY;
          ymax -= panY;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::Up) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          y += panY;
          ymax += panY;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::D) {
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::Space) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          double mx = floor(cWidth * 1 / 8);
          double my = floor(cHeight * 1 / 8);

          double nx = x + (mx / cWidth) * (xmax - x);
          double nxmax = x + ((cWidth - mx) / cWidth) * (xmax - x);

          ///std::cout << "Here: " << nx << " " << nxmax << "\n";
          ///std::cout << "Here: " << mx << " " << cWidth - mx << "\n";

          x = nx;
          xmax = nxmax;

          double ny = y + (my / cHeight) * (ymax - y);
          double nymax = y + ((cHeight - my) / cHeight) * (ymax - y);

          y = ny;
          ymax = nymax;

          maxIterations += 50;
          cx = 0;
          ticks = 0;

          double pan = ((x + ((xmax - x) / cWidth) * (1 - 0)) - x);
          panX = pan * floor(cWidth / 4);
          panY = pan * floor(cHeight / 4);
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::W) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          maxIterations += 50;
          cx = 0;
          ticks = 0;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::S) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          maxIterations -= 50;
          cx = 0;
          ticks = 0;
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::Q) {
          if (zoomy + 20 < cHeight) {
            zoomy += 20;
            zoom = floor(zoomy * (cWidth / cHeight));
          }
          zoomBox.setSize(sf::Vector2f(zoom, zoomy));
        }

        if (event.key.code == sf::Keyboard::A) {
          if (zoomy - 20 > 0) {
            zoomy -= 20;
            zoom = floor(zoomy * (cWidth / cHeight));
          }
          zoomBox.setSize(sf::Vector2f(zoom, zoomy));
        }

        if (event.key.code == sf::Keyboard::R) {
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
          }

          zoomy = 100;
          zoom = floor(zoomy * (cWidth / cHeight));
          x = -2.0;
          y = 2.0;
          xmax = (cWidth / cHeight) > 1 ? 2.0 * (cWidth / cHeight) + 2.0 : 2.0;
          ymax = -2.0;
          double pan = ((x + ((xmax - x) / cWidth) * (1 - 0)) - x);
          panX = pan * floor(cWidth / 4);
          panY = pan * floor(cHeight / 4);
          iterations = 0;
          maxIterations = 50;
          cx = 0;
          ticks = 0;
          zoomBox.setSize(sf::Vector2f(zoom, zoomy));
          thread_split(MAX_DRAWER_THREADS.return_value());
        }

        if (event.key.code == sf::Keyboard::P) {
          if (isPaused) {
            isPaused = false;
            window.setFramerateLimit(60);
          } else {
            isPaused = true;
            window.setFramerateLimit(2);
          }
        }

        if (event.key.code == sf::Keyboard::F) {
          if (!isPaused) {
            isPaused = false;
            window.setFramerateLimit(0);
          }
        }

        if (event.key.code == sf::Keyboard::I) {
          if (showText)
            showText = false;
          else
            showText = true;
        }

        if (event.key.code == sf::Keyboard::Enter) {
          bmpOutput.write_data(storageBMP);
          ///bmpOutput.flip_data_horizontal();
          bmpOutput.save_data();
        }

      }
    }

  }
  return 0;
}

rgb optimizedSmoothColor(double mu) {

  int clr1 = (int) mu;
  double t2 = mu - clr1;
  double t1 = 1 - t2;

  clr1 = clr1 % 50;

  int clr2 = (clr1 + 1) % 50;

  mutex.lock();
  int r = (int)(palleteOptimized[clr1].r * t1 + palleteOptimized[clr2].r * t2) % 256;
  int g = (int)(palleteOptimized[clr1].g * t1 + palleteOptimized[clr2].g * t2) % 256;
  int b = (int)(palleteOptimized[clr1].b * t1 + palleteOptimized[clr2].b * t2) % 256;
  mutex.unlock();

  return rgb(r, g, b);
}

rgb smoothColor(int iterations_t, int maxIterations_t, double i_t, double j_t,
  double rsquare_t, double isquare_t, double zsquare_t, double sx_t, double sy_t) {
  rgb secret;
  int ii;
  double index, zmag;
  int iterations_thread = iterations_t, maxIterations_thread = maxIterations_t;
  double i_thread = i_t, j_thread = j_t, rsquare_thread = rsquare_t,
    isquare_thread = isquare_t, zsquare_thread = zsquare_t,
    sx_thread = sx_t, sy_thread = sy_t;

  if (iterations_thread < maxIterations_thread) {
    for (ii = 0; ii < 3; ii++) {
      i_thread = rsquare_thread - isquare_thread + sx_thread;
      j_thread = zsquare_thread - rsquare_thread - isquare_thread + sy_thread;
      rsquare_thread = i_thread * i_thread;
      isquare_thread = j_thread * j_thread;
      zsquare_thread = (i_thread + j_thread) * (i_thread + j_thread);
      ++iterations_thread;
    }

    zmag = sqrt(rsquare_thread + isquare_thread);
    index = iterations_thread + 1 - log(log(zmag)) / mlog2;

    return optimizedSmoothColor(index);
  } else {
    return secret;
  }
}

void draw(mdbl_data data) {
  sf::Context context;
  rgb goodColor_thread;
  int vertex_pos = 0;

  mutex.lock();
  ss.str("");
  ss.precision(17);
  ///ss << "maxItrs:" << maxIterations << "\n" << int((cx_t+1)/cWidth_t*100) << "%";
  ss << "Press d to refresh\nx: " << data.x << "\n" << "y: " << data.y << "\n";
  ss << "xmax: " << data.xmax << "\n" << "ymax: " << data.ymax;
  TEXT -> setString(ss.str());
  mutex.unlock();

  double i_thread, x_thread = data.x, xmax_thread = data.xmax, cx_thread, sx_thread = data.sx, cWidth_thread = data.cWidth;
  double j_thread, y_thread = data.y, ymax_thread = data.ymax, cy_thread, sy_thread = data.sy, cHeight_thread = data.cHeight;
  double rsquare_thread, isquare_thread, zsquare_thread;
  int iterations_thread, maxIterations_thread = data.maxIterations;

  for (cx_thread = data.cx_start_thread; cx_thread < data.cx_end_thread; cx_thread++) {
    for (cy_thread = 0; cy_thread < cHeight_thread; cy_thread++) {

      //return fast if it needs to stop
      /*if(stopThreads.load()){
        std::cout << "Shutting down thread " << data.i << "\n";
        theThreadsDone += 1;
        return;
      }*/

      ++ticks;

      iterations_thread = 0;

      sx_thread = x_thread + (cx_thread / cWidth_thread) * (xmax_thread - x_thread);
      sy_thread = y_thread + (cy_thread / cHeight_thread) * (ymax_thread - y_thread);

      i_thread = 0;
      j_thread = 0;

      rsquare_thread = 0;
      isquare_thread = 0;
      zsquare_thread = 0;

      while ((rsquare_thread + isquare_thread) <= 4 && iterations_thread < maxIterations_thread) {
        i_thread = rsquare_thread - isquare_thread + sx_thread;
        j_thread = zsquare_thread - rsquare_thread - isquare_thread + sy_thread;
        rsquare_thread = i_thread * i_thread;
        isquare_thread = j_thread * j_thread;
        zsquare_thread = (i_thread + j_thread) * (i_thread + j_thread);
        ++iterations_thread;
      }

      ///mutex.lock();
      goodColor_thread = smoothColor(iterations_thread, data.maxIterations, i_thread, j_thread,
        rsquare_thread, isquare_thread, zsquare_thread,
        sx_thread, sy_thread);
      ///mutex.unlock();
      //std::cout << cx << ", " << cy << std::endl;
      storageBMP[(int(cx_thread) + int(data.cWidth) * int(cy_thread))] = goodColor_thread;

      //std::cout << goodColor_thread.r << " " << goodColor_thread.g << " " << goodColor_thread.b << "\n";
      ///mutex.lock();
      rt_array[split_canvas[data.i].index][vertex_pos].color = sf::Color(goodColor_thread.r, goodColor_thread.g, goodColor_thread.b);
      rt_array[split_canvas[data.i].index][vertex_pos].position = sf::Vector2f(cx_thread, cy_thread);
      ++vertex_pos;
      ///mutex.unlock();
      //std::cout << i << j << "\n";
    }
  }

  ///mutex.lock();
  ///ss << "\nticks:" << ticks;
  ///TEXT->setString(ss.str());
  ///mutex.unlock();

  theThreadsDone += 1;
  std::cout << "Done thread: " << data.i << "\n";
  //WINDOW->setActive(false);
}

void prepare_canvas(int numberOfThreads) {
  int i, temp;
  for (i = 0; i < numberOfThreads; i++) {
    //    split_canvas.push_back(resource_t(cWidth / numberOfThreads, cHeight));
    temp = int(cWidth) / numberOfThreads;
    split_canvas[i] = resource_t(temp, cHeight);
  }
  std::cout << "Canvas prepared\n";
}

void thread_split(int numberOfThreads) {
  std::cout << "Starting thread_split\n";
  std::cout << "Max Iterations: " << maxIterations << "\n";

  //split them into numberOfThreads
  float cx_start_thread = 0;
  float cx_end_thread = 0;
  int temp = int(cWidth) / numberOfThreads;
  float cx_current_count = temp;

  int i;
  for (i = 0; i < numberOfThreads; i++) {
    if (cx_end_thread + cx_current_count > cWidth) {
      cx_end_thread = cWidth;
    } else {
      cx_end_thread += cx_current_count;
    }

    mdbl_data here;
    here.i = i;
    here.cx_start_thread = cx_start_thread;
    here.cx_end_thread = cx_end_thread;
    here.maxIterations = maxIterations;
    here.x = x;
    here.y = y;
    here.sx = sx;
    here.sy = sy;
    here.cWidth = cWidth;
    here.cHeight = cHeight;
    here.xmax = xmax;
    here.ymax = ymax;

    pool.add_job(draw, here);

    cx_start_thread = cx_end_thread;
    //    list_of_threads.push_back(thread);
    std::cout << "Launching thread: " << i << "\n";
    //    thread.launch();
    ///thread.wait();
  }

  isThreadsWorking = true;

  std::cout << "All threads launched\n";

  //  for(i = 0; i < numberOfThreads; i++){
  //    ///list_of_threads[i]->launch();
  //    list_of_threads[i].join();
  //    std::cout << "Joining thread: " << i << "\n";
  //  }
}

void renderingThread(sf::RenderWindow * window) {
  sf::Context ctx;
  int max_cpu_drawer = MAX_DRAWER_THREADS.return_value();
  std::cout << "Number of CPU Cores: " << std::thread::hardware_concurrency() << "\n";
  std::cout << "Number of Drawing Threads: " << max_cpu_drawer << "\n";
  if (!window -> isOpen()) {
    std::cout << "Error\n";
    return;
  }
  while (!closeThreadRenderer.load()) {
    for (int ip = 0; ip < max_cpu_drawer; ip++) {
      window -> draw(rt_array[split_canvas[ip].index]);
    }

    window -> draw( * ZOOMBOX);

    if (showText)
      window -> draw( * TEXT);

    window -> display();

    if (theThreadsDone.load() == max_cpu_drawer) {

      stopRenderingThreads();

    }
  }
  std::cout << "Exiting Rendering Thread..\n";
  window -> setActive(false);
}

void zoomIn() {
  double mx = (double) mousex;
  double my = (double) mousey;

  double nx = x + (mx / cWidth) * (xmax - x);
  double nxmax = x + ((mx + zoom) / cWidth) * (xmax - x);

  x = nx;
  xmax = nxmax;

  double ny = y + (my / cHeight) * (ymax - y);
  double nymax = y + ((my + zoomy) / cHeight) * (ymax - y);

  y = ny;
  ymax = nymax;

  maxIterations += 50;
  cx = 0;
  ticks = 0;

  double pan = ((x + ((xmax - x) / cWidth) * (1 - 0)) - x);
  panX = pan * floor(cWidth / 4);
  panY = pan * floor(cHeight / 4);
}

void zoomOut() {
  double mx = (double) mousex;
  double my = (double) mousey;

  double fx0 = x + (0 / zoom) * (xmax - x);
  double fx1 = x + (1 / zoom) * (xmax - x);

  double dx = (fx1) - (fx0);

  double fy0 = y + (0 / zoomy) * (ymax - y);
  double fy1 = y + (1 / zoomy) * (ymax - y);

  double dy = (fy1) - (fy0);

  x = x - (mx * dx);
  y = y - (my * dy);

  double xback = cWidth - (mx + zoom);
  double yback = cHeight - (my + zoomy);

  xmax = xmax + (xback * dx);
  ymax = ymax + (yback * dy);

  maxIterations -= 50;
  cx = 0;
  ticks = 0;

  double pan = ((x + ((xmax - x) / cWidth) * (1 - 0)) - x);
  panX = pan * floor(cWidth / 4);
  panY = pan * floor(cHeight / 4);
}
/*
    resource_t.hpp - A simple class for keeping track of
    vertex collection in an sf::VertexArray array
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

#ifndef RESOURCE_T_LOCK
#define RESOURCE_T_LOCK

#include <SFML/Graphics.hpp>

sf::VertexArray rt_array[10];
unsigned int rt_array_current = 0;

class resource_t {
  public:
    unsigned int index;

  /*                   constructor                    */
  /* define and initialize*/
  resource_t(): index(0) {
  }

  resource_t(unsigned int width, unsigned int height): index(0) {
    //create the canvas
    index = rt_array_current;
    ++rt_array_current;
    rt_array[index] = sf::VertexArray(sf::Points, width * height);
  }~resource_t() {
    //std::cout << "Destroyed...\n";
  }
};

#endif
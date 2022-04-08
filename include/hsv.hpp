/*
    hsv.cpp - An implementation of Hue, Saturation and Value color class
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

#ifndef HSV_LOCK
#define HSV_LOCK

#include "TinyBitmapOut.hpp"
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

#endif
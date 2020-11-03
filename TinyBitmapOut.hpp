/*
    mandelbrot_cpp_viewer
    A mandelbrot viewer made in C++ using SFML library
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

#pragma once

#include <fstream>
#include <iostream>

class rgb final{
public:
    int r, g, b, a;
////////////////////////////////////////////////////////////
/// \brief Default constructor of rgb class
///
/// \warning Brief This initializes alpha to 255!
///
////////////////////////////////////////////////////////////
    rgb();
////////////////////////////////////////////////////////////
/// \brief Constructor of rgb class
///
/// \param w Red value
/// \param q Green Value
/// \param c Blue Value
///
////////////////////////////////////////////////////////////
    rgb(int w, int q, int c);
////////////////////////////////////////////////////////////
/// \brief Constructor of rgb class
///
/// \param w Red Value
/// \param q Green Value
/// \param c Blue Value
/// \param d Alpha Value
///
////////////////////////////////////////////////////////////
    rgb(int w, int q, int c, int d);
};

class TinyBitmapOut final
{
    int* data;
    bool is_data;
    int area;
    unsigned int width;
    unsigned int height;
    std::string path_to_file;
public:
////////////////////////////////////////////////////////////
/// \brief Default constructor of class TinyBitmapOut
///
////////////////////////////////////////////////////////////
    TinyBitmapOut();
////////////////////////////////////////////////////////////
/// \brief Constructor of class TinyBitmapOut
///
/// \param w Width of the bitmap
/// \param h Height of the bitmap
/// \param path Path to where the bitmap will be saved
///
////////////////////////////////////////////////////////////
    TinyBitmapOut(unsigned int w, unsigned int h, std::string path);
////////////////////////////////////////////////////////////
/// \brief Changes the width, height and path of the bitmap file
///
/// \param w Width of the bitmap
/// \param h Height of the bitmap
/// \param path Path to where the bitmap will be saved
///
////////////////////////////////////////////////////////////
    void change_settings(unsigned int w, unsigned int h, std::string path);
////////////////////////////////////////////////////////////
/// \brief Writes the data of the bitmap file
///
/// \param DATA The data of the bitmap in form rgb class array
///
////////////////////////////////////////////////////////////
    void write_data(rgb * DATA);
////////////////////////////////////////////////////////////
/// \brief Writes the data of the bitmap file
///
/// \param DATA The data of the bitmap in form int class array
///
////////////////////////////////////////////////////////////
    void write_data(int * DATA);
////////////////////////////////////////////////////////////
/// \brief Reverses the data in the bitmap file
///
////////////////////////////////////////////////////////////
    void reverse_data();
////////////////////////////////////////////////////////////
/// \brief Flips the data of the bitmap file horizontally
///
////////////////////////////////////////////////////////////
    void flip_data_horizontal();
////////////////////////////////////////////////////////////
/// \brief Flips the data of the bitmap file vertically
///
////////////////////////////////////////////////////////////
    void flip_data_vertical();
////////////////////////////////////////////////////////////
/// \brief Save the bitmap file
///
////////////////////////////////////////////////////////////
    void save_data();
////////////////////////////////////////////////////////////
/// \brief Save the bitmap file
///
/// \param path Saves the bitmap file to the given path
/// rather than the original path
///
////////////////////////////////////////////////////////////
    void save_data(std::string path);
////////////////////////////////////////////////////////////
/// \brief Deletes and releases all the allocated memory
/// of the bitmap file
///
/// \warning If you used write_data(int * DATA), you do not
/// need to delete the pointer array you pass to write_data
/// everything is handled by this library automatically :-)
///
////////////////////////////////////////////////////////////
    void delete_data();
    ~TinyBitmapOut();
};

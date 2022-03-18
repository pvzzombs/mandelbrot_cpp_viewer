/*
    TinyBitmapOut.cpp - Source file for Bitmap Manipulation
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

#include <fstream>
#include <iostream>
#include "TinyBitmapOut.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The code below will take an integer array of pixel colors as input and write it to a .bmp bitmap file.  ///
/// It only requires the <fstream> library.                                                                 ///
/// The input parameter path can be for example C:/path/to/your/image.bmp and data is formatted as          ///
/// data[x+y*width]=(red<<16)|(green<<8)|blue;, whereby red, green and blue are integers in the range 0-255 ///
/// and the pixel position is (x,y).                                                                        ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

rgb::rgb (){
    r = 0;
    g = 0;
    b = 0;
    a = 255;
}

rgb::rgb (int w, int q, int c){
    r = w;
    g = q;
    b = c;
    a = 255;
}

rgb::rgb (int w, int q, int c, int d){
    r = w;
    g = q;
    b = c;
    a = d;
}

void TinyBitmapOut::init (unsigned int w, unsigned int h, std::string path){
  width = w;
  height = h;
  path_to_file = path;
  area = w*h;
  data = new (std::nothrow) int[area];
  if(data == 0){
    width = 0;
    height = 0;
    area = 0;
    is_data = false;
  }else{
    is_data = true;
  }
}

TinyBitmapOut::TinyBitmapOut (){
  width = 0;
  height = 0;
  path_to_file = "";
  area = 0;
  data = nullptr;
  is_data = false;
}

TinyBitmapOut::TinyBitmapOut (unsigned int w, unsigned int h, std::string path){
  init(w, h, path);
}

TinyBitmapOut::~TinyBitmapOut (){
  if(is_data){
    is_data = false;
    delete [] data;
  }
}

void TinyBitmapOut::change_settings (unsigned int w, unsigned int h, std::string path){
  if(is_data){
    delete [] data;
  }
  init(w, h, path);
}

void TinyBitmapOut::write_data (int * DATA){
  //this is required to delete data of int
  //instead of looping, delete the current
  //data and replace with a new one instead
  //this assumes that a data size
  //is still the same
  if(is_data){
    delete [] data;
  }
  data = DATA;
  is_data = true;
}

void TinyBitmapOut::write_data (rgb * DATA){
  //this needs to be looped, rather
  //than creating a new one
  //this assumes that a data size
  //is still the same
  if(is_data){
    for(int i=0; i<area; i++){
      int w = DATA[i].r;
      int e = DATA[i].g;
      int r = DATA[i].b;
      data[i] = (w<<16) | (e<<8) | r;
    }
  }
}

void TinyBitmapOut::reverse_data (){
  if(is_data){
    int i, temp, half = (area % 2) ? (area-1)/2 : area/2;
    for(i = 0; i < half; i++){
      temp = data[i];
      data[i] = data[area-1 - i];
      data[area-1 - i] = temp;
    }
  }
}

void TinyBitmapOut::flip_data_horizontal (){
  if(is_data){
    unsigned int x, y, nHeight;
    int temp;
    nHeight = (height % 2) ? ((height-1)/2) : height/2;
    for(y = 0; y < nHeight; y++){
      for(x = 0; x < width; x++){
        temp = data[x + y * width];
        data[x + y * width] = data[x + ((height - (y+1)) * width)];
        data[x + ((height - (y+1)) * width)] = temp;
      }
    }
  }
}

void TinyBitmapOut::flip_data_vertical (){
  if(is_data){
    unsigned int x, y, nWidth;
    int temp;
    nWidth = (width % 2) ? ((width-1)/2) : width/2;
    for(y = 0; y < height; y++){
      for(x = 0; x < nWidth; x++){
        temp = data[x + y * width];
        data[x + y * width] = data[(x + y * width) + (width - 1) - (2*x)];
        data[(x + y * width) + (width - 1) - (2*x)] = temp;
      }
    }
  }
}

void TinyBitmapOut::save (std::string path){
  const int pad=(4-(3*width)%4)%4, filesize=54+(3*width+pad)*height; // horizontal line must be a multiple of 4 bytes long, header is 54 bytes
  char header[54] = { 'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0, 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };
  char padding[3] = { 0,0,0 };
  for(int i=0; i<4; i++) {
    header[ 2+i] = (unsigned char)((filesize>>(8*i))&255);
    header[18+i] = (unsigned char)((width   >>(8*i))&255);
    header[22+i] = (unsigned char)((height  >>(8*i))&255);
  }
  unsigned char* img = new unsigned char[3*area];
  for(int i=0; i<area; i++) {
    const int color = data[i];
    img[3*i  ] = (unsigned char)( color     &255);
    img[3*i+1] = (unsigned char)((color>> 8)&255);
    img[3*i+2] = (unsigned char)((color>>16)&255);
  }
  std::ofstream file(path.c_str(), std::ios::out|std::ios::binary);
  file.write(header, 54);
  for(int i=height-1; i>=0; i--) {
    file.write((char*)(img+3*width*i), 3*width);
    file.write(padding, pad);
  }
  file.close();
  delete[] img;
}

void TinyBitmapOut::save_data (){
  if(is_data){
    save(path_to_file);
  }
}

void TinyBitmapOut::save_data (std::string path){
  if(is_data){
    path_to_file = path;
    save(path_to_file);
  }
}

void TinyBitmapOut::delete_data (){
  //forcing to free the data
  //only do this if you will use
  //the writeData() again
  if(is_data){
    is_data = false;
    delete [] data;
  }
}

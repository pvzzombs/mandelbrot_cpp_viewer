/*
    TinyBitMapOut.hpp
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

// sdfml.cpp : Defines the entry point for the console application.
// update: change to window application

#include <Windows.h>
#include <iostream>
//#include "rgb.h"
#include "TinyBitmapOut.hpp"
#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>

using namespace sf;

//declaration of class hsv
class hsv{
public:
	rgb rgbs;
	int h, s, v, r, g, b;
	hsv();
	hsv(int, int, int);
	double f(int num){
		double k = (num + h * 360 / (255 * 60)) % 6;
		double lowest;

		if(k < (4 - k) && k < 1){
			lowest = k;
		}else if((4 - k) < k && (4 - k) < 1){
			lowest = (4 - k);
		}else if(1 < k && 1 < (4 - k)){
			lowest = 1;
		}else{

		}

		double max;
		if(lowest > 0){
			max = lowest;
		}else{
			max = 0;
		}
		double mix = (v / 255) - (v / 255) * (s / 255) * max;

		return mix;
	}
	rgb toRGBF(){
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

hsv::hsv(int hue, int saturation, int value){
	h = hue;
	s = saturation;
	v = value;
}

double cWidth = 400;
double cHeight = 400;
double zoom = 100;
double zoomy = 100;

long ticks = 0;

double x = -2.0;
double y = 2.0;
double xmax = 2.0;
double ymax = -2.0;
float cx = 0;
float cy = 0;
int iterations = 0;
int maxIterations = 50;

int mousex = 0;
int mousey = 0;

double sx = 0;
double sy = 0;
double i = 0;
double j = 0;

double rsquare = 0;
double isquare = 0;
double zsquare = 0;

double mlog2 = log((double)2);

rgb palleteOptimized[50] = {rgb(0, 7, 100), rgb(3, 15, 108), rgb(5, 23, 116), rgb(8, 31, 125), rgb(10, 39, 133), rgb(13, 47, 141), rgb(15, 55, 149), rgb(18, 63, 158), rgb(20, 71, 166), rgb(23, 79, 174), rgb(26, 87, 182), rgb(28, 95, 191), rgb(31, 103, 199), rgb(40, 113, 205), rgb(57, 125, 209), rgb(73, 137, 213), rgb(89, 148, 218), rgb(106, 160, 222), rgb(122, 172, 226), rgb(139, 184, 230), rgb(155, 196, 234), rgb(171, 208, 238), rgb(188, 219, 243), rgb(204, 231, 247), rgb(221, 243, 251), rgb(237, 255, 255), rgb(238, 248, 235), rgb(240, 241, 214), rgb(241, 235, 194), rgb(243, 228, 173), rgb(244, 221, 153), rgb(246, 214, 133), rgb(247, 207, 112), rgb(249, 201, 92), rgb(250, 194, 71), rgb(251, 187, 51), rgb(253, 180, 31), rgb(254, 173, 10), rgb(245, 163, 0), rgb(224, 150, 0), rgb(204, 136, 0), rgb(184, 123, 0), rgb(163, 110, 0), rgb(143, 96, 0), rgb(122, 83, 0), rgb(102, 69, 0), rgb(82, 56, 0), rgb(61, 42, 0), rgb(41, 29, 0), rgb(20, 15, 0)};
rgb optimizedSmoothColor(double mu);
rgb smoothColor();

void draw();
void refresh();
void zoomIn();
void zoomOut();

Font font;
std::stringstream ss;

RenderWindow *WINDOW;
RenderTexture *CANVAS;
Sprite *MANDELBROT;
RectangleShape *PIXEL;
RectangleShape *ZOOMBOX;
Text *TEXT;

rgb * storageBMP;
TinyBitmapOut bmpOutput;
bool isPaused = false;
std::ifstream File("config.txt");

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	int canvasSize = 400;
	//lets check if the file config.txt is open
	if(File.is_open()){
		std::string parsedLine = "";
		//read only once
		std::getline(File, parsedLine);
		//store the size
		canvasSize = atoi(parsedLine.c_str());
		File.close();
	}
	//store the const first
	cWidth = cHeight = canvasSize;

	//change the size of the storage
	storageBMP = new (std::nothrow) rgb[canvasSize * canvasSize];

	//operate for the storage of bmp file
	bmpOutput.change_settings(canvasSize, canvasSize, "output.bmp");

	//create the canvas
	RenderTexture canvas;
	canvas.create(canvasSize, canvasSize);

	//convert rendered texture to texture
	const Texture& texture = canvas.getTexture();

	//convert texture to sprite
	Sprite mandelbrot(texture);

	//create rendering window
  RenderWindow window(VideoMode(canvasSize, canvasSize), "Mandelbrot Set", Style::Close);

	//set limit of framerate for about 30-60 fps
	window.setFramerateLimit(60);

	//create a pixel using rectangle shape
	RectangleShape pixel(Vector2f(1, 1));

	//create a zoom shape (used as a magnifier)
	RectangleShape zoomBox(Vector2f(100, 100));
	zoomBox.setFillColor(Color::Transparent);
	zoomBox.setOutlineThickness(1);
	zoomBox.setOutlineColor(Color::Red);

	//load a font
	if(!font.loadFromFile("verdana.ttf")){
		exit(1);
	}

	//create a text
	Text text;
	text.setFont(font);
	text.setString("...");
	text.setFillColor(Color::White);
	text.setOutlineColor(Color::Black);
	text.setOutlineThickness(1.0f);
	text.setPosition(1, 1);
	text.setCharacterSize(16);

	//store important objects to be able to use outside
	WINDOW = &window;
	CANVAS = &canvas;
	PIXEL = &pixel;
	MANDELBROT = &mandelbrot;
	TEXT = &text;
	ZOOMBOX = &zoomBox;

	//draw
	draw();

	//prevent resize when programmatically size is set
	bool ignoreResize = false;

	//while window is open
    while (window.isOpen())
    {
      //check to finish drawing
      if(cx < cWidth){
        draw();
      }
      //inside the main while loop
        Event event;
        while (window.pollEvent(event))
        {
          if (event.type == Event::Closed){
            delete [] storageBMP;
            window.close();
          }

			if(event.type == Event::Resized){
				if(!ignoreResize){
					ignoreResize = true;
					window.setSize(Vector2u(canvasSize, canvasSize));
				}else{
					ignoreResize = false;
				}
			}

			if(event.type == Event::MouseButtonPressed){
				//left click
				if(event.mouseButton.button == Mouse::Left && (cx > cWidth/2)){
					mousex = event.mouseButton.x;
					mousey = cHeight - event.mouseButton.y - zoomy;
					zoomIn();
					draw();
				}

				//right click
				if(event.mouseButton.button == Mouse::Right && (cx > cWidth/2)){
					mousex = event.mouseButton.x;
					mousey = cHeight - event.mouseButton.y - zoomy;
					zoomOut();
					draw();
				}
			}

			if(event.type == Event::MouseMoved){
				zoomBox.setPosition(float(event.mouseMove.x), float(event.mouseMove.y));
			}

			if(event.type == Event::KeyPressed){
				if(event.key.code == Keyboard::W){
					maxIterations += 50;
					cx = 0;
					ticks = 0;
					draw();
				}

				if(event.key.code == Keyboard::S){
					maxIterations -= 50;
					cx = 0;
					ticks = 0;
					draw();
				}

				if(event.key.code == Keyboard::Q){
          if(zoom + 20 < cWidth)
            zoom = zoomy += 20;
          zoomBox.setSize(Vector2f(zoom, zoomy));
				}

				if(event.key.code == Keyboard::A){
          if(zoom - 20 > 0)
            zoom = zoomy -= 20;
					zoomBox.setSize(Vector2f(zoom, zoomy));
				}

				if(event.key.code == Keyboard::R){
					zoomy = 100;
					zoom = 100;
					x = -2.0;
					y = 2.0;
					xmax = 2.0;
					ymax = -2.0;
					iterations = 0;
					maxIterations = 50;
					cx = 0;
					ticks = 0;
					zoomBox.setSize(Vector2f(zoom, zoomy));
					draw();
				}

				if(event.key.code == Keyboard::P){
					if(isPaused){
						isPaused = false;
						window.setFramerateLimit(60);
					}else{
						isPaused = true;
						window.setFramerateLimit(2);
					}
				}

				if(event.key.code == Keyboard::Enter){
            bmpOutput.write_data(storageBMP);
            bmpOutput.flip_data_horizontal();
            bmpOutput.save_data();
				}

			}
    }
  window.clear(Color::White);
  window.draw(mandelbrot);
  window.draw(zoomBox);
  window.draw(text);
  window.display();
  }
  return 0;
}

rgb optimizedSmoothColor(double mu){

	int clr1 = (int)mu;
	double t2 = mu - clr1;
	double t1 = 1 - t2;

	clr1 = clr1 % 50;

	int clr2 = (clr1 + 1) % 50;

	int r = (int)(palleteOptimized[clr1].r * t1 + palleteOptimized[clr2].r * t2) % 256;
	int g = (int)(palleteOptimized[clr1].g * t1 + palleteOptimized[clr2].g * t2) % 256;
	int b = (int)(palleteOptimized[clr1].b * t1 + palleteOptimized[clr2].b * t2) % 256;

	return rgb(r, g, b);
}

rgb smoothColor(){
	rgb secret;
	int ii;
	double index, zmag;

	if(iterations < maxIterations){
		for(ii = 0; ii < 3; ii++){
			i = rsquare - isquare + sx;
			j = zsquare - rsquare - isquare + sy;
			rsquare = i * i;
			isquare = j * j;
			zsquare = (i + j) * (i + j);
			++iterations;
		}

		zmag = sqrt(rsquare + isquare);
		index = iterations + 1 - log(log(zmag)) / mlog2;

		return optimizedSmoothColor(index);
	}else{
		return secret;
	}
}

void draw(){
	rgb goodColor;
	ss.str("");
	ss.precision(17);
	ss << "maxItrs:" <<maxIterations << "\n" << int((cx+1)/cWidth*100) << "%";
	ss << "\n" << "x: " << x << "\n" << "y: " << y;
  for(cy = 0; cy < cHeight; cy++){
    ++ticks;
    iterations = 0;

    sx = x + (cx / cWidth) * (xmax - x);
    sy = y + (cy / cHeight) * (ymax - y);

    i = 0;
    j = 0;

    rsquare = 0;
    isquare = 0;
    zsquare = 0;

    while((rsquare + isquare) <= 4 && iterations < maxIterations){
      i = rsquare - isquare + sx;
      j = zsquare - rsquare - isquare + sy;
      rsquare = i * i;
      isquare = j * j;
      zsquare = (i + j) * (i + j);
      ++iterations;
    }

    goodColor = smoothColor();
    //std::cout << cx << ", " << cy << std::endl;
    storageBMP[(int(cx) + int(cWidth) * int(cy))] = goodColor;
    //(*a).b == a->b
    PIXEL->setFillColor(Color(goodColor.r, goodColor.g, goodColor.b));
    PIXEL->setPosition(cx, cy);
    CANVAS->draw(*PIXEL);
  }
  ++cx;
	ss << "\nticks:" << ticks;
	TEXT->setString(ss.str());
}

void zoomIn(){
	double mx = (double)mousex;
	double my = (double)mousey;

	double nx = x + (mx / cWidth) * (xmax - x);
	double nxmax =  x + ((mx + zoom) / cWidth) * (xmax - x);

	x = nx;
	xmax = nxmax;

	double ny = y + (my / cHeight) * (ymax - y);
	double nymax =  y + ((my + zoomy) / cHeight) * (ymax - y);

	y = ny;
	ymax = nymax;

	maxIterations += 50;
	cx=0;
	ticks = 0;
}

void zoomOut(){
	double mx = (double)mousex;
	double my = (double)mousey;

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
	cx=0;
	ticks = 0;
}

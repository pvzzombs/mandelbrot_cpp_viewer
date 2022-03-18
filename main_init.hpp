/*
    main_init.hpp - Header file for initialization inside main function
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

class main_init{
public:
  unsigned int determiner_of_cpu_cores;
  sf::RenderWindow window;
  sf::RectangleShape zoomBox;
  sf::Text text;
  std::thread thread_drawer;

  main_init():
    determiner_of_cpu_cores(std::thread::hardware_concurrency()),
    window(),
    zoomBox(sf::Vector2f(zoom, zoomy)),
    text() {

    //get the number of CPU CORES
    MAX_DRAWER_THREADS = (determiner_of_cpu_cores < 1) ? 1 : determiner_of_cpu_cores;

    split_canvas.reset(new resource_t[MAX_DRAWER_THREADS.return_value()]);

    //lets check if the file config.txt is open
    if (File.is_open()) {
      std::string parsedLine = "";
      //read only once
      std::getline(File, parsedLine);
      //store the size
      main_data.cWidth = atoi(parsedLine.c_str());

      std::getline(File, parsedLine);

      main_data.cHeight = atoi(parsedLine.c_str());
      //close the File
      File.close();
    }

    //store the const first
    main_data.cWidth = (main_data.cWidth > 2000 || main_data.cWidth < 100) ? 400 : main_data.cWidth;
    main_data.cHeight = (main_data.cHeight > 2000 || main_data.cHeight < 100) ? 400 : main_data.cHeight;
    zoom = std::floor(zoomy * (main_data.cWidth / main_data.cHeight));
    main_data.xmax = (main_data.cWidth / main_data.cHeight) > 1 ? 2.0 * (main_data.cWidth / main_data.cHeight) + 2.0 : 2.0;

    double pan = ((main_data.x + ((main_data.xmax - main_data.x) / main_data.cWidth) * (1 - 0)) - main_data.x);
    panX = pan * std::floor(main_data.cWidth / 4);
    panY = pan * std::floor(main_data.cHeight / 4);

    //change the size of the storage
    storageBMP.reset(new rgb[(unsigned int)(main_data.cWidth * main_data.cHeight)]);

    //operate for the storage of bmp file
    bmpOutput.change_settings(main_data.cWidth, main_data.cHeight, "output.bmp");

    //adjust rendering window
    window.create(sf::VideoMode((unsigned int)main_data.cWidth, (unsigned int) main_data.cHeight), "Mandelbrot Set", sf::Style::Close);

    //set it to inactive
    window.setActive(false);

    //set limit of framerate for about 30-60 fps
    window.setFramerateLimit(30);

    //create a zoom shape (used as a magnifier)
    zoomBox.setFillColor(sf::Color::Transparent);
    zoomBox.setOutlineThickness(1);
    zoomBox.setOutlineColor(sf::Color::Red);

    //load a font
    if (!font.loadFromFile("dsmp.otf")) {
      exit(1);
    }

    //create a text
    text.setFont(font);
    text.setString("...");
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.0f);
    text.setPosition(1, 1);
    text.setCharacterSize(16);

    //store important objects to be able to use outside
    TEXT = &text;
    ZOOMBOX = &zoomBox;
    WINDOW = &window;

    //move rendering thread
    std::thread thread_drawer_(renderingThread, &window);
    thread_drawer = std::move(thread_drawer_);

    //prepare all the sprites
    prepare_canvas(MAX_DRAWER_THREADS.return_value());

    // split thread into the number we wanted
    thread_split(MAX_DRAWER_THREADS.return_value());
  }

  void close(){
    stopRenderingThreads();
    pool.shutdown();
    closeThreadRenderer = true;
    thread_drawer.join();
    window.close();
  }
};
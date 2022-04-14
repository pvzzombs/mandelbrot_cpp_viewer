/*
    resource_t.hpp - A simple class for keeping track of
    vertex collection in an sf::VertexArray array
    LICENSE: GNU Lesser General Public License v3.0
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
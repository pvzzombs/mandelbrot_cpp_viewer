#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

void renderingThread(sf::RenderWindow * window){
  while(window->isOpen()){
    window->clear(sf::Color::White);
    window->display();
  }
}

int main(){
  sf::RenderWindow window(sf::VideoMode(200, 200), "Testing", sf::Style::Default);
  window.setActive(false);
  sf::Thread thread(&renderingThread, &window);
  thread.launch();

  while(window.isOpen()){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        window.close();
      }
    }
  }
  return 0;
}
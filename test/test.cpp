#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#if defined(unix) || defined(__unix) || defined(__unix__)
#include <X11/Xlib.h>
#endif

void renderingThread(sf::RenderWindow * window){
  while(window->isOpen()){
    window->clear(sf::Color::White);
    window->display();
  }
}

#if defined(_WIN32) || defined(_WIN64)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
#else
int main(int argc, char * argv[]){
#endif

#if defined(unix) || defined(__unix) || defined(__unix__)
  XInitThreads();
#endif

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
/*
    main.cpp - Main source file
    Part of mandelbrot viewer made in C++ using SFML library
    LICENSE: GNU Lesser General Public License v3.0
*/

#include "hsv.hpp"
#include "pool.hpp"
#include "resource_t.hpp"
#include "const_init.hpp"
#include "mdbl.hpp"
#include "main_init.hpp"

#if defined(_WIN32) || defined(_WIN64)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
#else
int main(int argc, char * argv[]){
#endif
  main_init main_init_;

  //Init XThreads (Linux)
#if defined(unix) || defined(__unix) || defined(__unix__)
  XInitThreads();
#endif

  //while window is open
  while (WINDOW->isOpen()) {
    sf::Event event;
    while (WINDOW->pollEvent(event)) {
      EVENT = &event;
      switch(event.type){
        case sf::Event::Closed:
          main_init_.close();
          break;
        case sf::Event::Resized:
          mdbl_evt_resize();
          break;
        case sf::Event::MouseButtonPressed:
          if (isThreadsWorking.load()) {
            stopRenderingThreads();
            continue;
          }
          switch(event.mouseButton.button){
            case sf::Mouse::Left:
              mdbl_evt_mouse_lbutton_pressed();
              break;
            case sf::Mouse::Right:
              mdbl_evt_mouse_rbutton_pressed();
              break;
          }
          break;
        case sf::Event::MouseMoved:
          mdbl_evt_mouse_moved();
          break;
        case sf::Event::KeyPressed:
          main_init_.switch_keys();
          break;
      }
    }
  }
  return 0;
}

inline rgb optimizedSmoothColor(double mu) {
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

inline rgb smoothColor(int iterations_t, int maxIterations_t, double i_t, double j_t,
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

    zmag = std::sqrt(rsquare_thread + isquare_thread);
    index = iterations_thread + 1 - std::log(std::log(zmag)) / mlog2;

    return optimizedSmoothColor(index);
  } else {
    return secret;
  }
}

void draw(const mdbl_data & data) {
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
      rt_array[split_canvas[data.i].index][vertex_pos].position = sf::Vector2f(static_cast<float>(cx_thread), static_cast<float>(cy_thread));
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
  std::cout << "Done thread: " << data.i << std::endl;
}

void prepare_canvas(int numberOfThreads) {
  int i, temp;
  for (i = 0; i < numberOfThreads; i++) {
    //    split_canvas.push_back(resource_t(cWidth / numberOfThreads, cHeight));
    temp = int(main_data.cWidth) / numberOfThreads;
    split_canvas[i] = resource_t(temp, static_cast<unsigned int>(main_data.cHeight));
  }
  std::cout << "Canvas prepared" << std::endl;
}

void thread_split(int numberOfThreads) {
  std::cout << "Starting thread_split" << std::endl;
  std::cout << "Max Iterations: " << main_data.maxIterations << std::endl;

  //split them into numberOfThreads
  float cx_start_thread = 0;
  float cx_end_thread = 0;
  int temp = int(main_data.cWidth) / numberOfThreads;
  float cx_current_count = static_cast<float>(temp);

  int i;
  for (i = 0; i < numberOfThreads; i++) {
    if (cx_end_thread + cx_current_count > main_data.cWidth) {
      cx_end_thread = static_cast<float>(main_data.cWidth);
    } else {
      cx_end_thread += cx_current_count;
    }

    mdbl_data here;
    here.i = i;
    here.cx_start_thread = cx_start_thread;
    here.cx_end_thread = cx_end_thread;
    here.maxIterations = main_data.maxIterations;
    here.x = main_data.x;
    here.y = main_data.y;
    here.sx = main_data.sx;
    here.sy = main_data.sy;
    here.cWidth = main_data.cWidth;
    here.cHeight = main_data.cHeight;
    here.xmax = main_data.xmax;
    here.ymax = main_data.ymax;

    pool.add_job(draw, here);

    cx_start_thread = cx_end_thread;
    //    list_of_threads.push_back(thread);
    std::cout << "Launching thread: " << i << std::endl;
    //    thread.launch();
    ///thread.wait();
  }
  isThreadsWorking = true;

  std::cout << "All threads launched" << std::endl;
}

void renderingThread(sf::RenderWindow * window) {
  sf::Context ctx;
  int max_cpu_drawer = MAX_DRAWER_THREADS.return_value();
  std::cout << "Number of CPU Cores: " << std::thread::hardware_concurrency() << std::endl;
  std::cout << "Number of Drawing Threads: " << max_cpu_drawer << std::endl;
  if (!window -> isOpen()) {
    std::cout << "Error" << std::endl;
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
  std::cout << "Exiting Rendering Thread.." << std::endl;
  window -> setActive(false);
}

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

void zoomIn() {
  double mx = (double) mousex;
  double my = (double) mousey;

  double nx = main_data.x + (mx / main_data.cWidth) * (main_data.xmax - main_data.x);
  double nxmax = main_data.x + ((mx + zoom) / main_data.cWidth) * (main_data.xmax - main_data.x);

  main_data.x = nx;
  main_data.xmax = nxmax;

  double ny = main_data.y + (my / main_data.cHeight) * (main_data.ymax - main_data.y);
  double nymax = main_data.y + ((my + zoomy) / main_data.cHeight) * (main_data.ymax - main_data.y);

  main_data.y = ny;
  main_data.ymax = nymax;

  main_data.maxIterations += 50;
  cx = 0;
  ticks = 0;

  double pan = ((main_data.x + ((main_data.xmax - main_data.x) / main_data.cWidth) * (1 - 0)) - main_data.x);
  panX = pan * std::floor(main_data.cWidth / 4);
  panY = pan * std::floor(main_data.cHeight / 4);
}

void zoomOut() {
  double mx = (double) mousex;
  double my = (double) mousey;

  double fx0 = main_data.x + (0 / zoom) * (main_data.xmax - main_data.x);
  double fx1 = main_data.x + (1 / zoom) * (main_data.xmax - main_data.x);

  double dx = (fx1) - (fx0);

  double fy0 = main_data.y + (0 / zoomy) * (main_data.ymax - main_data.y);
  double fy1 = main_data.y + (1 / zoomy) * (main_data.ymax - main_data.y);

  double dy = (fy1) - (fy0);

  main_data.x = main_data.x - (mx * dx);
  main_data.y = main_data.y - (my * dy);

  double xback = main_data.cWidth - (mx + zoom);
  double yback = main_data.cHeight - (my + zoomy);

  main_data.xmax = main_data.xmax + (xback * dx);
  main_data.ymax = main_data.ymax + (yback * dy);

  main_data.maxIterations -= 50;
  cx = 0;
  ticks = 0;

  double pan = ((main_data.x + ((main_data.xmax - main_data.x) / main_data.cWidth) * (1 - 0)) - main_data.x);
  panX = pan * std::floor(main_data.cWidth / 4);
  panY = pan * std::floor(main_data.cHeight / 4);
}

inline void mdbl_evt_resize(){
  if (!ignoreResize) {
    ignoreResize = true;
    WINDOW->setSize(sf::Vector2u(static_cast<unsigned int>(main_data.cWidth), static_cast<unsigned int>(main_data.cHeight)));
  } else {
    ignoreResize = false;
  }
}

inline void mdbl_evt_mouse_lbutton_pressed(){
  mousex = EVENT->mouseButton.x;
  mousey = EVENT->mouseButton.y;
  zoomIn();
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_mouse_rbutton_pressed(){
  mousex = EVENT->mouseButton.x;
  mousey = EVENT->mouseButton.y;
  zoomOut();
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_mouse_moved(){
  ZOOMBOX->setPosition(float(EVENT->mouseMove.x), float(EVENT->mouseMove.y));
}

inline void mdbl_evt_key_left_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.x -= panX;
  main_data.xmax -= panX;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_right_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.x += panX;
  main_data.xmax += panX;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_down_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.y -= panY;
  main_data.ymax -= panY;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_up_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.y += panY;
  main_data.ymax += panY;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_w_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.maxIterations += 50;
  cx = 0;
  ticks = 0;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_s_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  main_data.maxIterations -= 50;
  cx = 0;
  ticks = 0;
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_d_pressed(){
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_q_pressed(){
  if (zoomy + 20 < main_data.cHeight) {
    zoomy += 20;
    zoom = std::floor(zoomy * (main_data.cWidth / main_data.cHeight));
  }
  ZOOMBOX->setSize(sf::Vector2f(static_cast<float>(zoom), static_cast<float>(zoomy)));
}

inline void mdbl_evt_key_a_pressed(){
  if (zoomy - 20 > 0) {
    zoomy -= 20;
    zoom = std::floor(zoomy * (main_data.cWidth / main_data.cHeight));
  }
  ZOOMBOX->setSize(sf::Vector2f(static_cast<float>(zoom), static_cast<float>(zoomy)));
}

inline void mdbl_evt_key_r_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }
  zoomy = 100;
  zoom = std::floor(zoomy * (main_data.cWidth / main_data.cHeight));
  main_data.x = -2.0;
  main_data.y = 2.0;
  main_data.xmax = (main_data.cWidth / main_data.cHeight) > 1 ? 2.0 * (main_data.cWidth / main_data.cHeight) + 2.0 : 2.0;
  main_data.ymax = -2.0;
  double pan = ((main_data.x + ((main_data.xmax - main_data.x) / main_data.cWidth) * (1 - 0)) - main_data.x);
  panX = pan * std::floor(main_data.cWidth / 4);
  panY = pan * std::floor(main_data.cHeight / 4);
  main_data.iterations = 0;
  main_data.maxIterations = 50;
  cx = 0;
  ticks = 0;
  ZOOMBOX->setSize(sf::Vector2f(static_cast<float>(zoom), static_cast<float>(zoomy)));
  thread_split(MAX_DRAWER_THREADS.return_value());
}

inline void mdbl_evt_key_p_pressed(){
  if (isPaused) {
    isPaused = false;
    WINDOW->setFramerateLimit(30);
  } else {
    isPaused = true;
    WINDOW->setFramerateLimit(2);
  }
}

inline void mdbl_evt_key_f_pressed(){
  if (!isPaused) {
    isPaused = false;
    WINDOW->setFramerateLimit(0);
  }
}

inline void mdbl_evt_key_i_pressed(){
  if (showText)
    showText = false;
  else
    showText = true;
}

inline void mdbl_evt_key_enter_pressed(){
  bmpOutput.write_data(storageBMP.get());
  ///bmpOutput.flip_data_horizontal();
  bmpOutput.save_data();
}

inline void mdbl_evt_key_space_pressed(){
  if (isThreadsWorking.load()) {
    stopRenderingThreads();
  }

  double mx = std::floor(main_data.cWidth * 1 / 8);
  double my = std::floor(main_data.cHeight * 1 / 8);

  double nx = main_data.x + (mx / main_data.cWidth) * (main_data.xmax - main_data.x);
  double nxmax = main_data.x + ((main_data.cWidth - mx) / main_data.cWidth) * (main_data.xmax - main_data.x);

  ///std::cout << "Here: " << nx << " " << nxmax << "\n";
  ///std::cout << "Here: " << mx << " " << cWidth - mx << "\n";

  main_data.x = nx;
  main_data.xmax = nxmax;

  double ny = main_data.y + (my / main_data.cHeight) * (main_data.ymax - main_data.y);
  double nymax = main_data.y + ((main_data.cHeight - my) / main_data.cHeight) * (main_data.ymax - main_data.y);

  main_data.y = ny;
  main_data.ymax = nymax;

  main_data.maxIterations += 50;
  cx = 0;
  ticks = 0;

  double pan = ((main_data.x + ((main_data.xmax - main_data.x) / main_data.cWidth) * (1 - 0)) - main_data.x);
  panX = pan * std::floor(main_data.cWidth / 4);
  panY = pan * std::floor(main_data.cHeight / 4);
  thread_split(MAX_DRAWER_THREADS.return_value());
}

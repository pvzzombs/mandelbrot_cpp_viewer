# mandelbrot_cpp_viewer
[![CodeFactor](https://www.codefactor.io/repository/github/pvzzombs/mandelbrot_cpp_viewer/badge/main)](https://www.codefactor.io/repository/github/pvzzombs/mandelbrot_cpp_viewer/overview/main)  [![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fpvzzombs%2Fmandelbrot_cpp_viewer.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fpvzzombs%2Fmandelbrot_cpp_viewer?ref=badge_shield) [![Build status](https://ci.appveyor.com/api/projects/status/7927cvh177pyo3l6?svg=true)](https://ci.appveyor.com/project/pvzzombs/mandelbrot-cpp-viewer) [![CircleCI](https://circleci.com/gh/pvzzombs/mandelbrot_cpp_viewer.svg?style=svg)](https://circleci.com/gh/pvzzombs/mandelbrot_cpp_viewer)  

A mandelbrot set viewer made in C++ using SFML library. 

## Building
### Requirements:  
* C++ compiler that supports c++11(or above)
* SFML 2.5.1 or newer
* CMAKE 3.1 or newer

### Instructions:  
1. Clone this repo using git  
```
git clone https://github.com/pvzzombs/mandelbrot_cpp_viewer.git
cd mandelbrot_cpp_viewer
```
2. Clone the SFML repo by using git  
```
git clone --depth 1 --branch 2.5.1 https://github.com/SFML/SFML.git
cd SFML
```
3. Configure and build the SFML libraries using cmake  
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```
4. Configure and build mandelbrot_cpp_viewer  
```
cd ../../
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPROJECT_ARCH="x64" -DLINK_MANDELBROT_STATIC=0
cmake --build .
```
5. Done!  
## License
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fpvzzombs%2Fmandelbrot_cpp_viewer.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fpvzzombs%2Fmandelbrot_cpp_viewer?ref=badge_large)
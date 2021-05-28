start build/Release/Mandelbrot.exe
ping 127.0.0.1 -n 10
taskkill /im build/Release/Mandelbrot.exe /f

start build/Release/Test.exe
ping 127.0.0.1 -n 10
taskkill /im build/Release/Test.exe /f
echo Done...
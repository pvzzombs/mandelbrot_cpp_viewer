start build/Release/Mandelbrot.exe
ping 127.0.0.1 -n 10
taskkill /im Mandelbrot.exe /f

start build/Release/Test.exe
ping 127.0.0.1 -n 10
taskkill /im Test.exe /f
echo Done...
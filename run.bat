@echo off
set DIRECT=C:\Users\notebook\Desktop\files\mandelbrot_cpp_viewer\bin\Release

echo 0. Deleting old files.....................................
del final\*.* /s /f /q
echo Compressing files.........................................

echo 1. Compressing c++ dll files

call %DIRECT%/upx/upx.exe -9 %DIRECT%/libgcc_s_sjlj-1.dll -o %DIRECT%/final/libgcc_s_sjlj-1.dll
call %DIRECT%/upx/upx.exe -9 %DIRECT%/libstdc++-6.dll -o %DIRECT%/final/libstdc++-6.dll

echo 2. Compressing sfml dll files

call %DIRECT%/upx/upx.exe -9 %DIRECT%/sfml-audio-2.dll -o %DIRECT%/final/sfml-audio-2.dll
call %DIRECT%/upx/upx.exe -9 %DIRECT%/sfml-graphics-2.dll -o %DIRECT%/final/sfml-graphics-2.dll
call %DIRECT%/upx/upx.exe -9 %DIRECT%/sfml-network-2.dll -o %DIRECT%/final/sfml-network-2.dll
call %DIRECT%/upx/upx.exe -9 %DIRECT%/sfml-system-2.dll -o %DIRECT%/final/sfml-system-2.dll
call %DIRECT%/upx/upx.exe -9 %DIRECT%/sfml-window-2.dll -o %DIRECT%/final/sfml-window-2.dll

echo 3. Compressing the executable file

call %DIRECT%/upx/upx.exe -9 %DIRECT%/mandelbrot_cpp_viewer.exe -o %DIRECT%/final/main.exe

echo 4. Copying additional files

xcopy %DIRECT%\config.txt %DIRECT%\final
xcopy %DIRECT%\verdana.ttf %DIRECT%\final
xcopy %DIRECT%\COPYING.LESSER.txt %DIRECT%\final
xcopy %DIRECT%\CHANGES.txt %DIRECT%\final
xcopy %DIRECT%\NOTICE.txt %DIRECT%\final
xcopy %DIRECT%\INFO.txt %DIRECT%\final

echo Done 
pause
exit


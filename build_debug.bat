if not exist build_debug mkdir build_debug
cd build_debug
cmake -S ../ -B . -G "MinGW Makefiles" -DDEBUG=1
mingw32-make.exe
move debug__MapMaker.exe ../
cd ..
pause
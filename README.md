### Instructions for the GUI on MacOS adopted from [tutorial](https://www.youtube.com/watch?v=BBv3FkFcPwI):

Download wxWidgets from their [official website](https://www.wxwidgets.org/downloads/) and navigate to the folder
  ```bash
   cd path/to/wxWidgets
  ```
Download CMakes from their [official website](https://cmake.org/download/) and add to wxWidgets folder
  ```bash
   sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
  ```
Verify CMakes installation
  ```bash
   cmake --version
  ```
Make a folder in the wxWidgets folder
  ```bash
   mkdir inverse
   cd inverse
  ```
Run CMake to configure the build
  ```bash
   cmake ..
  ```
Build wxWidgets using 16 threads
  ```bash
   make -j 16
  ```
Create the wxWidgets application code
  ```bash
   vim inverse_word_search.cpp
  ```
Compile the code
  ```bash
   clang++ -std=c++17 inverse_word_search.cpp -o prog `./wx-config --cxxflags --libs`
  ```
Run the executable to use the GUI:
  ```bash
   ./prog
  ```

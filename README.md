Instructions for the GUI on MacOS adopted from tutorial available at https://www.youtube.com/watch?v=BBv3FkFcPwI:
- download wxWidgets (will show up in Downloads folder)
- download CMakes (I did it from their official website instead of homebrew) and then in the terminal, run command:
  sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
- check by run command:
  cmake --version
- change into wxWidgets directory
- make folder
- run command:
  cmake ..
- run command:
  make -j 16
- run command:
  vim filename.cpp
- clang++ filename.cpp -o gui_name `./wx-configs --cxxflags --lib`
- ./gui_name

# PacMan in GL

2003 CM203 University of Southampton Computer Graphics project. Vintage, and still runs!

# Linux (Raspbian)
```shell
sudo apt-get install freeglut3-dev g++
g++ -I /usr/include pacman.cpp -lGL -lglut -lGLU  -o pacman
```

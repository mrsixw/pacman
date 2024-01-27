# PacMan in GL

2003 CM203 University of Southampton Computer Graphics project. Vintage, and still runs!

## MacOS 
```shell
gcc pacman.cpp -framework GLUT -framework GLUT -o pacman
```

### Linux (Raspbian)
```shell
sudo apt-get install freeglut3-dev g++
g++ -I /usr/include pacman.cpp -lGL -lglut -lGLU  -o pacman
```

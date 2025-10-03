# roche-limit-sim

## Libraries:

``sudo apt-get install libglm-dev ``<br>
``sudo apt install build-essential libglfw3-dev libglfw3 libglew-dev``

## Command to compile: 
``g++ main.cpp src/glad.c -Iinclude -o sphere_app -lglfw -ldl -lGL``
#### for Moon_Maker_test
``g++ Moon_Maker_test.cpp src/glad.c -Iinclude -o moon_maker_test -lglfw -ldl -lGL``

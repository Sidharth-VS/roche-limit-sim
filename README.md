# roche-limit-sim

## Libraries:

``sudo apt-get install libglm-dev ``<br>
``sudo apt install build-essential libglfw3-dev libglfw3 libglew-dev``<br> 


## Command to compile: 
``g++ main.cpp src/glad.c -Iinclude -o sphere_app -lglfw -ldl -lGL``
#### for Moon_Maker_test
``g++ Moon_Maker_test.cpp src/glad.c -Iinclude -o moon_maker_test -lglfw -ldl -lGL``

## Test sample inputs
- Enter planet mass: 20 <br>
- Enter planet radius: 3 <br>
- Enter moon mass: 1 <br>
- Enter moon radius: 1 <br>
- Enter moon distance from planet: 9 <br>
- Enter moon velocity (Y component): 0.4 <br>
- Enter moon velocity (Z component): 0 <br>

=== Simulation Parameters === <br>
- Roche Limit: 6.62318 <br>
- Moon Distance: 9 <br>
- Recommended orbital velocity for stable orbit: 0.471405 <br>
- Your velocity magnitude: 0.4 <br>


# Elevator Simulation Project

A C++ project that simulates an elevator system, visualized using the Allegro game library. The program processes requests from an input file and provides a graphical representation of the elevator's operation.

---

## Table of Contents
1. [About the Project](#about-the-project)
2. [Dependencies](#dependencies)
3. [Compilation](#compilation)
4. [Usage](#usage)
5. [Features](#features)
6. [Bugs](#bugs)

---

## About the Project

This project simulates an elevator system that processes user requests from an input file and operates in a graphical environment. It demonstrates the following:
- Request handling (up/down/stopped states).
- Dynamic visualization using the Allegro library.
- State design pattern for managing elevator operations.
- MVC design pattern for managing visualization

---

## Dependencies

To run this project, you must have the Allegro library installed. This project uses the following Allegro components:
- `allegro`
- `allegro_font`
- `allegro_primitives`
- `allegro_image`
- `allegro_main`
- `allegro_ttf`
- `allegro_color`

## Compilation

You must run the following command to compile the source files:

"g++ ECGraphicViewImp.cpp ElevatorObserver.cpp ECElevatorSim.cpp Controller.cpp main.cpp
-lallegro -lallegro_font -lallegro_primitives -lallegro_image -lallegro_main -lallegro_ttf -lallegro_color -o elevator -std=c++17"


## Usage

This program requires an input file. Any lines that you wish to be ignored should start with '#'.
The first line of the input file should state the number of floors and the total simulation time.
All subsequent lines should be requests.
Requests are written in the form of : 

RequestTime SrcFloor DestinationFloor


An example input file would be:
---
8 10
2 3 1
---
Execute the following command to run:
./elevator input.txt

## Features

SPACE_BAR:

Pressing the space bar pauses the simulation. The simulation will be paused until the spacebar is pressed again.

PROGRESS BAR:

visual representation of simulation progress

PASSENGER AND REQUEST COUNTS:

keeps a visual tally of all passengers current in the cabin and all passengers waiting to get on the cabin.

SANTA HAT:

A festive decoration on passenger design for the holiday season.

NOT IMPLEMENTED (yet)

-scrolling mechanism to handle unlimited amount of floors while maintaining a constant floor size

## Bugs

The only known bug in this program is the handling of complex passenger animations. Sometimes the passengers will fail to go to the exact position when multiple are unloading/loading at the same time. However, this is just a small visualization issue that would be worth cleaning up in a more advanced version of this project. 
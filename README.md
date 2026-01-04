# 1D Transient heat conduction solver

This project is a one dimensional transient heat conduction solver designed to simulate temperature development through layered materials exposed to fire. The solver uses an explicit finite element approach and supports multiple fire curves, material layers, and temperature dependent properties.

## Project structure

### Create `config.txt`
In the same folder as the executable, create a file named config.txt

here the config.txt can include all these parameters: 
MODEL = 1

SIMULATION_TIME = 3600

NUMBER_OF_MM_PER_LAYER = 5

TIME_STEPS_PER_SECOND = 5

FIRE_CURVE_EXPOSED = 2

CONSTANT_TEMP = 500

EPSILON = 0.8

H_EXPOSED = 10

H_AMBIENT = 4

INITIAL_TEMPERATURE = 20

AMBIENT_TEMPERATURE = 20

SIGMA = 5.67e-8

These are the default parameters and can be adjusted by the user.

In the same directory as the executable, create a folder named: material
This folder will contain all material definition files.

### Add material files
In the material directory the material files are created by rows as:
Temperature Conductivity SpecificHeat Density


## Author

Developed by Emil Nyström, fire safety specialist.
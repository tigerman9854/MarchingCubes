# Marching Cubes

CS 553/453 Final Project

Ashley Pettibone, Kyle Hiebel, Braeden Kuether

# Building Instructions

1. Open `MarchingCubes.sln`
2. Set the startup project to `MarchingCubes.vcxproj` (Note: We already generated all datasets so there is no need to run `DataGen.vcxproj`)
3. Change the configuration to `Release` for better performance
4. Press F5 to compile and run (Note: OpenMP must be enabled to properly compile)
5. Use the number keys [1-4] to cycle between visualization modes. Use the mouse to rotate the visualization
6. If you want to change the scalar field, look for `void init()` in `MarchingCubes/main.cpp`, and uncomment a different dataset (i.e. `scalarField = ReadFile("../Output/sphere.txt");`)

### 1. Point Cloud
![](Images/points.PNG)

### 2. Lines
![](Images/lines.PNG)

### 3. Cubes
![](Images/cubes.PNG)

### 4. Marching Cubes
![](Images/marching.PNG)

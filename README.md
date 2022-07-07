# Loop Mesh Subdivision

## Overview

Coursework produced for the module 'COMP5821M - Geometric Processing' in course of my study of High-Performance Graphics and Games Engineering at the University of Leeds.

Renders a directed edge file, with a button to run the loop subdivision algorithm. The results can be viewed in the application and written out to a new directed edge file.

Tetrahedron       |  Tetrahedron after 3 subdivisons (zoomed)
:-------------------------:|:-------------------------:
![Tetrahedron-Base-Screenshot](https://user-images.githubusercontent.com/32328378/177786811-acaae044-087d-49b5-8314-f621f9b7044e.png)  |  ![Tetrahedron-3-Divisions-Zoomed-Screenshot](https://user-images.githubusercontent.com/32328378/177786781-4bb26a7b-a2d7-48f1-98c0-b1b81095f605.png)

## Usage
`.\LoopSubdivisionRelease.exe geometry`

Geometry included in [diredgenormals](diredgenormals)

E.g: `.\LoopSubdivisionRelease.exe ..\diredgenormals\tetrahedron.diredgenormals`

## Building and dependencies
Visual Studio 2019 Solution files are provided for Windows, and a Makefile for Linux

Depends on Qt 5.12.2

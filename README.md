[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/YybNWfh8)
<br/>
# Assignment 3
## Task Description
<br/>
The main aim of the task is performing multi-threaded programming in any physical concurrency supporting programming language. To achieve this following procedure should be implemented on the image:
<br/> 
1. Find the average color for the (square size) x (square size) boxes and set the color of the whole square to this average color. Result should be demonstrated by progress, not at once. This is an averaging operation.
<br/> 
2. Application shall take three arguments from the command line: file name, square size and the processing mode (Example: yourprogram somefile.jpg 5 S):
<br/>     
**file name** : the name of the graphic file of jpg format (no size constraints)
<br/>     
**square size** : the side of the square for the averaging
<br/>     
**processing mode** : 'S' - single threaded and 'M' - multi threaded
<br/> 
3. In the multi-processing mode, same procedure should be performed in parallel threads. The number of threads shall be selected according to the computer's CPU cores. It is recommended by instructor to perform the process in Single Thereaded from left-to-right and top-to-down, in Multi-threaded doing the same but in different regions in parallel.
<br/> 
4. There result shall be saved in a _result.jpg_ file

## Code in C++ Language

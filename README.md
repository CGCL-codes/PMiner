# PMiner v3.6.2205
Edited by Horsky on 2022\05\18

## Abstract
---
The general graph mining system is an important tool for discovering specific structural patterns from graph data. The core goal of the existing graph mining system research is to ensure fast and accurate mining and to reduce resource consumption in the mining process. With the rapid increase in the scale of graph data and the growing demand for mining of complex structures in various fields, the existing systems have been unable to adapt to large-scale and high-complexity mining tasks. The main reasons are as follows: (1) All the graph mining systems do not fully utilize the features and constraints contained in the pattern graph; (2) These systems fail to fully consider that the selection of different mining paths and sequences will have significant differences in system performance; (3) The graph mining systems save unnecessary intermediate results during the mining process, which consumes too much memory space.\
To meet the above challenges, the PMiner system is designed and implemented. By in-depth analysis of the constraint relationship between nodes and edges in the pattern graph, the system proposes the concept of constraint inclusion relationship, and designs a pattern graph segmentation based on constraint inclusion relationship, which reduces the scale of mining tasks by several times to several ten times. At the same time, the concepts of node selectivity and coverage are proposed, combined with pattern graph segmentation to achieve a better mining plan generation process. Finally, a new mining plan execution process is designed according to the pattern graph segmentation, and the methods of copy assignment and shared assignment are proposed for the generation and update of intermediate result sets, avoiding a large number of redundant computations and storing unnecessary intermediate result sets.\
Experimental results show that for 8 real graph datasets of different scales, the PMiner system performs 52～61 times faster than the current state-of-the-art graph mining systems GraphPi and Peregrine on average when mining complex pattern graphs. At the same time, the test proves that using the pattern graph segmentation can reduce the memory consumption of the intermediate result set generated in the mining process by 2～4 times.

Copyright (C) 2022, [STCS & CGCL](http://grid.hust.edu.cn/) and [Huazhong University of Science and Technology](https://www.hust.edu.cn/).

## Version update
---
1.Modify the original format of using node storage to use storage matching edges \
2.Use in-degree constraints to trim intermediate result sets during result combination。

## Installation
---
Before installation, please make sure that the linux system library has been updated and the oneTBB dependent library has been installed. For the installation of the oneTBB dependent library, see the link:[oneTBB](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html)\
The dependencies, compilers, and compilation options required for the installation have been encapsulated in the Makefile, enter the root directory of the PMiner system, and run the make command. No other operations are required.
```bash
cd /PMiner/
make
```

## Use
---
The user interface module provides users with a simple and convenient system operation interface. When performing graph mining tasks, the user only needs to input 5 parameters: the real graph address, the pattern graph address, the number of real graph nodes, the number of pattern graph nodes, and the output file address. The formats of the real graph and the schema graph support both the adjacency list format and the edge list format, and there is no restriction on whether the input graph has a ring or not.

For example:
```C++
./main /data/wordassociation-2011.txt p1.txt 10617 4 output.txt
```
This statement means to call the main function to mine the pattern graph p1 in the real graph wordassociation-2011, in which the number of real graph nodes is 10617, and the number of pattern graph nodes is 4, and the mining results are saved in output.txt.

## Specify the number of threads
---
By default, the PMiner system uses all the threads of the current platform to perform graph mining tasks to obtain the best performance. If you need to specify the number of threads for testing, please follow the steps below:\
1.Open the PatternMatching.cpp file
```bash
vim PatternMatching.cpp
```
2.Search for the following statements individually:
```C++
blocked_range<size_t>(0, minMatchID_PMR.size()), 0, [&](blocked_range<size_t>
```
```C++
parallel_for(blocked_range<size_t>(0, minMatchID_PMR.size()), [&](blocked_range<size_t> r)
```
Add the command before both statements:
```C++
tbb::task_scheduler_init init(tread_num);
```
Where thread_num represents the maximum number of threads that you want the system to use when running.

## Result description
The mining results are stored in a user-specified file, where each line represents a mining result, and the number represents the real graph ID. For example, the result of mining a triangle pattern graph may be as follows:
```
0 15 28
111 0 234
18 29 33
223 1345 322
433 7456 812
553 21 3434
425 566 65
99 201 38
···
```
Each row in the above result is a triangle in the real graph. For example, the result of the first row, 0 15 28, means that the three nodes with the node ID of 0 15 28 in the real graph have edges between each other to form a triangle.

## Author
PMiner is developed in National Engineering Research Center for Big Data Technology and System, Cluster and Grid Computing Lab, Services Computing Technology and System Lab, School of Computer Science and Technology, Huazhong University of Science and Technology, Wuhan, China by Tianyu Ma [(tyma@hust.edu.cn)](tyma@hust.edu.cn), Siyuan He [(syhe@hust.edu.cn)](syhe@hust.edu.cn), Pingpeng Yuan[(ppyuan@hust.edu.cn)](ppyuan@hust.edu.cn).

If you have any questions, please contact Pingpeng Yuan[(ppyuan@hust.edu.cn)](ppyuan@hust.edu.cn). We welcome you to commit your modification to support our project.

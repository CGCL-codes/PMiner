# PMiner v3.6.2303

PMiner is a high-performance graph mining system based on Pattern Graph Segmentation, which can efficiently and quickly deal with large-scale and high complexity graph mining problems. \
\
The general graph mining system is an important tool for discovering specific structural patterns from graph data. The core goal of the existing graph mining system research is to ensure fast and accurate mining and to reduce resource consumption in the mining process. With the rapid increase in the scale of graph data and the growing demand for mining of complex structures in various fields, the existing systems have been unable to adapt to large-scale and high-complexity mining tasks. The main reasons are as follows: (1) All the graph mining systems do not fully utilize the features and constraints contained in the pattern graph; (2) These systems fail to fully consider that the selection of different mining paths and sequences will have significant differences in system performance; (3) The graph mining systems save unnecessary intermediate results during the mining process, which consumes too much memory space.\
\
To meet the above challenges, the PMiner system is designed and implemented. By in-depth analysis of the constraint relationship between nodes and edges in the pattern graph, the system proposes the concept of constraint inclusion relationship, and designs a pattern graph segmentation based on constraint inclusion relationship, which reduces the scale of mining tasks by several times to several ten times. At the same time, the concepts of node selectivity and coverage are proposed, combined with pattern graph segmentation to achieve a better mining plan generation process. Finally, a new mining plan execution process is designed according to the pattern graph segmentation, and the methods of copy assignment and shared assignment are proposed for the generation and update of intermediate result sets, avoiding a large number of redundant computations and storing unnecessary intermediate result sets.\
\
Experimental results show that for 8 real graph datasets of different scales, the PMiner system performs 6677～8329 times faster than the current state-of-the-art graph mining systems Peregrine and GraphPi on average when mining complex pattern graphs. \
\
Copyright (C) 2022-2023, [STCS & CGCL](http://grid.hust.edu.cn/) and [Huazhong University of Science and Technology](https://www.hust.edu.cn/).

## Version update

---

1.Modify the original format of using node storage to use storage matching edges \
2.Use in-degree constraints to trim intermediate result sets during result combination 

## Installation

---

Before installation, please make sure that the linux system library has been updated and the oneTBB dependent library has been installed. For the installation of the oneTBB dependent library, see the link:[oneTBB](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html)\
\
The dependencies, compilers, and compilation options required for the installation have been encapsulated in the Makefile, enter the root directory of the PMiner system, and run the make command. No other operations are required.

```bash
cd /PMiner/
make
```

## Usage

---

The user interface module provides users with a simple and convenient system operation interface. When performing graph mining tasks, the user only needs to input 5 parameters: the real graph address, the pattern graph address, the number of real graph nodes, the number of pattern graph nodes,  and the number of threads. The formats of the real graph and the schema graph support both the adjacency list format and the edge list format, and there is no restriction on whether the input graph has a ring or not.

```c++
$ ./bin/count
Usage: ./bin/count <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum>
```

For example:

```C++
./bin/count ./data/Wiki-Vote.txt ./pattern/7_pb.txt 8298 7 28
```

This statement means using the counting method (just not output) to mine the pattern graph 7_pb in the real graph Wiki-Vote with 28 threads, in which the number of real graph nodes is 8298, and the number of pattern graph nodes is 7.

## Author

PMiner is developed in National Engineering Research Center for Big Data Technology and System, Cluster and Grid Computing Lab, Services Computing Technology and System Lab, School of Computer Science and Technology, Huazhong University of Science and Technology, Wuhan, China by Yujiang Wang [(wangyj_@hust.edu.cn)](wangyj_@hust.edu.cn), Tianyu Ma [(tyma@hust.edu.cn)](tyma@hust.edu.cn), Siyuan He [(syhe@hust.edu.cn)](syhe@hust.edu.cn), Pingpeng Yuan[(ppyuan@hust.edu.cn)](ppyuan@hust.edu.cn).

If you have any questions, please contact Pingpeng Yuan[(ppyuan@hust.edu.cn)](ppyuan@hust.edu.cn). We welcome you to commit your modification to support our project.


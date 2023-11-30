# PMiner v4.0.2311

PMiner is a high-performance graph matching system based on Pattern Graph Segmentation, which can efficiently and quickly deal with large-scale and high complexity graph mining problems. 

The general graph mining system is an important tool for discovering specific structural patterns from graph data. The core goal of the existing graph mining system research is to ensure fast and accurate mining and to reduce resource consumption in the mining process. With the rapid increase in the scale of graph data and the growing demand for mining of complex structures in various fields, the existing systems have been unable to adapt to large-scale and high-complexity mining tasks. The main reasons are as follows: (1) All the graph mining systems do not fully utilize the features and constraints contained in the pattern graph; (2) These systems fail to fully consider that the selection of different mining paths and sequences will have significant differences in system performance; (3) The graph mining systems save unnecessary intermediate results during the mining process, which consumes too much memory space.

To meet the above challenges, the PMiner system is designed and implemented. By in-depth analysis of the constraint relationship between nodes and edges in the pattern graph, the system proposes the concept of constraint inclusion relationship, and designs a pattern graph segmentation based on constraint inclusion relationship, which reduces the scale of mining tasks by several times to several ten times. At the same time, the concepts of node selectivity and coverage are proposed, combined with pattern graph segmentation to achieve a better mining plan generation process. Finally, a new mining plan execution process is designed according to the pattern graph segmentation, and the methods of copy assignment and shared assignment are proposed for the generation and update of intermediate result sets, avoiding a large number of redundant computations and storing unnecessary intermediate result sets.

Experimental results show that for 10 real graph datasets of different scales, the PMiner system performs 9-150 times faster than the current state-of-the-art graph mining systems Graphflow、Peregrine and GraphPi on average when mining complex pattern graphs. 

## Installation

---

Before installation, please make sure that  the oneTBB dependent library has been installed. For the installation of the oneTBB dependent library, see the link:[oneTBB](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html)

The dependencies, compilers, and compilation options required for the installation have been encapsulated in the Makefile, enter the root directory of the PMiner system, and run the make command. No other operations are required.

```bash
cd /PMiner/
make
```

## Usage

---

The user interface module provides users with a simple and convenient system operation interface. When performing graph mining tasks, the user only needs to input 5 parameters: the real graph address, the pattern graph address, the number of real graph nodes, the number of pattern graph nodes,  and the number of threads. The formats of the real graph and the schema graph support both the adjacency list format and the edge list format, and there is no restriction on whether the input graph has a ring or not.

```c++
$ ./bin/PMiner
Usage: ./bin/PMiner <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum>
```

For example:

```C++
./bin/PMiner ./data/Wiki-Vote.txt ./pattern/7_pb.txt 8298 7 28
```

This statement means using the enumeration method  (just not output to a file)  to mine the pattern graph 7_pb in the real graph Wiki-Vote with 28 threads, in which the number of real graph nodes is 8298, and the number of pattern graph nodes is 7.




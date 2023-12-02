# PMiner v4.0.2311

PMiner is a high-performance graph matching system based on Pattern Graph Reduction. There are several important features of PMiner: 

Pattern Graph Reduction. By analyzing the relationship among constraints specified in a pattern graph, our approach finds a minimum set of constraints which cover other constraints. By this way, PMiner reduces the given pattern graph to a simpler form. Thus, it reduces data ccess cost and memory usage.

Efficient Execution. PMiner generates an efficient execution plan to guide its exploration and materialization for a task. PMiner reuses the matchings of edges. So, it does not need to materialize each edge using data graph directly. This results in much lesser data access and computation compared to the state-of-the-art graph matching approaches. Moreover, PMiner postpones the initialization of edges until a suitable time. It reduces redundancy intermediate results, resulting in lesser memory consumption.

High Performance. PMiner outperforms state-of-the-art undirected graph matching systems. Furthermore, PMiner can scale to large graphs and complex graph matching tasks which could not be efficiently handled by other graph matching systems.

## Installation

---

PMiner is implemented using Intel oneTBB. Before the installation of PMiner, please make sure that the libraries required by Intel oneTBB have been installed. For the requirements (OS, libraries) of Intel oneTBB, please refer to  Intel [oneTBB](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html).

The dependencies, compilers, and compilation options required for the installation have been encapsulated in the Makefile. Users enter the directory of PMiner and run the make command. No other operations are required.

```bash
cd /PMiner/
make
```

## Usage

---

The user interface is simple. When performing graph matching tasks, users only need to input 5 parameters: the file to store graph data, the file of the pattern graph, the number of nodes (data graph), the number of vertices (pattern graph), and the number of threads. The data graph or pattern graph can be stored in the adjacency list or the edge list. 

```c++
$ ./bin/PMiner
Usage: ./bin/PMiner <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum>
```

For example:

```C++
./bin/PMiner ./data/Wiki-Vote.txt ./pattern/7_pb.txt 8298 7 28
```

This statement means to find the matches of pattern graph 7_pb in the data graph Wiki-Vote with 28 threads, in which the number of data graph nodes is 8298, and the number of pattern graph vertices is 7. PMiner can enumerate or count the results of matches.




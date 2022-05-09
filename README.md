# PMiner v3.6.2205
Horsky编辑于20220509

## 摘要
---
通用图挖掘系统是从图数据中发现特定结构模式的重要工具，当前图挖掘系统相关研究的核心目标是保证快速准确得到挖掘结果并减少挖掘过程中的资源消耗。随着图数据规模的急剧增长以及各领域对复杂图结构快速挖掘的需求不断增长，现有的系统已经不能适应大规模、高复杂度的图挖掘任务，其原因主要在于（1）对模式图的分析不够深入，没有充分利用模式图中包含的特征与约束；（2）未能充分考虑到不同挖掘路径和挖掘顺序的选择会对系统性能产生的影响；（3）图挖掘过程中保存不必要的中间结果，消耗过多内存空间。\
为了应对以上的挑战，设计并实现PMiner系统。该系统通过深入分析模式图中各节点间以及各边间存在的约束关系，提出约束包含关系的概念，并设计基于约束包含关系的模式图分割方案，将挖掘任务的规模缩小数倍至数十倍。同时提出节点选择度与覆盖度的概念，结合模式图分割实现了更好的挖掘计划生成过程，给出最优的挖掘路径与顺序。最后设计实现了基于模式图分割的挖掘计划执行流程，对中间结果集的生成和更新提出复制赋值方法和共享赋值方法，从而避免了大量的冗余计算以及对不必要中间结果集的存储。\
实验结果表明，对于8个不同规模的真实图数据集，PMiner系统在进行复杂模式图的挖掘任务时其执行速度快于最新的图挖掘系统GraphPi和Peregrine平均高达52～61倍。同时测试证明使用模式图分割方案可以有效缩减挖掘过程中产生的中间结果集大小，对内存的消耗缩减达到2～4倍。

关键词： 图数据；图挖掘；模式匹配

## Abstract
---
The general graph mining system is an important tool for discovering specific structural patterns from graph data. The core goal of the existing graph mining system research is to ensure fast and accurate mining and to reduce resource consumption in the mining process. With the rapid increase in the scale of graph data and the growing demand for mining of complex structures in various fields, the existing systems have been unable to adapt to large-scale and high-complexity mining tasks. The main reasons are as follows: (1) All the graph mining systems do not fully utilize the features and constraints contained in the pattern graph; (2) These systems fail to fully consider that the selection of different mining paths and sequences will have significant differences in system performance; (3) The graph mining systems save unnecessary intermediate results during the mining process, which consumes too much memory space.\
To meet the above challenges, the PMiner system is designed and implemented. By in-depth analysis of the constraint relationship between nodes and edges in the pattern graph, the system proposes the concept of constraint inclusion relationship, and designs a pattern graph segmentation based on constraint inclusion relationship, which reduces the scale of mining tasks by several times to several ten times. At the same time, the concepts of node selectivity and coverage are proposed, combined with pattern graph segmentation to achieve a better mining plan generation process. Finally, a new mining plan execution process is designed according to the pattern graph segmentation, and the methods of copy assignment and shared assignment are proposed for the generation and update of intermediate result sets, avoiding a large number of redundant computations and storing unnecessary intermediate result sets.\
Experimental results show that for 8 real graph datasets of different scales, the PMiner system performs 52~61 times faster than the current state-of-the-art graph mining systems GraphPi and Peregrine on average when mining complex pattern graphs. At the same time, the test proves that using the pattern graph segmentation can reduce the memory consumption of the intermediate result set generated in the mining process by 2~4 times.

Key words: Graph Data, Graph Mining, Pattern Matching

## 版本更新内容：
---
1.将原有的采用节点存储的格式修改为采用存储匹配边 \
2.在结果组合过程中使用出入度约束削减中间结果集。

## 安装说明
---
安装前请确保已更新linux系统库并安装oneTBB依赖库，oneTBB依赖库的安装见链接：[oneTBB](https://spec.oneapi.io/versions/latest/elements/oneTBB/source/nested-index.html)\
已经将安装所需的依赖项、编译器、编译选项封装在Makefile文件中，进入PMiner系统根目录，运行make命令即可，无需其它操作。
```bash
cd /PMiner/
make
```

## 使用说明
---
用户接口模块为用户提供了简单便捷的系统操作接口，在进行图挖掘任务时用户仅需输入5个参数：真实图地址、模式图地址、真实图节点数、模式图节点数、输出文件地址。其中真实图与模式图的格式同时支持邻接表格式和边表格式，对输入图是否带环无限制。

举例说明：
```C++
./main /data/wordassociation-2011.txt p1.txt 10617 4 output.txt
```
该语句表示调用main函数，在真实图wordassociation-2011中挖掘模式图p1，其中真实图节点数10617，模式图节点数4，挖掘结果保存在output.txt中。

## 指定线程数
---
PMiner系统默认使用当前平台全部线程数进行图挖掘任务，以获取最佳性能表现，如果需要指定线程数进行测试请按照以下流程操作：\
1.打开PatternMatching.cpp文件
```bash
vim PatternMatching.cpp
```
2.分别搜索以下语句：
```C++
blocked_range<size_t>(0, minMatchID_PMR.size()), 0, [&](blocked_range<size_t>
```
```C++
parallel_for(blocked_range<size_t>(0, minMatchID_PMR.size()), [&](blocked_range<size_t> r)
```
在两个语句前均添加命令：
```C++
tbb::task_scheduler_init init(tread_num);
```
其中thread_num表示希望系统运行时使用的最大线程数。

## 结果说明
挖掘结果存储在用户指定的文件中，其中每一行代表一个挖掘结果，数字代表真实图ID，比如挖掘三角形模式图的结果可能如下：
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
上述结果中每一行都是真实图中一个三角形，比如第一行的结果 0 15 28 代表在真实图中节点ID为 0 15 28 的三个节点互相之间存在边构成了三角形。

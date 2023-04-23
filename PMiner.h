#ifndef PMINER_H_
#define PMINER_H_
#include <tbb/tbb.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <limits.h>
#include <unordered_map>
#include <map>
#include <list>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <limits.h>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <set>
#include <condition_variable>
#include "output.h"
#define BLOCKNUM 10000

using namespace std;
using namespace tbb;

typedef unsigned P_ID;
typedef unsigned R_ID;
struct Degree
{
    unsigned indeg;
    unsigned outdeg;
};

struct P_edge
{
    int to;
    int flag;
};

class block
{
private:
    int size; //块内能够存储的总边数
    R_ID startId;
    R_ID endId;
    vector<R_ID> adj;
    unordered_map<R_ID, int> adjIndex; // key为点id，value为该点的邻居在block中的起始位置
public:
    block(int n, R_ID begin, R_ID end)
    {
        size = n;
        adj.resize(n);
        startId = begin;
        endId = end;
    }
    void buildIndex(Degree* degree_R)//块内索引
    {
        adjIndex[startId] = 0;
        for(R_ID i=startId+1;i<=endId;i++){
            adjIndex[i]=adjIndex[i-1]+degree_R[i-1].outdeg;
        }
        return;
    }
    void insertEdge(R_ID to){
        adj.push_back(to);
        return ;
    }
    int getStartPos(R_ID from){
        return adjIndex[from];
    }
    R_ID getNeibor(R_ID from,int i){
        return adj[adjIndex[from]+i];
    }
};

class PMiner
{
public:
    
    PMiner(){};                                           
    virtual ~PMiner();                                   
    void init(const std::string& Output_dir, int thread_num);                              // sky211116
    
    void FIXLINE(char *s);
    bool build_degree_R(const std::string& inputfile, unsigned vertexNum); //得到degree_R
    bool build_R_adj(const std::string& inputfile);                        //得到用一维数组存储的邻接表和逆邻接表
    bool build_P_adj(const std::string& inputfile, unsigned vertexNum);    //用邻接矩阵存储模式图


    bool matchPR_expand();
    bool PVAllVisited(vector<int> P_visited);
    void findSym(); //判断P中是否有等价点
    void sym_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, long long &result, int ori_centerID, unordered_map<R_ID,int> isTraversed);
    void sym_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish,unordered_map<R_ID,int> isTraversed);
    void sym_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish,unordered_map<R_ID,int> isTraversed);
    void build_constraint();//构建模式图的约束包含关系
    void build_center_order();
    void center_order_dfs(P_ID preCenter, std::vector<std::vector<P_ID>>P_adj_copy, vector<P_ID> temp_center_order, int center_oreder_size, int cur_order_sel,int & order_sel, vector<int> marked);



    void searchALLCircle();
    unsigned long long three_Circle_Multithreaded_search(unsigned i);
    unsigned long long three_full_Circle_Multithreaded_search(unsigned i);
    unsigned long long four_Circle_Multithreaded_search(unsigned i);
    unsigned long long four_full_Circle_Multithreaded_search(unsigned i);
    unsigned R_adj_merge(R_ID first_R_id, R_ID last_R_id);
    unsigned R_adj_merge_four(R_ID first_R_id, R_ID second_R_id,  R_ID last_R_id);
    void R_adj_merge_full(R_ID first_R_id, R_ID last_R_id, vector<R_ID> & PMR);
    void R_adj_merge_full_four(R_ID first_R_id, R_ID second_R_id, R_ID last_R_id, vector<R_ID> & PMR);
    unsigned long long eqCircle_Multithreaded_search(R_ID i);
    void minDFS(vector<R_ID> PMR_copy,R_ID ori_centerId,long long& result,int curTime,unordered_map<R_ID,int> mp);



    void searchAllPR();

    unsigned long long Multithreaded_search(R_ID i);
    unsigned long long searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, int P_center_index,unordered_set<R_ID> isTraversed);
    bool isNextEPatternEmpty(std::vector<std::vector<P_ID>> &P_adj_copy); //判断剩下边模式是否为空
    bool extendEdgePattern(P_ID v_ps, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<std::vector<P_ID>> &P_adj,unordered_set<R_ID> isTraversed);
    bool reverse_extendEdgePattern(P_ID v_pt, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<std::vector<P_ID>> &P_adj, unordered_set<R_ID> isTraversed);
    unsigned long long count_set(std::vector<std::vector<unsigned>> &PMR_copy, unordered_set<R_ID> isTraversed); 
    unsigned long long count_full(std::vector<std::vector<unsigned>> &PMR_copy, unordered_set<R_ID> isTraversed); 



    bool eliminate(std::vector<unsigned> &PMR_remain, std::vector<unsigned> &isTraversed);
    unsigned set_operation2(std::vector<std::vector<unsigned>> &PMR_copy);
    unsigned set_operation3(std::vector<std::vector<unsigned>> &PMR_copy);
    unsigned set_operation4(std::vector<std::vector<unsigned>> &PMR_copy);
    unsigned long long full_permutation2(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation3(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation4(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation5(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation6(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation7(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation8(std::vector<std::vector<unsigned>> &PMR_remain);
    unsigned long long full_permutation(std::vector<std::vector<unsigned>> &PMR_remain, int index, unordered_set<unsigned>& set);
    void merge_un_set(std::vector<P_ID>& v1, std::vector<P_ID>& v2, std::unordered_set<P_ID>& un_set);
    void merge_set(std::vector<P_ID>& v1, std::vector<P_ID>& v2, std::vector<P_ID>& v3); //求并
    unsigned merge_count(std::vector<P_ID>& v1, std::vector<P_ID>& v2);
    bool intersection(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline); //求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空


    bool De_duplication(std::vector<unsigned> unchecked_res); 
    unsigned full_arrangement(std::vector<vector<R_ID>> cur_thread_PMR);
    bool check_result(std::vector<std::vector<unsigned>> &PMR_copy); // sky211118
    bool check_result(vector<unsigned> PMR_copy);
    bool get_Radj_Index(R_ID v_r, int &start, int &end); // sky211118


    void print_PMR();
    void print_P_adj(std::vector<std::vector<P_ID>> &P_adj);
    void print_PMR(std::vector<std::vector<unsigned>> &PMR_copy);
    void print_totxt_start(vector<R_ID> minMatchID_PMR);
    void input_start(vector<R_ID>& minMatchID_PMR);

private:
    int ThreadNum; // 线程数
    unsigned minMatchNum; //最小匹配的节点数
    vector<P_ID> minMatchIDs; // 模式图出入度最大的点；matchPR_expand中计算
    P_ID minMatchID;      //初始最小匹配节点；从minMatchIDs中的点出发确定路径，最终确定minMatchID
    R_ID maxID;
    Degree *degree_P;
    Degree *degree_R;
    std::vector<int> sel; //每个模式图点的选择度，记录了每个模式图点的在实际图中的匹配数量
    unsigned vertexNum_R;
    unsigned vertexNum_P;
    unsigned edgeNum_R;
    unsigned edgeNum_P;
    std::vector<std::vector<P_ID>> P_adj;    //模式图邻接矩阵，0代表两点之间没有边，1代表两点之间有边且未被访问过，2代表两点之间有边且已被访问过

    unsigned *R_adj;         //邻接表
    unsigned *R_reverse_adj; //逆邻接表
    unsigned *R_adjIndex;    //邻接表from节点在邻接表中的起始位置
    unsigned *R_reverseAdjIndex;
    std::shared_ptr<SynchronizedFile> outputfile_ptr; // sky211116
    std::vector<R_ID> minMatchID_PMR;

    // findSym
    unordered_map<P_ID, int> sym;            //模式图中的等价点组,key为模式图id，value为等价点组编号，value相同的模式图点在同一等价点组中
    unordered_map<int, set<P_ID>> sym_group; // key为等价点组，value为同组所有等价点
    std::vector<std::vector<unsigned>> Equivalent_order; // 二维数组，严格等价点排序数组
    bool isSym; //记录当前模式图是否自同构，如果自同构则需要进行同构检测
    int symNum; //自同构个数 自己挖掘自己
    int circleSize; // 等价环顶点个数

    std::vector<std::vector<P_ID>> extend_oreder; 
    int need_full; 
    bool need_full_no_dup_rem; 
    vector<int> full_index; 
    std::vector<std::vector<P_ID>> antecedent_pid; 
    std::vector<std::vector<std::vector<P_ID>>> successor_pid; 
    std::vector<std::vector<std::vector<P_ID>>> str_successor_pid; 

    vector<P_ID> center_order; 
public:
    bool isEqCircle;
    string startsIdsfilename;


};
#endif // !PMINER_H_

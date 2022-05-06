#ifndef PATTERNMATCHING_H_
#define PATTERNMATCHING_H_

#include <iostream>
#include <vector>
#include <cstring>
#include <limits.h>
#include <unordered_map>
#include <map>
#include <set>
#include <list>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <limits.h>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <tbb/tbb.h>
#include "output.h"

using namespace std;
using namespace tbb;
constexpr unsigned ThreadNum = 5;

typedef unsigned int P_ID;
typedef unsigned int R_ID;
constexpr unsigned int UNSIGNEDMAX = 0xffffffff;
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

class PatternMatching
{
public:
    PatternMatching(){};                                            //构造函数
    virtual ~PatternMatching();                                     //析构函数
    void init(std::string Output_dir);                              //初始化函数
    bool build_degree_R(std::string inputfile, unsigned vertexNum); //得到degree_R
    bool build_R_adj(std::string inputfile);                        //得到用一维数组存储的邻接表和逆邻接表
    bool build_P_adj(std::string inputfile, unsigned vertexNum);    //用邻接矩阵存储模式图
    bool matchPR_expand();
    void searchAllPR();

    vector<unsigned> Max_coverage(std::vector<std::vector<unsigned>> P_adj, std::vector<std::vector<unsigned>> P_split, std::vector<std::vector<unsigned>> include_set);
    void findSym();                                                 //判断P中是否有等价点
    bool isEqResult(vector<R_ID> can_PMR, vector<R_ID> result_PMR); //用来比较两个PMR是否代表同一个结果，用以去重
    void sym_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum);
    void sym_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum);
    void changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<vector<R_ID>> &PMR_new, vector<R_ID> PMR_cur, int times); //转化PMR的格式，转化后row为每组结果，col为P_id对应的R_id
    void changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<R_ID> PMR_new, P_ID currentId);                           //将环的匹配结果输出到结果集
    void reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum);
    void extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum);
    bool check_result(vector<unsigned> PMR_copy);
    bool intersection(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline); //求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空
    bool isNextEPatternEmpty(); //判断剩下边模式是否为空
    bool isNextEPatternEmpty(std::vector<std::vector<P_ID>> &P_adj_copy);
    void FIXLINE(char *s);
    bool isfinish(P_ID id);
    void print_PMR(std::vector<std::vector<unsigned>> &PMR_copy);
    void searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int &result, int num, int ori_centerID, bool isSymMatch, unordered_map<int, set<R_ID>> &sym_PMR, vector<vector<R_ID>> &PMR_new);
    P_ID getMaxSel_cur(P_ID curid);
    void eqCircle_Multithreaded_search(R_ID i);
    void eqCircle_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int ori_centerID, int visited_edgeNum);
    int Multithreaded_search(R_ID i);
    void check_result(std::vector<std::vector<unsigned>> &PMR_copy);
    bool get_Radj_Index(R_ID v_r, int &start, int &end);
    bool PVAllVisited(vector<int> P_visited);
    void Find_Constaint_Inclusion_Vertices();
    void Split_Pattern_Graph();
    void searchSplit(P_ID v, std::vector<unsigned> &cur_line, std::vector<std::vector<unsigned>> &G);
    bool Somplified_GraphMining(P_ID P_start_vec, R_ID R_start_vec, vector<vector<unsigned>> &visit_order,vector<vector<unsigned>> &cur_thread_PMR_edge);
    void extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<P_ID>> &P_max_cov_adj_copy, std::set<R_ID> &set,vector<R_ID> &cur_edge_match);
    void reverse_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<P_ID>> &P_max_cov_adj_copy, std::set<R_ID> &set,vector<R_ID> &cur_edge_match);
    void split_vecAssign(P_ID v_ps, P_ID v_pt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<std::vector<unsigned>> split_order, std::set<R_ID> &set, P_ID originalID, bool isExtend,vector<vector<unsigned>> &visit_order, vector<vector<unsigned>> &cur_thread_PMR_edge,int &visited_edgeIndex,vector<R_ID> &cur_edge_match);
    void extendEdgePattern(std::vector<std::vector<P_ID>> P_adj_copy, P_ID v_ps, P_ID v_pt, R_ID cur_r_vt, std::set<R_ID> &set,vector<R_ID> &cur_edge_match);
    void reverse_extendEdgePattern(std::vector<std::vector<P_ID>> P_adj_copy, P_ID v_ps, P_ID v_pt, R_ID cur_r_vt, std::set<R_ID> &set,vector<R_ID> &cur_edge_match);
    bool intersection_edge(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline,vector<vector<unsigned>> &visit_order, vector<vector<unsigned>> &cur_thread_PMR_edge,P_ID vec); //求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空, 同时更新与求交点相关的所有边
    bool share_point_check(std::vector<unsigned> edge_order, std::vector<unsigned> match_edge, std::vector<unsigned> &true_ans);
    void test();
    bool isEqCircle;                                          //检测模式图是否为等价环
    bool De_duplication(std::vector<unsigned> unchecked_res); //同构体去除
    unsigned full_arrangement(std::vector<vector<R_ID>> cur_thread_PMR); //全排列优化函数
    unsigned full_arrangement(std::vector<std::vector<unsigned>> cur_thread_PMR, std::vector<std::vector<unsigned>> visitE_index); //全排列函数重载，用于边模式组合与结果检验

private:
    unsigned *R_visited;  //标记真实图中结点是否已访问，用于排除中心点,char类型节约存储空间
    unsigned minMatchNum; //最小匹配的节点数
    P_ID minMatchID;      //初始最小匹配节点
    R_ID maxID;
    Degree *degree_P;
    Degree *degree_R;
    P_ID maxSelId;        //在getNextEPattern()中用到，记录每轮选择度最大的顶点id
    std::vector<int> sel; //每个模式图点的选择度，记录了每个模式图点的在实际图中的匹配数量
    unsigned vertexNum_R;
    unsigned vertexNum_P;
    unsigned edgeNum_R;
    unsigned edgeNum_P;
    std::vector<std::vector<P_ID>> P_adj; //模式图邻接矩阵，0代表两点之间没有边，1代表两点之间有边且未被访问过，2代表两点之间有边且已被访问过
    unsigned R_adjSize;                   //大小为edgeNum_R*2+vertexNum_R*2
    std::vector<int> PMR_index;           //每个模式图点在PMR中起始匹配位置
    std::vector<std::vector<unsigned>> PMR;
    std::vector<unsigned> Max_cov_vec; //最终返回值，最大覆盖度子图队列
    std::vector<std::vector<unsigned>> split_order; //剪枝序列
    unsigned *R_adj;                                //邻接表
    unsigned *R_reverse_adj;                        //逆邻接表
    unsigned *R_adjIndex;                           //邻接表from节点在邻接表中的起始位置
    unsigned *R_reverseAdjIndex;
    unordered_map<P_ID, int> sym; //模式图中的等价点组,key为模式图id，value为等价点组编号，value相同的模式图点在同一等价点组中
    concurrent_vector<vector<R_ID>> eqCircleResult;
    std::shared_ptr<SynchronizedFile> outputfile_ptr; //sky211116
    std::vector<R_ID> minMatchID_PMR;
    bool isMinmatchIDSymmetrical;
    unordered_map<int, set<P_ID>> sym_group; //key为等价点组，value为同组所有等价点
    int circleSize;                          //中心等价环大小
    int symNum;                              //等价点总数
    //sky20211208
    std::vector<std::vector<unsigned>> P_include_set;    //约束包含集合Include，存储格式为 [当前结点ID]:(包含结点ID,包含边起点ID,包含边终点ID),(...)
    std::vector<std::vector<unsigned>> Equivalent_order; //sky20220301
};
#endif // !PATTERNMATCHING_H_

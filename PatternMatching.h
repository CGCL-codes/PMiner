#ifndef PATTERNMATCHING_H_
#define PATTERNMATCHING_H_
//#include "concurrentqueue.h"
#include <tbb/tbb.h>
#include <iostream>
//#include "HashID.h"
//#include "ThreadPool.h"
#include <vector>
#include <cstring>
#include <limits.h>
//#include <windows.h>
#include <unordered_map>
#include <map>
#include <list>
//#include "BuildAdjGraph.h"
//#include "Bitmap.h"
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <limits.h>
#include <unordered_set>
#include <unordered_map>
#include <queue>
//#include <queue>
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
constexpr unsigned ThreadNum = 5;

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

class PatternMatching
{
public:
    
    PatternMatching(){};                                            //待补充，构造函数
    virtual ~PatternMatching();                                     //待补充，析构函数
    void init(std::string Output_dir);                              // sky211116
    bool build_degree_R(std::string inputfile, unsigned vertexNum); //得到degree_R
    bool build_R_adj(std::string inputfile);                        //得到用一维数组存储的邻接表和逆邻接表
    bool build_P_adj(std::string inputfile, unsigned vertexNum);    //用邻接矩阵存储模式图
    // bool matchPR();                            //计算P中各节点在G中的匹配集并返回
    // 211018sky需要修改为逐步拓展式
    bool matchPR_expand();
    // void searchPR();                           //以模式图p中的s点为起点，Vr,s为中心点开始搜索
    void findSym(); //判断P中是否有等价点
    void searchAllPR();
    void Find_Constaint_Inclusion_Vertices();
    // void reverse_extendEdgePattern();
    P_ID getMaxSel(); //获取选择度最大的模式图点
    // bool get_Mvp_Index(P_ID v_p, R_ID& start, R_ID& end);              //得到M(v_p)的索引起止范围
    std::vector<R_ID> reverse_getMV(P_ID &v_ps, P_ID &v_pt, R_ID &v_rt); //得到图G上以vrt为终点，与边模式vps, vpt相匹配的端点集合，即以vrt为终点点与vps相匹配的顶点集
    // void extendEdgePattern(P_ID v_ps, P_ID v_pt);             //逐步扩展边进行匹配

    bool extendEPatternwithsource(P_ID v_ps, P_ID v_pt, R_ID v_rs);
    bool extendEPatternwithtarget(P_ID v_ps, P_ID v_pt, R_ID V_rt);
    bool PVAllVisited(vector<int> P_visited);
    void sym_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed);
    void reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed);
    void eqCircle_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,R_ID ori_centerId);
    void eqCircle_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,R_ID ori_centerId);
    void sym_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed);
    void extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed);
    // P_ID getNextEPattern(P_ID& v_ps);                            //返回未被访问的匹配顶点数最少的分支边终点ID

    std::vector<R_ID> getMV(P_ID &v_ps, P_ID &v_pt, R_ID &v_rs); //得到图G上以v_rs为源，与边模式(v_ps, v_pt)相匹配的顶点集
    // bool intersection(std::vector<R_ID>& Mtemp, R_ID& start, R_ID& end);    //求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空
    bool intersection(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline);                                    //求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空
    void changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<R_ID> PMR_new, P_ID currentId);                           //将环的匹配结果输出到结果集
    void changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<vector<R_ID>> &PMR_new, vector<R_ID> PMR_cur, int times); //转化PMR的格式，转化后row为每组结果，col为P_id对应的R_id
    bool updataPMR(P_ID &v_p1, P_ID &v_p2);                                                                              // M(v_p)的更新函数，输入v_p1和v_p2，用v_p1去更新v_p2，重载了函数PatternMatching::intersection
    // void searchSplits(P_ID &v_ps);                                        //遍历模式图中结点v_ps的后继，即该点的各分支
    // void MergeSplit(P_ID &v_ps);                                          //合并中间结果，缩小中间结果集
    bool isNextEPatternEmpty(); //判断剩下边模式是否为空
    bool isNextEPatternEmpty(std::vector<std::vector<P_ID>> &P_adj_copy);
    // P_ID getNextEPattern(P_ID& v_ps);                            //返回未被访问的匹配顶点数最少的分支边终点ID
    // std::vector<R_ID> getMV(P_ID& v_ps, P_ID& v_pt, R_ID& v_rs); //得到图G上以v_rs为源，与边模式(v_ps, v_pt)相匹配的顶点集
    void FIXLINE(char *s);
    // void output(std::string Dir); //输出函数，将筛选出的结果集进行组合后以文本格式输出
    bool isEqResult(vector<R_ID> can_PMR, vector<R_ID> result_PMR);                                                                //用来比较两个PMR是否代表同一个结果，用以去重
    void recoverPattern(std::vector<std::vector<P_ID>> &PMR_ori, std::vector<std::vector<P_ID>> &PMR_tmp);                         //对匹配的结果进行补全
    void recoverGroup(std::vector<std::vector<P_ID>> &PMR_ori, std::unordered_map<int, std::vector<int>> symResult, int groupNum); //补全等价点组号为groupNum的等价点结果
    bool isPMRComplete(std::vector<std::vector<P_ID>> PMR_tmp);
    void deleteSymVertex(std::vector<std::vector<P_ID>> &symP_adj, P_ID symId);//删除除了symId外的其他等价分支
    void fullArray(int &resultNum, vector<R_ID> cur_PMR, int cur_time, unordered_map<int, set<R_ID>> sym_PMR); //全排列，输出结果
    bool isfinish(P_ID id);
   void minDFS(vector<R_ID> PMR_copy,R_ID ori_centerId,long long& result,int curTime,unordered_map<R_ID,int> mp);
   void maxDFS(vector<R_ID> PMR_copy,R_ID ori_centerId,long long& result,int curTime,unordered_map<R_ID,int> mp);
    void print_PMR();
    bool isRecov(vector<R_ID> a, vector<R_ID> b); //判断分支a和分支b是否有可能重组模式
    void eqCircle_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int ori_centerID, int visited_edgeNum);
    void eqCircle_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int ori_centerID, int visited_edgeNum,int& result);
    void searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, long long &result, int num, int ori_centerID, bool isSymMatch, unordered_map<int, set<R_ID>> &sym_PMR, vector<vector<R_ID>> &PMR_new,unordered_map<R_ID,int> isTraversed);
    P_ID getMaxSel_cur(P_ID curid);
    long long eqCircle_Multithreaded_search(R_ID i);
    long long Multithreaded_search(R_ID i, vector<vector<P_ID>> &P_adj_copy);
    bool check_result(std::vector<std::vector<unsigned>> &PMR_copy); // sky211118
    bool check_result(vector<unsigned> PMR_copy);
    bool get_Radj_Index(R_ID v_r, int &start, int &end); // sky211118

private:
    // std::vector<std::vector<int>> neighbor_p;//存储模式图中各节点在图上的邻居集合
    unsigned *R_visited;  //标记真实图中结点是否已访问，用于排除中心点,char类型节约存储空间
    unsigned minMatchNum; //最小匹配的节点数
    vector<block*> global_index;
    P_ID minMatchID;      //初始最小匹配节点
    R_ID maxID;
    Degree *degree_P;
    Degree *degree_R;
    bool isEqCircle;
    P_ID maxSelId;        //在getNextEPattern()中用到，记录每轮选择度最大的顶点id
    std::vector<int> sel; //每个模式图点的选择度，记录了每个模式图点的在实际图中的匹配数量
    unsigned vertexNum_R;
    unsigned vertexNum_P;
    unsigned edgeNum_R;
    unsigned edgeNum_P;
    int brachNum;//分支基数
    unordered_map<P_ID,bool> brachConstraint;//分支间约束，true代表相等关系，false代表不等关系
    bool allEqVertex;//是否全部是等价点
    // std::vector<std::vector<P_edge>> P_adj; //模式图的邻接表
    std::vector<std::vector<P_ID>> P_adj;    //模式图邻接矩阵，0代表两点之间没有边，1代表两点之间有边且未被访问过，2代表两点之间有边且已被访问过
    unordered_map<P_ID, int> sym;            //模式图中的等价点组,key为模式图id，value为等价点组编号，value相同的模式图点在同一等价点组中
    unordered_map<int, set<P_ID>> sym_group; // key为等价点组，value为同组所有等价点
    int symNum;                              //等价点总数
    vector<P_ID> restPid;                    //模式图分割后的模式图节点
    int circleSize;                          //中心等价环大小
    unsigned R_adjSize;                      //大小为edgeNum_R*2+vertexNum_R*2
    std::vector<int> PMR_index;              //每个模式图点在PMR中起始匹配位置
    // set <vector<int>> eqCircleSet;//用于去重等价环的匹配结果
    // unsigned* PMR;//用一维数组存储M(Vp)
    //使用从小到大的PMR拓展过程20211008
    std::vector<std::vector<unsigned>> PMR;
    vector<P_ID> asym; //存储除起始点外的非等价点
    // std::vector<vector<unsigned>> PMR; //二维指针数组存储PMR

    unsigned *R_adj;         //邻接表
    unsigned *R_reverse_adj; //逆邻接表
    unsigned *R_adjIndex;    //邻接表from节点在邻接表中的起始位置
    unsigned *R_reverseAdjIndex;
    // unsigned total_num;//存储M(Vp)的总的个数，在get_Mvp_Index中会用到
    concurrent_vector<vector<R_ID>> eqCircleResult;
    concurrent_vector<vector<R_ID>> minEqCircleResult;
    concurrent_vector<vector<R_ID>> maxEqCircleResult;
    // bool branchFinish ;//当前分支完整指标，非完整分支直接抛弃，完整分支可以进行挖掘
    // std::atomic<int> Mining_result_count;
    std::atomic<int> control_flag; //值为0时代表对eqCircleResult正在进行访问，值为1时代表对eqCircleResult进行插入
    // int Mining_result_count;
    std::shared_ptr<SynchronizedFile> outputfile_ptr; // sky211116
    std::vector<R_ID> minMatchID_PMR;
    bool isMinmatchIDSymmetrical;
};


#endif // !PATTERNMATCHING_H_

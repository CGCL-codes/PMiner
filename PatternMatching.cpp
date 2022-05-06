#include "PatternMatching.h"
#include <tbb/tbb.h>
#include <mutex>
#include <ctime>
#include <time.h>
#include <queue>
#include <map>
using namespace tbb;
using namespace std;
#pragma warning(disable : 4996)

PatternMatching::~PatternMatching()
{
    if (R_visited)
    {
        free(R_visited);
        R_visited = nullptr;
    }
    if (degree_R)
    {
        free(degree_R);
        degree_R = nullptr;
    }

    if (R_adj)
    {
        free(R_adj);
        R_adj = nullptr;
    }
    if (R_reverse_adj)
    {
        free(R_reverse_adj);
        R_reverse_adj = nullptr;
    }
    if (R_adjIndex)
    {
        free(R_adjIndex);
        R_adjIndex = nullptr;
    }
    if (R_reverseAdjIndex)
    {
        free(R_reverseAdjIndex);
        R_reverseAdjIndex = nullptr;
    }
}

void PatternMatching::init(std::string Output_dir)
{
    outputfile_ptr = std::make_shared<SynchronizedFile>(Output_dir);
    R_visited = (unsigned *)malloc(vertexNum_R * sizeof(unsigned));
    memset(R_visited, 0, vertexNum_R * sizeof(unsigned));
}

bool PatternMatching::build_degree_R(std::string inputfile, unsigned vertexNum) //创建degree_R
{
    maxID = 0;     //图中最大id
    edgeNum_R = 0; //图的边数
    vertexNum_R = vertexNum;
    degree_R = (Degree *)calloc(vertexNum_R, sizeof(Degree));
    FILE *inf = fopen(inputfile.c_str(), "r");
    if (inf == NULL)
    {
        std::cerr << "Could not load :" << inputfile << " error: " << strerror(errno)
                  << std::endl;
    }
    assert(inf != NULL);
    std::cout << "Reading in adjacency list format!" << std::endl;
    int maxlen = 100000000;
    char *s = (char *)malloc(maxlen); //暂时存放一次读入的数据
    char delims[] = " \t";            //字符串的分隔符（tab键）
    size_t linenum = 0;               //行数
    unsigned from = 0, to = 0;        //起点id  //终点id
    while (fgets(s, maxlen, inf) != NULL)
    {
        linenum++;
        FIXLINE(s);                  //将换行符变成'\0'
        char *t = strtok(s, delims); //返回的是tab之前的那一段字符串的首地址
        from = atoi(t);              //提取起点id
        unsigned num = 0;            //出度个数
        while ((t = strtok(NULL, delims)) != NULL)
        {
            to = atoi(t);
            if (from != to)
            {
                maxID = max(to, maxID);  //找出整张图中的最大id
                degree_R[from].outdeg++; //出度
                degree_R[to].indeg++;    //入度
                num++;
            }
        }
        edgeNum_R += num; //统计所有from的总的出度个数 == 总的边数
    }
    std::cout << "edgeNum_R:" << edgeNum_R << std::endl;
    std::cout << "finish first read R_adj... maxID:" << maxID << std::endl;
    free(s);
    fclose(inf);
    return true;
}
void PatternMatching::FIXLINE(char *s)
{
    int len = (int)strlen(s) - 1;
    if (s[len] == '\n')
        s[len] = 0;
}
bool PatternMatching::build_R_adj(std::string inputfile) //创建邻接表R_adj和逆邻接表R_reverse_adj，测试成功
{
    R_adj = new unsigned[edgeNum_R];
    R_reverse_adj = new unsigned[edgeNum_R];
    R_adjIndex = new unsigned[vertexNum_R];
    R_reverseAdjIndex = new unsigned[vertexNum_R];
    unsigned *R_reverseAdjIndex_tail = new unsigned[vertexNum_R];
    memset(R_reverseAdjIndex_tail, 0, vertexNum_R);
    R_adjIndex[0] = 0;
    R_reverseAdjIndex[0] = 0;
    R_reverseAdjIndex_tail[0] = 0;
    unsigned tail = 0;
    FILE *inf = fopen(inputfile.c_str(), "r");
    if (inf == NULL)
    {
        std::cerr << "Could not load :" << inputfile << " error: " << strerror(errno)
                  << std::endl;
    }
    assert(inf != NULL);
    std::cout << "Reading in adjacency list format!" << std::endl;
    int maxlen = 100000000;
    char *s = (char *)malloc(maxlen); //暂时存放一次读入的数据
    char delims[] = " \t";            //字符串的分隔符（tab键）
    size_t linenum = 0;               //行数
    unsigned from = 0, to = 0;        //起点id  //终点id
    for (unsigned i = 1; i < vertexNum_R; i++)
    {
        R_adjIndex[i] = R_adjIndex[i - 1] + degree_R[i - 1].outdeg;
        R_reverseAdjIndex[i] = R_reverseAdjIndex[i - 1] + degree_R[i - 1].indeg;
        R_reverseAdjIndex_tail[i] = R_reverseAdjIndex[i];
    }

    while (fgets(s, maxlen, inf) != NULL)
    {
        linenum++;
        FIXLINE(s);                  //将换行符变成'\0'
        char *t = strtok(s, delims); //返回的是tab之前的那一段字符串的首地址
        from = atoi(t);              //提取起点id
        while ((t = strtok(NULL, delims)) != NULL)
        {
            to = atoi(t);
            if (from != to)
            {
                R_adj[tail++] = to;
                R_reverse_adj[R_reverseAdjIndex_tail[to]++] = from;
            }
        }
    }
    std::cout << "finish second read R_adj..." << std::endl;
    //两个点之间是否存在重复边的检测
    for (int i = 0; i < vertexNum_R; i++)
    {
        if (degree_R[i].outdeg < 2)
            continue;
        for (int j = 1; j < degree_R[i].outdeg; j++)
        {
            if (R_adj[R_adjIndex[i] + j] == R_adj[R_adjIndex[i] + j - 1])
                std::cout << "exist same edge" << endl;
        }
    }

    free(s);
    free(R_reverseAdjIndex_tail);
    fclose(inf);
    return true;
}

bool PatternMatching::build_P_adj(std::string inputfile, unsigned vertexNum)
{
    vertexNum_P = vertexNum;
    edgeNum_P = 0;
    degree_P = (Degree *)calloc(vertexNum_P, sizeof(Degree));
    std::vector<std::vector<P_ID>> tmp(vertexNum_P, std::vector<P_ID>(vertexNum_P)); //临时存储
    FILE *inf = fopen(inputfile.c_str(), "r");
    if (inf == NULL)
    {
        std::cerr << "Could not load :" << inputfile << " error: " << strerror(errno)
                  << std::endl;
    }
    assert(inf != NULL);
    std::cout << "Reading in adjacency list format!" << std::endl;
    int maxlen = 100000000;
    char *s = (char *)malloc(maxlen); //暂时存放一次读入的数据
    char delims[] = " \t";            //字符串的分隔符（tab键）
    size_t linenum = 0;               //行数
    unsigned from = 0, to = 0;        //起点id  //终点id
    while (fgets(s, maxlen, inf) != NULL)
    {
        linenum++;
        FIXLINE(s);                  //将换行符变成'\0'
        char *t = strtok(s, delims); //返回的是tab之前的那一段字符串的首地址
        from = atoi(t);              //提取起点id
        while ((t = strtok(NULL, delims)) != NULL)
        {
            to = atoi(t);
            if (from != to)
            {
                degree_P[from].outdeg++; //出度
                degree_P[to].indeg++;    //入度
                tmp[from][to] = 1;
                edgeNum_P++;
            }
        }
    }
    P_adj = tmp;
    std::cout << "finish read P_adj..."
              << "|E| = " << edgeNum_P << std::endl;
    free(s);
    fclose(inf);
    return true;
}

bool PatternMatching::isNextEPatternEmpty()
{
    for (unsigned i = 0; i < P_adj.size(); i++)
    {
        for (unsigned j = 0; j < P_adj[i].size(); j++)
        {
            if (P_adj[i][j] == 1)
                return false;
        }
    }
    return true;
}

bool PatternMatching::isNextEPatternEmpty(std::vector<std::vector<P_ID>> &P_adj_copy)
{
    for (unsigned i = 0; i < vertexNum_P; i++)
    {
        for (unsigned j = i + 1; j < vertexNum_P; j++)
        {
            if (P_adj_copy[i][j] == 1 || P_adj_copy[j][i] == 1)
                return false;
        }
    }
    return true;
}

bool PatternMatching::isfinish(P_ID id)
{
    for (P_ID i = 0; i < vertexNum_P; i++)
    {
        if (P_adj[id][i] == 1)
            return false;
    }
    for (P_ID i = 0; i < vertexNum_P; i++)
    {
        if (P_adj[i][id] == 1)
            return false;
    }
    return true;
}

void PatternMatching::searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int &result, int visited_edgeNum, int ori_centerID, bool isSymMatch, unordered_map<int, set<R_ID>> &sym_PMR, vector<vector<R_ID>> &PMR_new)
{
    //对于中心点Initialize模式图每顶点的选择度Sel;除开vp,s之外，其它模式图顶点选择度为无穷大
    if (current_match_RID == ori_centerID)
    {
        for (int i = 0; i < vertexNum_P; ++i)
        {
            if (i != current_match_PID)
            {
                sel_copy[i] = INT_MAX;
            }
        }
    }
    //当前选择的中心点其PMR集合应当只包含一个值，这里来对其进行初始化
    /*PMR_copy[current_match_PID].resize(1);
    PMR_copy[current_match_PID][0] = current_match_RID;*/
    //相比上面的方法，有更快的算法，如下，使用交换的方式
    std::vector<unsigned> temp;
    temp.emplace_back(current_match_RID);
    PMR_copy[current_match_PID].swap(temp);
    sel_copy[current_match_PID] = 1;

    //开始图匹配过程
    while (!isNextEPatternEmpty(P_adj_copy) /* visited_edgeNum<edgeNum_P */)
    {
        //下面我们开始寻找从current_match_PID出发的所有未访问边，并对每条边做extend操作
        P_ID neighborID = UINT_MAX; //由这两个点构成最小匹配的边模式
        bool isReverse = true;
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_adj_copy[i][current_match_PID] == 1)
            {
                neighborID = i;
                isReverse = true;

                if (!isSymMatch)
                    reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                else
                    sym_reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                if (branchFinish == false)
                {
                    return;
                }
            }
        }

        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_adj_copy[current_match_PID][i] == 1)
            {
                neighborID = i;
                isReverse = false;
                if (!isSymMatch)
                    extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                else
                    sym_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                if (branchFinish == false)
                {
                    return;
                }
            }
        }

        //如果上面的找边结束后neighborID为初始值(9294967296是我们支持的最大结点数)，那么证明current_match_PID的所有边都访问结束，需要重新计算current_match_PID，递归开始
        for (P_ID i = 0; i < vertexNum_P; ++i)
        {
            for (P_ID j = i + 1; j < vertexNum_P; ++j)
            {
                if ((P_adj_copy[i][j] == 1 || P_adj_copy[j][i] == 1) && (sel_copy[i] != INT_MAX || sel_copy[j] != INT_MAX))
                {
                    if (sel_copy[i] <= sel_copy[j])
                    {
                        current_match_PID = i;
                        neighborID = j;
                    }
                    else
                    {
                        current_match_PID = j;
                        neighborID = i;
                    }
                    //从这里开始非中心点的递归过程，也就是中心点所拓展的边已经全部匹配完成
                    for (auto match_RID : PMR_copy[current_match_PID])
                    {
                        searchPG(PMR_copy, sel_copy, P_adj_copy, match_RID, current_match_PID, branchFinish, result, visited_edgeNum, ori_centerID, isSymMatch, sym_PMR, PMR_new);
                    }
                    branchFinish = false;
                }
            }
        }
    }
    
    if (branchFinish == false)
    {
        return;
    }

    unsigned cur_count = 1;
    for (unsigned i = 0; i < vertexNum_P; ++i)
    {
        cur_count *= PMR_copy[i].size();
    }
    result += cur_count;

    return;
}

bool PatternMatching::matchPR_expand()
{
    sel.resize(vertexNum_P); //初始化选择度
    minMatchID = 0;          //具有最小匹配数的模式图节点ID
    minMatchNum = UINT_MAX;  //最小匹配数
    unsigned current_size = 0;
    unsigned total_size = 0; //存储M(Vp)所需的空间大小
    
    for (unsigned i = 0; i < vertexNum_P; i++) //计算与每个模式图点匹配的实际图中节点数量
    {
        for (R_ID j = 0; j < vertexNum_R; j++)
        {
            if (degree_R[j].indeg >= degree_P[i].indeg && degree_R[j].outdeg >= degree_P[i].outdeg)
            {
                current_size++;
            }
        }
        sel[i] = current_size;
        PMR_index.push_back(current_size);
        total_size += current_size;

        if (current_size < minMatchNum)
        {
            minMatchNum = current_size;
            minMatchID = i;
        }
        current_size = 0;
    }
    PMR.resize(vertexNum_P);
    //选取非等价点中总度数最大的为起始点
    if (sym.count(minMatchID) != 0)
    {
        P_ID cur_minMatchID = minMatchID;
        int cur_degree = 0;
        for (int i = 0; i < vertexNum_P; i++)
        {
            if (sym.count(i) == 0 && degree_P[i].indeg + degree_P[i].outdeg > cur_degree)
            {
                minMatchID = i;
                cur_degree = degree_P[i].indeg + degree_P[i].outdeg;
            }
        }
    }
    std::cout << "minMatchID = " << minMatchID << std::endl;
    return true;
}

//定义模式图中寻找包含约束点的函数，使用while循环保证全部包含关系搜寻完成
//增加了split_order私有变量的更新过程
void PatternMatching::Find_Constaint_Inclusion_Vertices()
{
    std::vector<std::vector<unsigned>> include_set;
    include_set.assign(vertexNum_P, {});

    //判断距离为1的包含关系
    for (unsigned v1 = 0; v1 < vertexNum_P; ++v1)
    {
        for (unsigned v2 = 0; v2 < vertexNum_P; ++v2)
        {
            if (v1 == v2)
            {
                continue;
            }

            //存储v1和v2两个节点的所有正向和逆向边邻居节点集合
            std::vector<unsigned> v1_neighbor;
            std::vector<unsigned> v2_neighbor;
            std::vector<unsigned> v1_neighbor_r;
            std::vector<unsigned> v2_neighbor_r;
            for (unsigned i = 0; i < vertexNum_P; ++i)
            {
                if (P_adj[v1][i] != 0)
                {
                    v1_neighbor.emplace_back(i);
                }
                if (P_adj[v2][i] != 0)
                {
                    v2_neighbor.emplace_back(i);
                }
                if (P_adj[i][v1] != 0)
                {
                    v1_neighbor_r.emplace_back(i);
                }
                if (P_adj[i][v2] != 0)
                {
                    v2_neighbor_r.emplace_back(i);
                }
            }

            //分别对正向邻居集合和逆向邻居集合内部求交集，判断是否有共源点或共终点
            bool sec = intersection(v1_neighbor, v2_neighbor);
            bool sec_r = intersection(v1_neighbor_r, v2_neighbor_r);

            //如果v1和v2存在共同终点
            if (!sec)
            {
                if (degree_P[v1].indeg + degree_P[v1].outdeg <= degree_P[v2].indeg + degree_P[v2].outdeg)
                {
                    for (auto u : v2_neighbor)
                    { //这里由于intersection函数复用的原因，交集被更新到了v2_neighbor中
                        //插入约束边ec1,sky211221
                        include_set[v1].emplace_back(v1);
                        include_set[v1].emplace_back(u);
                        //插入节点
                        include_set[v1].emplace_back(v2);
                        //插入约束边ec2
                        include_set[v1].emplace_back(v2);
                        include_set[v1].emplace_back(u);
                    }
                }
            }

            //如果v1和v2存在共同源点
            if (!sec_r)
            {
                if (degree_P[v1].indeg + degree_P[v1].outdeg <= degree_P[v2].indeg + degree_P[v2].outdeg)
                {
                    for (auto u : v2_neighbor_r)
                    { //这里由于intersection函数复用的原因，交集被更新到了v2_neighbor_r中
                        //插入约束边ec1,sky211221
                        include_set[v1].emplace_back(u);
                        include_set[v1].emplace_back(v1);
                        //插入节点
                        include_set[v1].emplace_back(v2);
                        //插入约束边ec2
                        include_set[v1].emplace_back(u);
                        include_set[v1].emplace_back(v2);
                    }
                }
            }
        }
    }

    //结果输出有重复值，本段进行去重，使用set来去重
    for (unsigned i = 0; i < vertexNum_P; ++i)
    {
        std::vector<unsigned> no_rep_line;
        std::set<string> set_temp;
        for (unsigned j = 0; j < include_set[i].size(); j += 5)
        {
            //读入五个值（一条边加一个点加一条边）转换为字符串求和
            string str = to_string(include_set[i][j]) + to_string(include_set[i][j + 1]) + to_string(include_set[i][j + 2]) + to_string(include_set[i][j + 3]) + to_string(include_set[i][j + 4]);
            auto insert_res = set_temp.insert(str);
            //如果插入字符串非重复值，则更新no_rep_line
            if (insert_res.second == true)
            {
                no_rep_line.push_back(include_set[i][j]);
                no_rep_line.push_back(include_set[i][j + 1]);
                no_rep_line.push_back(include_set[i][j + 2]);
                no_rep_line.push_back(include_set[i][j + 3]);
                no_rep_line.push_back(include_set[i][j + 4]);
            }
        }
        //当前行处理完毕后，使用no_rep_line来更新include_set[i]
        include_set[i].swap(no_rep_line);
    }

    //这里输出include_set来测试结果是否正确
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "include_set is:" << std::endl;
    for (unsigned i = 0; i < vertexNum_P; ++i)
    {
        std::cout << i << ": ";
        int VE_flag = 0; //点边标志位
        for (auto j : include_set[i])
        {
            if (VE_flag == 0 || VE_flag == 3)
            {
                std::cout << "(" << j << " ";
                VE_flag++;
            }
            else if (VE_flag == 1)
            {
                std::cout << j << ") ";
                VE_flag++;
            }
            else if (VE_flag == 4)
            {
                std::cout << j << ") ";
                VE_flag = 0;
            }
            else
            {
                std::cout << j << " ";
                VE_flag++;
            }
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;
    // 用include_set对P_include_set进行初始化
    P_include_set = include_set;
}

// 求解并返回单个结点的覆盖度，使用内联函数以提升效率
inline unsigned vec_coverage(unsigned vec, std::vector<std::vector<unsigned>> include_set, std::set<unsigned> &inc_vec_set)
{
    unsigned coverage = 1; //初始化覆盖度，由于单个结点至少覆盖自身，所以初值为1
    if (inc_vec_set.count(vec))
    {
        coverage = 0;
    }
    else
    {
        inc_vec_set.insert(vec);
    }

    for (unsigned i = 3; i < include_set[vec].size(); i += 5)
    {
        if (inc_vec_set.count(include_set[vec][i]))
        {
            continue;
        }
        else
        {
            inc_vec_set.insert(include_set[vec][i]);
            coverage++;
        }
    }
    return coverage;
}

//用于在分割后的模式图中计算并找出最大覆盖度联通子图
std::vector<unsigned> PatternMatching::Max_coverage(std::vector<std::vector<unsigned>> P_adj, std::vector<std::vector<unsigned>> P_split, std::vector<std::vector<unsigned>> include_set)
{
    std::vector<std::vector<unsigned>> fir_Econtrol(P_adj);
    //依次读取P_split中的分割边，并在fir_Econtrol中标记为不可访问
    for (unsigned i = 0; i < P_split.size(); ++i)
    {
        int VE_flag = 0; //点边标志位
        unsigned fir, sec;
        for (unsigned j = 1; j < P_split[i].size(); ++j)
        {
            if (VE_flag == 1)
            {
                fir = P_split[i][j];
                VE_flag++;
            }
            else if (VE_flag == 2)
            {
                sec = P_split[i][j];
                //此处已找到一条边，标记为不可访问
                fir_Econtrol[fir][sec] = 2;
                VE_flag = 0;
            }
            else
            {
                VE_flag++;
            }
        }
    }

    //使用fir_Econtrol来初始化sec_Econtrol，实质上两者互补构成一个完整模式图
    std::vector<std::vector<unsigned>> sec_Econtrol(fir_Econtrol);
    for (unsigned i = 0; i < P_adj.size(); ++i)
    {
        for (unsigned j = 0; j < P_adj.size(); ++j)
        {
            if (sec_Econtrol[i][j] == 2)
            {
                sec_Econtrol[i][j] = 1;
            }
            else if (sec_Econtrol[i][j] == 1)
            {
                sec_Econtrol[i][j] = 2;
            }
            else
            {
                continue;
            }
        }
    }

    //在fir_Econtrol和sec_Econtrol中分别遍历寻找最大覆盖度联通子图
    //在sec_Econtrol中遍历寻找最大覆盖度联通子图
    unsigned Max_coverage = 0;
    std::vector<unsigned> visited;
    visited.assign(P_adj.size(), 0);
    for (unsigned startID = 0; startID < P_adj.size(); ++startID)
    {
        if (visited[startID] == 1)
        {
            continue;
        }

        std::set<unsigned> sec_inc_vec_set{}; //用于去重
        visited[startID] = 1;
        std::vector<unsigned> cur_cov_vec{}; //当前起点所在联通子图
        unsigned cur_coverage = vec_coverage(startID, include_set, sec_inc_vec_set); //初始化当前覆盖度
        std::queue<unsigned> v_queue{};
        v_queue.push(startID);

        //使用队列进行广度优先遍历
        while (v_queue.size() > 0)
        {
            unsigned cur = v_queue.front();
            v_queue.pop();
            //找到当前结点的所有邻居
            for (unsigned i = 0; i < P_adj.size(); ++i)
            {
                if (visited[i] == 0 && sec_Econtrol[cur][i] == 1)
                {
                    v_queue.push(i);
                    cur_cov_vec.push_back(cur);
                    cur_cov_vec.push_back(i);
                    cur_coverage += vec_coverage(i, include_set, sec_inc_vec_set); 
                    visited[i] = 1;
                }
                if (visited[i] == 0 && sec_Econtrol[i][cur] == 1)
                {
                    v_queue.push(i);
                    cur_cov_vec.push_back(i);
                    cur_cov_vec.push_back(cur);
                    cur_coverage += vec_coverage(i, include_set, sec_inc_vec_set); 
                    visited[i] = 1;
                }
            }
        }

        // 100%覆盖度直接返回当前路径
        if (cur_coverage >= P_adj.size())
        {
            return cur_cov_vec;
        }
        //非100%覆盖则找最大
        if (cur_coverage > Max_coverage)
        {
            Max_coverage = cur_coverage;
            Max_cov_vec.swap(cur_cov_vec);
        }
    }

    //在fir_Econtrol中遍历寻找最大覆盖度联通子图
    visited.assign(P_adj.size(), 0);
    for (unsigned startID = 0; startID < P_adj.size(); ++startID)
    {
        if (visited[startID] == 1)
        {
            continue;
        }

        std::set<unsigned> fir_inc_vec_set{}; //用于去重
        visited[startID] = 1;
        std::vector<unsigned> cur_cov_vec{}; //当前起点所在联通子图
        unsigned cur_coverage = vec_coverage(startID, include_set, fir_inc_vec_set); //初始化当前覆盖度
        std::queue<unsigned> v_queue{};
        v_queue.push(startID);

        //使用队列进行广度优先遍历
        while (v_queue.size() > 0)
        {
            unsigned cur = v_queue.front();
            v_queue.pop();
            //找到当前结点的所有邻居
            for (unsigned i = 0; i < P_adj.size(); ++i)
            {
                if (visited[i] == 0 && fir_Econtrol[cur][i] == 1)
                {
                    v_queue.push(i);
                    cur_cov_vec.push_back(cur);
                    cur_cov_vec.push_back(i);
                    cur_coverage += vec_coverage(i, include_set, fir_inc_vec_set); // FIXME
                    visited[i] = 1;
                }
                if (visited[i] == 0 && fir_Econtrol[i][cur] == 1)
                {
                    v_queue.push(i);
                    cur_cov_vec.push_back(i);
                    cur_cov_vec.push_back(cur);
                    cur_coverage += vec_coverage(i, include_set, fir_inc_vec_set); // FIXME
                    visited[i] = 1;
                }
            }
        }

        // 100%覆盖度直接返回当前路径
        if (cur_coverage >= P_adj.size())
        {
            return cur_cov_vec;
        }
        //非100%覆盖则找最大
        if (cur_coverage > Max_coverage)
        {
            Max_coverage = cur_coverage;
            Max_cov_vec.swap(cur_cov_vec);
        }
    }

    return Max_cov_vec;
}

void PatternMatching::sym_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum)
{

    //判断两点间是否存在正向边，有则标记并进行正向扩展
    if (P_adj[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have edge." << std::endl;
        return;
    }
    else
    {
        visited_edgeNum++;
        P_adj[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }
    //这里判断当前分支是否完整可以继续挖掘下去
    if (branchFinish == false)
        return;
    std::vector<R_ID> Mtemp;
    for (int i = 0; i < vertexNum_P; i++)
    {
        if (P_adj[cur_r_vs][i] != 0 && degree_P[i].indeg == degree_P[v_pt].indeg && degree_P[i].outdeg == degree_P[v_pt].outdeg)
        {
            Mtemp.push_back(i);
        }
    }
    //当求得的点尚未计算PMR集，直接更新PMR集和sel集
    if (PMR_copy[v_pt].size() == 0)
    {
        PMR_copy[v_pt].swap(Mtemp);
        sel_copy[v_pt] = PMR_copy[v_pt].size();
    }
    //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
    else
    {
        intersection(Mtemp, PMR_copy[v_pt]);
        sel_copy[v_pt] = PMR_copy[v_pt].size();
    }
    if (sel_copy[v_pt] == 0)
    {
        branchFinish = false;
    }
    return;
}

//用于对模式图进行分割，简化挖掘过程
void PatternMatching::Split_Pattern_Graph()
{
    std::vector<std::vector<unsigned>> G = P_adj;
    P_ID v = minMatchID;
    unsigned i = 0, j = 0;
    std::vector<std::vector<unsigned>> P_split; //第一列存储P[i].start，后续列存储P[i].split
    P_split.assign(vertexNum_P, {});
    P_split[0].push_back(v);
    searchSplit(v, P_split[0], G);
    //建议对照模式图分解算法2的伪代码进行分析，不然较难理解本段
    while (i <= j)
    {
        for (int wi = 1; wi < P_split[i].size(); wi += 3)
        {
            unsigned w = P_split[i][wi];
            for (int xi = 2; xi < P_include_set[w].size(); xi += 5)
            {
                unsigned x = P_include_set[w][xi];
                if (++j >= vertexNum_P)
                {
                    break;
                }
                P_split[j].push_back(x);
                searchSplit(x, P_split[j], G);
            }
            ++i;
            if (i >= vertexNum_P)
            {
                break;
            }
        }
        ++i;
        if (i >= vertexNum_P)
        {
            break;
        }
    }
    //输出结果来测试函数运行是否正确
    std::cout << "P_split is :" << std::endl;
    std::cout << "P.start\tP.split" << std::endl;
    for (unsigned pi = 0; pi < P_split.size(); ++pi)
    {
        if (P_split[pi].size() <= 1)
        {
            continue;
        }
        std::cout << P_split[pi][0] << ":\t";
        unsigned VE_flag = 0; //点边标志位
        for (unsigned pj = 1; pj < P_split[pi].size(); ++pj)
        {
            if (VE_flag == 0)
            {
                std::cout << P_split[pi][pj] << " ";
                VE_flag++;
            }
            else if (VE_flag == 1)
            {
                std::cout << "(" << P_split[pi][pj] << " ";
                VE_flag++;
            }
            else if (VE_flag == 2)
            {
                std::cout << P_split[pi][pj] << ") ";
                VE_flag = 0;
            }
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;
    //在分割好的模式图中寻找最大覆盖度联通子图，输出其边集合
    Max_cov_vec = Max_coverage(P_adj, P_split, P_include_set);
    std::cout << "Max coverage edge set is:" << std::endl;
    int E_flag = 0;
    for (auto vec : Max_cov_vec)
    {
        if (E_flag == 0)
        {
            std::cout << "( " << vec << ", ";
            E_flag = 1;
        }
        else
        {
            std::cout << vec << " )" << std::endl;
            E_flag = 0;
        }
    }
    std::cout << "----------------------------------------------------" << std::endl;

    //生成split_order参数
    for (unsigned i = 0; i < Max_cov_vec.size(); i += 2)
    {
        std::vector<unsigned> temp_line{};
        temp_line.emplace_back(Max_cov_vec[i]);
        temp_line.emplace_back(Max_cov_vec[i + 1]);
        //在include_set中寻找分割边
        for (auto line_include_set : P_include_set)
        {
            for (unsigned j = 0; j < line_include_set.size(); j += 5)
            {
                if (Max_cov_vec[i] == line_include_set[j] && Max_cov_vec[i + 1] == line_include_set[j + 1])
                {
                    temp_line.emplace_back(line_include_set[j + 3]);
                    temp_line.emplace_back(line_include_set[j + 4]);
                }
            }
        }
        split_order.emplace_back(temp_line);
    }

    std::cout << "The value of split_order is:" << std::endl;
    for (auto i : split_order)
    {
        for (auto j : i)
        {
            std::cout << j << " ";
        }
        std::cout << endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;

    unsigned Min_equ = 0;
    unsigned Min_size = UINT_MAX;
    for (auto i : Max_cov_vec)
    {
        if (P_include_set[i].size() < Min_size)
        {
            Min_equ = i;
            Min_size = P_include_set[i].size();
        }
    }
    minMatchID = Min_equ;
    cout << "Minimum equivalent constraint point is:" << Min_equ << endl;
}

//重载版本，用于检测对称点
void PatternMatching::sym_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum)
{
    //判断两点间是否存在逆向边，有则标记并进行逆向扩展，注意这里P_adj[v_pt][v_ps]两个参数的顺序
    if (P_adj[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have reverse edge." << std::endl;
        return;
    }
    else
    {
        P_adj[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
        visited_edgeNum++;
    }
    //这里判断当前分支是否完整可以继续挖掘下去
    if (branchFinish == false)
        return;

    std::vector<R_ID> Mtemp;
    for (int i = 0; i < vertexNum_P; i++)
    {
        if (P_adj[i][cur_r_vt] != 0 && degree_P[i].indeg == degree_P[v_ps].indeg && degree_P[i].outdeg == degree_P[v_ps].outdeg)
        {
            Mtemp.push_back(i);
        }
    }
    //当求得的点尚未计算PMR集，直接更新PMR集和sel集
    if (PMR_copy[v_ps].size() == 0)
    {
        PMR_copy[v_ps].swap(Mtemp);
        sel_copy[v_ps] = PMR_copy[v_ps].size();
    }
    //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
    else
    {
        intersection(Mtemp, PMR_copy[v_ps]);
        sel_copy[v_ps] = PMR_copy[v_ps].size();
    }
    if (sel_copy[v_ps] == 0)
    {
        branchFinish = false;
    }
    return;
}
//对选定的模式图结点进行约束包含点的贪心覆盖搜索并返回对应的边集合，实质上这些边集合代表了一组分割好的约束关系
void PatternMatching::searchSplit(P_ID v, std::vector<unsigned> &cur_line, std::vector<std::vector<unsigned>> &G)
{
    std::queue<unsigned> q; //循环控制队列q
    //遍历并存储点v的所有邻居节点
    do
    {
        std::vector<unsigned> v_neighbors;
        for (unsigned i = 0; i < vertexNum_P; ++i)
        {
            if (G[v][i] == 1 || G[i][v] == 1)
            {
                v_neighbors.push_back(i);
            }
        }
        while (v_neighbors.size() > 0)
        {
            //找到所有邻居节点中约束包含集P_include_set中最大的并将其压入循环控制队列q中
            unsigned max_inc_size = 0;
            // int max_inc_size = -1;
            unsigned w = 0; // w是邻居中约束包含集最大的节点ID
            //找出邻居中最大约束包含点w
            unsigned del_wi = 0;
            for (int i = 0; i < v_neighbors.size(); ++i)
            {
                // sky220112，在找最大约束包含点时，用>=判断代表ID大者优先，用>判断反之
                if (P_include_set[v_neighbors[i]].size() > max_inc_size)
                {
                    w = v_neighbors[i];
                    del_wi = i;
                    max_inc_size = P_include_set[v_neighbors[i]].size();
                }
            }
            if (v_neighbors.size() != 0)
            { //这个判断是为了避免重复入列w=0而进入死循环
                q.push(w);
                //注意这里需要将w从v_neighbors中删去，否则当前while循环永远无法跳出
                v_neighbors.erase(v_neighbors.begin() + del_wi);
                //删除选中结点的邻居边
                if (G[v][w] == 1)
                {
                    G[v][w] = 2;
                }
                else
                {
                    G[w][v] = 2;
                }
            }
            //将w点的约束包含集插入到输出结果中去，并在邻接表中标记w点的所有约束包含边状态为2，已访问并不再访问
            unsigned cf = 0; //由于P_include_set特殊的存储方式，这里需要用cf标志位来读写数据
            unsigned fir = 0;
            unsigned sec = 0;
            for (unsigned inc = 0; inc < P_include_set[w].size(); ++inc)
            {
                //在输出结果中插入点
                if (cf == 2)
                {
                    cur_line.push_back(P_include_set[w][inc]);
                    //在v_neighbors中将w点的所有包含结点都删去
                    for (int i = 0; i < v_neighbors.size(); ++i)
                    {
                        if (v_neighbors[i] == P_include_set[w][inc])
                        {
                            v_neighbors.erase(v_neighbors.begin() + i);
                            break;
                        }
                    }
                }
                //在输出结果中插入边
                if (cf == 3)
                {
                    fir = P_include_set[w][inc];
                    cf++;
                }
                else if (cf == 4)
                {
                    sec = P_include_set[w][inc];
                    cur_line.push_back(fir);
                    cur_line.push_back(sec);
                    //在G中删除找到的覆盖边，等价于标记为2（已访问且不可再次访问）
                    G[fir][sec] = 2;
                    cf = 0;
                }
                else
                {
                    cf++;
                }
            }
        }
        //从q列表中弹出一个新的v结点，进行新一轮循环，直到全图被覆盖（实质上是两个条件都要满足：q为空以及v_neighbors为空）
        if (q.size() == 0)
        {
            v = NULL;
            break;
        }
        v = q.front();
        q.pop();
    } while (v != NULL);
}

bool PatternMatching::PVAllVisited(vector<int> P_visited)
{
    for (auto k : P_visited)
    {
        if (k == 0)
            return false;
    }
    return true;
}

void PatternMatching::findSym()
{
    cout << "-----" << endl;
    vector<int> P_visited(vertexNum_P, 0);
    vector<vector<P_ID>> P_adjcp = P_adj;
    int setNum = 1; //等价点组数
    while (!PVAllVisited(P_visited))
    {
        int curID;
        for (int i = 0; i < vertexNum_P; i++)
        {
            if (P_visited[i] == 0)
            {
                curID = i;
                break;
            }
        }
        P_visited[curID] = 1;
        vector<int> nums; //可能与curID等价的点
        for (int i = 0; i < vertexNum_P; i++)
        {
            if (degree_P[i].indeg == degree_P[curID].indeg && degree_P[i].outdeg == degree_P[curID].outdeg && i != curID)
            {
                nums.push_back(i);
            }
        }
        if (nums.empty())
            continue;
        //判断nums中的点是否与curID等价
        for (auto tmpid : nums)
        {
            vector<vector<unsigned>> PMRcpy(vertexNum_P);
            vector<int> selcpy(vertexNum_P);
            int result = 0;
            bool branchFinish = true;
            int visited_edgeNum = 0;
            int oriID = tmpid;
            bool isSymMatch = true;
            unordered_map<int, set<R_ID>> sym_PMR;
            vector<vector<R_ID>> PMR_new;
            searchPG(PMRcpy, selcpy, P_adjcp, tmpid, curID, branchFinish, result, visited_edgeNum, oriID, isSymMatch, sym_PMR, PMR_new);
            if (result != 0)
            {
                P_visited[tmpid] = 1;
                sym[tmpid] = setNum;
                sym[curID] = setNum;
            }
        }
        setNum++;
    }
    //判断等价环
    isEqCircle = false;
    for (auto it = sym.begin(); it != sym.end(); it++)
    {
        P_ID id = it->first;
        int group = it->second;
        sym_group[group].insert(id);
    }
    for (auto it = sym_group.begin(); it != sym_group.end(); it++)
    {
        auto s = it->second;
        P_ID tmp = *(s.begin());
        for (int i = 0; i < vertexNum_P; i++)
        {
            if (P_adj[i][tmp] == 1 || P_adj[tmp][i] == 1)
            {
                if (sym.count(i) != 0 && sym[i] == sym[tmp])
                    isEqCircle = true;
                circleSize = sym_group[sym[tmp]].size();
                break;
            }
        }
    }
    //将等价点存储成二维vector
    int eqNum = sym.size();          //等价点总数
    int eqSetNum = sym_group.size(); //等价点组数
    int vecRow = vertexNum_P - eqNum + eqSetNum;
    // vector<vector<int>> eqVec;
    Equivalent_order.resize(vecRow);
    P_ID curid = 0;
    unordered_map<P_ID, int> mp;
    for (int i = 0; i < vecRow; i++)
    {
        while (mp.count(curid) != 0 && curid < vertexNum_P)
        {
            curid++;
        }
        if (sym.count(curid) != 0)
        {
            int curSetNum = sym[curid];
            auto eqVertex = sym_group[curSetNum];
            for (auto it = eqVertex.begin(); it != eqVertex.end(); ++it)
            {
                mp[*it] = 1;
                Equivalent_order[i].push_back(*it);
            }
        }
        else
        {
            mp[curid] = 1;
            Equivalent_order[i].push_back(curid);
        }
    }

    cout << "Equivalent_order:" << endl;
    for (auto vec : Equivalent_order)
    {
        for (auto k : vec)
        {
            cout << k << " ";
        }
        cout << endl;
    }

    cout << "equivalence vertex:" << endl;
    symNum = 0;
    for (auto it = sym.begin(); it != sym.end(); ++it)
    {
        symNum++;
        cout << it->first << " " << it->second << endl;
    }
    if (isEqCircle)
    {
        cout << "exist equivalent circle" << endl;
    }
}

void PatternMatching::eqCircle_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int ori_centerID, int visited_edgeNum)
{
    if (current_match_RID == ori_centerID)
    {
        for (int i = 0; i < vertexNum_P; ++i)
        {
            if (i != current_match_PID)
            {
                sel_copy[i] = INT_MAX;
            }
        }
    }

    //当前选择的中心点其PMR集合应当只包含一个值，这里来对其进行初始化
    /*PMR_copy[current_match_PID].resize(1);
    PMR_copy[current_match_PID][0] = current_match_RID;*/
    //相比上面的方法，有更快的算法，如下，使用交换的方式
    std::vector<unsigned> temp;
    temp.emplace_back(current_match_RID);
    PMR_copy[current_match_PID].swap(temp);
    sel_copy[current_match_PID] = 1;

    //开始图匹配过程
    while (!isNextEPatternEmpty(P_adj_copy) /* visited_edgeNum<edgeNum_P */)
    {
        //下面我们开始寻找从current_match_PID出发的所有未访问边，并对每条边做extend操作
        P_ID neighborID = UINT_MAX; //由这两个点构成最小匹配的边模式
        // int minMatchNum = INT_MAX;//neighborID的最小匹配数
        bool isReverse = true;
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_adj_copy[i][current_match_PID] == 1)
            {
                // minMatchNum = sel[i];
                neighborID = i;
                isReverse = true;
                reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                if (branchFinish == false)
                {

                    return;
                }
            }
        }
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_adj_copy[current_match_PID][i] == 1)
            {
                // minMatchNum = sel[i];
                neighborID = i;
                isReverse = false;
                extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum);
                if (branchFinish == false)
                {

                    return;
                }
            }
        }

        //如果上面的找边结束后neighborID为初始值(9294967296是我们支持的最大结点数)，那么证明current_match_PID的所有边都访问结束，需要重新计算current_match_PID，递归开始
        for (P_ID i = 0; i < vertexNum_P; ++i)
        {
            for (P_ID j = i + 1; j < vertexNum_P; ++j)
            {
                if ((P_adj_copy[i][j] == 1 || P_adj_copy[j][i] == 1) && (sel_copy[i] != INT_MAX || sel_copy[j] != INT_MAX))
                {
                    if (sel_copy[i] <= sel_copy[j])
                    {
                        current_match_PID = i;
                        neighborID = j;
                    }
                    else
                    {
                        current_match_PID = j;
                        neighborID = i;
                    }
                    //从这里开始非中心点的递归过程，也就是中心点所拓展的边已经全部匹配完成
                    for (auto match_RID : PMR_copy[current_match_PID])
                    {
                        eqCircle_searchPG(PMR_copy, sel_copy, P_adj_copy, match_RID, current_match_PID, branchFinish, ori_centerID, visited_edgeNum);
                    }
                    branchFinish = false;
                }
            }
        }
    }

    if (branchFinish == false)
    {

        return;
    }

    //转换PMR的格式
    vector<R_ID> PMR_new;
    changePMRFormat(PMR_copy, PMR_new, 0);
    return;
}

//逆向扩展第三版
void PatternMatching::reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum)
{
    bool flag = true;
    //判断两点间是否存在逆向边，有则标记并进行逆向扩展，注意这里P_adj[v_pt][v_ps]两个参数的顺序
    if (P_adj[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have reverse edge." << std::endl;
        return;
    }
    else
    {
        P_adj[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
        visited_edgeNum++;
    }
    //这里判断当前分支是否完整可以继续挖掘下去
    if (branchFinish == false)
    {
        return;
    }

    std::vector<R_ID> Mtemp;
    //注意，如果代码运行结果存在逻辑错误，请检查此处
    for (int i = 0; i < degree_R[cur_r_vt].indeg; ++i)
    {
        int tmpid = R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i];
        if (degree_R[tmpid].indeg >= degree_P[v_ps].indeg && degree_R[tmpid].outdeg >= degree_P[v_ps].outdeg)
        {
            Mtemp.emplace_back(R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i]);
        }
    }
    //当求得的点尚未计算PMR集，直接更新PMR集和sel集
    if (PMR_copy[v_ps].size() == 0)
    {
        PMR_copy[v_ps].swap(Mtemp);
        sel_copy[v_ps] = PMR_copy[v_ps].size();
    }
    //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
    else
    {
        intersection(Mtemp, PMR_copy[v_ps]);
        sel_copy[v_ps] = PMR_copy[v_ps].size();
    }
    if (sel_copy[v_ps] == 0)
    {
        branchFinish = false;
    }
    return;
}

//等价环多线程函数
void PatternMatching::eqCircle_Multithreaded_search(R_ID i)
{
    std::vector<std::vector<unsigned>> PMR_initialValue;
    PMR_initialValue.resize(vertexNum_P);
    PMR_initialValue[minMatchID].emplace_back(minMatchID_PMR[i]);
    vector<int> sel_cp = sel;
    bool branchFinish = true;
    int visited_edgeNum = 0;
    int ori_centerID = minMatchID_PMR[i]; //原始中心节点ID
    vector<vector<P_ID>> P_adjcp = P_adj;
    eqCircle_searchPG(PMR_initialValue, sel_cp, P_adjcp, minMatchID_PMR[i], minMatchID, branchFinish, ori_centerID, 0);
}

bool PatternMatching::get_Radj_Index(R_ID v_r, int &start, int &end)
{
    if (v_r == vertexNum_R - 1)
    {
        start = R_adjIndex[v_r];
        end = edgeNum_R;
    }
    else
    {
        start = R_adjIndex[v_r];
        end = R_adjIndex[v_r + 1];
    }
    return true;
}

bool PatternMatching::check_result(std::vector<unsigned> PMR_copy)
{
    for (auto k : PMR_copy)
    {
        if (k == -1)
        {
            return false;
        }
    }
    for (int i = 0; i < vertexNum_P; i++)
    {
        for (int j = 0; j < vertexNum_P; j++)
        {
            if (P_adj[i][j] == 1)
            {
                auto R_i = PMR_copy[i];
                auto R_j = PMR_copy[j];
                int i_start = 0, i_end = 0;
                bool find_v = false;
                get_Radj_Index(R_i, i_start, i_end);
                for (int cur = i_start; cur < i_end; ++cur)
                {
                    if (R_adj[cur] == R_j)
                    {
                        find_v = true;
                        break;
                    }
                }
                if (!find_v)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

//同构体去除函数，参数为待检查的结果，Equivalent_order为等价点的严格顺序二维数组
bool PatternMatching::De_duplication(std::vector<unsigned> unchecked_res)
{
    //先检查当前结果中是否存在重复值
    unordered_set<unsigned> res_set;
    for (auto i : unchecked_res)
    {
        res_set.insert(i);
    }
    if (res_set.size() != vertexNum_P)
    {
        return false;
    }
    //再进行同构体去除
    unsigned lines = Equivalent_order.size();
    for (unsigned i = 0; i < lines; ++i)
    {
        unsigned weight = Equivalent_order[i].size();
        if (weight < 2)
        {
            continue;
        }
        else
        {
            unsigned pre = unchecked_res[Equivalent_order[i][0]];
            for (unsigned j = 1; j < weight; ++j)
            {
                unsigned cur = unchecked_res[Equivalent_order[i][j]];
                if (cur <= pre)
                {
                    return false;
                }
                else
                {
                    pre = cur;
                }
            }
        }
    }
    return true;
}

unsigned PatternMatching::full_arrangement(std::vector<vector<R_ID>> cur_thread_PMR)
{
    std::vector<unsigned> carry_table; //进位表
    unsigned count = 0;                //正确结果计数
    carry_table.assign(vertexNum_P, 0);
    for (unsigned line = 0; line < vertexNum_P; ++line)
    {
        carry_table[line] = cur_thread_PMR[line].size();
    }

    //按照进位表生成全排列
    std::vector<unsigned> full_arrangement;
    full_arrangement.assign(vertexNum_P, 0);
    int cur_row = vertexNum_P - 1;
    while (true)
    {
        if (full_arrangement[cur_row] + 1 < carry_table[cur_row])
        {
            full_arrangement[cur_row]++;
            cur_row = vertexNum_P - 1;

            std::vector<unsigned> PMR_cur(vertexNum_P, INT_MAX);
            for (unsigned i = 0; i < vertexNum_P; ++i)
            {
                PMR_cur[i] = cur_thread_PMR[i][full_arrangement[i]];
            }

            if (check_result(PMR_cur))
            {

                //此处添加去重函数
                if (De_duplication(PMR_cur))
                {
                    count++;
                }
            }
        }
        else
        {
            cur_row--;
            if (cur_row < 0)
            {
                break;
            }
            else
            {
                full_arrangement[cur_row + 1] = 0;
            }
        }
    }

    return count;
}

//边模式的全排列结果组合与检验过程
unsigned PatternMatching::full_arrangement(std::vector<std::vector<unsigned>> cur_thread_PMR, std::vector<std::vector<unsigned>> visitE_index)
{
    //先遍历一遍visitE_index把边顺序、未访问边列表和边映射索引分别存储起来方便后续使用
    std::vector<unsigned> Edge_order{};
    std::vector<unsigned> Edge_index{};
    std::vector<unsigned> Edge_unvisit{};
    for (unsigned i = 0; i < vertexNum_P; ++i) {
        for (unsigned j = 0; j < vertexNum_P; ++j) {
            if (visitE_index[i][j] != UNSIGNEDMAX) {
                Edge_order.push_back(i);
                Edge_order.push_back(j);
                Edge_index.push_back(visitE_index[i][j]);
            }
            else if (P_adj[i][j] == 1) {
                Edge_unvisit.push_back(i);
                Edge_unvisit.push_back(j);
            }
        }
    }
    //接下来全排列各边映射结果集合
    std::vector<unsigned> carry_table; //进位表
    unsigned count = 0;                //正确结果计数
    carry_table.assign(Edge_index.size(), 0); // Edge_index.size()代表了有多少条边参与排列组合
    for (unsigned line = 0; line < Edge_index.size(); ++line)
    {
        carry_table[line] = cur_thread_PMR[line].size() / 2; //按边存储两个节点为一组
    }

    //按照进位表生成全排列
    std::vector<unsigned> full_arrangement;
    full_arrangement.assign(Edge_index.size(), 0);
    int cur_row = Edge_index.size() - 1;//从最后一位开始进位
    while (true)
    {
        if (full_arrangement[cur_row] + 1 < carry_table[cur_row])
        {
            full_arrangement[cur_row]++;
            cur_row = Edge_index.size() - 1;
            //这里对全排列结果进行检查和去重
            std::vector<unsigned> temp_ans(vertexNum_P, INT_MAX);
            std::vector<unsigned> match_edge(Edge_index.size() * 2, INT_MAX);
            for (unsigned i = 0; i < Edge_index.size(); ++i)
            {
                //注意检查越界访问问题
                match_edge[i * 2] = cur_thread_PMR[Edge_index[i]][full_arrangement[i] * 2];
                match_edge[i * 2 + 1] = cur_thread_PMR[Edge_index[i]][full_arrangement[i] * 2 + 1];
            }
            
            if (share_point_check(Edge_order, match_edge, temp_ans))
            {
                if (check_result(temp_ans))
                {
                    
                    if (De_duplication(temp_ans))
                    {
                        //此处的检测通过后即为正确结果
                        count++;
                    }
                }
            }
            
        }
        else
        {
            cur_row--;
            if (cur_row < 0)
            {
                break;
            }
            else
            {
                full_arrangement[cur_row + 1] = 0;
            }
        }
    }
    return count;
}

void PatternMatching::changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<vector<R_ID>> &PMR_new, vector<R_ID> PMR_cur, int times)
{
    if (times >= vertexNum_P)
    {
        if (check_result(PMR_cur))
        {
            //此处添加去重函数
            if (De_duplication(PMR_cur))
            {
                PMR_new.push_back(PMR_cur);
            }
        }
        return;
    }
    for (auto k : PMR_copy[times])
    {
        PMR_cur[times] = k;
        changePMRFormat(PMR_copy, PMR_new, PMR_cur, times + 1);
    }
}

int PatternMatching::Multithreaded_search(R_ID i)
{
    std::vector<std::vector<unsigned>> visit_order(vertexNum_P, std::vector<unsigned>(vertexNum_P)); //邻接矩阵，存储的值为该边的匹配边在cur_thread_PMR_edge中的索引
    std::vector<std::vector<unsigned>> cur_thread_PMR_edge; //当前线程的PMR
    for (int i = 0; i < vertexNum_P; i++)
    {
        for (int j = 0; j < vertexNum_P; j++)
        {
            visit_order[i][j] = UNSIGNEDMAX; //该边不存在或者没有匹配边则为UNSIGNEDMAX
        }
    }
    cur_thread_PMR_edge.resize(edgeNum_P);
    //下面的函数会挖掘得到各边的映射结果集合并存入cur_thread_PMR_edge中
    bool isHasRes = Somplified_GraphMining(minMatchID, minMatchID_PMR[i], visit_order, cur_thread_PMR_edge);
    //接下来对边结果映射集全排列，并进行边首尾链接检测，空映射检测，重复映射检测，未访问边存在性检测，自同构检测并输出正确结果
    if (isHasRes) {
        return full_arrangement(cur_thread_PMR_edge, visit_order);
    }
    else {
        return 0;
    }    
}

void PatternMatching::searchAllPR()
{
    std::cout << "Start graph mining..." << std::endl;
    for (R_ID j = 0; j < vertexNum_R; j++)
    {
        if (degree_R[j].indeg >= degree_P[minMatchID].indeg && degree_R[j].outdeg >= degree_P[minMatchID].outdeg)
        {
            minMatchID_PMR.emplace_back(j);
            R_visited[j] = 1;
        }
    }

    //分两种情况讨论，1.带环等价点，2.非带环
    if (isEqCircle)
    {
        parallel_for(blocked_range<size_t>(0, minMatchID_PMR.size()), [&](blocked_range<size_t> r)
                     {
                         for (auto i = r.begin(); i != r.end(); ++i)
                         {
                             eqCircle_Multithreaded_search(i);
                         } });
        cout << "Mining result is : " << eqCircleResult.size() << endl;
    }
    else
    {
        auto finalAns = parallel_reduce(
            blocked_range<size_t>(0, minMatchID_PMR.size()), 0, [&](blocked_range<size_t> r, int ans)
            {
                for (int i = r.begin(); i != r.end(); ++i)
                {
                    ans += Multithreaded_search(i);
                }
                return ans; },
            plus<int>());
        std::cout << "Mining result is : " << finalAns << std::endl;
    }
}

bool PatternMatching::share_point_check(std::vector<unsigned> edge_order, std::vector<unsigned> match_edge, std::vector<unsigned> &true_ans)
{
    std::vector<set<unsigned>> check_ans(vertexNum_P);
    set<unsigned> true_ans_set;
    bool isTrue = true;
    for (int i = 0; i < edge_order.size(); i += 2)
    {
        check_ans[edge_order[i]].insert(match_edge[i]);
        if (check_ans[edge_order[i]].size() > 1)
        { //该点有多个匹配结果则提前结束
            isTrue = false;
            return isTrue;
        }
        check_ans[edge_order[i + 1]].insert(match_edge[i + 1]);
        if (check_ans[edge_order[i + 1]].size() > 1)
        { //该点有多个匹配结果则提前结束
            isTrue = false;
            return isTrue;
        }
    }
    for (int j = 0; j < vertexNum_P; j++)
    {
        true_ans_set.insert(*check_ans[j].begin()); //判断是否有同一个真实点匹配了多个模式点  这里如果去重了则匹配了多个点
    }
    if (true_ans_set.size() != vertexNum_P)
    { //判断是否有同一个真实点匹配了多个模式点
        isTrue = false;
        return isTrue;
    }
    else
    {
        for (int j = 0; j < vertexNum_P; j++)
        {
            true_ans[j] = (*check_ans[j].begin());
        }
    }
    return isTrue;
}

//模式图P'在真实图R中进行挖掘核心函数
//输出为当前线程计算所得的PMR集合vector<vector<unsigned>> cur_thread_PMR
//输入参数有5个，在并入主路径代码时可以简化，此处为了独立说明而添加
//参数一：最大覆盖子图队列 vector<unsigned> Max_cov_vec
//参数二：剪枝序列 vector<vector<unsigned>> split_order
//参数三：起始模式图点 unsigned P_start_vec
//参数四：起始模式图点在真实图中的匹配点 unsigned R_start_vec
bool PatternMatching::Somplified_GraphMining(P_ID P_start_vec, R_ID R_start_vec, vector<vector<unsigned>> &visit_order, vector<vector<unsigned>> &cur_thread_PMR_edge)
{
    P_ID originalID = P_start_vec;
    std::vector<std::vector<unsigned>> cur_thread_PMR; //初始化函数输出
    cur_thread_PMR.resize(vertexNum_P);
    std::vector<std::vector<P_ID>> P_max_cov_adj(vertexNum_P, std::vector<P_ID>(vertexNum_P)); //最大覆盖图的邻接表
    std::queue<int> center_q;                                                                  //存储下一个中心点的队列
    std::queue<int> un_PMR_vec_q;                                                              //最后遗漏的模式图点
    int edgeNum_P_max_cov = 0;                                                                 //记录最大覆盖图边数
    int visited_edgeNum = 0;                                                                   //已经访问的边数
    int visited_edgeIndex = 0;                                                                 // visit_order中存边的索引
    std::set<int> Max_cov_vec_set;                                                             //用set存储最大访问边的顶点  用于后面检测是否可以不需要进行补缺匹配

    //将Max_cov_vec转换成邻接表的存储格式
    for (int i = 0; i < Max_cov_vec.size(); i += 2)
    {
        Max_cov_vec_set.insert(Max_cov_vec[i]);
        Max_cov_vec_set.insert(Max_cov_vec[i + 1]);
        P_max_cov_adj[Max_cov_vec[i]][Max_cov_vec[i + 1]] = 1;
        edgeNum_P_max_cov++;
    }

    cur_thread_PMR[P_start_vec].emplace_back(R_start_vec);
    //每次找到一条边后在Max_cov_vec中删除或标记该边，直到Max_cov_vec为空时当前循环结束
    while (visited_edgeNum < edgeNum_P_max_cov)
    {
        //对与P_start_vec相连的每一条边 ，根据当前选定的一条边、R_start_vec和已更新的结点的PMR集合，在R_adj中匹配边，删除或标记该边
        P_ID neighborID = UINT_MAX; //由这两个点构成最小匹配的边模式
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_max_cov_adj[i][P_start_vec] == 1)
            {
                neighborID = i;
                visited_edgeNum++;
                center_q.push(neighborID);
                std::set<R_ID> set;               //求并集的匹配结果
                std::vector<R_ID> cur_edge_match; //当前边所有的匹配边
                for (auto match_RID : cur_thread_PMR[P_start_vec])
                {
                    //每次匹配完成一条边后，更新PMR集合并查询split_order对等价约束边上的结点进行赋值，如无初值则直接复制拷贝，有则调用求交集函数intersection，更新为交集
                    //等价约束边上的结点进行赋值和求交集在下面函数内部实现
                    reverse_extendEdgePattern(neighborID, P_start_vec, match_RID, P_max_cov_adj, set, cur_edge_match);
                }
                split_vecAssign(neighborID, P_start_vec, cur_thread_PMR, split_order, set, originalID, false, visit_order, cur_thread_PMR_edge, visited_edgeIndex, cur_edge_match);
            }
        }
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            if (P_max_cov_adj[P_start_vec][i] == 1)
            {
                neighborID = i;
                visited_edgeNum++;
                center_q.push(neighborID);
                std::set<R_ID> set;               //求并集的匹配结果
                std::vector<R_ID> cur_edge_match; //当前边所有的匹配边
                for (auto match_RID : cur_thread_PMR[P_start_vec])
                {
                    extendEdgePattern(P_start_vec, neighborID, match_RID, P_max_cov_adj, set, cur_edge_match);
                }
                split_vecAssign(P_start_vec, neighborID, cur_thread_PMR, split_order, set, originalID, true, visit_order, cur_thread_PMR_edge, visited_edgeIndex, cur_edge_match);
            }
        }
        P_start_vec = center_q.front(); //换中心点，广度优先的方式
        center_q.pop();
    }

    //将匹配结果的每条边都按严格出入度比较一次，削减匹配集合
    for(int i = 0; i<vertexNum_P;i++){
      for(int j = 0; j<vertexNum_P;j++){
        if(visit_order[i][j] != UNSIGNEDMAX){
          int maxNum = cur_thread_PMR_edge[visit_order[i][j]].size();
          vector<unsigned> temp ;
          for(int k =0 ; k<maxNum-1 ;k += 2){
            R_ID s = cur_thread_PMR_edge[visit_order[i][j]][k];
            R_ID t = cur_thread_PMR_edge[visit_order[i][j]][k+1];
            if(degree_R[s].indeg >= degree_P[i].indeg && degree_R[s].outdeg >= degree_P[i].outdeg && 
            degree_R[t].indeg >= degree_P[j].indeg && degree_R[t].outdeg >= degree_P[j].outdeg){
              temp.push_back(s);
              temp.push_back(t);
            }
          }
          cur_thread_PMR_edge[visit_order[i][j]].swap(temp);

        }
      }
    }

    //当while循环结束后，检查PMR集合，是否有遗漏的模式图点，无则输出，有则继续匹配
    //循环找到所有遗漏的点，并加入队列
    bool isMax_cov_vec_matched = true; //判断最大覆盖顶点是否都有匹配结果
    std::vector<std::vector<P_ID>> P_adj_copy = P_adj;
    for (int i = 0; i < cur_thread_PMR.size(); i++)
    {
        //抽取PMR集合中每一行，判断是否已更新
        if (cur_thread_PMR[i].size() != 0)
        { //已更新，跳过
            continue;
        }
        else
        { //未更新，加入队列
            // i点是最大覆盖顶点中的点，但是没有匹配结果则表示该论匹配不可能有结果，直接结束匹配
            if (Max_cov_vec_set.count(i) != 0)
            {
                isMax_cov_vec_matched = false;
                return false; //本轮匹配不可能有正确结果
            }
            un_PMR_vec_q.push(i);
        }
    }

    //缺失的结点进行单独匹配,直到所有未更新的点都更新
    //如果最大覆盖顶点都没有匹配结果则该匹配一定没有正确结果不需要再匹配
    while (!un_PMR_vec_q.empty() && isMax_cov_vec_matched)
    {
        int i = un_PMR_vec_q.front();
        un_PMR_vec_q.pop();
        bool flag = false; //记录i点是否更新成功
        //  执行匹配操作
        // TODO
        for (P_ID j = 0; j < vertexNum_P; j++)
        {
            if (P_adj[i][j] == 1 && cur_thread_PMR[j].size() != 0)
            {                                     //逆向扩展, 且终点j的PMR有匹配结果
                std::set<R_ID> set;               //求并集的匹配结果
                std::vector<R_ID> cur_edge_match; //当前边所有的匹配边

                for (auto match_RID : cur_thread_PMR[j])
                {
                    reverse_extendEdgePattern(P_adj_copy, i, j, match_RID, set, cur_edge_match);
                }
                std::vector<R_ID> Mtemp;
                Mtemp.assign(set.begin(), set.end());
                cur_thread_PMR[i].swap(Mtemp);
                visit_order[i][j] = visited_edgeIndex;
                cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                visited_edgeIndex++;
                flag = true;
                break;
            }
            else if (P_adj[j][i] == 1 && cur_thread_PMR[j].size() != 0)
            {                                     //正向扩展，且起点j的PMR有匹配结果
                std::set<R_ID> set;               //求并集的匹配结果
                std::vector<R_ID> cur_edge_match; //当前边所有的匹配边

                for (auto match_RID : cur_thread_PMR[j])
                {
                    extendEdgePattern(P_adj_copy, j, i, match_RID, set, cur_edge_match);
                }
                std::vector<R_ID> Mtemp;
                Mtemp.assign(set.begin(), set.end());
                cur_thread_PMR[i].swap(Mtemp);

                visit_order[j][i] = visited_edgeIndex;
                cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                visited_edgeIndex++;
                flag = true;
                break;
            }
        }
        if (!flag)
        {                         // i点未更新成功，及与i相连的点也没有匹配集合
            un_PMR_vec_q.push(i); //再次将i入队，等待与它相连的点的匹配集合更新
        }
    }

    //输出结果
    return true;
}

void PatternMatching::changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<R_ID> PMR_new, P_ID currentId)
{

    if (PMR_new.size() == circleSize)
    {
        for (auto it = eqCircleResult.cbegin(); it != eqCircleResult.cend(); ++it)
        {
            if ((*it).size() == circleSize && isEqResult(PMR_new, *it) && !eqCircleResult.empty())
            {
                return;
            }
        }
        
        eqCircleResult.push_back(PMR_new);
        return;
    }
    for (auto k : PMR_copy[currentId])
    {
        PMR_new.push_back(k);
        changePMRFormat(PMR_copy, PMR_new, currentId + 1);
        PMR_new.pop_back();
    }
}

bool PatternMatching::isEqResult(vector<R_ID> can_PMR, vector<R_ID> result_PMR)
{
    int canStart = 0;
    int resultStart = 0;
    while (can_PMR[canStart] != result_PMR[resultStart] && resultStart < result_PMR.size())
    {
        resultStart++;
    }
    if (resultStart == result_PMR.size())
    {
        return false;
    }
    for (int i = 1; i < vertexNum_P; i++)
    {
        if (can_PMR[(canStart + i) % vertexNum_P] != result_PMR[(resultStart + i) % vertexNum_P])
        {
            return false;
        }
    }
    return true;
}

//正向扩展 约束条件是出入度之和
void PatternMatching::extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<P_ID>> &P_max_cov_adj_copy, std::set<R_ID> &set, vector<R_ID> &cur_edge_match)
{
    //判断两点间是否存在正向边，有则标记并进行正向扩展
    if (P_max_cov_adj_copy[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have edge." << std::endl;
        return;
    }
    else
    {
        P_max_cov_adj_copy[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }

    for (int i = 0; i < degree_R[cur_r_vs].outdeg; ++i)
    {
        R_ID tmpid = R_adj[R_adjIndex[cur_r_vs] + i];
        if ((degree_R[tmpid].indeg + degree_R[tmpid].outdeg) >= (degree_P[v_pt].indeg + degree_P[v_pt].outdeg))
        {
            cur_edge_match.push_back(cur_r_vs);
            cur_edge_match.push_back(R_adj[R_adjIndex[cur_r_vs] + i]);
            set.insert(R_adj[R_adjIndex[cur_r_vs] + i]);
        }
    }
}

//逆向扩展 约束条件是出入度之和
void PatternMatching::reverse_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vt, std::vector<std::vector<P_ID>> &P_max_cov_adj_copy, std::set<R_ID> &set, vector<R_ID> &cur_edge_match)
{
    //判断两点间是否存在逆向边，有则标记并进行逆向扩展，注意这里P_adj[v_pt][v_ps]两个参数的顺序
    if (P_max_cov_adj_copy[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have reverse edge." << std::endl;
        return;
    }
    else
    {
        P_max_cov_adj_copy[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }
    //注意，如果代码运行结果存在逻辑错误，请检查此处
    for (int i = 0; i < degree_R[cur_r_vt].indeg; ++i)
    {
        int tmpid = R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i];
        if ((degree_R[tmpid].indeg + degree_R[tmpid].outdeg) >= (degree_P[v_ps].indeg + degree_P[v_ps].outdeg))
        {
            cur_edge_match.push_back(R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i]);
            cur_edge_match.push_back(cur_r_vt);
            set.insert(R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i]);
        }
    }
}

//更改后的函数，计算与模式边vps, vpt端点vpt匹配的真实图顶点
void PatternMatching::extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum)
{
    //判断两点间是否存在正向边，有则标记并进行正向扩展
    if (P_adj[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have edge." << std::endl;
        return;
    }
    else
    {
        visited_edgeNum++;
        P_adj[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }
    //这里判断当前分支是否完整可以继续挖掘下去
    if (branchFinish == false)
    {

        return;
    }
    std::vector<R_ID> Mtemp;
    for (int i = 0; i < degree_R[cur_r_vs].outdeg; ++i)
    {
        R_ID tmpid = R_adj[R_adjIndex[cur_r_vs] + i];
        if (degree_R[tmpid].indeg >= degree_P[v_pt].indeg && degree_R[tmpid].outdeg >= degree_P[v_pt].outdeg)
        {
            Mtemp.emplace_back(R_adj[R_adjIndex[cur_r_vs] + i]);
        }
    }

    //当求得的点尚未计算PMR集，直接更新PMR集和sel集
    if (PMR_copy[v_pt].size() == 0)
    {
        PMR_copy[v_pt].swap(Mtemp);
        sel_copy[v_pt] = PMR_copy[v_pt].size();
    }
    //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
    else
    {
        intersection(Mtemp, PMR_copy[v_pt]);
        sel_copy[v_pt] = PMR_copy[v_pt].size();
    }
    if (sel_copy[v_pt] == 0)
    {
        branchFinish = false;
    }
    return;
}

//对等价约束边的点赋值
void PatternMatching::split_vecAssign(P_ID v_ps, P_ID v_pt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<std::vector<unsigned>> split_order, std::set<R_ID> &set,
                                      P_ID originalID, bool isExtend, vector<vector<unsigned>> &visit_order, vector<vector<unsigned>> &cur_thread_PMR_edge, int &visited_edgeIndex, vector<R_ID> &cur_edge_match)
{
    //每次匹配完成一条边后，更新PMR集合并查询split_order对等价约束边上的结点进行赋值，如无初值则直接复制拷贝，有则调用求交集函数intersection，更新为交集
    std::vector<R_ID> Mtemp_s; //等价约束边的起点匹配集合
    std::vector<R_ID> Mtemp_t; //等价约束边的终点匹配集合
    if (isExtend)
    { //正向扩展情况
        Mtemp_s = PMR_copy[v_ps];
        Mtemp_t.assign(set.begin(), set.end());

        //对最大覆盖图本身这条边的点更新PMR
        std::vector<R_ID> Mtemp_tcopy = Mtemp_t;
        //当求得的点尚未计算PMR集，直接更新PMR集和sel集
        if (PMR_copy[v_pt].size() == 0)
        {
            PMR_copy[v_pt].swap(Mtemp_tcopy);

            // v_pt没有匹配结果 则v_ps，v_pt边一定没有匹配
            visit_order[v_ps][v_pt] = visited_edgeIndex;
            cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
            visited_edgeIndex++;
        }
        //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
        else
        {
            if (visit_order[v_ps][v_pt] == UNSIGNEDMAX)
            { // v_pt点已经匹配结果 但v_ps，v_pt边没有匹配结果
                visit_order[v_ps][v_pt] = visited_edgeIndex;
                cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                visited_edgeIndex++;
            }
            intersection_edge(Mtemp_tcopy, PMR_copy[v_pt], visit_order, cur_thread_PMR_edge, v_pt);
        }
    }
    else
    { //逆向扩展情况
        Mtemp_s.assign(set.begin(), set.end());
        Mtemp_t = PMR_copy[v_pt];

        //对最大覆盖图本身这条边的点更新PMR
        std::vector<R_ID> Mtemp_scopy = Mtemp_s;
        //当求得的点尚未计算PMR集，直接更新PMR集和sel集
        if (PMR_copy[v_ps].size() == 0)
        {
            PMR_copy[v_ps].swap(Mtemp_scopy);

            // v_ps没有匹配结果 则v_ps，v_pt边一定没有匹配
            visit_order[v_ps][v_pt] = visited_edgeIndex;
            cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
            visited_edgeIndex++;
        }
        //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
        else
        {

            if (visit_order[v_ps][v_pt] == UNSIGNEDMAX)
            { // v_pt点已经匹配结果 但v_ps，v_pt边没有匹配结果
                visit_order[v_ps][v_pt] = visited_edgeIndex;
                cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                visited_edgeIndex++;
            }
            intersection_edge(Mtemp_scopy, PMR_copy[v_ps], visit_order, cur_thread_PMR_edge, v_ps);
        }
    }

    //对等价约束边的点更新PMR
    std::vector<unsigned> split_order_temp; //当前边的等价边集合
    for (int i = 0; i < split_order.size(); i++)
    {
        if ((split_order[i][0] == v_ps && split_order[i][1] == v_pt))
        {
            split_order_temp = split_order[i];
        }
    }

    if (split_order_temp.size() > 2)
    {                                     //有等价边的情况
        std::vector<unsigned> Mtemp_edge; //与最大覆盖边中当前边匹配的所有真实图中的边的集合
        Mtemp_edge = cur_thread_PMR_edge[visit_order[v_ps][v_pt]]; //拿到等价边的匹配集合

        for (int i = 2; i < split_order_temp.size(); i += 2)
        {
            int equal_ID_s = split_order_temp[i];
            int equal_ID_t = split_order_temp[i + 1];

            //条件1：不是同一个点则赋值, 条件2：赋值的点和被赋值的点都不能是原始中心点，条件3：被赋值点的总出入度和大于等于赋值点的总出入度之和
            //条件4：逆向且不是同起点，则可以给起点值赋值，反过来理解：要给起点赋值只能是逆向且不同起点的情况
            if (!isExtend && equal_ID_s != v_ps && v_ps != originalID && equal_ID_s != originalID &&
                ((degree_R[equal_ID_s].indeg + degree_R[equal_ID_s].outdeg) >= (degree_P[v_ps].indeg + degree_P[v_ps].outdeg)))
            {

                //更新等价边的更新等价边的cur_thread_PMR
                std::vector<R_ID> Mtemp_scopy = Mtemp_s;
                //当求得的点尚未计算PMR集，直接更新PMR集和sel集
                if (PMR_copy[equal_ID_s].size() == 0)
                {
                    //更新等价边的cur_thread_PMR_edge
                    // equal_ID_s没有匹配结果 则v_ps，v_pt边一定没有匹配
                    std::vector<unsigned> Mtemp_edge_copy = Mtemp_edge;
                    visit_order[equal_ID_s][equal_ID_t] = visited_edgeIndex;
                    cur_thread_PMR_edge[visited_edgeIndex].swap(Mtemp_edge_copy); //将v_ps，v_pt边的匹配边赋值给等价边添加到边equal_ID_s，equal_ID_t的PMR集合中
                    visited_edgeIndex++;

                    PMR_copy[equal_ID_s].swap(Mtemp_scopy);
                    // print_PMR(visit_order);
                    // print_PMR(cur_thread_PMR_edge);
                }
                //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
                else
                {
                    if (visit_order[equal_ID_s][equal_ID_t] == -1)
                    { // equal_ID_s点已经匹配结果 但equal_ID_s，equal_ID_t边没有匹配结果
                        visit_order[equal_ID_s][equal_ID_t] = visited_edgeIndex;
                        cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                        visited_edgeIndex++;
                    }
                    // intersection(Mtemp_scopy, PMR_copy[equal_ID_s]);
                    intersection_edge(Mtemp_scopy, PMR_copy[equal_ID_s], visit_order, cur_thread_PMR_edge, equal_ID_s);
                }
            }

            //条件1：不是同一个点则赋值, 条件2：赋值的点和被赋值的点都不能是原始中心点，条件3：被赋值点的总出入度和大于等于赋值点的总出入度之和
            //条件4：正向且不是同终点，则可以给终点赋值，反过来理解：要给终点赋值只能是正向且不同终点的情况
            if (isExtend && equal_ID_t != v_pt && v_pt != originalID && equal_ID_t != originalID &&
                ((degree_R[equal_ID_t].indeg + degree_R[equal_ID_t].outdeg) >= (degree_P[v_pt].indeg + degree_P[v_pt].outdeg)))
            {

                //更新等价边的更新等价边的cur_thread_PMR
                std::vector<R_ID> Mtemp_tcopy = Mtemp_t;
                //当求得的点尚未计算PMR集，直接更新PMR集和sel集
                if (PMR_copy[equal_ID_t].size() == 0)
                {
                    //更新等价边的cur_thread_PMR_edge
                    // equal_ID_t没有匹配结果 则v_ps，v_pt边一定没有匹配
                    std::vector<unsigned> Mtemp_edge_copy = Mtemp_edge;
                    visit_order[equal_ID_s][equal_ID_t] = visited_edgeIndex;
                    cur_thread_PMR_edge[visited_edgeIndex].swap(Mtemp_edge_copy); //将v_ps，v_pt边的匹配边赋值给等价边添加到边equal_ID_s，equal_ID_t的PMR集合中
                    visited_edgeIndex++;

                    PMR_copy[equal_ID_t].swap(Mtemp_tcopy);
                }
                //当求得的点已经存在PMR集时，求交集，然后更新PMR集和sel集
                else
                {
                    if (visit_order[equal_ID_s][equal_ID_t] == -1)
                    { // equal_ID_s点已经匹配结果 但equal_ID_s，equal_ID_t边没有匹配结果
                        visit_order[equal_ID_s][equal_ID_t] = visited_edgeIndex;
                        cur_thread_PMR_edge[visited_edgeIndex].swap(cur_edge_match); //将v_ps，v_pt边的匹配边添加到边的PMR集合中
                        visited_edgeIndex++;
                    }
                    intersection_edge(Mtemp_tcopy, PMR_copy[equal_ID_t], visit_order, cur_thread_PMR_edge, equal_ID_t);
                }
            }
        }
    }
}

//正向扩展 约束条件是出入度严格匹配
void PatternMatching::extendEdgePattern(std::vector<std::vector<P_ID>> P_adj_copy, P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::set<R_ID> &set, vector<R_ID> &cur_edge_match)
{
    //判断两点间是否存在正向边，有则标记并进行正向扩展
    if (P_adj_copy[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have edge." << std::endl;
        return;
    }
    else
    {
        P_adj_copy[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }

    for (int i = 0; i < degree_R[cur_r_vs].outdeg; i++)
    {
        R_ID tmpid = R_adj[R_adjIndex[cur_r_vs] + i];
        // cout<<tmpid<<endl;
        if (degree_R[tmpid].indeg >= degree_P[v_pt].indeg && degree_R[tmpid].outdeg >= degree_P[v_pt].outdeg)
        {
            // cout<<cur_r_vs<<R_adj[R_adjIndex[cur_r_vs] + i];
            cur_edge_match.push_back(cur_r_vs);
            cur_edge_match.push_back(R_adj[R_adjIndex[cur_r_vs] + i]);
            set.insert(R_adj[R_adjIndex[cur_r_vs] + i]);
        }
    }
}

//逆向扩展 约束条件是出入度严格匹配
void PatternMatching::reverse_extendEdgePattern(std::vector<std::vector<P_ID>> P_adj_copy, P_ID v_ps, P_ID v_pt, R_ID cur_r_vt, std::set<R_ID> &set, vector<R_ID> &cur_edge_match)
{
    //判断两点间是否存在逆向边，有则标记并进行逆向扩展，注意这里P_adj[v_pt][v_ps]两个参数的顺序
    if (P_adj_copy[v_ps][v_pt] == 0)
    {
        std::cout << "Error: can't reverse_extendEdgePattern, point " << v_pt << " and " << v_ps << " don't have reverse edge." << std::endl;
        return;
    }
    else
    {
        P_adj_copy[v_ps][v_pt] = 2; // 2代表两点间有边且已访问
    }
    //注意，如果代码运行结果存在逻辑错误，请检查此处
    for (int i = 0; i < degree_R[cur_r_vt].indeg; i++)
    {
        int tmpid = R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i];
        if (degree_R[tmpid].indeg >= degree_P[v_ps].indeg && degree_R[tmpid].outdeg >= degree_P[v_ps].outdeg)
        {
            cur_edge_match.push_back(R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i]);
            cur_edge_match.push_back(cur_r_vt);

            set.insert(R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i]);
        }
    }
}

//求Mtemp和Mpt的交集，已针对大规模数据进行优化，返回值为交集是否为空, 同时更新与求交点相关的所有边
bool PatternMatching::intersection_edge(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline, vector<vector<P_ID>> &visit_order,
                                        vector<vector<unsigned>> &cur_thread_PMR_edge, P_ID vec)
{
    std::unordered_set<R_ID> temp(Mtemp.begin(), Mtemp.end());
    bool is_empty = true;
    // std::cout << "Intersection set is: ";
    std::vector<R_ID> intersect_result;
    for (auto i : PMR_copy_oneline)
    {
        auto p = temp.find(i);
        if (p != temp.end())
        {
            // std::cout << i << " ";
            temp.erase(i);
            intersect_result.emplace_back(i);
            is_empty = false;
        }
    }
    //找到与求交点所有相关的模式图中的边，并更新对应的匹配的真实图的点
    std::set<R_ID> PMR_set(intersect_result.begin(), intersect_result.end());
    for (int i = 0; i < vertexNum_P; i++)
    {
        int cur_visited_edgeIndex = visit_order[vec][i]; //该边的起点是求交点
        if (cur_visited_edgeIndex != -1)
        {
            std::vector<R_ID> temp_edge;
            for (int k = 0; k < cur_thread_PMR_edge[cur_visited_edgeIndex].size(); k += 2)
            {
                if (PMR_set.find(cur_thread_PMR_edge[cur_visited_edgeIndex][k]) != PMR_set.end())
                {
                    temp_edge.push_back(cur_thread_PMR_edge[cur_visited_edgeIndex][k]);
                    temp_edge.push_back(cur_thread_PMR_edge[cur_visited_edgeIndex][k + 1]);
                }
            }
            cur_thread_PMR_edge[cur_visited_edgeIndex].swap(temp_edge);
        }
        cur_visited_edgeIndex = visit_order[i][vec]; //该边的终点是求交点
        if (cur_visited_edgeIndex != -1)
        {
            std::vector<R_ID> temp_edge;
            for (int k = 1; k < cur_thread_PMR_edge[cur_visited_edgeIndex].size(); k += 2)
            {
                if (PMR_set.find(cur_thread_PMR_edge[cur_visited_edgeIndex][k]) != PMR_set.end())
                {
                    temp_edge.push_back(cur_thread_PMR_edge[cur_visited_edgeIndex][k - 1]);

                    temp_edge.push_back(cur_thread_PMR_edge[cur_visited_edgeIndex][k]);
                }
            }
            cur_thread_PMR_edge[cur_visited_edgeIndex].swap(temp_edge);
        }
    }
    //将求得的结果赋值给PMR_copy的当前行
    PMR_copy_oneline.swap(intersect_result);
    return is_empty;
}

//求交集函数第三版本，输入为两个一维集合,将求解两个集合交集并将结果更新到后者
bool PatternMatching::intersection(std::vector<R_ID> &Mtemp, std::vector<R_ID> &PMR_copy_oneline)
{
    std::unordered_set<R_ID> temp(Mtemp.begin(), Mtemp.end());
    bool is_empty = true;

    // std::cout << "Intersection set is: ";

    std::vector<R_ID> intersect_result;
    for (auto i : PMR_copy_oneline)
    {
        auto p = temp.find(i);
        if (p != temp.end())
        {
            // std::cout << i << " ";
            temp.erase(i);
            intersect_result.emplace_back(i);
            is_empty = false;
        }
    }

    //将求得的结果赋值给PMR_copy的当前行
    PMR_copy_oneline.swap(intersect_result);

    return is_empty;
}

//打印PMR
void PatternMatching::print_PMR(std::vector<std::vector<unsigned>> &PMR_copy)
{
    std::cout << "Print current PMR collection." << std::endl;
    for (int i = 0; i < vertexNum_P; i++)
    {
        std::cout << "P" << i << ": ";
        for (auto j : PMR_copy[i])
        {
            std::cout << j << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Finish print PMR collection" << std::endl;
}

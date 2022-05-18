#include "PatternMatching.h"
#include <tbb/tbb.h>
#include <mutex>
#include <ctime>
#include <sys/time.h>
#include <set>
using namespace std;
using namespace tbb;
// class FindPattern {
// 	 public:
// 		void operator() (const blocked_range < size_t > &r)const {
// 			for (size_t i = r.begin(); i != r.end(); ++i) {
// 				PatternMatching::Multithreaded_search(i);
// 			}
// 		}
// 	};
PatternMatching::~PatternMatching()
{
    /* if (R_visited)
    {
        free(R_visited);
        R_visited = nullptr;
    } */
    if (degree_R)
    {
        free(degree_R);
        degree_R = nullptr;
    }

    /*if (PMR) {
        free(PMR);
        PMR = nullptr;
    }*/

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
    // Mining_result_count=0;
    // eqCircleResult.reserve(15000);
    control_flag = 0;
    outputfile_ptr = std::make_shared<SynchronizedFile>(Output_dir); // sky211116
    /* R_visited = (unsigned *)malloc(vertexNum_R * sizeof(unsigned));
    memset(R_visited, 0, vertexNum_R * sizeof(unsigned)); */
}

bool PatternMatching::build_degree_R(std::string inputfile, unsigned vertexNum) //创建degree_R，测试成功
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
    // std::cout << "Reading in adjacency list format!" << std::endl;
    int maxlen = 100000000;
    char *s = (char *)malloc(maxlen); //暂时存放一次读入的数据
    size_t bytesread = 0;             //读到的总字节数
    char delims[] = " \t";            //字符串的分隔符（tab键）
    size_t linenum = 0;               //行数
    size_t lastlog = 0;               //最后一个字节
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
    // std::cout<<edgeNum_R<<std::endl;
    cout<<"maxid: "<<maxID<<endl;
    std::cout << "finish first read R_adj" << std::endl;
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
    // std::cout << "Reading in adjacency list format!" << std::endl;
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
    /* for(int i=0;i<edgeNum_R;i++){
        cout<<R_adj[i]<<" ";
    }
    cout<<endl;
    for(int i=0;i<vertexNum_R;i++){
        cout<<R_adjIndex[i]<<" ";
    }
    cout<<endl; */
    // std::cout << "finish second read R_adj..." << std::endl;
    //多重边检测
    /* bool isMul=false;
    for(int i=0;i<vertexNum_R;i++){
        if(degree_R[i].outdeg<2)
        continue;
        for(int j=1;j<degree_R[i].outdeg;j++){
            if(R_adj[R_adjIndex[i]+j]==R_adj[R_adjIndex[i]+j-1]){
                cout<<"exist same edge"<<endl;
                isMul=true;
            }

        }
    }
    if(!isMul){
        cout<<"There are no multiple edges in the data set"<<endl;
    }
 */

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
    std::vector<std::vector<P_ID>> tmp(vertexNum_P, std::vector<P_ID>(vertexNum_P)); //临时存储，方便初始化
    FILE *inf = fopen(inputfile.c_str(), "r");
    if (inf == NULL)
    {
        std::cerr << "Could not load :" << inputfile << " error: " << strerror(errno)
                  << std::endl;
    }
    assert(inf != NULL);
    // std::cout << "Reading in adjacency list format!" << std::endl;
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

bool PatternMatching::matchPR_expand()
{
    // total_num = 0;
    sel.resize(vertexNum_P); //初始化选择度
    minMatchID = 0;          //具有最小匹配数的模式图节点ID
    minMatchNum = UINT_MAX;  //最小匹配数
    unsigned current_size = 0;
    unsigned total_size = 0; //存储M(Vp)所需的空间大小
    /*  struct timeval start_time, end_time, tmp_start, tmp_end;
    gettimeofday(&start_time, NULL);//起始时间
    std::mutex m;
    for (unsigned i = 0; i < vertexNum_P; i++)//计算与每个模式图点匹配的实际图中节点数量
    {
        parallel_for(blocked_range<size_t>(0,vertexNum_R),[&](blocked_range<size_t> r){
            for(int j=r.begin();j!=r.end();++j){
                if(degree_R[j].indeg >= degree_P[i].indeg && degree_R[j].outdeg >= degree_P[i].outdeg){
                    m.lock();
                    sel[i]++;
                    m.unlock();
                }
            }
        });
    }
    gettimeofday(&tmp_end, NULL);
    std::cout << "match time elapse:" << ((tmp_end.tv_sec - start_time.tv_sec) * 1000000 + (tmp_end.tv_usec - start_time.tv_usec)) / 1000000.0 << " s" << std::endl;
    for(int i=0;i<vertexNum_P;i++){
        if(sel[i]<minMatchNum){
            minMatchNum=sel[i];
            minMatchID=i;
        }
    } */
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
    //统计除起始点外的非等价点
    for (P_ID i = 0; i < vertexNum_P; i++)
    {
        if (sym.count(i) == 0 && i != minMatchID)
        {
            asym.push_back(i);
        }
    }
    std::cout << "minMatchID = " << minMatchID << std::endl;
    return true;
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
            long long result = 0;
            bool branchFinish = true;
            int visited_edgeNum = 0;
            int oriID = tmpid;
            bool isSymMatch = true;
            unordered_map<int, set<R_ID>> sym_PMR;
            vector<vector<R_ID>> PMR_new;
            unordered_map<R_ID, int> mp;
            // cout<<"in tmpid "<<tmpid<<"  curid "<<curID<<endl;
            searchPG(PMRcpy, selcpy, P_adjcp, tmpid, curID, branchFinish, result, visited_edgeNum, oriID, isSymMatch, sym_PMR, PMR_new, mp);
            // cout<<"out tmpid "<<tmpid<<"  curid "<<curID<<endl;
            if (result != 0)
            {
                P_visited[tmpid] = 1;
                sym[tmpid] = setNum;
                sym[curID] = setNum;
            }
        }
        setNum++;
    }
    //判断是否所有点都是等价点
    allEqVertex = true;
    for (P_ID i = 0; i < vertexNum_P; i++)
    {
        if (sym.count(i) == 0)
        {
            allEqVertex = false;
        }
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

    //测试
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

// PatternMatching::matchPR()

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

// P_ID PatternMatching::getMaxSel() {
//     int maxSelID = 0;
//     P_ID curid = 0;
//     bool flag = true;
//     for (; curid < vertexNum_P; curid++) {//找到id最小的模式边没有被全部访问的点
//         P_ID j = 0;
//         for (; j < vertexNum_P; j++) {
//             if (P_adj[curid][j] == 1) {
//                 flag = false;
//                 break;
//             }
//         }
//         if (!flag)
//             break;
//     }
//     maxSelID = curid;
//     for (P_ID i = 0; i < sel.size(); i++) {
//         if (sel[i] < sel[maxSelID] && !isfinish(i)) {  //匹配数越小的模式图节点选择度越大
//             maxSelID = i;
//         }
//     }
//     return maxSelID;
// }

//这里是得到当前选择度表下匹配数最少的模式图点编号并返回
P_ID PatternMatching::getMaxSel_cur(P_ID curid)
{
    int maxSelID = 0;
    int maxSel = INT_MAX;
    // P_ID curid = minMatchID;

    for (int i = 0; i < vertexNum_P; ++i)
    {
        if (P_adj[curid][i] == 1 || P_adj[i][curid])
        {
            if (sel[i] < maxSel)
            {
                maxSel = sel[i];
                maxSelID = i;
            }
        }
    }

    // bool flag = true;
    // for (; curid < vertexNum_P; curid++) {//找到id最小的模式边没有被全部访问的点
    //     P_ID j = 0;
    //     for (; j < vertexNum_P; j++) {
    //         if (P_adj[curid][j] == 1) {
    //             flag = false;
    //             break;
    //         }
    //     }
    //     if (!flag)
    //         break;
    // }
    // maxSelID = curid;
    // for (P_ID i = 0; i < sel.size(); i++) {
    //     if (sel[i] < sel[maxSelID] && !isfinish(i)) {  //匹配数越小的模式图节点选择度越大
    //         maxSelID = i;
    //     }
    // }
    if (maxSel == INT_MAX)
        return INT_MAX;
    return maxSelID;
}

void PatternMatching::fullArray(int &resultNum, vector<R_ID> cur_PMR, int cur_time, unordered_map<int, set<R_ID>> sym_PMR)
{

    if (cur_time >= vertexNum_P)
    {
        /* cout<<"curPMR: "<<endl;
        for(auto k:cur_PMR){
            cout<<k<<" ";
        }
        cout<<endl; */
        if (check_result(cur_PMR))
        {
            //测试
            /* cout << "curPMR: " << endl;
            for (auto k : cur_PMR)
            {
                cout << k << " ";
            }
            cout << endl; */

            resultNum++;
            /* Writer writer(outputfile_ptr);
            writer.DataProcessing(cur_PMR); */
        }
        return;
    }
    if (sym.count(cur_time) != 0)
    {
        //保证每次赋值的等价点对应的R_ID必须要大于之前已经赋值过的等价点对应的R_ID
        R_ID RmaxID = 0; //当前已赋值的同组等价点中最大匹配id
        int group = sym[cur_time];
        for (auto it = sym_group[group].begin(); it != sym_group[group].end(); ++it)
        {
            auto curId = *it;
            if (cur_PMR[curId] > RmaxID)
            {
                RmaxID = cur_PMR[curId];
            }
        }
        for (auto it = sym_PMR[group].begin(); it != sym_PMR[group].end(); ++it)
        {
            auto curId = *it;
            if (curId <= RmaxID)
            {
                continue;
            }
            else
            {
                cur_PMR[cur_time] = curId;
                fullArray(resultNum, cur_PMR, cur_time + 1, sym_PMR);
                cur_PMR[cur_time] = -1;
            }
        }
    }
    else
    {
        for (auto it = sym_PMR[cur_time + 100].begin(); it != sym_PMR[cur_time + 100].end(); it++)
        {
            auto curId = *it;
            cur_PMR[cur_time] = curId;
            fullArray(resultNum, cur_PMR, cur_time + 1, sym_PMR);
            cur_PMR[cur_time] = -1;
        }
    }
}

bool PatternMatching::isRecov(vector<R_ID> a, vector<R_ID> b)
{
    //判断两组结果的非等价点是否一致
    for (auto k : asym)
    {
        if (a[k] != b[k])
        {
            return false;
        }
    }
    set<R_ID> st;
    for (int i = 0; i < a.size(); ++i)
    {
        if (a[i] != INT_MAX)
        {
            st.insert(a[i]);
        }
        if (b[i] != INT_MAX)
        {
            st.insert(b[i]);
        }
    }
    if (st.size() == vertexNum_P)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//多线程核心函数
long long PatternMatching::Multithreaded_search(R_ID i, vector<vector<P_ID>> &P_adj_cp)
{
    long long Mining_result_count = 0; // parallel_reduce版本才加这一句
    std::vector<std::vector<unsigned>> PMR_initialValue;
    PMR_initialValue.resize(vertexNum_P);
    PMR_initialValue[minMatchID].emplace_back(minMatchID_PMR[i]);
    vector<int> sel_cp = sel;
    bool branchFinish = true;
    int visited_edgeNum = 0;
    int ori_centerID = minMatchID_PMR[i]; //原始中心节点ID
    bool isSymMatch = false;
    unordered_map<int, set<R_ID>> sym_PMR; //存储每个等价点组对应的匹配集合，非等价的匹配集合key为id+100用以区分等价点
    vector<vector<R_ID>> PMR_new;
    unordered_map<R_ID, int> isTraversed;
    isTraversed[ori_centerID] = 1;
    searchPG(PMR_initialValue, sel_cp, P_adj_cp, minMatchID_PMR[i], minMatchID, branchFinish, Mining_result_count, visited_edgeNum, ori_centerID, isSymMatch, sym_PMR, PMR_new, isTraversed);
    PMR_initialValue.clear();
    //测试
    /* cout<<"start id: "<<minMatchID_PMR[i]<<endl;
    for (auto vec : PMR_new)
    {
        for (auto k : vec)
        {
            cout << k << " ";
        }
        cout << endl;
    } */
    //测试
    /* for (auto it = sym_PMR.begin(); it != sym_PMR.end(); ++it)
    {
        cout << "key: " << it->first << " ";
        auto st = it->second;
        cout << "value: ";
        for (auto iter = st.begin(); iter != st.end(); ++iter)
        {
            cout << *iter << " ";
        }
        cout << endl;
    } */

    if (!sym.empty())
    {
        //全排列组合结果，性能较差
        /* vector<R_ID> cur_PMR(vertexNum_P, 0);
        int resultNum = 0;
        fullArray(resultNum, cur_PMR, 0, sym_PMR);
        return resultNum; */
        //对称重组，注意：1.所有点不重复 2.先确定非等价点

        //仅计数，但不保存结果
        long long resultNum = 0;
        for (int i = 0; i < PMR_new.size(); ++i)
        {
            //重组
            for (int j = i + 1; j < PMR_new.size(); j++)
            {
                if (isRecov(PMR_new[i], PMR_new[j]))
                {
                    resultNum++;
                }
            }
        }
        return resultNum;
    }
    else
    {
        return Mining_result_count;
    }
}

void PatternMatching::deleteSymVertex(vector<vector<P_ID>> &symP_adj, P_ID symId)
{
    int group = sym[symId];
    auto s = sym_group[group];
    for (auto it = s.begin(); it != s.end(); it++)
    {
        P_ID tmpid = *it;
        if (tmpid != symId)
        {
            for (P_ID i = 0; i < vertexNum_P; i++)
            {
                symP_adj[i][tmpid] = 0;
                symP_adj[tmpid][i] = 0;
            }
        }
    }
}

//等价环多线程函数
long long PatternMatching::eqCircle_Multithreaded_search(R_ID i)
{
    /* std::vector<std::vector<unsigned>> PMR_initialValue;
    PMR_initialValue.resize(vertexNum_P);
    PMR_initialValue[minMatchID].emplace_back(minMatchID_PMR[i]);
    vector<int> sel_cp = sel;
    bool branchFinish = true;
    int visited_edgeNum = 0;
    int ori_centerID = minMatchID_PMR[i]; //原始中心节点ID
    vector<vector<P_ID>> P_adjcp = P_adj;
    int result = 0;
    eqCircle_searchPG(PMR_initialValue, sel_cp, P_adjcp, minMatchID_PMR[i], minMatchID, branchFinish, ori_centerID, 0, result);
    return result; */
    // cout<<"oir "<<ori_centerID<<endl;
    vector<R_ID> PMR_copy;
    PMR_copy.resize(vertexNum_P);
    PMR_copy[0] = minMatchID_PMR[i];
    long long result = 0;
    unordered_map<R_ID, int> mp;
    mp[PMR_copy[0]] = 1;
    minDFS(PMR_copy, minMatchID_PMR[i], result, 0, mp);
    /*  int r_result = 0;
     maxDFS(PMR_copy, minMatchID_PMR[i], r_result, 0,mp); */
    return result;
}

void PatternMatching::searchAllPR()
{
    std::cout << "Start graph mining..." << std::endl;
    // Mining_result_count = 0; //每次挖掘开始，初始化结果计数

    // std::vector<int> minMatchID_PMR;
    for (R_ID j = 0; j < vertexNum_R; j++)
    {
        if (degree_R[j].indeg >= degree_P[minMatchID].indeg && degree_R[j].outdeg >= degree_P[minMatchID].outdeg)
        {
            minMatchID_PMR.emplace_back(j);
            // R_visited[j] = 1;
        }
    }
    //分三种情况讨论，1.带环等价点，2.非带环等价点，3.非等价点
    if (isEqCircle)
    {
        //tbb::task_scheduler_init init(1);
        long long finalAns = parallel_reduce(
            blocked_range<size_t>(0, minMatchID_PMR.size()), 0, [&](blocked_range<size_t> r, long long ans)
            {
                
        for(int i=r.begin();i!=r.end();++i){
            ans+=eqCircle_Multithreaded_search(i);
        }
        return ans; },
            plus<long long>());
        // cout << "min_size: " << minEqCircleResult.size() << endl;
        /* cout << "max_size: " << maxEqCircleResult.size() << endl;
        int cnt=0;
        for(auto k:maxEqCircleResult){
            bool hasEq=false;
            for(auto j:minEqCircleResult){
                if(isEqResult(k,j)){
                    hasEq=true;
                    break;
                }
            }
            if(hasEq==false){
                cnt++;
                for(auto i:k){
                    cout<<i<<" ";
                }
                cout<<endl;
            }
        }
        cout<<"cnt: "<<cnt<<endl; */
        std::cout << "Mining result count is : " << finalAns << std::endl;
        //单线程
        /* for (unsigned i = 0; i < minMatchID_PMR.size(); ++i)
        {
            eqCircle_Multithreaded_search(i);
            // cout<<"size:"<<eqCircleResult.size()<<endl;
        } */
        //测试
        /* for(auto it=eqCircleResult.begin();it!=eqCircleResult.end();++it){
            auto vec=*it;
            for(auto k:vec){
                cout<<k<<" ";
            }
            cout<<endl;
        } */
        // cout << "result: " << eqCircleResult.size() << endl;
    }
    else
    {
        //通过等价点削减模式图
        unordered_map<P_ID, int> lastSym;
        vector<vector<P_ID>> symP_adj = P_adj;
        //如果所有点都是等价点，保留起始点的等价点组
        int reserveGroup = 0;
        if (allEqVertex)
        {
            reserveGroup = sym[minMatchID];
        }
        for (auto it = sym_group.begin(); it != sym_group.end(); it++)
        { //保留每组等价点中的一个，其他点删除，注意保证最后得到的分支是联通的
            if (it == sym_group.begin())
            {
                if (allEqVertex&&it->first == reserveGroup)
                {
                    continue;
                }
                else
                {
                    P_ID symId = *(it->second.begin());
                    /* if(allEqVertex&&it->first==reserveGroup){
                        continue;
                    } */
                    deleteSymVertex(symP_adj, symId);
                    lastSym[symId] = 1;
                }
            }
            else
            {
                 if(allEqVertex&&it->first==reserveGroup){
                    continue;
                }
                bool isfound = false; //判断是否找到连通的等价点
                for (auto iter = it->second.begin(); iter != it->second.end(); iter++)
                {
                    auto tmp = *iter;
                    for (auto iter1 = lastSym.begin(); iter1 != lastSym.end(); iter1++)
                    {
                        auto lastSymId = iter1->first;
                        if (P_adj[tmp][lastSymId] == 1 || P_adj[lastSymId][tmp] == 1)
                        { //注意这里用到的是P_adj进行判断，因为symP_adj中的原始边已经被删除了
                            deleteSymVertex(symP_adj, tmp);
                            // lastSymId=tmp;
                            lastSym[tmp] = it->first;
                            isfound = true;
                            break;
                        }
                    }
                    if (isfound)
                        break;
                }
               /*  P_ID symId = *(it->second.begin());
                deleteSymVertex(symP_adj, symId);
                lastSym[symId] = it->first; */
            }
        }

        //存储分割后的模式图点
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            for (P_ID j = 0; j < vertexNum_P; j++)
            {
                if (symP_adj[i][j] == 1)
                {
                    bool i_exist = false, j_exist = false;
                    for (auto k : restPid)
                    {
                        if (k == i)
                        {
                            i_exist = true;
                        }
                        if (k == j)
                        {
                            j_exist = true;
                        }
                    }
                    if (!i_exist)
                    {
                        restPid.push_back(i);
                    }
                    if (!j_exist)
                    {
                        restPid.push_back(j);
                    }
                }
            }
        }
        //测试
        /* cout << "restpid:" << endl;
        for (auto k : restPid)
        {
            cout << k << " ";
        }
        cout << endl; */
        //测试
        cout << "symP_adj:" << endl;
        for (P_ID i = 0; i < vertexNum_P; i++)
        {
            for (P_ID j = 0; j < vertexNum_P; j++)
            {
                cout << symP_adj[i][j] << " ";
            }
            cout << endl;
        }
        long long finalAns = parallel_reduce(
            blocked_range<size_t>(0, minMatchID_PMR.size()), 0, [&](blocked_range<size_t> r, long long ans)
            {
                
        for(int i=r.begin();i!=r.end();++i){
            ans+=Multithreaded_search(i,symP_adj);
        }
        return ans; },
            plus<long long>());
        //单线程
        /* int finalAns = 0;
        for (int i = 0; i < minMatchID_PMR.size(); i++)
        {
            finalAns += Multithreaded_search(i, symP_adj);
        } */
        std::cout << "Mining result count is : " << finalAns << std::endl;
    }
}

void PatternMatching::minDFS(vector<R_ID> PMR_copy, R_ID ori_centerId, long long &result, int curTime, unordered_map<R_ID, int> mp)
{
    if (curTime == circleSize - 1)
    {
        for (int i = 0; i < degree_R[PMR_copy[vertexNum_P - 1]].outdeg; i++)
        {
            R_ID tmpid = R_adj[R_adjIndex[PMR_copy[vertexNum_P - 1]] + i];
            if (tmpid == PMR_copy[0])
            {
                result++;
                return;
            }
        }
        return;
    }
    for (int i = 0; i < degree_R[PMR_copy[curTime]].outdeg; i++)
    {
        R_ID tmpid = R_adj[R_adjIndex[PMR_copy[curTime]] + i];
        if (tmpid > ori_centerId && mp.count(tmpid) == 0)
        {
            PMR_copy[curTime + 1] = tmpid;
            mp[tmpid] = 1;
            minDFS(PMR_copy, ori_centerId, result, curTime + 1, mp);
            mp.erase(tmpid);
        }
    }
}

void PatternMatching::maxDFS(vector<R_ID> PMR_copy, R_ID ori_centerId, long long &result, int curTime, unordered_map<R_ID, int> mp)
{
    if (curTime == circleSize - 1)
    {
        for (int i = 0; i < degree_R[PMR_copy[vertexNum_P - 1]].outdeg; i++)
        {
            R_ID tmpid = R_adj[R_adjIndex[PMR_copy[vertexNum_P - 1]] + i];
            if (tmpid == PMR_copy[0])
            {
                //测试
                /* cout<<"ori: "<<ori_centerId<<endl;
                for(auto k:PMR_copy){
                    cout<<k<<" ";
                }
                cout<<endl; */
                // maxEqCircleResult.push_back(PMR_copy);
                result++;
                return;
            }
        }
        return;
    }
    for (int i = 0; i < degree_R[PMR_copy[curTime]].outdeg; i++)
    {
        R_ID tmpid = R_adj[R_adjIndex[PMR_copy[curTime]] + i];
        if (tmpid < ori_centerId && mp.count(tmpid) == 0)
        {
            PMR_copy[curTime + 1] = tmpid;
            mp[tmpid] = 1;
            maxDFS(PMR_copy, ori_centerId, result, curTime + 1, mp);
            mp.erase(tmpid);
        }
        /* PMR_copy[curTime + 1] = tmpid;
        DFS(PMR_copy, ori_centerId, result, curTime + 1); */
    }
}

void PatternMatching::eqCircle_searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, int ori_centerID, int visited_edgeNum, int &result)
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
    // cout<<"ori cen "<<ori_centerID<<endl;
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
        // P_ID minSelId = getMaxSel_cur(current_match_PID);
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
                eqCircle_reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum, ori_centerID);
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
                eqCircle_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum, ori_centerID);
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
                        eqCircle_searchPG(PMR_copy, sel_copy, P_adj_copy, match_RID, current_match_PID, branchFinish, ori_centerID, visited_edgeNum, result);
                    }
                    branchFinish = false;
                }
            }
        }

        /*if (!isReverse) {
            extendEdgePattern(current_match_PID,neighborID,current_match_RID);
        }
        else {
            reverse_extendEdgePattern(current_match_PID, neighborID);
        }*/
        // cout<<visited_edgeNum<<endl;
    }
    // cout<<visited_edgeNum<<endl;
    if (branchFinish == false)
    {

        return;
    }
    /* cout<<"ori "<<ori_centerID<<endl;
     for (int i = 0; i < vertexNum_P; ++i) {
        std::cout << 'P' << i << ": ";
        for (auto j : PMR_copy[i]) {
            std::cout << j << ", ";
        }
        std::cout << std::endl;
    }
    cout<<endl; */
    //转换PMR的格式
    unsigned cur_count = 1;
    for (unsigned i = 0; i < vertexNum_P; ++i)
    {
        cur_count *= PMR_copy[i].size();
    }
    result += cur_count;
    return;
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

void PatternMatching::changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<vector<R_ID>> &PMR_new, vector<R_ID> PMR_cur, int times)
{
    if (times >= restPid.size())
    {
        PMR_new.push_back(PMR_cur);
        return;
    }
    for (auto k : PMR_copy[restPid[times]])
    {
        PMR_cur[restPid[times]] = k;
        changePMRFormat(PMR_copy, PMR_new, PMR_cur, times + 1);
    }
}

void PatternMatching::changePMRFormat(vector<vector<R_ID>> PMR_copy, vector<R_ID> PMR_new, P_ID currentId)
{

    if (PMR_new.size() == circleSize)
    {
        for (auto it = eqCircleResult.cbegin(); it != eqCircleResult.cend(); ++it)
        {
            //去重，如果结果重复直接返回
            /* if((*it).size()!=3){
                for(auto k:(*it)){
                    cout<<k<<" ";
                }
                cout<<endl;
            } */
            if ((*it).size() == circleSize && isEqResult(PMR_new, *it))
            {
                return;
            }
        }
        /* int n=eqCircleResult.size();
        for(int i=0;i<n;i++){
            //去重，如果结果重复直接返回
            if(isEqResult(PMR_new,eqCircleResult[i])){
                return ;
            }
        }
        int new_n=eqCircleResult.size();
        for(int i=n;i<new_n;i++){
            //去重，如果结果重复直接返回
            if(isEqResult(PMR_new,eqCircleResult[i])){
                return ;
            }
        } */
        eqCircleResult.push_back(PMR_new);
        // cout<<"size: "<<eqCircleResult.size()<<endl;
        return;
    }
    for (auto k : PMR_copy[currentId])
    {
        PMR_new.push_back(k);
        changePMRFormat(PMR_copy, PMR_new, currentId + 1);
        PMR_new.pop_back();
    }
}

void PatternMatching::searchPG(std::vector<std::vector<unsigned>> PMR_copy, std::vector<int> sel_copy, std::vector<std::vector<P_ID>> P_adj_copy, R_ID current_match_RID, P_ID current_match_PID, bool branchFinish, long long &result, int visited_edgeNum, int ori_centerID, bool isSymMatch, unordered_map<int, set<R_ID>> &sym_PMR, vector<vector<R_ID>> &PMR_new, unordered_map<R_ID, int> isTraversed)
{
    // branchFinish = true;//初始化分支完整指标
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
        // P_ID minSelId = getMaxSel_cur(current_match_PID);
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

                /*clock_t start_reverse_extendEdgePattern, end_reverse_extendEdgePattern;
                start_reverse_extendEdgePattern = clock();
                reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy);
                end_reverse_extendEdgePattern = clock();
                std::cout << "reverse_extendEdgePattern run time for " << current_match_PID << "-" << neighborID << " and " << current_match_PID << "->" << current_match_RID << " is:" << (end_reverse_extendEdgePattern - start_reverse_extendEdgePattern) / CLK_TCK * 1000 << "ms" << std::endl;*/
                if (!isSymMatch)
                    reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum,isTraversed);
                else
                    sym_reverse_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum, isTraversed);
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
                if (!isSymMatch)
                    extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum,isTraversed);
                else
                    sym_extendEdgePattern(current_match_PID, neighborID, current_match_RID, PMR_copy, sel_copy, P_adj_copy, branchFinish, visited_edgeNum, isTraversed);
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
                        if (isTraversed.count(match_RID) == 0)
                        {
                            isTraversed[match_RID] = 1;
                            searchPG(PMR_copy, sel_copy, P_adj_copy, match_RID, current_match_PID, branchFinish, result, visited_edgeNum, ori_centerID, isSymMatch, sym_PMR, PMR_new, isTraversed);
                            isTraversed.erase(match_RID);
                        }
                    }
                    branchFinish = false;
                }
            }
        }

        /*if (!isReverse) {
            extendEdgePattern(current_match_PID,neighborID,current_match_RID);
        }
        else {
            reverse_extendEdgePattern(current_match_PID, neighborID);
        }*/
        // cout<<visited_edgeNum<<endl;
    }
    // cout<<visited_edgeNum<<endl;
    if (branchFinish == false)
    {

        return;
    }

    // while循环结束代表当前挖掘分支完全访问，可以输出当前结果，调用return回溯到递归的前一分支继续挖掘。
    /* if (!isSymMatch)
    {
        std::cout << "Current branch mining results: " << ori_centerID << std::endl;
        for (int i = 0; i < vertexNum_P; ++i)
        {
            std::cout << 'P' << i << ": ";
            for (auto j : PMR_copy[i])
            {
                std::cout << j << ", ";
            }
            std::cout << std::endl;
        }
    } */

    if (!isSymMatch)
    {
        //若P中不存在等价点，直接统计输出结果
        if (sym.empty())
        {
            /*  Writer writer(outputfile_ptr);
             writer.DataProcessing(PMR_copy); */
            /* if (check_result(PMR_copy))
            {
                unsigned cur_count = 1;
                for (unsigned i = 0; i < vertexNum_P; ++i)
                {
                    cur_count *= PMR_copy[i].size();
                }
                result += cur_count;
            } */
            /* std::cout << "Current branch mining results: " << ori_centerID << std::endl;
        for (int i = 0; i < vertexNum_P; ++i)
        {
            std::cout << 'P' << i << ": ";
            for (auto j : PMR_copy[i])
            {
                std::cout << j << ", ";
            }
            std::cout << std::endl;
        } */
            long long cur_count = 1;
            for (unsigned i = 0; i < vertexNum_P; ++i)
            {
                cur_count *= PMR_copy[i].size();
            }
            result += cur_count;
        }
        //若存在等价点，统计结果，作进一步计算
        else
        {
            vector<R_ID> PMR_cur(vertexNum_P, INT_MAX);
            changePMRFormat(PMR_copy, PMR_new, PMR_cur, 0);
            for (int i = 0; i < vertexNum_P; i++)
            {
                if (sym.count(i) == 0)
                {
                    for (auto k : PMR_copy[i])
                    {
                        sym_PMR[i + 100].insert(k); //将非等价点也存储在该集合中
                    }
                }
                else
                {
                    for (auto k : PMR_copy[i])
                    {
                        sym_PMR[sym[i]].insert(k);
                    }
                }
            }
        }
    }
    else
    {
        /* if(!check_result(PMR_copy)){
            return ;
        } */
        /*  for (int i = 0; i < vertexNum_P; ++i)
         {
             std::cout << 'P' << i << ": ";
             for (auto j : PMR_copy[i])
             {
                 std::cout << j << ", ";
             }
             std::cout << std::endl;
         } */
        long long cur_count = 1;
        for (unsigned i = 0; i < vertexNum_P; ++i)
        {
            cur_count *= PMR_copy[i].size();
        }
        result += cur_count;
    }

    return;
}

bool PatternMatching::isPMRComplete(vector<vector<P_ID>> PMR_tmp)
{
    for (int i = 0; i < vertexNum_P; i++)
    {
        if (PMR_tmp[i].size() == 0)
            return false;
    }
    return true;
}

void PatternMatching::recoverGroup(vector<vector<P_ID>> &PMR_tmp, unordered_map<int, vector<int>> symResult, int groupNum)
{
}

void PatternMatching::recoverPattern(vector<vector<P_ID>> &PMR_ori, vector<vector<P_ID>> &PMR_tmp)
{
    //预处理PMR的结果
    vector<int> groupNum; //存储所有等价点组号
    for (auto it = sym_group.begin(); it != sym_group.end(); it++)
    {
        groupNum.push_back(it->first);
    }
    unordered_map<int, vector<int>> symResult; //等价点组的匹配结果,key为等价点组号，value为该等价点组的匹配结果
    //对等价点结果进行提取，并初始化PMR_tmp
    for (int i = 0; i < vertexNum_P; i++)
    {
        if (sym.count(i) != 0 && PMR_ori[i].size() != 0)
        {
            for (int j = 0; j < PMR_ori[i].size(); j++)
            {
                symResult[sym[i]].push_back(PMR_ori[i][j]);
            }
        }
        if (sym.count(i) == 0)
        {
            PMR_tmp[i] = PMR_ori[i];
        }
    }
    //逐组恢复等价点结果
    for (int i = 0; i < groupNum.size(); i++)
    {
        recoverGroup(PMR_tmp, symResult, groupNum[i]);
    }
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
    /*if (is_empty == true) {
        std::cout << "NULL";
    }*/
    // std::cout << std::endl;

    //将求得的结果赋值给PMR_copy的当前行
    PMR_copy_oneline.swap(intersect_result);

    return is_empty;
}

//更改后的函数，计算与模式边vps, vpt端点vpt匹配的真实图顶点
void PatternMatching::extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed)
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
        if (degree_R[tmpid].indeg >= degree_P[v_pt].indeg && degree_R[tmpid].outdeg >= degree_P[v_pt].outdeg&&isTraversed.count(tmpid) == 0)
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

//重载，用于等价点检测
void PatternMatching::sym_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum, unordered_map<R_ID, int> isTraversed)
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
        if (P_adj[cur_r_vs][i] != 0 && degree_P[i].indeg == degree_P[v_pt].indeg && degree_P[i].outdeg == degree_P[v_pt].outdeg && isTraversed.count(i) == 0)
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

void PatternMatching::eqCircle_extendEdgePattern(P_ID v_ps, P_ID v_pt, R_ID cur_r_vs, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum, R_ID ori_centerId)
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
        if (degree_R[tmpid].indeg >= degree_P[v_pt].indeg && degree_R[tmpid].outdeg >= degree_P[v_pt].outdeg && tmpid > ori_centerId)
        {
            Mtemp.emplace_back(tmpid);
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

void PatternMatching::eqCircle_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum, R_ID ori_centerId)
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
    /* if(R_visited[cur_r_vt]==1&&cur_r_vt>ori_centerID)
    return;  */
    std::vector<R_ID> Mtemp;
    //注意，如果代码运行结果存在逻辑错误，请检查此处
    for (int i = 0; i < degree_R[cur_r_vt].indeg; ++i)
    {
        R_ID tmpid = R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i];
        if (degree_R[tmpid].indeg >= degree_P[v_ps].indeg && degree_R[tmpid].outdeg >= degree_P[v_ps].outdeg && tmpid > ori_centerId)
        {
            Mtemp.emplace_back(tmpid);
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

//逆向扩展第三版
void PatternMatching::reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum,unordered_map<R_ID,int> isTraversed)
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
    /* if(R_visited[cur_r_vt]==1&&cur_r_vt>ori_centerID)
    return;  */
    std::vector<R_ID> Mtemp;
    //注意，如果代码运行结果存在逻辑错误，请检查此处
    for (int i = 0; i < degree_R[cur_r_vt].indeg; ++i)
    {
        R_ID tmpid = R_reverse_adj[R_reverseAdjIndex[cur_r_vt] + i];
        if (degree_R[tmpid].indeg >= degree_P[v_ps].indeg && degree_R[tmpid].outdeg >= degree_P[v_ps].outdeg&&isTraversed.count(tmpid) == 0)
        {
            Mtemp.emplace_back(tmpid);
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

//重载版本，用于检测对称点
void PatternMatching::sym_reverse_extendEdgePattern(P_ID v_pt, P_ID v_ps, R_ID cur_r_vt, std::vector<std::vector<unsigned>> &PMR_copy, std::vector<int> &sel_copy, std::vector<std::vector<P_ID>> &P_adj, bool &branchFinish, int &visited_edgeNum, unordered_map<R_ID, int> isTraversed)
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
    /* if(R_visited[cur_r_vt]==1&&cur_r_vt>ori_centerID)
    return;  */
    std::vector<R_ID> Mtemp;
    for (int i = 0; i < vertexNum_P; i++)
    {
        if (P_adj[i][cur_r_vt] != 0 && degree_P[i].indeg == degree_P[v_ps].indeg && degree_P[i].outdeg == degree_P[v_ps].outdeg && isTraversed.count(i) == 0)
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

bool PatternMatching::check_result(std::vector<unsigned> PMR_copy)
{
    // cout<<"----------"<<endl;
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
                        // cout<<"P_i:"<<i<<" "<<"P_j:"<<j<<"   "<<"R_i:"<<R_i<<" "<<"R_j:"<<R_j<<endl;
                        find_v = true;
                        break;
                    }
                }
                if (!find_v)
                {
                    // std::cout << "An incorrect result was found: R_vertex" << R_i << " -> R_vertex" << R_j << " don't have an edge." << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}
// sky211118
bool PatternMatching::check_result(std::vector<std::vector<unsigned>> &PMR_copy)
{
    //先检查挖掘结果是否有结点映射为空集合
    for (auto P_i : PMR_copy)
    {
        if (P_i.size() == 0)
        {
            std::cout << "There is an empty mapping set." << std::endl;
            for (int i = 0; i < vertexNum_P; ++i)
            {
                std::cout << 'P' << i << ": ";
                for (auto j : PMR_copy[i])
                {
                    std::cout << j << ", ";
                }
                std::cout << std::endl;
            }
            return false;
        }
    }
    //接下来从模式图中取边，判断挖掘结果中相应点间是否存在该边
    for (P_ID i = 0; i < vertexNum_P; ++i)
    {
        for (P_ID j = 0; j < vertexNum_P; ++j)
        {
            if (P_adj[i][j] == 1)
            {
                //如果i点和j点之间有边，那么我们就需要判断PMR_copy[i]与PMR_copy[j]两个集合中的实际图结点间是否有边
                for (auto R_i : PMR_copy[i])
                {
                    for (auto R_j : PMR_copy[j])
                    {
                        //注意R_adj是用一维数组加索引存储的
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
                            std::cout << "An incorrect result was found: R_vertex" << R_i << " -> R_vertex" << R_j << " don't have an edge." << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
    //当前分支判断结束，输出
    // std::cout << "Current branch check is complete." << std::endl;
}
bool PatternMatching::get_Radj_Index(R_ID v_r, int &start, int &end)
{
    if (v_r == vertexNum_R - 1)
    {
        start = R_adjIndex[v_r];
        // end = R_adjSize;
        end = edgeNum_R;
    }
    else
    {
        start = R_adjIndex[v_r];
        end = R_adjIndex[v_r + 1];
    }
    return true;
}

//输出函数，将筛选出的结果集进行组合后以文本格式输出，输出文件名：out.txt
/*
void PatternMatching::output(string Dir) {
    ofstream ofile(Dir + "out.txt", ios::out);
}
*/

//打印PMR，用于测试
void PatternMatching::print_PMR()
{
    std::cout << "Print current PMR collection." << std::endl;
    int row = 0;
    for (auto i : PMR)
    {
        std::cout << "V_p num: " << row << "    PMR nums: ";
        for (auto j : i)
        {
            std::cout << j << " ";
        }
        row++;
        std::cout << std::endl;
    }
    std::cout << "Finish print PMR collection" << std::endl;
}

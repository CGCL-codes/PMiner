#include <ctime>
#include <sys/time.h>
#include "PMiner.h"

// g++ main.cpp PMcircleFast.cpp -ltbb -o main
int main(int argc, char* argv[]) {
    //sky211116
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <Output Dir> <ThreadNum>\n", argv[0]);   
        return -1;
    }

    //用于内存消耗测试，读入起始点地址
    // if (argc != 8) {
    //     fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <Output Dir> <ThreadNum> <startsIdsfile>\n", argv[0]);   
    //     return -1;
    // }
    
    std::string R_input_file = argv[1];
    std::string P_input_file = argv[2];
    unsigned int R_vnums = atoi(argv[3]);
    unsigned int P_vnums = atoi(argv[4]);
    std::string Output_dir = argv[5];//sky211116
    int ThreadNum = atoi(argv[6]);
    // string startsIdsfile = argv[7]; //用于内存消耗测试，读入起始点地址
    //std::string flag=argv[6];//模式图的最小匹配节点是否有等价（对称）点，若有flag为true，否则flag为false
  cout<<"==============================================================================="<<endl;

  // cout<<R_input_file<<endl;
  // cout<<P_input_file<<endl;

  // R_input_file = "../../dataset/Orkut.txt";
	PatternMatching* pm = new PatternMatching();
  // pm->startsIdsfilename = startsIdsfile; //用于内存消耗测试，读入起始点地址
  struct timeval load_start_time, load_end_time, load_tmp_start, load_tmp_end;
  gettimeofday(&load_start_time, NULL);//起始时间
  // pm->build_degree_Rs(R_input_file, R_vnums);
	// pm->build_R_adjs(R_input_file);
  pm->build_degree_R(R_input_file, R_vnums);
	pm->build_R_adj(R_input_file);
	gettimeofday(&load_tmp_end, NULL);
	pm->build_P_adj(P_input_file, P_vnums);
	std::cout << "load data time:" << ((load_tmp_end.tv_sec - load_start_time.tv_sec) * 1000000 + 
  (load_tmp_end.tv_usec - load_start_time.tv_usec)) / 1000000.0 << " s" << std::endl;

  struct timeval pre_start_time, pre_end_time, pre_tmp_start, pre_tmp_end;
  gettimeofday(&pre_start_time, NULL);//起始时间
	pm->init(Output_dir, ThreadNum);//sky211116
  pm->matchPR_expand();
  pm->findSym();
  pm->build_constraint();
  pm->build_center_order();
  gettimeofday(&pre_tmp_end, NULL);
	std::cout << "pre produce time:" << ((pre_tmp_end.tv_sec - pre_start_time.tv_sec) * 1000000 + 
  (pre_tmp_end.tv_usec - pre_start_time.tv_usec)) / 1000000.0 << " s" << std::endl;

  // cout<<"{"<<R_input_file<<"}"<<endl;
  // cout<<"["<<P_input_file<<"]"<<endl;
  cout<<R_input_file<<endl;
  cout<<P_input_file<<endl;

  // 打印挖掘开始时间
  // time_t now = time(nullptr);  
  // tm* curr_tm = gmtime(&now);  // 返回的结构体存储位置未知，不知何时释放，因此推荐使用安全版本。  
  // char time[80] = {0};  
  // strftime(time, 80, "%Y-%m-%d %H:%M:%S", curr_tm);  
  // cout<< "Start mining time: "<<time<<endl;

  // char* curr_time = ctime(&now); cout << curr_time <<endl;



  struct timeval start_time, end_time, tmp_start, tmp_end;
  gettimeofday(&start_time, NULL);//起始时间
  // pm->isEqCircle = false;
  if(pm->isEqCircle){
    pm->searchALLCircle();
  }else{
    pm->searchAllPR();
  }
	gettimeofday(&tmp_end, NULL);
	std::cout << "graph mining time elapse:" << ((tmp_end.tv_sec - start_time.tv_sec) * 1000000 + (tmp_end.tv_usec - start_time.tv_usec)) / 1000000.0 << " s" << std::endl;

	return 0;
}


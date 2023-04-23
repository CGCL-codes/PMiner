#include <ctime>
#include <sys/time.h>
#include "PMiner.h"

int main(int argc, char* argv[]) {
    //sky211116
    if (argc != 6) {
        // ./bin/count ./data/Wiki-Vote.txt ./pattern/8_p.txt 8298 8 28
        fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum>\n", argv[0]);   
        return -1;
    }

    // 输出
    // if (argc != 7) {
    //     fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum> <Output Dir>\n", argv[0]);   
    //     return -1;
    // }

    //用于内存消耗测试，读入起始点地址
    // if (argc != 8) {
    //     fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <ThreadNum> <Output Dir> <startsIdsfile>\n", argv[0]);   
    //     return -1;
    // }
    
    std::string R_input_file = argv[1];
    std::string P_input_file = argv[2];
    unsigned int R_vnums = atoi(argv[3]);
    unsigned int P_vnums = atoi(argv[4]);
    int ThreadNum = atoi(argv[5]);
    std::string Output_dir = "";
    // std::string Output_dir = argv[6];//sky211116
    // string startsIdsfile = argv[7]; //用于内存消耗测试，读入起始点地址
    //std::string flag=argv[6];//模式图的最小匹配节点是否有等价（对称）点，若有flag为true，否则flag为false
  std::cout<<"==============================================================================="<<std::endl;


	PMiner* pm = new PMiner();
  // pm->startsIdsfilename = startsIdsfile; //用于内存消耗测试，读入起始点地址
  pm->build_degree_R(R_input_file, R_vnums);
	pm->build_R_adj(R_input_file);
	pm->build_P_adj(P_input_file, P_vnums);
	pm->init(Output_dir, ThreadNum);//sky211116
  pm->matchPR_expand();
  pm->findSym();
  pm->build_constraint();
  pm->build_center_order();

  std::cout<<R_input_file<<std::endl;
  std::cout<<P_input_file<<std::endl;

  struct timeval start_time, end_time, tmp_start, tmp_end;
  gettimeofday(&start_time, NULL);
  if(pm->isEqCircle){
    pm->searchALLCircle();
  }else{
    pm->searchAllPR();
  }
	gettimeofday(&tmp_end, NULL);
	std::cout << "graph mining time elapse:" << ((tmp_end.tv_sec - start_time.tv_sec) * 1000000 + (tmp_end.tv_usec - start_time.tv_usec)) / 1000000.0 << " s" << std::endl;
	return 0;
}


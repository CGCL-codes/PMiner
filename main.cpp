#include <ctime>
#include <sys/time.h>
#include "PatternMatching.h"
int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		fprintf(stderr, "Usage: %s <RDataset> <PDataset> <RVertex Number> <PVertex Number> <Output Dir>\n", argv[0]);
		return -1;
	}

	std::string R_input_file = argv[1];
	std::string P_input_file = argv[2];
	unsigned int R_vnums = atoi(argv[3]);
	unsigned int P_vnums = atoi(argv[4]);
	std::string Output_dir = argv[5];

	PatternMatching *pm = new PatternMatching();
	pm->build_degree_R(R_input_file, R_vnums);
	pm->build_R_adj(R_input_file);
	pm->build_P_adj(P_input_file, P_vnums);
	pm->init(Output_dir);
	pm->findSym();
	pm->matchPR_expand();

	if (!pm->isEqCircle)
	{
		pm->Find_Constaint_Inclusion_Vertices();
		pm->Split_Pattern_Graph();
	}

	struct timeval start_time, end_time, tmp_start, tmp_end;
	gettimeofday(&start_time, NULL); //起始时间
	pm->searchAllPR();
	gettimeofday(&tmp_end, NULL);
	std::cout << "graph mining time elapse:" << ((tmp_end.tv_sec - start_time.tv_sec) * 1000000 + (tmp_end.tv_usec - start_time.tv_usec)) / 1000000.0 << " s" << std::endl;
	return 0;
}

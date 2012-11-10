// IPCA.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	std::string dir = "C:/develop/xampp/htdocs/VolatileTwit/data/";
	std::string base = dir + "zeta/";
	std::string filter = dir + "keywords_filter.idf.zeta.csv";
	std::string list = dir + "status_list.zeta.csv";
	std::string marked = dir + "status_list.mark.zeta.csv";
	std::string cluster = dir + "cluster.csv";
	std::string evaluate = dir + "evaluate.csv";

	CLearningMachine *learner = new CLearningMachine();
	learner->SetBaseDir(base.c_str());
	learner->SetFilter(filter.c_str());

	learner->Learn(marked.c_str());	//複数回
//	learner->Cluster(list.c_str(), cluster.c_str());
//	learner->Evaluate(list.c_str(), 1, evaluate.c_str());
	return 0;
}


// IPCA.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CLearningMachine *learner = new CLearningMachine();
	learner->SetBaseDir("../data/test/");
	learner->SetFilter("../data/keywords_filter.idf.delta.csv");

	learner->Learn("../data/status_list.mark.test.csv");
	return 0;
}


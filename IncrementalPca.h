#pragma once
#include "stdafx.h"

class CIncrementalPca
{
private:
	int m_MainMax, m_FrameNow;
	int m_Length;
//	int m_Height, m_Width;
	float m_Amnesic;

	float* m_ArrImgMain;//!<主要ベクトル。でもArr[0]は擬似平均ベクトルか。
	float* m_ArrImgSub;//!< Run()が呼び出されるごとに確保するのも処理が重くなるので

	int m_ReflectTarget;
	float *m_ReflectNormal;
	float *m_ReflectLine;

	float *m_VecA;
	float *m_ImgA;

	long m_ClockPre;

public:
	CIncrementalPca(int mainMax=32, int length=76800, float amnesic=0.0f);
	~CIncrementalPca(void);

	int GetLength(){ return m_Length; }
	int GetMainMax(){ return m_MainMax; }

	void Learn(float* imgSrc);
	void ProjectPCA(float* imgSrc, float* vecRes);
	void BackProjectPCA(float* vecRes, float* imgDst);
	float ReflectProject(float* imgSrc, int target);

	void SaveBinMainSub( char* dir );
	void LoadBinMainSub( char* dir );

private:
	float *GetImgMain(int i){ return m_ArrImgMain + m_Length*i; }
	float *GetImgSub(int i){ return m_ArrImgSub + m_Length*i; }

	void SaveMinMax( char* dir );
	void GetImgMinMax(float *img, float &min, float &max);

	void SetupReflectNormal();
	float *MakeReflectLine( int target );
public:
	void ArrCopy(float *in, float *out){ memcpy(out, in, sizeof(float)*m_Length); }
	void ArrMul(float *in1, float *in2, float *out );
	double ArrDot(float *in1, float *in2);
	double ArrNorm(float *in1){ return sqrt( ArrDot(in1, in1) ); }
	void ArrAdd(float *in1, float scale1, float *in2, float scale2, float *out );
	void ArrSub(float *in1, float *in2, float *out );
};

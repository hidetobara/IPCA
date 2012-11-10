#include "stdafx.h"

CIncrementalPca::CIncrementalPca(int mainMax, int length, float amnesic)
{
	m_MainMax = mainMax;
	m_Length = length;
	m_Amnesic = amnesic;
	if( m_Amnesic < 0.0f ) m_Amnesic = 0.0f;
	if( m_MainMax < 1 ) m_MainMax = 2;

	int sizeImg = m_Length;
	int sizeArr = sizeImg * m_MainMax;
	m_ArrImgMain = new float[sizeArr];
	m_ArrImgSub = new float[sizeArr];

	m_ReflectTarget = 0;
	m_ReflectNormal = NULL;
	m_ReflectLine = NULL;

	m_ImgA = new float[sizeImg];
	m_VecA = new float[m_MainMax];

	m_FrameNow = 0;
	m_ClockPre = 0L;
}
CIncrementalPca::~CIncrementalPca(void)
{
	delete [] m_ArrImgMain;
	delete [] m_ArrImgSub;
	m_ArrImgMain = NULL;
	m_ArrImgSub = NULL;

	if(m_ReflectNormal != NULL) delete [] m_ReflectNormal;
	if(m_ReflectLine != NULL) delete [] m_ReflectLine;
	m_ReflectNormal = NULL;
	m_ReflectLine = NULL;

	delete [] m_ImgA;
	m_ImgA = NULL;
	delete [] m_VecA;
	m_VecA = NULL;
}

void CIncrementalPca::Learn(float* imgSrc)
{
	ArrCopy(imgSrc, GetImgSub(0) );

	int iterateMax = m_MainMax -1;
	if( m_MainMax > m_FrameNow ) iterateMax = m_FrameNow;

	double scalerA, scalerB, scalerC;
	double nrmV;

	for( int i=0; i<=iterateMax; i++ )
	{
		if( i == m_FrameNow ){
			ArrCopy( GetImgSub(i), GetImgMain(i) );
			continue;
		}

		///// Vi(n) = [a= (n-1-l)/n * Vi(n-1)] + [b= (1+l)/n * Ui(n)T Vi(n-1)/|Vi(n-1)| * Ui(n) ]
		nrmV = ArrNorm( GetImgMain(i) );

		scalerA = (double)(m_FrameNow - 1.0 - m_Amnesic) / (double)m_FrameNow;

		double dotUV = ArrDot( GetImgSub(i), GetImgMain(i) );
		scalerB = ((double)(1+m_Amnesic) * dotUV) / ((double)m_FrameNow * nrmV );

		ArrAdd( GetImgMain(i), (float)scalerA, GetImgSub(i), (float)scalerB, GetImgMain(i) );

		///// Ui+1(n) = Ui(n) - [c= Ui(n)T Vi(n)/|Vi(n)| * Vi(n)/|Vi(n)| ]
		if( i == iterateMax ) continue;

		nrmV = ArrNorm( GetImgMain(i) );
		dotUV = ArrDot( GetImgSub(i), GetImgMain(i) );
		scalerC = dotUV / (nrmV * nrmV);
		ArrAdd( GetImgSub(i), 1.0f, GetImgMain(i), (float)-scalerC, GetImgSub(i+1) );
	}

	if( m_FrameNow % 1000 == 0 )
	{
		long clockNow = clock();
		if( m_ClockPre > 0 )
		{
			float sec = (float)(clockNow-m_ClockPre)/(float)CLOCKS_PER_SEC;
			printf( "%d frames: %2.2f fps\n", m_FrameNow, 1000.0f/sec );
		}
		m_ClockPre = clockNow;
	}
	m_FrameNow++;//!<どれくらいまで大きくなるのだろう？
}

void CIncrementalPca::ProjectPCA(float* imgSrc, float* vecRes)
{
	///// 平均を引いて
	ArrSub( imgSrc, GetImgMain(0), m_ImgA );

	///// 掛け算
	vecRes[ 0 ] = 0.0f;
	for( int c=1; c<m_MainMax; c++ )
	{
		double amt = 0.0;
		float *pM = GetImgMain(c);
		float *pT = m_ImgA;
		float *pMEnd = pM + m_Length;

		while( pM < pMEnd )
		{
			amt += (double)(*pM) * (double)(*pT);
			pM++;
			pT++;
		}
		vecRes[ c ] = (float)amt;
	}
}

void CIncrementalPca::BackProjectPCA(float* vecRes, float* imgDst)
{
	///// 各固有ベクトルの長さを求める
	m_VecA[0] = 1.0f;
	for( int c=1; c<m_MainMax; c++ )
	{
		double amt = 0.0;
		float *p = GetImgMain(c);
		float *pEnd = GetImgMain(c) + m_Length;
		while( p < pEnd )
		{
			amt += (double)(*p) * (double)(*p);
			p++;
		}

		if( amt > 0.0 ) m_VecA[ c ] = 1.0f * vecRes[ c ] / (float)amt;
		else m_VecA[ c ] = 0.0f;
	}

	float *pD = imgDst;
	for( int l=0; l<m_Length; l++, pD++ )
	{
		double amt = 0.0;
		for( int c=0; c<m_MainMax; c++ )
		{
			float *pM = GetImgMain(c) + l;
			amt += (double)(*pM) * (double)m_VecA[c];
		}
		*pD = (float)amt;
	}
}

float CIncrementalPca::ReflectProject(float* imgSrc, int target)
{
	if( m_ReflectTarget != 0 && m_ReflectLine == NULL )
	{
		SetupReflectNormal();
		MakeReflectLine( target );
	}

	double amt = GetImgMain(0)[target];
	for( int i = 0; i < m_Length; i++ )
	{
		amt += (double)(m_ReflectLine[i] * (imgSrc[i]-GetImgMain(0)[i]));
	}
	return (float)amt;
}

void CIncrementalPca::SetupReflectNormal()
{
	if( m_ReflectNormal != NULL ) return;
	m_ReflectNormal = new float[ GetMainMax() ];
	m_ReflectNormal[0] = 1.0f;

	for( int m = 1; m < m_MainMax; m++ )
	{
		float *pM = GetImgMain(m);
		double amt = 0.0;
		for( int i = 0; i < m_Length; i++ )
		{
			amt += (double)(pM[ i ] * pM[ i ]);
		}

		if( amt > 0.0 ) m_ReflectNormal[ m ] = (float)(1.0 / amt);
		else m_ReflectNormal[ m ] = 0.0;
	}
}

float *CIncrementalPca::MakeReflectLine( int target )
{
	if( m_ReflectLine != NULL ) return m_ReflectLine;
	m_ReflectLine = new float[m_Length];
	for( int i = 0; i < m_Length; i++ )
	{
		double amt = 0.0;
		for( int m = 0; m < m_MainMax; m++ )
		{
			amt += (double)(m_ReflectNormal[ m ] * GetImgMain(m)[ target ] * GetImgMain(m)[ i ]);
		}
		m_ReflectLine[ i ] = (float)amt;
	}
	return m_ReflectLine;
}

void CIncrementalPca::SaveBinMainSub( char* dir )
{
	FILE *f;
	char path[256];
	int sizeArr = m_Length*m_MainMax;

	int serialized[ 3 ] = { m_MainMax, m_Length, m_FrameNow };
	sprintf( path, "%sipcaConf.bin", dir );
	f = fopen( path, "wb" );
	fwrite( serialized, sizeof(int), 3, f );
	fclose( f );

	sprintf( path, "%sipcaMain.bin", dir );
	f = fopen( path, "wb" );
	fwrite( m_ArrImgMain, sizeof(float), sizeArr, f );
	fclose( f );

	printf( "INFO: size=%d, [%f,%f,%f,%f,...]", sizeof(float), m_ArrImgMain[0], m_ArrImgMain[1], m_ArrImgMain[2], m_ArrImgMain[3] );

	sprintf( path, "%sipcaSub.bin", dir );
	f = fopen( path, "wb" );
	fwrite( m_ArrImgSub, sizeof(float), sizeArr, f );
	fclose( f );

	SaveMinMax( dir );
}
void CIncrementalPca::LoadBinMainSub( char* dir )
{
	FILE *f;
	char path[256];
	int sizeArr = m_Length*m_MainMax;

	int serialized[ 3 ];
	sprintf( path, "%sipcaConf.bin", dir );
	f = fopen( path, "rb" );
	if( f == NULL )
	{
		printf("WARN: No conf file\n");
		return;
	}

	fread( serialized, sizeof(int), 3, f );
	fclose( f );
	if( serialized[0] != m_MainMax || serialized[1] != m_Length )
	{
		printf("ERROR: Main=%d, Length=%d\n", m_MainMax, m_Length);
		return;
	}
	m_FrameNow = serialized[2];

	sprintf( path, "%sipcaMain.bin", dir );
	f = fopen( path, "rb" );
	fread( m_ArrImgMain, sizeof(float), sizeArr, f );
	fclose( f );

	sprintf( path, "%sipcaSub.bin", dir );
	f = fopen( path, "rb" );
	fread( m_ArrImgSub, sizeof(float), sizeArr, f );
	fclose( f );
}

void CIncrementalPca::SaveMinMax( char *dir )
{
	char path[256];
	sprintf( path, "%smainMax.csv", dir );
	FILE *fout = fopen( path, "w" );

	for( int m = 0; m < m_MainMax; m++ )
	{
		float min, max;
		GetImgMinMax( GetImgMain(m), min, max );
		fprintf( fout, "%d,%0.08f,%0.08f\n", m, min, max );
	}
	fclose( fout );
}

void CIncrementalPca::GetImgMinMax(float *img, float &min, float &max)
{
	min = 0.0f;
	max = 0.0f;
	float *pEnd = img + m_Length;
	for( float *p = img; p < pEnd; p++ )
	{
		if( min < *p ) min = *p;
		if( max > *p ) max = *p;
	}
}

void CIncrementalPca::ArrMul(float *in1, float *in2, float *out )
{
	float *pEnd1 = in1 + m_Length;
	for(float *p1=in1, *p2=in2, *po=out; p1<pEnd1; p1++, p2++, po++){
		*po = (*p1) * (*p2);
	}
}

double CIncrementalPca::ArrDot(float *in1, float *in2)
{
	double amt = 0.0;
	float *pEnd1 = in1 + m_Length;
	for(float *p1=in1, *p2=in2; p1<pEnd1; p1++, p2++){
		amt += (double)(*p1) * (*p2);
	}
	return amt;
}

void CIncrementalPca::ArrAdd(float *in1, float scale1, float *in2, float scale2, float *out )
{
	float *pEnd1 = in1 + m_Length;
	for(float *p1=in1, *p2=in2, *po=out; p1<pEnd1; p1++, p2++, po++){
		*po = (*p1) * scale1 + (*p2) * scale2;
	}
}

void CIncrementalPca::ArrSub(float *in1, float *in2, float *out )
{
	float *pEnd1 = in1 + m_Length;
	for(float *p1=in1, *p2=in2, *po=out; p1<pEnd1; p1++, p2++, po++){
		*po = (*p1) - (*p2);
	}
}

#include "StdAfx.h"
#include "LearningMachine.h"

CLearningMachine::CLearningMachine(void)
{
	m_ColElementsStart = 3;
	m_Count = 0;
	m_ClockPrevious = 0;
	m_Ipca = new CIncrementalPca();
	m_DirBase[0] = 0;
	m_PathFilter[0] = 0;
}

CLearningMachine::~CLearningMachine(void)
{
	delete m_Ipca;
}

CLearnSpace* CLearningMachine::SetupFilter()
{
	CLearnSpace *spaceFilter = NULL;
	FILE *fin = fopen( m_PathFilter, "r" );
	if( fin != NULL )
	{
		spaceFilter = new CLearnSpace(m_Ipca->GetLength());
		spaceFilter->LoadImage_1Line1Element( fin, 0, 1 );
		fclose( fin );
	}
	return spaceFilter;
}

void CLearningMachine::SetupTimer()
{
	m_Count = 0;
	m_ClockPrevious = 0;
}
void CLearningMachine::TickTimer( int border )
{
	if( m_Count > 0 && m_Count % border == 0 )
	{
		long clockNow = clock();
		float sec = (float)( clockNow - m_ClockPrevious ) / (float)CLOCKS_PER_SEC;
		printf( "\t%d frames : %2.2f fps\n", m_Count, (float)border/sec );
		m_ClockPrevious = clockNow;
	}
	m_Count++;
}

bool CLearningMachine::Learn( char *pathInput )
{
	FILE *fin;
	CLearnSpace *spaceFilter = SetupFilter();

	fin = fopen( pathInput, "r" );
	if( fin == NULL ) return false;

	m_Ipca->LoadBinMainSub( m_DirBase );
	CLearnSpace *spaceInput = new CLearnSpace(m_Ipca->GetLength());
	SetupTimer();
	while( spaceInput->LoadImage_1LineElements( fin, m_ColElementsStart ) )
	{
		if( spaceFilter != NULL ) m_Ipca->ArrMul( spaceInput->m_Data, spaceFilter->m_Data, spaceInput->m_Data );
		m_Ipca->Learn( spaceInput->m_Data );
		TickTimer();
	}
	fclose( fin );

	m_Ipca->SaveBinMainSub( m_DirBase );

	if( spaceFilter != NULL ) delete spaceFilter;
	if( spaceInput != NULL ) delete spaceInput;
	return true;
}

bool CLearningMachine::Cluster( char *pathInput, char *pathCluster )
{
	FILE *fin, *fout;
	CLearnSpace *spaceFilter = SetupFilter();

	fin = fopen( pathInput, "r" );
	if( fin == NULL ) return false;
	fout = fopen( pathCluster, "w" );
	if( fout == NULL ) return false;

	m_Ipca->LoadBinMainSub( m_DirBase );

	CLearnSpace *spaceInput = new CLearnSpace(m_Ipca->GetLength());
	float vecRes[ 32 ];
	SetupTimer();
	while( spaceInput->LoadImage_1LineElements( fin, m_ColElementsStart ) )
	{
		if( spaceFilter != NULL ) m_Ipca->ArrMul( spaceInput->m_Data, spaceFilter->m_Data, spaceInput->m_Data );

		m_Ipca->ProjectPCA( spaceInput->m_Data, vecRes );
		fprintf( fout, "%s", spaceInput->GetName() );
		for(int m = 1; m < m_Ipca->GetMainMax(); m++) fprintf( fout, ",%0.08f", vecRes[m] );
		fprintf( fout, "\n" );

		TickTimer();
	}
	fclose( fin );
	fclose( fout );

	if( spaceFilter != NULL ) delete spaceFilter;
	if( spaceInput != NULL ) delete spaceInput;
	return true;
}

bool CLearningMachine::Evaluate( char *pathInput, int target, char *pathEvaluate )
{
	FILE *fin, *fout;
	CLearnSpace *spaceFilter = SetupFilter();

	fin = fopen( pathInput, "r" );
	if( fin == NULL ) return false;
	fout = fopen( pathEvaluate, "w" );
	if( fout == NULL ) return false;

	m_Ipca->LoadBinMainSub( m_DirBase );

	CLearnSpace *spaceInput = new CLearnSpace(m_Ipca->GetLength());
	SetupTimer();
	while( spaceInput->LoadImage_1LineElements( fin, m_ColElementsStart ) )
	{
		if( spaceFilter != NULL ) m_Ipca->ArrMul( spaceInput->m_Data, spaceFilter->m_Data, spaceInput->m_Data );

		float evaluate = m_Ipca->ReflectProject( spaceInput->m_Data, target );
		fprintf( fout, "%s,%0.08f\n", spaceInput->GetName(), evaluate );
		TickTimer();
	}
	fclose( fin );
	fclose( fout );

	if( spaceFilter != NULL ) delete spaceFilter;
	if( spaceInput != NULL ) delete spaceInput;
	return true;
}

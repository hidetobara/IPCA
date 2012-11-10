#pragma once

class CLearningMachine
{
private:
	int m_ColElementsStart;

	int m_Count;
	long m_ClockPrevious;

	CIncrementalPca *m_Ipca;
	char m_DirBase[256];
	char m_PathFilter[256];

	CLearnSpace* SetupFilter();
	void SetupTimer();
	bool TickTimer( int border=1000 );

public:
	CLearningMachine(void);
	~CLearningMachine(void);

	void SetBaseDir( const char *dir ){ strncpy( m_DirBase, dir, 256 ); }
	void SetFilter( const char *pathFilter ){ strncpy( m_PathFilter, pathFilter, 256 ); }

	bool Learn( const char *pathInput );
	bool Cluster( const char *pathInput, const char *pathCluster );
	bool Evaluate( const char *pathInput, int target, const char *pathEvaluate );
};

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
	void TickTimer( int border=1000 );

public:
	CLearningMachine(void);
	~CLearningMachine(void);

	void SetBaseDir( char *dir ){ strncpy( m_DirBase, dir, 256 ); }
	void SetFilter( char *pathFilter ){ strncpy( m_PathFilter, pathFilter, 256 ); }

	bool Learn( char *pathInput );
	bool Cluster( char *pathInput, char *pathCluster );
	bool Evaluate( char *pathInput, int target, char *pathEvaluate );
};

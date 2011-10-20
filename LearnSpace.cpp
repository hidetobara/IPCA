#include "StdAfx.h"
#include "LearnSpace.h"

CLearnSpace::CLearnSpace(int length)
{
	m_Length = length;
	m_Data = new float[m_Length];
}

CLearnSpace::~CLearnSpace(void)
{
	delete [] m_Data;
}

void CLearnSpace::Clear( float v )
{
	float *pEnd = m_Data + m_Length;
	*pEnd = 1.0f;
	for( float *p=m_Data+1; p<pEnd; p++ ) *p = v;
}

void CLearnSpace::Copy( float *data )
{
	memcpy( m_Data, data, m_Length*sizeof(float) );
}

bool CLearnSpace::LoadImage_1Line1Element( FILE *fin, int colkey, int colval )
{
	char line[1024];
	char *arr[8];

	Clear( 0.0f );

	while( fgets( line, 1024, fin ) != NULL )
	{
		if( line[0] == '#' || line[0] == '\n' )
		{
			if( strncmp(line, "#END",4)==0 ) return true;
			continue;
		}

		int cnt = CCharUtil::SplitComma( line, arr, 8 );
		if( cnt <= colkey || cnt <= colval ) continue;

		int key = atoi(arr[colkey]);
		float value = (float)atof(arr[colval]);
		m_Data[ key ] = value;
	}
	return false;
}

bool CLearnSpace::LoadImage_1LineElements( FILE *fin, int colval /*, int time*/ )
{
	char line[1024];
	char *arr[256];
	if( fgets( line, 1024, fin ) == NULL ) return false;
	if( CCharUtil::SplitComma( line, arr, 256 ) < colval+1 ) return true;

	///// first
	Clear( 0.0f );
	///// name, index
	///// values
	for( int i=colval; arr[i] != 0; i++ )
	{
		int key = atoi(arr[i]);
		m_Data[ key ] = 1.0f;
	}

	return true;
}

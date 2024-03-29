#pragma once

class CLearnSpace
{
public:
	char m_Name[256];
	int m_Length;
	float *m_Data;

public:
	CLearnSpace(int length);
	~CLearnSpace(void);

	char *GetName(){ return m_Name; }

	void Clear( float v=0.0f );
	void Copy( float *data );
	bool LoadImage_1Line1Element( FILE *fin, int colkey, int colval );
	bool LoadImage_1LineElements( FILE *fin, int colval );
};

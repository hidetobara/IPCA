#pragma once

class CCharUtil
{
public:
	static int SplitComma( char* line, char** arr, int limit=3 );
	static bool RetrieveKeyValue( char* line, int& key, int& value );
};

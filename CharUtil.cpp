#include "StdAfx.h"
#include "CharUtil.h"


int CCharUtil::SplitComma( char* line, char** arr, int limit )
{
	if( line[0] == 0 ) return 0;
	arr[0] = line;
	int cnt = 1;

	char *c = line;
	while( *c != 0 ){
		if( *c == ',' ){ if( c[1] != 0 ){ arr[cnt] = c+1; } c[0]=0; cnt++; }
		if( limit <= cnt ) break;
		c++;
	}

	arr[cnt] = 0;
	return cnt;
}

bool CCharUtil::RetrieveKeyValue( char* line, int& key, int& value )
{
	char *strKey = line, *strValue = NULL;
	key = 0;
	value = 0;

	char *c = line;
	while( *c != 0 ){
		if( *c == '=' ){ strValue=c+1; *c=0; break; }
		c++;
	}
	if( strValue == NULL ) return false;

	key = atoi( strKey );
	value = atoi( strValue );
	return true;
}

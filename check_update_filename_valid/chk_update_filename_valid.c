#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define UPDATE_FILENAME_LEADING		"SGKS_680X_V"
#define UPDATE_FILENAME_SUFFIX		".bin"

int chk_ver_str_valid(const char *pStr)
{
	char *p = pStr;
	int dot_cnt=0;

	while(*p)
	{
		//isdigit checks actual value, so need to -'0'
		if(isdigit(*p-'0') == 0){
			p++;
			dot_cnt=0;
			continue;
		} else if (*p == '.' && dot_cnt == 0){
			p++;
			dot_cnt++;
			continue;
		} else {  
                        return -1;
                }
	}

	return 0;
}

int main(int argc, char **argv)
{
	char filename[] = "SGKS_680X_V1.9.4.110.bin";
	char curVer[] = "V1.9.3.9";
	char updVer[32] = {0};
	char *pVerStart, *pVerEnd;
	int ret;
	
	//check filename leading characters
	ret = memcmp(UPDATE_FILENAME_LEADING, filename, strlen(UPDATE_FILENAME_LEADING));
	if(ret != 0){
		printf("filename leading not match.\n");
		return -1;
	}
	pVerStart = filename + strlen(UPDATE_FILENAME_LEADING);	

	//check filename suffix
	pVerEnd = filename + strlen(filename) - strlen(UPDATE_FILENAME_SUFFIX);
	ret = memcmp(UPDATE_FILENAME_SUFFIX, pVerEnd, strlen(UPDATE_FILENAME_SUFFIX));
	if(ret != 0)
	{
                printf("filename suffix not match.\n");
                return -1;
	}
	
	//copy version string to updVer
	memcpy(updVer, pVerStart, pVerEnd-pVerStart);

	//check version string if only contains numbers and dot
	if(chk_ver_str_valid(updVer))
	{
		printf("update version string not valid. str:%s\n", updVer);
		return -1;
	}

	//compare with current version
	if(strncmp(curVer, updVer, sizeof(updVer))){
		printf("start update, ver:%s.\n", updVer);
	} else {
		printf("updVer is the same as curVer, no need update.\n");
		return -1;	
	}

	return 0;
}


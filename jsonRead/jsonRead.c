#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define PATH_MAX 256

void usage(const char *prog)
{
	printf("usage: jsonRead inputfile keyString. \n");
	printf("e.p.: %s $filename $string.\n", prog);
}

int main(int argc, char **argv)
{
	char inputName[PATH_MAX] = {0};
	char findstr[256] = {0};
	FILE* Fp = NULL;
	int Ret = 0;
	unsigned int ReadLenth = 0;
	char *DataBuf = NULL;
	char *Prandsalt = NULL;
	cJSON *cjson = NULL, *root_object = NULL;
	cJSON *str_in_file = NULL, *enableInfo = NULL;

	if (argc < 3) {
		usage(argv[0]);
		exit(-1);
	}

	strncpy(inputName, argv[1], sizeof(inputName) - 1);
	strncpy(findstr, argv[2], sizeof(findstr) - 1);

	Fp = fopen(inputName, "r");
	if(NULL == Fp)
	{
		printf("fopen %s fail \n", inputName);
		return -1;
	}

	Ret  = fseek(Fp, 0, SEEK_END);
	if (Ret)
	{
		printf("fseek %s fail \n", inputName);
		Ret = -1;
		goto quit;
	}

	ReadLenth = ftell(Fp);
	if (ReadLenth == -1L)
	{
		printf("get file %s size fail \n", inputName);
		Ret = -1;
		goto quit;
	}

	DataBuf = (char *)malloc(ReadLenth+1);
	if (DataBuf == NULL)
	{
		printf("MALLOC Failed\n");
		Ret = -1;
		goto quit;
	}

	memset(DataBuf, 0, ReadLenth+1);

	Ret = fseek(Fp, 0, SEEK_SET);
	if (Ret)
	{
		printf("SEEK file's start FAILED!\n");
		Ret = -1;
		goto quit;
	}

	Ret = fread(DataBuf, 1, ReadLenth, Fp);
	if (Ret != ReadLenth)
	{
		printf("READ file ERROR!\n");
		Ret = -1;
		goto quit;
	}
	DataBuf[ReadLenth] = '\0';

	/*1.按照JSON 格式解析*/
	cjson = cJSON_Parse(DataBuf);
	if (NULL == cjson)
	{
		printf("JSON Parse Failed, Try again\n");
		goto quit;
	}
	else
	{
		/*cJSAN 解析是树状分层解析的，不能一次直接找到底层的键值对*/
		root_object = cJSON_GetObjectItemCaseSensitive(cjson, "Wireless");
		if (NULL == root_object)
		{
			Ret = -1;
			printf("the config file is damaged! \n");
			goto quit;
		}

		str_in_file = cJSON_GetObjectItemCaseSensitive(root_object, findstr);
		if (NULL == str_in_file)
		{
			printf("can not find 3G info\n");
			Ret = -1;
			goto quit;
		}

		printf("%s \n", cJSON_Print(str_in_file));

		enableInfo = cJSON_GetObjectItemCaseSensitive(str_in_file, "Enable");
		if (NULL == enableInfo)
		{
			printf("can not find Enable info\n");
			Ret = -1;
			goto quit;
		}

		Ret = enableInfo->type;
		//printf("%d\n", enableInfo->type);
	}

quit:
	if (Fp)
		fclose(Fp);

	if (cjson)
	{
		cJSON_Delete(cjson);
	}

	if (DataBuf)
	{
		free(DataBuf);
		DataBuf = NULL;
	}
	return Ret;
}


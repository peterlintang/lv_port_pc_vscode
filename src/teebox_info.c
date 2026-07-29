
#include "course_info.h"


int course_get_teebox_nm_by_index(char *file_name, int index, char *nm, int *nm_len)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *teebox = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	teebox = cJSON_GetObjectItem(root, ITEM_TEEBOX);
	num = cJSON_GetArraySize(teebox);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(teebox, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_NM);
		int len = strlen(item2->valuestring);
		len = len > *nm_len ? *nm_len : len;
		strncpy(nm, item2->valuestring, len);
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_teebox_rt_by_index(char *file_name, int index, double *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *teebox = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	teebox = cJSON_GetObjectItem(root, ITEM_TEEBOX);
	num = cJSON_GetArraySize(teebox);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(teebox, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_RT);
		*value = item2->valuedouble;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_teebox_sl_by_index(char *file_name, int index, double *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *teebox = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	teebox = cJSON_GetObjectItem(root, ITEM_TEEBOX);
	num = cJSON_GetArraySize(teebox);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(teebox, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_SL);
		*value = item2->valuedouble;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_teebox_wrt_by_index(char *file_name, int index, double *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *teebox = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	teebox = cJSON_GetObjectItem(root, ITEM_TEEBOX);
	num = cJSON_GetArraySize(teebox);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(teebox, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_WRT);
		*value = item2->valuedouble;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_teebox_wsl_by_index(char *file_name, int index, double *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *teebox = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	teebox = cJSON_GetObjectItem(root, ITEM_TEEBOX);
	num = cJSON_GetArraySize(teebox);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(teebox, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_WSL);
		*value = item2->valuedouble;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}


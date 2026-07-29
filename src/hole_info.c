
#include "course_info.h"

int course_get_hole_hl_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_HL);
		*value = item2->valueint;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_p_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_P);
		*value = item2->valueint;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_h_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_H);
		*value = item2->valueint;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_gw_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_GW);
		*value = item2->valueint;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_ha_by_index(char *file_name, int index, double *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_HA);
		*value = (double)item2->valuedouble;
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_plNum_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_PL);
		*value = cJSON_GetArraySize(item2);
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_gPtsNum_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_GPTS);
		*value = cJSON_GetArraySize(item2);
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_hole_tPtsNum_by_index(char *file_name, int index, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *item = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (index < num && index >= 0)
	{
		item = cJSON_GetArrayItem(hls, index);
		cJSON *item2 = cJSON_GetObjectItem(item, ITEM_TPTS);
		*value = cJSON_GetArraySize(item2);
	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}



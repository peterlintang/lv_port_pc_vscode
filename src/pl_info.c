
#include "course_info.h"

int course_get_pl_n_by_index(char *file_name, int hole_index, int pl_index, char *n, int *n_len)
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

	if (hole_index < num && hole_index >= 0)
	{
		item = cJSON_GetArrayItem(hls, hole_index);
		cJSON *pl = cJSON_GetObjectItem(item, ITEM_PL);

		int pl_num = 0;
		pl_num = cJSON_GetArraySize(pl);
		if (pl_index < pl_num && pl_index >= 0)
		{
			cJSON *pl_item = NULL;
			pl_item = cJSON_GetArrayItem(pl, pl_index);
			cJSON *n_item = cJSON_GetObjectItem(pl_item, ITEM_N);
			int len = 0;
			len = strlen(n_item->valuestring);
			len = len > *n_len ? *n_len : len;
			strncpy(n, n_item->valuestring, len);
			*n_len = len;
		}
		else
		{
			ret = -1;
		}

	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_pl_pt_by_index(char *file_name, int hole_index, int pl_index, struct gps_point *pt)
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

	if (hole_index < num && hole_index >= 0)
	{
		item = cJSON_GetArrayItem(hls, hole_index);
		cJSON *pl = cJSON_GetObjectItem(item, ITEM_PL);

		int pl_num = 0;
		pl_num = cJSON_GetArraySize(pl);
		if (pl_index < pl_num && pl_index >= 0)
		{
			cJSON *pl_item = NULL;
			pl_item = cJSON_GetArrayItem(pl, pl_index);
			cJSON *pt_item = cJSON_GetObjectItem(pl_item, ITEM_PT);
			cJSON *iter = NULL;
			item = cJSON_GetArrayItem(pt_item, 0);
			pt->x = (double)item->valuedouble;
			item = cJSON_GetArrayItem(pt_item, 1);
			pt->y = (double)item->valuedouble;
		}
		else
		{
			ret = -1;
		}

	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}

int course_get_pl_alt_by_index(char *file_name, int hole_index, int pl_index, double *alt)
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

	if (hole_index < num && hole_index >= 0)
	{
		item = cJSON_GetArrayItem(hls, hole_index);
		cJSON *pl = cJSON_GetObjectItem(item, ITEM_PL);

		int pl_num = 0;
		pl_num = cJSON_GetArraySize(pl);
		if (pl_index < pl_num && pl_index >= 0)
		{
			cJSON *pl_item = NULL;
			pl_item = cJSON_GetArrayItem(pl, pl_index);
			cJSON *alt_item = cJSON_GetObjectItem(pl_item, ITEM_ALT);
			*alt = (double)alt_item->valuedouble;
		}
		else
		{
			ret = -1;
		}

	}
	else
	{
		ret = -1;
	}

	cJSON_Delete(root);
	free(buff);
	return ret;
}


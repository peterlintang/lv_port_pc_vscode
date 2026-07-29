
#include "course_info.h"

int course_get_tPts_arrayItemNum_by_index(
		char *file_name, 
		int hole_index, 
		int item_index, 
		int *value)
{
	// TODO seems all to be 2 items
	*value = 2;
	return 0;
}

int course_get_tPts_arrayItemInfo_by_index(
		char *file_name, 
		int hole_index, 
		int tpts_index, 
		struct gps_point *pts	// 2 items
		)
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
		cJSON *tpts = cJSON_GetObjectItem(item, ITEM_TPTS);

		int tpts_num = 0;
		tpts_num = cJSON_GetArraySize(tpts);
		if (tpts_index < tpts_num && tpts_index >= 0)
		{
			cJSON *tpts_item = NULL;
			tpts_item = cJSON_GetArrayItem(tpts, tpts_index);
			cJSON *iter = cJSON_GetArrayItem(tpts_item, 0);
			cJSON *iter_n = cJSON_GetArrayItem(iter, 0);
			pts[0].x = (double)iter_n->valuedouble;
			iter_n = cJSON_GetArrayItem(iter, 1);
			pts[0].y = (double)iter_n->valuedouble;
			iter = cJSON_GetArrayItem(tpts_item, 1);
			iter_n = cJSON_GetArrayItem(iter, 0);
			pts[1].x = (double)iter_n->valuedouble;
			iter_n = cJSON_GetArrayItem(iter, 1);
			pts[1].y = (double)iter_n->valuedouble;
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

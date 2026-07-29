
#include "course_info.h"

int course_get_gPts_arrayItemNum_by_index(
		char *file_name, 
		int hole_index, 
		int item_index, 
		int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *hl = NULL;
	int num = 0;
	int gpts_num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);
	if (hole_index < num && hole_index >= 0)
	{
		hl = cJSON_GetArrayItem(hls, hole_index);
		cJSON *gptsA = cJSON_GetObjectItem(hl, ITEM_GPTS);
		gpts_num = cJSON_GetArraySize(gptsA);
		if (item_index < gpts_num && item_index >= 0)
		{
			cJSON *gpts_item = cJSON_GetArrayItem(gptsA, item_index);
			int pts_num = cJSON_GetArraySize(gpts_item);
			*value = pts_num;
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

int course_get_gPts_arrayItemInfo_by_index(
		char *file_name, 
		int hole_index, 
		int gpts_index, 
		struct gps_point *pts, 
		int pts_num)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *hls = NULL;
	cJSON *hl = NULL;
	int num = 0;
	int ret = 0;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	hls = cJSON_GetObjectItem(root, ITEM_HLS);
	num = cJSON_GetArraySize(hls);

	if (hole_index < num && hole_index >= 0)
	{
		hl = cJSON_GetArrayItem(hls, hole_index);
		cJSON *gptsA = cJSON_GetObjectItem(hl, ITEM_GPTS);

		int gpts_num = 0;
		gpts_num = cJSON_GetArraySize(gptsA);
		if (gpts_index < gpts_num && gpts_index >= 0)
		{
			cJSON *gpts_item = NULL;
			gpts_item = cJSON_GetArrayItem(gptsA, gpts_index);

			int A_num = cJSON_GetArraySize(gpts_item);
			if (pts_num < A_num)
			{
				ret = -1;
				goto out;
			}
			for (int i = 0; i < pts_num; i++)
			{
				cJSON *iter = cJSON_GetArrayItem(gpts_item, i);
				cJSON *iter_n = cJSON_GetArrayItem(iter, 0);
				pts[i].x = (double)iter_n->valuedouble;
				iter_n = cJSON_GetArrayItem(iter, 1);
				pts[i].y = (double)iter_n->valuedouble;
			}
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

out:
	cJSON_Delete(root);
	free(buff);
	return ret;
}


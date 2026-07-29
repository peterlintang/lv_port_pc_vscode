
#include "course_info.h"

/*
 * get course id from file @file_name
 * int file_name:	course file
 * out id:		store id value
 * in_out id:		the max len of @id, return length of @id,
 * return value:	0 success 
 * 			-1 fail
 */
int course_get_id(char *file_name, char *id, int *id_len)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *item = NULL;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	item = cJSON_GetObjectItem(root, ITEM_ID);

	int len = 0;
	len = strlen(item->valuestring);
	len = *id_len > len ? len : *id_len;

	strncpy(id, item->valuestring, len);
	*id_len = len;

	cJSON_Delete(root);
	free(buff);
	return 0;
}

int course_get_ver(char *file_name, char *ver, int *ver_len)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *item = NULL;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	item = cJSON_GetObjectItem(root, ITEM_VER);

	int len = 0;
	len = strlen(item->valuestring);
	len = *ver_len > len ? len : *ver_len;

	strncpy(ver, item->valuestring, len);
	*ver_len = len;

	cJSON_Delete(root);
	free(buff);
	return 0;
}

int course_get_isC(char *file_name, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *item = NULL;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	item = cJSON_GetObjectItem(root, ITEM_ISC);

	*value = item->valueint;

	cJSON_Delete(root);
	free(buff);
	return 0;
}

int course_get_TeeBoxNum(char *file_name, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *item = NULL;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	item = cJSON_GetObjectItem(root, ITEM_TEEBOX);

	*value = cJSON_GetArraySize(item);

	cJSON_Delete(root);
	free(buff);
	return 0;
}

int course_get_hlsNum(char *file_name, int *value)
{
	unsigned char *buff = NULL;
	cJSON *root = NULL;
	cJSON *item = NULL;

	buff = get_file(file_name);
	root = cJSON_Parse(buff);
	item = cJSON_GetObjectItem(root, ITEM_HLS);

	*value = cJSON_GetArraySize(item);

	cJSON_Delete(root);
	free(buff);
	return 0;
}


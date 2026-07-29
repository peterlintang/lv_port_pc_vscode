
#ifndef COURSE_INFO_H__
#define COURSE_INFO_H__

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITEM_ID		"id" 
#define ITEM_TEEBOX	"TeeBox" 
#define ITEM_HLS	"hls" 
#define ITEM_VER	"ver" 
#define ITEM_ISC	"isC" 
#define ITEM_HL		"hl"
#define ITEM_P		"p"
#define ITEM_H		"h"
#define ITEM_GW		"gw"
#define ITEM_HA		"ha"
#define ITEM_PL		"pl"
#define ITEM_GPTS	"gPts"
#define ITEM_TPTS	"tPts"
#define ITEM_ALT	"alt"
#define ITEM_PT		"pt"
#define ITEM_N		"n"
#define ITEM_NM		"nm"
#define ITEM_RT		"rt"
#define ITEM_SL		"sl"
#define ITEM_WRT	"wrt"
#define ITEM_WSL	"wsl"

struct gps_point {
	double x;
	double y;
};

static inline char *get_file(char *file_name)
{
	FILE *fp = NULL;
	unsigned char *info = NULL;
	long length = 0;

	fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		printf("open %s error\n", file_name);
	}
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	info = (unsigned char *)calloc(1, length+1);
	fseek(fp, 0, SEEK_SET);
	fread(info, 1, length, fp);
	fclose(fp);

	return info;
}

int course_get_id(char *file_name, char *id, int *id_len);
int course_get_ver(char *file_name, char *ver, int *ver_len);
int course_get_isC(char *file_name, int *value);
int course_get_TeeBoxNum(char *file_name, int *value);
int course_get_hlsNum(char *file_name, int *value);

int course_get_hole_hl_by_index(char *file_name, int index, int *value);
int course_get_hole_p_by_index(char *file_name, int index, int *value);
int course_get_hole_h_by_index(char *file_name, int index, int *value);
int course_get_hole_gw_by_index(char *file_name, int index, int *value);
int course_get_hole_ha_by_index(char *file_name, int index, double *value);
int course_get_hole_plNum_by_index(char *file_name, int index, int *value);
int course_get_hole_gPtsNum_by_index(char *file_name, int index, int *value);
int course_get_hole_tPtsNum_by_index(char *file_name, int index, int *value);

int course_get_pl_n_by_index(char *file_name, int hole_index, int pl_index, char *n, int *n_len);
int course_get_pl_pt_by_index(char *file_name, int hole_index, int pl_index, struct gps_point *pt);
int course_get_pl_alt_by_index(char *file_name, int hole_index, int pl_index, double *alt);

int course_get_teebox_nm_by_index(char *file_name, int index, char *nm, int *nm_len);
int course_get_teebox_rt_by_index(char *file_name, int index, double *value);
int course_get_teebox_sl_by_index(char *file_name, int index, double *value);
int course_get_teebox_wrt_by_index(char *file_name, int index, double *value);
int course_get_teebox_wsl_by_index(char *file_name, int index, double *value);

int course_get_tPts_arrayItemNum_by_index(
		char *file_name, 
		int hole_index, 
		int item_index, 
		int *value);
int course_get_tPts_arrayItemInfo_by_index(
		char *file_name, 
		int hole_index, 
		int tpts_index, 
		struct gps_point *pts	// 2 items
		);


int course_get_gPts_arrayItemNum_by_index(
		char *file_name, 
		int hole_index, 
		int item_index, 
		int *value);
int course_get_gPts_arrayItemInfo_by_index(
		char *file_name, 
		int hole_index, 
		int gpts_index, 
		struct gps_point *pts, 
		int pts_num);



double calc_2gpspoints_distance(struct gps_point *A, struct gps_point *B);
/*
 *	get the fcb for green 
 *	@green:		(in)green points
 *	@pts_num:	(in)green points num
 *	@current:	(in)user current gps info
 *	@center:	(in)center of green
 *	@f:		(out)front distance
 *	@c:		(out)center distance
 *	@b:		(out)rear distance
 *	return value:	0 sucess, -1 fail;
 */
int green_fcb(struct gps_point *green, int pts_num, 
		struct gps_point *current, 
		struct gps_point *center, 
		struct gps_point *f_p,
		struct gps_point *b_p,
		double *f,
		double *c,
		double *b);

#endif

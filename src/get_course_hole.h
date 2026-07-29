

#ifndef GET_COURSE_HOLE_H__
#define GET_COURSE_HOLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "course_info.h"

#define false 0
#define true 1

int minmea_scan(const char *sentence, const char *format, ...);
struct record *parse_record(char *n_l, struct record *new);
void free_record(struct record *item);

struct record {
	char *id;
	char *course;
	char *address;
	char *country;
	char *unknow3;
	char *state;
	char *longtitude;
	char *latitiude;
	char *unknow1;
	char *unknow2;
	char *folderid;
};


int course_search_nearest_hole(char *course, struct gps_point *current, int *hole_num);

int course_get_near_courses_by_position(char *course_list, struct gps_point *current, struct record **courses, int *courses_num, double ref_degree);

int search_nearest_course(struct record **courses, 
		int courses_num, 
		struct record *nearest, 
		struct gps_point *current);

void copy_record(struct record *item, struct record *new);
int fs_read_line(FILE *file, char *data);

void print_record(struct record *item);

#endif // end of GET_COURSE_HOLE_H__




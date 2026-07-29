

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "course_info.h"
#include "get_course_hole.h"

#define HOLE_PL_DISTANCE 	(20.0)
#define HOLE_GREEN_DISTANCE	(10.0)
#define HOLE_TEEBOX_DISTANCE	(10.0)

static int is_near_hole(char *course, int hole_id, struct gps_point *current)
{
    double distance = 0.0;
    struct gps_point point = { 0.0, 0.0 };

    int pls_num = 0;
    course_get_hole_plNum_by_index(course, hole_id, &pls_num);

    for (int i = 0; i < pls_num; i++)
    {

    	course_get_pl_pt_by_index(course, hole_id, i, &point);
	distance = calc_2gpspoints_distance(current, &point);
	/*
	if (distance < 200.0)
	printf("%s %d: (%.8f %.8f) (%.8f %.8f) %.8f\n", 
			__func__, hole_id,
			point.y, point.x,
			current->y, current->x,
			distance);
	*/
	if (distance < HOLE_PL_DISTANCE)
	{
		return 1;
	}
    }

    int greens_num = 0;
    course_get_hole_gPtsNum_by_index(course, hole_id, &greens_num);

    for (int i = 0; i < greens_num; i++)
    {
        int points_num = 0;
        struct gps_point *points = NULL;
	int found = 0;

        course_get_gPts_arrayItemNum_by_index(course, hole_id, i, &points_num);
        points = (struct gps_point *)calloc(points_num, sizeof(struct gps_point));
        course_get_gPts_arrayItemInfo_by_index(course, hole_id, i, points, points_num); 

	for (int j = 0; j < points_num; j++)
	{
	    distance = calc_2gpspoints_distance(current, &points[j]);
	    /*
	if (distance < 200.0)
	printf("%s %d: (%.8f %.8f) (%.8f %.8f) %.8f\n", 
			__func__, hole_id,
			points[j].y, points[j].x,
			current->y, current->x,
			distance);
			*/
	    if (distance < HOLE_GREEN_DISTANCE)
	    {
		found = 1;
		break;
	    }
	}

        free(points);
	if (found == 1)
	{
		return 1;
	}
    }

    int t_num = 0;
    course_get_hole_tPtsNum_by_index(course, hole_id, &t_num);

    for (int i = 0; i < t_num; i++)
    {
	struct gps_point pts[2] = { 0.0, 0.0 };
	struct gps_point teebox_point;

	course_get_tPts_arrayItemInfo_by_index(course, hole_id, i, pts);
	for (int j = 0; j < 2; j++)
	{
		teebox_point.x = pts[j].x;
		for (int k = 0; k < 2; k++)
		{
			teebox_point.y = pts[k].y;
	    		distance = calc_2gpspoints_distance(current, &teebox_point);
			/*
	if (distance < 200.0)
	printf("%s %d: (%.8f %.8f) (%.8f %.8f) %.8f\n", 
			__func__, hole_id, 
			teebox_point.y, teebox_point.x,
			current->y, current->x,
			distance);
			*/
	    		if (distance < HOLE_TEEBOX_DISTANCE)
	    		{
				return 1;
	    		}
		}
	}
    }

    return 0;
}

int course_search_nearest_hole(char *course, struct gps_point *current, int *hole_num)
{
	int i;
	int holes_num = 0;
	course_get_hlsNum(course, &holes_num);

	for (i = 0; i < holes_num; i++)
	{
		if (is_near_hole(course, i, current))
			break;
	}

	if (i == holes_num)
	{
		return -1;
	}

	*hole_num = i;

	return 0;
}

int search_nearest_course(struct record **courses, 
		int courses_num, 
		struct record *nearest, 
		struct gps_point *current)
{
	int index = -1;
	double distance = 0.0f;
	double nearest_distance = 0.0f;
	struct gps_point course_point;
	double x = 0.0;
	double y = 0.0;

	if (courses_num <= 0)
		return -1;

	course_point.x = strtod(courses[0]->longtitude, NULL);
	course_point.y = strtod(courses[0]->latitiude, NULL);
	nearest_distance = calc_2gpspoints_distance(&course_point, current);
	/*
	printf("%s: current: %.8f %.8f\n", __func__, current->y, current->x);
	printf("%s distance: %.8f(%.8f %.8f)\n", courses[0]->id, nearest_distance, 
			course_point.y, course_point.x);
			*/
	index = 0;

	for (int i = 1; i < courses_num; i++)
	{
	    course_point.x = strtod(courses[i]->longtitude, NULL);
	    course_point.y = strtod(courses[i]->latitiude, NULL);
	    distance = calc_2gpspoints_distance(&course_point, current);
	    /*
	    printf("%s distance: %.8f(%.8f %.8f)\n", courses[i]->id, distance, 
			course_point.y, course_point.x);
			*/
	    if (distance < nearest_distance)
	    {
		    nearest_distance = distance;
		    index = i;
	    }
	}

	copy_record(courses[index], nearest);

	return 0;
}

static void strip_line(char *line)
{
	int len = strlen(line);
	for (int i = len - 1; i >= 0; i--)
	{
		if (line[i] == '\n' || line[i] == '\r')
			line[i] = '\0';
	}
}

int course_get_near_courses_by_position(char *course_list, 
		struct gps_point *current, 
		struct record **courses, 
		int *courses_num, 
		double ref_degree)
{
	FILE *fp = NULL;
	char line[1024] = { 0 };
	struct record *new;
	double x_min;
	double x_max;
	double y_min;
	double y_max;
	int index = 0;
	int index_max = *courses_num;
	int ret = 0;

	x_min = current->x - ref_degree;
	x_max = current->x + ref_degree;
	y_min = current->y - ref_degree;
	y_max = current->y + ref_degree;

	fp = fopen(course_list, "r");

	while (fs_read_line(fp, line) > 0)
	{
		strip_line(line);
		double x;
		double y;
		new = (struct record *)calloc(1, sizeof(*new));
		parse_record(line, new);
		y = strtod(new->latitiude, NULL);
		x = strtod(new->longtitude, NULL);
		if (x >= x_min && x <= x_max && y >= y_min && y <= y_max)
		{
			if (index < index_max)
			{
				courses[index++] = new;
			}
			else
			{
				free_record(new);
				free(new);
				ret = -1;
				break;
			}
		}
		else
		{
			free_record(new);
			free(new);
		}
	}

	fclose(fp);

	*courses_num = index;

	return ret;
}

struct record *records[41000];

int fs_read_line(FILE *file, char *data)
{
	char line[1024] = { 0 }; 
	size_t n = 1024;
	char *y = line;

	int x = getline(&y, &n, file);
	memcpy(data, line, 360);
	return x;
	
}

void print_record(struct record *item)
{
	printf("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", 
			item->id,
			item->course,
			item->address,
			item->country,
			item->unknow3,
			item->state,
			item->latitiude,
			item->longtitude,
			item->unknow1,
			item->unknow2,
			item->folderid
			);
}

void copy_record(struct record *item, struct record *new)
{
	new->id = strdup(item->id);
	new->course = strdup(item->course);
	new->address = strdup(item->address);
	new->country = strdup(item->country);
	new->unknow3 = strdup(item->unknow3);
	new->state = strdup(item->state);
	new->longtitude = strdup(item->longtitude);
	new->latitiude = strdup(item->latitiude);
	new->unknow1 = strdup(item->unknow1);
	new->unknow2 = strdup(item->unknow2);
	new->folderid = strdup(item->folderid);
			
}
void free_record(struct record *item)
{
	free(item->id);
	free(item->course);
	free(item->address);
	free(item->country);
	free(item->unknow3);
	free(item->state);
	free(item->longtitude);
	free(item->latitiude);
	free(item->unknow1);
	free(item->unknow2);
	free(item->folderid);
			
}

struct record *parse_record(char *n_l, struct record *new)
{

	char line[1024] = { 0 };
	char s1[128] = { 0 };
	char s2[128] = { 0 };
	char s3[128] = { 0 };
	char s4[128] = { 0 };
	char s5[128] = { 0 };
	char s6[128] = { 0 };
	char s7[128] = { 0 };
	char s8[128] = { 0 };
	char s9[128] = { 0 };
	char s10[128] = { 0 };
	char s11[128] = { 0 };
	strcpy(line, n_l);
	minmea_scan(line, "sssssssssss", s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11); 

	new->id = strdup(s1);
	new->course = strdup(s2);
	new->address = strdup(s3);
	new->country = strdup(s4);
	new->unknow3 = strdup(s5);
	new->state = strdup(s6);
	new->latitiude = strdup(s7);
	new->longtitude = strdup(s8);
	new->unknow1 = strdup(s9);
	new->unknow2 = strdup(s10);
	new->folderid = strdup(s11);

	return new;
}

/*
int main(int argc, char *argv[])
{
	struct record **near_courses =  NULL ;
	int near_courses_num = 100;
	struct gps_point current = { 
		.x = 114.03351200,
		.y = 22.538504000,
	};


	near_courses = (struct record **)calloc(near_courses_num, sizeof(struct record *));
	course_get_near_courses_by_position(argv[1], &current, near_courses, &near_courses_num, 0.3);

	printf("num: %d\n", near_courses_num);

	for (int i = 0; i < near_courses_num; i++)
	{
		print_record(near_courses[i]);
		free_record(near_courses[i]);
		free(near_courses[i]);
	}

	free(near_courses);

	return 0;
}

*/

static inline int minmea_isfield(char c) {
    return c != '|';
}

int minmea_scan(const char *sentence, const char *format, ...)
{
    int result = false;
    int optional = false;

    if (sentence == NULL)
        return false;

    va_list ap;
    va_start(ap, format);

    const char *field = sentence;
#define next_field() \
    do { \
        /* Progress to the next field. */ \
        while (minmea_isfield(*sentence)) \
            sentence++; \
        /* Make sure there is a field there. */ \
        if (*sentence == '|') { \
            sentence++; \
            field = sentence; \
        } else { \
            field = NULL; \
        } \
    } while (0)


    while (*format) {
        char type = *format++;

        if (type == ';') {
            // All further fields are optional.
            optional = true;
            continue;
        }

        if (!field && !optional) {
            // Field requested but we ran out if input. Bail out.
            goto parse_error;
        }

        switch (type) {
            case 's': { // String value (char *).
                char *buf = va_arg(ap, char *);

                if (field) {
                    while (minmea_isfield(*field))
                        *buf++ = *field++;
                }

                *buf = '\0';
            } break;

            default: { // Unknown.
                goto parse_error;
            }
        }

        next_field();
    }

    result = true;

parse_error:
    va_end(ap);
    return result;
}


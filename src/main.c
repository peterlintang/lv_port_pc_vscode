
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include <SDL.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern void freertos_main(void);

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "course_info.h"
#include "get_course_hole.h"

static struct gps_point ref_min = { 361.0, 361.0 };
static struct gps_point ref_max = { -361.0, -361.0 };
static lv_obj_t *obj = NULL;
static lv_obj_t *canvas = NULL;
lv_timer_t * timer = NULL;
static int refresh = 0;
static int quit_flag = 0;
static char nearest_course_path[256];
static int greens_num = 1;
static int hole_select = -1;
static struct gps_point current;
static int course_debug = 1;

#define MY_WIDTH	800
#define MY_HEIGHT	800

static int to_x(double x)
{
	double ref = ref_max.x - ref_min.x > ref_max.y - ref_min.y ? ref_max.x - ref_min.x : ref_max.y - ref_min.y;

	double x_min = 0;
	double x_max = (ref_max.x - ref_min.x) / ref * MY_WIDTH;

	int aa = (int)(((x - ref_min.x) / ref * MY_WIDTH) - ((x_min + x_max) / 2 - MY_WIDTH / 2));
	if (aa <= 0 || aa >= MY_WIDTH)
	{
		printf("X VERY ERROR\n\n");
	}
	return ((x - ref_min.x) / ref * MY_WIDTH) - ((x_min + x_max) / 2 - MY_WIDTH / 2);
}

static int to_y(double y)
{
	double ref = ref_max.x - ref_min.x > ref_max.y - ref_min.y ? ref_max.x - ref_min.x : ref_max.y - ref_min.y;

	double y_min = 0;
	double y_max = (ref_max.y - ref_min.y) / ref * MY_HEIGHT;

	int aa= (int)(((y - ref_min.y) / ref * MY_HEIGHT) - ((y_min + y_max) / 2 - MY_HEIGHT / 2));
	if (aa <= 0 || aa >= MY_HEIGHT)
	{
		printf("Y VERY ERROR\n\n");
	}
	return MY_HEIGHT - (((y - ref_min.y) / ref * MY_HEIGHT) - ((y_min + y_max) / 2 - MY_HEIGHT / 2));
}

static void lvgl_draw_pl(char *pl_type, struct gps_point *point)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_arc_dsc_t dsc;
    lv_draw_arc_dsc_init(&dsc);
    if (strncmp("t", pl_type, 1) == 0)
    	dsc.color = lv_palette_main(LV_PALETTE_RED);
    else if (strncmp("f", pl_type, 1) == 0)
    	dsc.color = lv_palette_main(LV_PALETTE_BLUE);
    else if (strncmp("r", pl_type, 1) == 0)
    	dsc.color = lv_palette_main(LV_PALETTE_GREEN);
    else
    	dsc.color = lv_palette_main(LV_PALETTE_YELLOW);
    dsc.width = 1;
    dsc.center.x = to_x(point->x);
    dsc.center.y = to_y(point->y);
    dsc.width = 1;
    dsc.radius = 1;
    dsc.start_angle = 0;
    dsc.end_angle = 360;

    lv_draw_arc(&layer, &dsc);

    lv_canvas_finish_layer(canvas, &layer);

    {
        lv_layer_t layer;
        lv_canvas_init_layer(canvas, &layer);

        lv_draw_letter_dsc_t letter_dsc;
        lv_draw_letter_dsc_init(&letter_dsc);
        letter_dsc.color = lv_color_hex(0xaabc00);
        letter_dsc.font = lv_font_get_default();

        int16_t len = strlen(pl_type);
        for (int16_t i = 0; i < len; i++)
        {
	    const lv_point_t pos = { .x = to_x(point->x) + (i + 1) * 8, .y = to_y(point->y) };
	    letter_dsc.unicode = (uint32_t)pl_type[i % len];
	    lv_draw_letter(&layer, &letter_dsc, &pos);
        }
        lv_canvas_finish_layer(canvas, &layer);
    }


}

static void draw_pl(char *course, int h_id, int pl_id)
{
    char pl_type[8] = { 0 };
    int pl_type_len = 8;
    struct gps_point point = { 0.0, 0.0 };

    course_get_pl_n_by_index(course, h_id, pl_id, pl_type, &pl_type_len);
    course_get_pl_pt_by_index(course, h_id, pl_id, &point);
    if (course_debug) 
    	printf("type: %s, %.8f %.8f (%d %d)\n", pl_type, point.y, point.x, to_y(point.y), to_x(point.x));

    lvgl_draw_pl(pl_type, &point);
}

static void draw_pls(char *course, int h_id)
{
    int pls_num = 0;
    course_get_hole_plNum_by_index(course, h_id, &pls_num);

    for (int i = 0; i < pls_num; i++)
    {
	draw_pl(course, h_id, i);
    }
}

static void lvgl_draw_green(struct gps_point *points, int points_num)
{
    for (int i = 0; i < points_num; i++)
    {
	    lv_layer_t layer;
	    lv_canvas_init_layer(canvas, &layer);

	    lv_draw_line_dsc_t dsc;
	    lv_draw_line_dsc_init(&dsc);
	    dsc.color = lv_palette_main(LV_PALETTE_GREEN);
	    dsc.width = 1;
	    dsc.round_end = 1;
	    dsc.round_start = 1;
	    dsc.p1.x = to_x(points[i % points_num].x);
	    dsc.p1.y = to_y(points[i % points_num].y);
	    dsc.p2.x = to_x(points[(i + 1) % points_num].x);
	    dsc.p2.y = to_y(points[(i + 1) % points_num].y);
	    lv_draw_line(&layer, &dsc);

	    lv_canvas_finish_layer(canvas, &layer);

    }
}

static void draw_green(char *course, int h_id, int g_id)
{
    int points_num = 0;
    struct gps_point *points = NULL;

    course_get_gPts_arrayItemNum_by_index(course, h_id, g_id, &points_num);

    points = (struct gps_point *)calloc(points_num, sizeof(struct gps_point));

    course_get_gPts_arrayItemInfo_by_index(course, h_id, g_id, points, points_num); 

    /*
    printf("draw green: %d\n", g_id);
    for (int i = 0; i < points_num; i++)
    {
	    printf("%.8f %.8f\n", points[i].y, points[i].x);
    }
    */

    lvgl_draw_green(points, points_num);

    free(points);
}

static void draw_greens(char *course, int h_id)
{
	int greens_num = 0;
	course_get_hole_gPtsNum_by_index(course, h_id, &greens_num);

	for (int i = 0; i < greens_num; i++)
	{
	    draw_green(course, h_id, i);
	}
}

static void lvgl_draw_teebox(struct gps_point pts[2])
{
	int x_min = to_x(pts[0].x) > to_x(pts[1].x) ? to_x(pts[1].x) : to_x(pts[0].x);
	int x_max = to_x(pts[0].x) > to_x(pts[1].x) ? to_x(pts[0].x) : to_x(pts[1].x);
	int y_min = to_y(pts[0].y) > to_y(pts[1].y) ? to_y(pts[1].y) : to_y(pts[0].y);
	int y_max = to_y(pts[0].y) > to_y(pts[1].y) ? to_y(pts[0].y) : to_y(pts[1].y);

	int x[4] = { x_min, x_max, x_max, x_min };
	int y[4] = { y_min, y_min, y_max, y_max };

	for (int i = 0; i < 4; i++)
	{
	    lv_layer_t layer;
	    lv_canvas_init_layer(canvas, &layer);

	    lv_draw_line_dsc_t dsc;
	    lv_draw_line_dsc_init(&dsc);
	    dsc.color = lv_palette_main(LV_PALETTE_GREEN);
	    dsc.width = 1;
	    dsc.round_end = 1;
	    dsc.round_start = 1;
	    dsc.p1.x = x[i];
	    dsc.p1.y = y[i];
	    dsc.p2.x = x[(i + 1) % 4];
	    dsc.p2.y = y[(i + 1) % 4];
	    lv_draw_line(&layer, &dsc);
	    lv_canvas_finish_layer(canvas, &layer);
	}
}

static void draw_teebox(char *course, int h_id, int t_id)
{
	struct gps_point pts[2] = { 0.0, 0.0 };

	course_get_tPts_arrayItemInfo_by_index(course, h_id, t_id, pts);

	lvgl_draw_teebox(pts);
}

static void draw_teeboxes(char *course, int h_id)
{
	int t_num = 0;
	course_get_hole_tPtsNum_by_index(course, h_id, &t_num);

	for (int i = 0; i < t_num; i++)
	{
		draw_teebox(course, h_id, i);
	}
}

static void find_min_max_tPts(char *course, int h_id, 
		struct gps_point *ref_min, 
		struct gps_point *ref_max)
{
    int t_num = 0;
    course_get_hole_tPtsNum_by_index(course, h_id, &t_num);

    for (int i = 0; i < t_num; i++)
    {
	struct gps_point points[2] = { 0.0, 0.0 };

	course_get_tPts_arrayItemInfo_by_index(course, h_id, i, points);
	for (int j = 0; j < 2; j++)
	{
		if (points[j].x < ref_min->x)
			ref_min->x = points[j].x;
		if (points[j].y < ref_min->y)
			ref_min->y = points[j].y;
		if (points[j].x > ref_max->x)
			ref_max->x = points[j].x;
		if (points[j].y > ref_max->y)
			ref_max->y = points[j].y;
	}
    }
}

static void find_min_max_green(char *course, int h_id, int g_id, 
		struct gps_point *ref_min,
		struct gps_point *ref_max)
{
    int points_num = 0;
    struct gps_point *points = NULL;

    course_get_gPts_arrayItemNum_by_index(course, h_id, g_id, &points_num);

    points = (struct gps_point *)calloc(points_num, sizeof(struct gps_point));
    course_get_gPts_arrayItemInfo_by_index(course, h_id, g_id, points, points_num); 

    for (int j = 0; j < points_num; j++)
    {
	if (points[j].x < ref_min->x)
		ref_min->x = points[j].x;
	if (points[j].y < ref_min->y)
		ref_min->y = points[j].y;
	if (points[j].x > ref_max->x)
		ref_max->x = points[j].x;
	if (points[j].y > ref_max->y)
		ref_max->y = points[j].y;
    }

    free(points);
}

static void find_min_max_greens(char *course, int h_id,
		struct gps_point *ref_min,
		struct gps_point *ref_max)
{
    int greens_num = 0;
    course_get_hole_gPtsNum_by_index(course, h_id, &greens_num);

    for (int i = 0; i < greens_num; i++)
    {
	find_min_max_green(course, h_id, i, 
		ref_min,
		ref_max);
    }
}

static void find_min_max_pls(char *course, int h_id,
		struct gps_point *ref_min,
		struct gps_point *ref_max)
{
    struct gps_point point =  { 0.0, 0.0 };
    int pls_num = 0;
    course_get_hole_plNum_by_index(course, h_id, &pls_num);

    for (int i = 0; i < pls_num; i++)
    {
        course_get_pl_pt_by_index(course, h_id, i, &point);
	if (point.x < ref_min->x)
		ref_min->x = point.x;
	if (point.y < ref_min->y)
		ref_min->y = point.y;
	if (point.x > ref_max->x)
		ref_max->x = point.x;
	if (point.y > ref_max->y)
		ref_max->y = point.y;
    }
}

static void find_min_max_points(char *course, int h_id)
{
    find_min_max_pls(course, h_id, &ref_min, &ref_max);
    find_min_max_greens(course, h_id, &ref_min, &ref_max);
    find_min_max_tPts(course, h_id, &ref_min, &ref_max);
}

static void draw_hole(char *course, int h_id)
{
    draw_teeboxes(course, h_id);
    draw_pls(course, h_id);
    draw_greens(course, h_id);
}

static void draw_course(char *course)
{
    int hls_num = 0;
    course_get_hlsNum(course, &hls_num);

    for (int i = 0; i < hls_num; i++)
    {
    	if (course_debug) 
		printf("drawing hole: %d\n", i);
	draw_hole(course, i);
    }
}

static void lvgl_draw_fcb(struct gps_point *f_point, double *f, 
			struct gps_point *c_point, double *c, 
			struct gps_point *b_point, double *b)
{
    struct gps_point points[3];
    double distances[3];

    points[0].x = f_point->x;
    points[0].y = f_point->y;
    points[1].x = c_point->x;
    points[1].y = c_point->y;
    points[2].x = b_point->x;
    points[2].y = b_point->y;
    distances[0] = *f;
    distances[1] = *c;
    distances[2] = *b;

    for (int i = 0; i < 3; i++)
    {
	    lv_layer_t layer;
	    lv_canvas_init_layer(canvas, &layer);

	    lv_draw_line_dsc_t dsc;
	    lv_draw_line_dsc_init(&dsc);
	    dsc.color = lv_palette_main(LV_PALETTE_GREEN);
	    dsc.width = 1;
	    dsc.round_end = 1;
	    dsc.round_start = 1;
	    dsc.p1.x = to_x(current.x);
	    dsc.p1.y = to_y(current.y);
	    dsc.p2.x = to_x(points[i].x);
	    dsc.p2.y = to_y(points[i].y);
	    lv_draw_line(&layer, &dsc);
	    lv_canvas_finish_layer(canvas, &layer);
	    {
		char text[64] = { 0 };
		snprintf(text, 64, "%.2f", distances[i]);
		lv_layer_t layer;
		lv_canvas_init_layer(canvas, &layer);

		lv_draw_letter_dsc_t letter_dsc;
		lv_draw_letter_dsc_init(&letter_dsc);
		letter_dsc.color = lv_color_hex(0xaabc00);
		letter_dsc.font = lv_font_get_default();

		int16_t len = strlen(text);
    if (course_debug) 
		printf("current:%.8f %.8f(%d %d), len: %d\n", 
				current.y, 
				current.x, 
				to_y(current.y), 
				to_x(current.x), 
				len);
    if (course_debug) 
		printf("points[%d]:%.8f %.8f(%d %d)\n", i,
				points[i].y, 
				points[i].x, 
				to_y(points[i].y), 
				to_x(points[i].x));
		for (int16_t j = 0; j < len; j++)
		{
			const lv_point_t pos = { 
				/*
				.x = to_x((current.x + points[i].x) / 2.0) + (j + 1) * 8, 
				.y = to_y((current.y + points[i].y) / 2.0) 
				*/
				.x = to_x((points[i].x)) + (j + 1) * 8, 
				.y = to_y((points[i].y)) 
			};
			letter_dsc.unicode = (uint32_t)text[j % len];
			lv_draw_letter(&layer, &letter_dsc, &pos);
		}
		lv_canvas_finish_layer(canvas, &layer);
	    }
    }
}

static void draw_fcb(char *course, int h_id)
{
	int pls_num = 0;
	int greens_num = 0;
	struct gps_point *centers = NULL;

	course_get_hole_plNum_by_index(course, h_id, &pls_num);
	course_get_hole_gPtsNum_by_index(course, h_id, &greens_num);

	centers = (struct gps_point *)calloc(greens_num, sizeof(*centers));

	for (int i = 0; i < pls_num; i++)
	{
		char pl_type[8] = { 0 };
		int pl_type_len = 8;
		struct gps_point point = { 0.0, 0.0 };

		course_get_pl_n_by_index(course, h_id, i, pl_type, &pl_type_len);
		course_get_pl_pt_by_index(course, h_id, i, &point);
		if (strcmp(pl_type, "g") == 0)
		{
			centers[0].x = point.x;
			centers[0].y = point.y;
		}
		else if ((strncmp(pl_type, "gR", 2) == 0) || (strncmp(pl_type, "gL", 2) == 0))
		{
			centers[1].x = point.x;
			centers[1].y = point.y;
		}
	}

    if (course_debug) 
    {
	for (int i = 0; i < greens_num; i++)
	{
		printf("center: %.8f %.8f\n", centers[i].y, centers[i].x);
	}
    }


	for (int i = 0; i < greens_num; i++)
	{
		int points_num = 0;
		struct gps_point *points = NULL;
		struct gps_point f_point;
		struct gps_point b_point;
		double f, c ,b;

		course_get_gPts_arrayItemNum_by_index(course, h_id, i, &points_num);
		points = (struct gps_point *)calloc(points_num, sizeof(struct gps_point));
		course_get_gPts_arrayItemInfo_by_index(course, h_id, i, points, points_num); 

		for (int k = 0; k < points_num; k++)
		{
			printf("%.8f %.8f\n", points[k].y, points[k].x);
		}

		int ret = green_fcb(points, points_num, 
				&current, 
				&centers[i], 
				&f_point,
				&b_point,
				&f, &c, &b);
		if (ret != 0)
		{
			printf("course: %s, hole: %d, green: %d, current: (%.8f %.8f) fcb failed ret: %d\n",
					course, h_id, i, current.y, current.x, ret);
		}
		else
		{
    		if (course_debug) 
			printf("f: %.8f %.8f(%.2f), c: %.8f %.8f(%.2f), b: %.8f %.8f(%.2f)\n",
					f_point.y, f_point.x, f,
					centers[i].y, centers[i].x, c,
					b_point.y, b_point.x, b);
			lvgl_draw_fcb(&f_point, &f, 
				&centers[i], &c, 
				&b_point, &b);
		}


		free(points);
	}

	free(centers);
}

static void init_canvas(void)
{
    obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, MY_WIDTH, MY_HEIGHT);

    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, MY_WIDTH, MY_HEIGHT, LV_COLOR_FORMAT_ARGB8888);
    LV_DRAW_BUF_INIT_STATIC(draw_buf);

    canvas = lv_canvas_create(obj);
    lv_canvas_set_draw_buf(canvas, &draw_buf);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_palette_main(LV_PALETTE_NONE), LV_OPA_COVER);
//    lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
}

static void set_ref(char *course, int h_id, int green_id)
{
    int hls_num = 0;
    course_get_hlsNum(course, &hls_num);

    ref_min.x = 361.0;
    ref_max.x = -361.0;
    ref_min.y = 361.0;
    ref_max.y = -361.0;

    if (h_id == -1)
    {
        for (int i = 0; i < hls_num; i++)
    	    find_min_max_points(course, i);
        ref_min.x -= 0.0001;
        ref_min.y -= 0.0001;
        ref_max.x += 0.0001;
        ref_max.y += 0.0001;
    }
    else if (green_id == -1)
    {
    	find_min_max_points(course, h_id);
        ref_min.x -= 0.00005;
        ref_min.y -= 0.00005;
        ref_max.x += 0.00005;
        ref_max.y += 0.00005;
    }
    else
    {
	find_min_max_green(course, h_id, green_id, &ref_min, &ref_max);
        ref_min.x -= 0.00001;
        ref_min.y -= 0.00001;
        ref_max.x += 0.00001;
        ref_max.y += 0.00001;
    }

    
    

if (course_debug) 
    printf("ref x: %.8f, %.8f, %.8f\n", ref_min.x, ref_max.x, ref_max.x - ref_min.x);
if (course_debug) 
    printf("ref y: %.8f, %.8f, %.8f\n", ref_min.y, ref_max.y, ref_max.y - ref_min.y);
}


static void my_timer_cb(lv_timer_t *arg)
{
    if (refresh != 0)
    {
if (course_debug) 
	printf("%s fresh: %s %d\n", __func__, nearest_course_path, hole_select);
	refresh = 0;

        set_ref(nearest_course_path, hole_select, -1);
        lv_canvas_fill_bg(canvas, lv_palette_main(LV_PALETTE_NONE), LV_OPA_COVER);
        draw_hole(nearest_course_path, hole_select);
	draw_fcb(nearest_course_path, hole_select);
    }
}

static void *work_thread(void *arg)
{
    int fd = -1;
    int len = 0;
    char buf[128];

if (course_debug) 
    printf("%s hello\n", __func__);

    pthread_detach(pthread_self());

    mkfifo("./a_pipe", S_IRUSR | S_IWUSR);
    fd = open("./a_pipe", O_RDONLY);

    
    while (quit_flag == 0)
    {
	memset(buf, 0, 128);
	len = read(fd, buf, 128);
	if (len < 0)
	{
            printf("what\n");
	    break;
	}
	else if (len == 0)
	{
	    close(fd);
            fd = open("./a_pipe", O_RDONLY);
	}
	else if (len > 0)
	{
	    char *list = NULL;

	    list = (char *)calloc(256, sizeof(char));
	    snprintf(list, 256, "%s", "../bin/mapdata/map/CourseList.txt");

	    int ret = sscanf(buf, "%lf %lf", &current.y, &current.x);
	    if (ret != 2)
	    {
		printf("wrong format: %s\n", buf);
		free(list);
		continue;
	    }
	    printf("current: %.8f %.8f\n", current.y, current.x);

	    struct record **near_courses =  NULL ;
	    struct record nearest_course;
	    int near_courses_num = 100;

	    near_courses = (struct record **)calloc(near_courses_num, sizeof(struct record *));
	    ret = course_get_near_courses_by_position(list, &current, near_courses, &near_courses_num, 0.1);
	    if (ret < 0)
	    {
		    printf("ERROR too many courses to fill\n");
	    }

            ret = search_nearest_course(near_courses, 
		              	near_courses_num, 
				&nearest_course, 
				&current);
	    printf("nearest course:\n");
	    print_record(&nearest_course);

	    printf("num: %d\n", near_courses_num);

	    for (int i = 0; i < near_courses_num; i++)
	    {
		print_record(near_courses[i]);
		free_record(near_courses[i]);
		free(near_courses[i]);
	    }

	    free(near_courses);

	    if (ret < 0)
	    {
		    printf("ERROR no find nearest course\n");
	    }
	    else
	    {
  			char cwd[64] = { 0 };
  			size_t cwd_len = 64;
  			getcwd(cwd, cwd_len);
		if (course_debug) 
  			printf("cwd: %s\n", cwd);
		    snprintf(nearest_course_path, 256, "%s/%s/%s/%s.json", 
				    cwd,
				    "../bin/mapdata/MapDatabase", 
				    nearest_course.folderid, 
				    nearest_course.id);

		    ret = course_search_nearest_hole(nearest_course_path, 
				    &current, &hole_select);
		    if (ret == -1)
		    {
		    	printf("ERROR no found %s near hole(%.8f %.8f)\n",
					nearest_course_path,
					current.y, current.x
					    );
		    }
		    else
		    {
		    	refresh = 1;
		    }
		    printf("path: %s, hole: %d\n", 
				    nearest_course_path,
				    hole_select);
	    }


	    free(list);
	}
    }
    

    close(fd);
    unlink("./a_pipe");
}

int main(int argc, char **argv)
{
  char *course = NULL;
  int hole_id = 0;
  int green_id = 0;

  course = argv[1];
  hole_id = argc >= 3 ? atoi(argv[2]) : -1;
  green_id = argc >= 4 ? atoi(argv[3]) : -1;
  printf("course: %s, hole id: %d\n", course, hole_id);

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(MY_WIDTH, MY_HEIGHT);

//lv_example_canvas_8();

  init_canvas();


  timer = lv_timer_create(my_timer_cb, 1000, course);
  pthread_t pid;
  pthread_create(&pid, NULL, work_thread, NULL);
  
  if (argc == 2)
  {
      set_ref(course, hole_id, green_id);
      draw_course(course);
  }
  else if (argc == 3)
  {
      set_ref(course, hole_id, green_id);
      draw_hole(course, hole_id);
  }
  else if (argc == 4)
  {
      set_ref(course, hole_id, green_id);
      draw_green(course, hole_id, green_id);
  }
  
  
  #if LV_USE_OS == LV_OS_NONE

  /* Run the default demo */
  /* To try a different demo or example, replace this with one of: */
  /* - lv_demo_benchmark(); */
  /* - lv_demo_stress(); */
  /* - lv_example_label_1(); */
  /*
lv_example_animimg_1();
lv_example_arc_1();
lv_example_arc_2();
lv_example_arc_3();
*/
  /*
    lv_obj_t * btn;
    lv_obj_t * label;

    btn = lv_button_create(lv_screen_active());
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 20);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Original theme");
    */
//lv_example_style_14();
/*
lv_example_arclabel_1();
lv_example_bar_1();
lv_example_bar_2();
lv_example_bar_3();
lv_example_bar_4();
lv_example_bar_5();
lv_example_bar_6();
lv_example_bar_7();
lv_example_button_1();
lv_example_button_2();
lv_example_button_3();

lv_example_buttonmatrix_1();
lv_example_buttonmatrix_2();
lv_example_buttonmatrix_3();

lv_example_calendar_1();
lv_example_calendar_2();

lv_example_canvas_1();
lv_example_canvas_2();
lv_example_canvas_3();
lv_example_canvas_4();
lv_example_canvas_5();
lv_example_canvas_6();
lv_example_canvas_7();
lv_example_canvas_8();
lv_example_canvas_9();
lv_example_canvas_10();
lv_example_canvas_11();

lv_example_chart_1();
lv_example_chart_2();
lv_example_chart_3();
lv_example_chart_4();
lv_example_chart_5();
lv_example_chart_6();
lv_example_chart_7();
lv_example_chart_8();

lv_example_checkbox_1();
lv_example_checkbox_2();

lv_example_dropdown_1();
lv_example_dropdown_2();
lv_example_dropdown_3();

lv_example_image_1();
lv_example_image_2();
lv_example_image_3();
lv_example_image_4();
lv_example_image_5();

lv_example_imagebutton_1();

lv_example_keyboard_1();
lv_example_keyboard_2();
lv_example_keyboard_3();

lv_example_label_1();
lv_example_label_2();
lv_example_label_3();
lv_example_label_4();
lv_example_label_5();
lv_example_label_6();

lv_example_led_1();

lv_example_line_1();

lv_example_list_1();
lv_example_list_2();

lv_example_lottie_1();
lv_example_lottie_2();

lv_example_menu_1();
lv_example_menu_2();
lv_example_menu_3();
lv_example_menu_4();
lv_example_menu_5();

lv_example_msgbox_1();
lv_example_msgbox_2();

lv_example_obj_1();
lv_example_obj_2();
lv_example_obj_3();

lv_example_roller_1();
lv_example_roller_2();
lv_example_roller_3();

lv_example_scale_1();
lv_example_scale_2();
lv_example_scale_3();
lv_example_scale_4();
lv_example_scale_5();
lv_example_scale_6();
lv_example_scale_7();
lv_example_scale_8();
lv_example_scale_9();
lv_example_scale_10();
lv_example_scale_11();
lv_example_scale_12();

lv_example_slider_1();
lv_example_slider_2();
lv_example_slider_3();
lv_example_slider_4();

lv_example_span_1();

lv_example_spinbox_1();

lv_example_spinner_1();

lv_example_switch_1();
lv_example_switch_2();

lv_example_table_1();
lv_example_table_2();

lv_example_tabview_1();
lv_example_tabview_2();

lv_example_textarea_1();
lv_example_textarea_2();
lv_example_textarea_3();
lv_example_textarea_4();
lv_example_tileview_1();
lv_example_win_1();
*/

//    lv_example_arc_3();
  /* - etc. */
  /*lv_demo_widgets();*/

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
#ifdef _MSC_VER
    Sleep(5);
#else
    usleep(5 * 1000);
#endif
  }

  #elif LV_USE_OS == LV_OS_FREERTOS

  /* Run FreeRTOS and create lvgl task */
  freertos_main();

  #endif

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}


#include "course_info.h"
#include <math.h>

double calc_2gpspoints_distance(struct gps_point *A, struct gps_point *B)
{
	double Ax = A->x * M_PI / 180.0;
	double Ay = A->y * M_PI / 180.0;
	double Bx = B->x * M_PI / 180.0;
	double By = B->y * M_PI / 180.0;
	double lat;
	double lon;
	double a;
	double c;
	double d;
	double R = 6371.0 * 1000;

	lon = Bx - Ax;
	lat = By - Ay;
	a = pow(sin(lat / 2), 2) + cos(Ay) * cos(By) * pow(sin(lon / 2), 2);
	c = 2 * atan2(sqrt(a), sqrt(1 - a));
	return d = R * c;
}

static int isLineIntersectSide(struct gps_point *A, struct gps_point *B,
		struct gps_point *C, struct gps_point *D)
{
	double fc = 0.0f;
	double fd = 0.0f;

	fc = (C->y - A->y) * (A->x - B->x) - (C->x - A->x) * (A->y - B->y);
	fd = (D->y - A->y) * (A->x - B->x) - (D->x - A->x) * (A->y - B->y);

	if (fc * fd > 0)
		return 0;
	else
		return 1;
}

/*
 *
 * TODO shi fou ying gai yong ping mian dian dai ti gps dian zuo biao??
 * compute line(A-B) cross point with line segment(C-D)
 * return value: 	1 get point
 * 			0 no cross point
 * @E: the cross point
 *
 */
static int intersectionPoint2LinesSegments(struct gps_point *A, struct gps_point *B,
		struct gps_point *C, struct gps_point *D,
		struct gps_point *E)
{
	double a1 = 0.0f;
	double a2 = 0.0f;
	double b1 = 0.0f;
	double b2 = 0.0f;
	double c1 = 0.0f;
	double c2 = 0.0f;
	double d = 0.0f;
	int result = 0;

	a1 = B->y - A->y;
	a2 = D->y - C->y;

	b1 = -(B->x - A->x);
	b2 = -(D->x - C->x);

	c1 = -A->x * B->y + B->x * A->y;
	c2 = -C->x * D->y + D->x * C->y;

	d = a1 * b2 - a2 * b1;

	// mei kao lv gong xian deng wen ti
	result = isLineIntersectSide(A, B, C, D);
	if (result == 1)
	{
		/*
		fprintf(stdout, "%s: a1: %.8f, a2: %.8f, b1: %.8f, b2: %.8f, c1: %.8f, c2: %.8f, d: %.8f\n",
				__func__, a1, a2, b1, b2, c1, c2, d);
		*/
		E->x = (b1 * c2 - b2 * c1) / d;
		E->y = (a2 * c1 - a1 * c2) / d;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 *	get the fcb for green 
 *	@green:		(in)green points
 *	@pts_num:	(in)green points num
 *	@current:	(in)user current gps info
 *	@center:	(in)center of green
 *	@f:		(out)front point
 *	@b:		(out)rear point
 *	@f_distance:	(out)front distance
 *	@b_distance:	(out)rear distance
 */
static int s_green_fcb(struct gps_point *green, int pts_num, 
		struct gps_point *current, 
		struct gps_point *center, 
		struct gps_point *f,
		struct gps_point *b,
		double *f_distance,
		double *b_distance)
{
	int i = 0;
	int ret = 0;
	struct gps_point out[2] = {{ 0.0, 0.0 }, {0.0, 0.0}};
	int out_index = 0;

	for (i = 0; i < pts_num; i++)
	{
		ret = intersectionPoint2LinesSegments(current, center, 
				&green[(i) % pts_num],
				&green[(i + 1) % pts_num],
				&out[out_index]);
		//printf("pts_num: %d, ret: %d\n", i, ret);
		if (ret)
		{
			out_index++;
		}
	}

	if (out_index == 2)
	{
		double distance1 = 0.0;
		double distance2 = 0.0;

		distance1 = calc_2gpspoints_distance(current, &out[0]);
		distance2 = calc_2gpspoints_distance(current, &out[1]);
		if (distance1 > distance2)
		{
			b->x = out[0].x;
			b->y = out[0].y;
			f->x = out[1].x;
			f->y = out[1].y;
			*f_distance = distance2;
			*b_distance = distance1;
		}
		else
		{
			f->x = out[0].x;
			f->y = out[0].y;
			b->x = out[1].x;
			b->y = out[1].y;
			*f_distance = distance1;
			*b_distance = distance2;
		}
		return 0;
	}
	{
		printf("get points error: %d\n", out_index);
		return -1;
	}
}


static int InsidePolygon4( struct gps_point *polygon, int N, struct gps_point *pp )
{
    int i,j;
    int inside = 0;
    int count1 = 0;
    int count2 = 0;

    struct gps_point p;

    p.x = pp->x;
    p.y = pp->y;

    for (i = 0,j = N - 1;i < N;j = i++)
    {
        double value = (p.x - polygon[j].x) * (polygon[i].y - polygon[j].y) - (p.y - polygon[j].y) * (polygon[i].x - polygon[j].x);
        if (value > 0)
            ++count1;
        else if (value < 0)
            ++count2;
    }

    if (0 == count1 ||
        0 == count2)
    {
        inside = 1;
    }
    return inside;
}

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
		double *b)
{
	int ret = 0;
	struct gps_point f_point = { 0.0, 0.0 };
	struct gps_point b_point = { 0.0, 0.0 };

	/*
	double distance = 0.0;
	distance = calc_2gpspoints_distance(current, center);
	if (distance < 1e-4)
	{
		return -2;
	}
	*/
	if (InsidePolygon4(green, pts_num, current))
	{
		return -2;
	}

	ret = s_green_fcb(green, pts_num, current, center, &f_point, &b_point, f, b);
	if (ret == 0)
	{
		*c = calc_2gpspoints_distance(current, center);
		f_p->x = f_point.x;
		f_p->y = f_point.y;
		b_p->x = b_point.x;
		b_p->y = b_point.y;
		return 0;
	}
	else
	{
		return -1;
	}
}


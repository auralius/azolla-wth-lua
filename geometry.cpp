#include "stdafx.h"
#include "geometry.h"

using namespace SIMPLE_GEOM;

CGeom::CGeom(void)
{
}

CGeom::~CGeom(void)
{
}

void CGeom::DoLineToLineIntersect(CPoint &line_seg_begin_A, CPoint &line_seg_end_A, CPoint &line_seg_begin_B, CPoint &line_seg_end_B)
{
	// Line-line intersection http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
	float denom = ((line_seg_end_A.y - line_seg_begin_A.y)*(line_seg_end_B.x - line_seg_begin_B.x)) - ((line_seg_end_A.x - line_seg_begin_A.x)*(line_seg_end_B.y - line_seg_begin_B.y));
	float nume_a = ((line_seg_end_A.x - line_seg_begin_A.x)*(line_seg_begin_B.y - line_seg_begin_A.y)) - ((line_seg_end_A.y - line_seg_begin_A.y)*(line_seg_begin_B.x - line_seg_begin_A.x));
	float nume_b = ((line_seg_end_B.x - line_seg_begin_B.x)*(line_seg_begin_B.y - line_seg_begin_A.y)) - ((line_seg_end_B.y - line_seg_begin_B.y)*(line_seg_begin_B.x - line_seg_begin_A.x));
	

	// No intersection, just return the end of the line
	res = line_seg_end_A;

	
	if (denom != 0) {
		
		float ua = nume_a / denom;
		float ub = nume_b / denom;

		if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			// Get the intersection point.
			res.x = line_seg_begin_B.x + ua*(line_seg_end_B.x - line_seg_begin_B.x);
			res.y = line_seg_begin_B.y + ua*(line_seg_end_B.y - line_seg_begin_B.y);
		}
	}
}

void CGeom::DoLineToReactangleIntersect(CPoint &line_seg_begin, CPoint &line_seg_end, CPoint &rect_begin, CPoint &rect_end)
{
	// Liang-Barsky algorithm http://cs-people.bu.edu/jalon/cs480/Oct11Lab/clip.c
	CPoint p1 = line_seg_begin, p2 = line_seg_end;
	float u1 = 0.0, u2 = 1.0, dx = p2.x - p1.x, dy;

	// No intersection, just return the end of the line
	res = line_seg_end;

	if(ClipTest (-dx, p1.x - rect_begin.x, &u1, &u2))
		if(ClipTest (dx, rect_end.x - p1.x, &u1, &u2))
		{
			dy = p2.y - p1.y;
			if(ClipTest (-dy, p1.y - rect_begin.y, &u1, &u2))
				if(ClipTest (dy, rect_end.y - p1.y, &u1, &u2))
				{
					if (u2 < 1.0)
					{
						p2.x = p1.x + u2 * dx;
						p2.y = p1.y + u2 * dy;
					}
					if (u1 > 0.0)
					{
						p1.x += u1 * dx;
						p1.y += u1 * dy;
					}
					//return p1;
					if (p1 == line_seg_begin)
						res = p2;
					else
						res = p1;
				} 
		}
}

void CGeom::DoLineToEllipseIntersect(CPoint &line_seg_begin, CPoint &line_seg_end, CPoint &ellipse_rect_begin, CPoint &ellipse_rect_end)
{
	float b = abs((float)((ellipse_rect_end.y - ellipse_rect_begin.y) / 2));
	float a = abs((float)((ellipse_rect_end.x - ellipse_rect_begin.x) / 2));

	float cy = (float)((ellipse_rect_end.y + ellipse_rect_begin.y) / 2);
	float cx = (float)((ellipse_rect_end.x + ellipse_rect_begin.x) / 2);

	float m =(float)(line_seg_end.y - line_seg_begin.y)/(float)(line_seg_end.x - line_seg_begin.x);
	float c = -line_seg_begin.x*m + line_seg_begin.y;

	float A = b * b + a * a * m * m;
	float B = a * a * 2 * m *(c - cy) + b * b * (-2 * cx);
	float C = b * b * cx * cx + a * a * (c * c - 2 * c * cy + cy * cy - b * b);

	float det = B * B - 4 * A * C;

	if (det < 0) // No interssection
		res = line_seg_end;

	else if (det == 0) // Tangent value
	{
		float mu = -B/(2*A);
		float intersect_x = line_seg_begin.x + mu * (line_seg_end.x - line_seg_begin.x);
		float intersect_y = line_seg_begin.y + mu * (line_seg_end.y - line_seg_begin.y);
		res = CPoint (intersect_x, intersect_y);
	}

	else // Two intersection
	{
		float intersect_x1 = (-B + sqrt(det)) / (2 * A);
		float intersect_y1 = m * intersect_x1 + c;

		float intersect_x2 = (-B - sqrt(det)) / (2 * A);
		float intersect_y2 = m * intersect_x2 + c;

		// Check the closest to begining of line_segment
		float d1 = Calc2PointDist(line_seg_begin, CPoint(intersect_x1, intersect_y1));
		float d2 = Calc2PointDist(line_seg_begin, CPoint(intersect_x2, intersect_y2));

		int max_x = max(line_seg_begin.x,line_seg_end.x);
		int max_y = max(line_seg_begin.y,line_seg_end.y);
		int min_x = min(line_seg_begin.x,line_seg_end.x);
		int min_y = min(line_seg_begin.y,line_seg_end.y);

		res = line_seg_end;
	
		if (d1 < d2 && WithinRectangle(intersect_x1, intersect_y1, max_x, max_y, min_x, min_y))
			res = CPoint(intersect_x1, intersect_y1);
		else if (d1 >= d2 && WithinRectangle(intersect_x2, intersect_y2, max_x, max_y, min_x, min_y))
			res = CPoint(intersect_x2, intersect_y2);	
	}
}

CPoint CGeom::GetIntersectResult()
{
	return res;
}

float CGeom::GetIntersectDist(CPoint &from)
{
	return Calc2PointDist(res, from);
}

inline float CGeom::Calc2PointDist(CPoint &pt1, CPoint &pt2)
{
	return sqrt(pow((float)(pt2.x - pt1.x), 2) + pow((float)(pt2.y - pt1.y), 2));
}

CPoint CGeom::Rotate(CPoint &center_pt, float dist_from_center_pt, float theta)
{
	return CPoint(dist_from_center_pt * cos(theta) + center_pt.x, dist_from_center_pt * sin(theta) + center_pt.y);
}


///////////////////
// PRIVATE MEMBERS 
///////////////////

int CGeom::ClipTest(float p, float q, float *u1, float *u2)
{
	float r;
	int retval = TRUE;

	if (p < 0.0)
	{
		r = q / p;
		if (r > *u2)
		retval = FALSE;
		else if (r > *u1)
		*u1 = r;
	}
	else if (p > 0.0)
	{
		r = q / p;
		if (r < * u1)
		retval = FALSE;
		else if (r < *u2)
		*u2 = r;
	}
	else if(q < 0.0)
		/* p = 0, so line is parallel to this clipping edge */ 
		/* Line is outside clipping edge */
		retval = FALSE;

	return retval;
}

bool CGeom::WithinRectangle(float ptx, float pty, int max_x, int max_y, int min_x, int min_y)
{
	if (ptx <= max_x && ptx >= min_x && pty <= max_y && pty >= min_y)
		return true;

	return false;
}
// auralius@lavabit.com
//
// Contains:
//    Simple geometry for line segment, reactangle, and ellipse
//    Distance measurement
//    Rotation


#pragma once

#include <math.h>
#include <vector>

namespace SIMPLE_GEOM
{
	class CGeom
	{
	public:
		CGeom(void);
		~CGeom(void);

		void	DoLineToLineIntersect(CPoint &line_seg_begin_A, CPoint &line_seg_end_A, CPoint &line_seg_begin_B, CPoint &line_seg_end_B);
		void	DoLineToReactangleIntersect(CPoint &line_seg_begin, CPoint &line_seg_end, CPoint &rect_begin, CPoint &rect_end);
		void	DoLineToEllipseIntersect(CPoint &line_seg_begin, CPoint &line_seg_end, CPoint &ellipse_rect_begin, CPoint &ellipse_rect_end);

		float	Calc2PointDist(CPoint &pt1, CPoint &pt2);
		CPoint	Rotate(CPoint &center_pt, float dist_from_center_pt, float theta);

		CPoint	GetIntersectResult();
		float	GetIntersectDist(CPoint &from);	

	private:

		int		ClipTest(float p, float q, float *u1, float *u2);
		bool	WithinRectangle(float ptx, float pty, int max_x, int max_y, int min_x, int min_y);
		
		CPoint res;	
	};
};
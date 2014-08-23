// Author : Auralius Manurung
// Contact : auralius@lavabit.com

#include "StdAfx.h"
#include "geometry.h"
#include "Canvas.h"
#include <windows.h>

#include <iostream>
#include <fstream>



#define round(a) ( int ) ( a + .5 )

CCanvas::CCanvas(void)
{
	// Init
	mIsSnapToGrid	= false;
	mToolType		= TOOL_SELECT;
	mStatus		= STATUS_NONE;
	mZoomFactor		= 1;

	strcpy(mMessage,"\n");
}

CCanvas::~CCanvas(void)
{
	// Let's clean up !!!
	while(!mObjectContainer.empty()){
		free(mObjectContainer.back());
		mObjectContainer.pop_back();
	}
}

void CCanvas::DrawGrid(CDC *dc, HWND hwnd)
{
	// Adapted from Johan Rosengren, Abstrakt Mekanik AB
	CRect rect;
	GetClientRect(hwnd, &rect);

	int width = rect.Width();
	int height = rect.Height();
	int stepx = rect.right / (mGridsizeX * mZoomFactor) + 1;
	int stepy = rect.bottom / (mGridsizeY * mZoomFactor) + 1;

	CPen pen;
	pen.CreatePen(PS_DOT, 1, RGB(200,200,200));
	CPen *oldPen = dc->SelectObject(&pen);

	// Draw grid - x axis
	for( int x = 0 ; x <= stepx; x=x+4 )
	{
		int axisstep = mGridsizeX * x * mZoomFactor;

		dc->MoveTo(axisstep, 0);
		dc->LineTo(axisstep, height );
	}

	// Draw grid - y axis
	for( int y = 0; y <= stepy; y=y+4 )
	{
		int axisstep = mGridsizeY * y * mZoomFactor;

		dc->MoveTo(0, axisstep);
		dc->LineTo(width, axisstep);
	}

	dc->SelectObject(oldPen); // recover pen
}

void CCanvas::DrawHitMark(CDC *dc, int object_index)
{	
	if (object_index > -1){ // '-1' means no object was selected
		
		CPen sensor_pen(PS_SOLID, 2, RGB(0,0,255));
		CPen *old_pen = dc->SelectObject(&sensor_pen);

		OBJECT *obj = mObjectContainer.at(object_index);;

		// Calculate and draw selection mark
		if (obj->type == TOOL_RECTANGLE || obj->type == TOOL_CIRCLE)
		{
			float h = (obj->point[1].x - obj->point[0].x)/2;
			float w = (obj->point[1].y - obj->point[0].y)/2;

			mHitMarkPos[LCENTER] = CPoint(obj->point[0].x, obj->point[0].y + w);
			mHitMarkPos[RCENTER] = CPoint(obj->point[1].x, obj->point[0].y + w);
			mHitMarkPos[TCENTER] = CPoint(obj->point[0].x + h, obj->point[0].y);
			mHitMarkPos[BCENTER] = CPoint(obj->point[0].x + h, obj->point[1].y);
			mHitMarkPos[TLCORNER] = CPoint(obj->point[0].x, obj->point[0].y); 
			mHitMarkPos[TRCORNER] = CPoint(obj->point[1].x, obj->point[0].y); 
			mHitMarkPos[BLCORNER] = CPoint(obj->point[0].x, obj->point[1].y);
			mHitMarkPos[BRCORNER] = CPoint(obj->point[1].x, obj->point[1].y);

			for (UINT count = 0; count < 8; count++ )
				dc->Rectangle(mHitMarkPos[count].x - 3, mHitMarkPos[count].y - 3, mHitMarkPos[count].x + 3, mHitMarkPos[count].y + 3);
	}
		else if (obj->type == TOOL_LINE)
		{
			mHitMarkPos[TLCORNER] = CPoint(obj->point[0].x, obj->point[0].y);
			mHitMarkPos[BRCORNER] = CPoint(obj->point[1].x, obj->point[1].y);

			dc->Ellipse(mHitMarkPos[TLCORNER].x - 3, mHitMarkPos[TLCORNER].y - 3, mHitMarkPos[TLCORNER].x + 3, mHitMarkPos[TLCORNER].y + 3);			// arrow use 2 point only
			dc->Ellipse(mHitMarkPos[BRCORNER].x - 3, mHitMarkPos[BRCORNER].y - 3, mHitMarkPos[BRCORNER].x + 3, mHitMarkPos[BRCORNER].y + 3);	 
		}

	dc->SelectObject(old_pen);
	}
}

bool CCanvas::IsTooSmall()
{
	if (abs(mCurrentPos.x - mLClickPos.x) < 10 && abs(mCurrentPos.y - mLClickPos.y) < 10)
		return true;
	else
		return false;
}

int CCanvas::GetResizingDirection()
{
	if (mStatus == STATUS_RESIZING)
		return mResizeDirection;

	for (UINT count = 0; count < 8; count++ ){
		if (mCurrentPos.x >= (mHitMarkPos[count].x - 2) && mCurrentPos.x <= (mHitMarkPos[count].x + 2) && 
		mCurrentPos.y >= (mHitMarkPos[count].y - 2) && mCurrentPos.y <= (mHitMarkPos[count].y + 2))
		{
			switch (count){
				case LCENTER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
					break;
				case RCENTER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
					break;
				case TCENTER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
					break;
				case BCENTER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
					break;
				case TLCORNER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
					break;
				case BLCORNER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
					break;
				case TRCORNER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
					break;
				case BRCORNER:
					::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
			}
			return count;
		}
	}

	//::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return NONE;		
}

void CCanvas::ResizeObject()
{
	if (mIndex > -1)
	{
		OBJECT *obj = mObjectContainer.at(mIndex);

		int h = obj->point[1].x - obj->point[0].x;
		int w = obj->point[1].y - obj->point[0].y;
		
		if (mResizeDirection == TLCORNER)
		{
			obj->point[0].x = obj->point[0].x + mDeltaPos.x;
			obj->point[0].y = obj->point[0].y + mDeltaPos.y;
		}
		else if (mResizeDirection == TRCORNER)
		{
			obj->point[1].x = obj->point[0].x + h + mDeltaPos.x;
			obj->point[0].y = obj->point[0].y + mDeltaPos.y;
		}
		else if (mResizeDirection == BLCORNER)
		{
			obj->point[1].y = obj->point[0].y + w + mDeltaPos.y;
			obj->point[0].x = obj->point[0].x + mDeltaPos.x;
		}
		else if (mResizeDirection == BRCORNER)
		{
			obj->point[1].x = obj->point[0].x + h + mDeltaPos.x;
			obj->point[1].y = obj->point[0].y + w + mDeltaPos.y;
		}
		else if (mResizeDirection == RCENTER)
		{
			obj->point[1].x = obj->point[0].x + h + mDeltaPos.x;
		}
		else if (mResizeDirection == LCENTER)
		{
			obj->point[0].x = obj->point[0].x + mDeltaPos.x;
		}
		else if (mResizeDirection == TCENTER)
		{
			obj->point[0].y = obj->point[0].y + mDeltaPos.y;
		}
		else if (mResizeDirection == BCENTER)
		{
			obj->point[1].y = obj->point[0].y + w + mDeltaPos.y;
		}		
	}
}

void CCanvas::SwapLong(LONG &a, LONG &b)
{
	LONG buf;
	buf = a;
	a = b;
	b = buf;
}

bool CCanvas::FindObject(CPoint pt)
{
	DeselectAllObjects();

	for (INT count = mObjectContainer.size()-1 ; count >= 0; count--)
	{
		OBJECT *obj = mObjectContainer.at(count);		

		if (obj->type == TOOL_RECTANGLE)
		{
			// We have four quadrants! Be careful !!!
			if (obj->point[0].x > obj->point[1].x)
				SwapLong(obj->point[0].x, obj->point[1].x);
			if (obj->point[0].y > obj->point[1].y)
				SwapLong(obj->point[0].y, obj->point[1].y);

			if( pt.x >= obj->point[0].x && pt.x <= obj->point[1].x && pt.y >= obj->point[0].y && pt.y <= obj->point[1].y )
			{
				obj->selected = true;
				mIndex = count;
				return true;
			}
		}

		else if (obj->type == TOOL_CIRCLE)
		{
			float b = abs((obj->point[1].y - obj->point[0].y) / 2);
			float a = abs((obj->point[1].x - obj->point[0].x) / 2);

			float cy = (obj->point[1].y + obj->point[0].y) / 2;
			float cx = (obj->point[1].x + obj->point[0].x) / 2;

			if (((pt.x - cx) * (pt.x - cx) / (a * a) + (pt.y - cy) * (pt.y - cy) / (b * b)) <= 1)
			{
				obj->selected = true;
				mIndex = count;
				return true;
			}
		}

		else if (obj->type == TOOL_LINE)
		{
			bool checkX, checkY;
			float a = obj->point[1].y - obj->point[0].y; // a = y2 - y1
			float b = obj->point[1].x - obj->point[0].x; // b = x2 - x1

			// watch for vertical lines, it will give b = 0!
			if (abs(b) < 20){
				checkY = pt.y <= max(obj->point[0].y, obj->point[1].y) + 5 && pt.y >= min (obj->point[0].y, obj->point[1].y) - 5;
				checkX = pt.x <= max(obj->point[0].x, obj->point[1].x) + 5 && pt.x >= min (obj->point[0].x, obj->point[1].x) - 5; 
			}

			else{
				float y = a / b * (pt.x - obj->point[0].x) + obj->point[0].y;
				checkX = pt.x <= max(obj->point[0].x, obj->point[1].x) + 2 && pt.x >= min (obj->point[0].x, obj->point[1].x) - 2;
				checkY = pt.y >= y - 15 && pt.y <= y + 5;
			}
		
			//if (pt.y >= y - 10 && pt.y <= y + 10 && pt.x <= max(obj->point[0].x, obj->point[1].x) + 2 && pt.x >= min (obj->point[0].x, obj->point[1].x) - 2)
			if (checkX && checkY)
			{
				obj->selected = true;
				mIndex = count;
				return true;
			}			
		}
			
	}

	return false;
}

CPoint CCanvas::GetLineIntersectionWithObjects(CPoint &line_segment_begin, CPoint &line_segment_end)
{
	SIMPLE_GEOM::CGeom geom;
	CPoint res = line_segment_end; // Put maximum value cause we want to find minimum value

	for (INT count = mObjectContainer.size()-1 ; count >= 0; count--)
	{
		OBJECT *obj = mObjectContainer.at(count);

		if (obj->type == TOOL_LINE)
			geom.DoLineToLineIntersect(line_segment_begin, line_segment_end, obj->point[0], obj->point[1]);

		else if (obj->type == TOOL_RECTANGLE)
			geom.DoLineToReactangleIntersect(line_segment_begin, line_segment_end, obj->point[0], obj->point[1]);

		else if (obj->type == TOOL_CIRCLE)
			geom.DoLineToEllipseIntersect(line_segment_begin, line_segment_end, obj->point[0], obj->point[1]);

		// Find minimum value
		if (geom.GetIntersectDist(line_segment_begin)< geom.Calc2PointDist(line_segment_begin, res))
			res = geom.GetIntersectResult();		
	}

	return res;
}


void CCanvas::DeselectAllObjects()
{
	for (UINT count = 0; count < mObjectContainer.size(); count++)
	{
		OBJECT *obj = mObjectContainer.at(count);
		if (obj->selected == true)
			obj->selected = false;
	}
	mIndex = -1;
}

void CCanvas::MoveSelectedObject()
{
	// Update movement
	for (UINT count = 0; count < mObjectContainer.size(); count++)
	{
		OBJECT *obj = mObjectContainer.at(count);

		if (obj->selected){
			int h = obj->point[1].x - obj->point[0].x;
			int w = obj->point[1].y - obj->point[0].y;

			obj->point[0].x = obj->point[0].x + mDeltaPos.x;
			obj->point[0].y = obj->point[0].y + mDeltaPos.y;
			obj->point[1].x = obj->point[0].x + h;
			obj->point[1].y = obj->point[0].y + w;
		}
	}
}

void CCanvas::SelectAll()
{
	mToolType = TOOL_SELECT_ALL;
	mIndex = -1;

	if (!mObjectContainer.empty()){

		for (UINT count = 0; count < mObjectContainer.size(); count++)
		{
			OBJECT *obj = mObjectContainer.at(count);
			obj->selected = true;
		}
	}
}

void CCanvas::DrawFromContainer(CDC *dc)
{
	for (UINT count = 0; count < mObjectContainer.size(); count++){
		OBJECT *obj = mObjectContainer.at(count);
		CRect r(obj->point[0], obj->point[1]);

		if (obj->type == TOOL_RECTANGLE)
			dc->Rectangle(&r);
		else if (obj->type == TOOL_CIRCLE)
			dc->Ellipse(&r);
		else if (obj->type == TOOL_LINE){
			dc->MoveTo(obj->point[0]);
			dc->LineTo(obj->point[1]);
		}

		if (obj->selected == true)
			DrawHitMark(dc, count);
	}
}

void CCanvas::OnDraw(CDC *dc)
{
	// Draw message at the upper part
	SetTextColor(*dc, RGB(0,0,127));
	dc->DrawText(mMessage, CRect(0, 0, 800, 800),0);

	// SetTool pen
	CPen sensor_pen(PS_SOLID, 2, OBSTACLE);
	CPen *old_pen = dc->SelectObject(&sensor_pen);

	DrawFromContainer(dc);

	switch (mStatus){

		case STATUS_DRAWING:
			if (IsTooSmall())
				break;

			if (mToolType == TOOL_RECTANGLE){
				CRect r(mLClickPos, mCurrentPos);
				dc->Rectangle(&r);
			}
			else if (mToolType == TOOL_CIRCLE){
				CRect r(mLClickPos, mCurrentPos);
				dc->Ellipse(&r);
			}
			else if (mToolType == TOOL_LINE){
				dc->MoveTo(mLClickPos);
				dc->LineTo(mCurrentPos);
			}
			break;

		case STATUS_MOVING: 	
			MoveSelectedObject();
			break;

		case STATUS_RESIZING:
			ResizeObject();
			break;

		case STATUS_DELETING:
			DeleteSelectedObject();
	}

	// Return previous pen and brush
	dc->SelectObject(old_pen); 

}

void CCanvas::OnMouseMove(CPoint pt)
{
	//SnapToGrid(pt);

	mDeltaPos = pt - mCurrentPos;
	mCurrentPos = pt;

	GetResizingDirection();	
}

void CCanvas::OnLButtonDown(CPoint pt)
{
	//SnapToGrid(pt);
	mLClickPos = pt;

	switch (mToolType){
		case TOOL_SELECT:
			mResizeDirection = GetResizingDirection();
			if (mResizeDirection != NONE)
				mStatus = STATUS_RESIZING;
			else if (FindObject(pt) == true)
				mStatus = STATUS_MOVING;
			break;

		case TOOL_SELECT_ALL:
			mStatus = STATUS_MOVING;
			break;

		case TOOL_DELETE:
			if (FindObject(pt) == true)
				mStatus = STATUS_DELETING;
			break;

		default:
			mStatus = STATUS_DRAWING;
	}
}

void CCanvas::OnLButtonUp(CPoint pt)
{
	SnapToGrid(pt);
	mCurrentPos = pt;

	mStatus = STATUS_NONE;

	if (mToolType == TOOL_SELECT_ALL)
		mToolType = TOOL_SELECT;

	StoreObject();
}

int CCanvas::Zoom(float zoom_factor)
{
	// Store all history of zooming
	mZoomFactor = mZoomFactor * zoom_factor;
	
	if (mObjectContainer.empty())
		return 0;

	for (UINT count = 0; count < mObjectContainer.size(); count++)
	{
		OBJECT *obj = mObjectContainer.at(count);
		
		float point_x = obj->point[0].x * zoom_factor;
		float point_y = obj->point[0].y * zoom_factor;
		obj->point[0] = CPoint(point_x, point_y);

		point_x = obj->point[1].x * zoom_factor;
		point_y = obj->point[1].y * zoom_factor;
		obj->point[1] = CPoint(point_x, point_y);
	}

	return 1;
}

int CCanvas::DeleteSelectedObject()
{
	if (!FindObject(mCurrentPos))
		return -1;

	if (mObjectContainer.empty())
		return -1;

	mObjectContainer.erase(mObjectContainer.begin()+mIndex);
	mIndex = -1;

	return 0;
}

void CCanvas::SetSnapToGrid(bool setting, int gridsizeX, int gridsizeY)
{
	mIsSnapToGrid = setting;
	mGridsizeX = gridsizeX;
	mGridsizeY = gridsizeY;
}

void CCanvas::SnapToGrid(CPoint &pt)
{
	if (mIsSnapToGrid)
		pt = CPoint(pt.x / mGridsizeX * mGridsizeX, pt.y / mGridsizeY * mGridsizeY);
}

void CCanvas::SetTool(int tool)
{
	DeselectAllObjects();
	mToolType = tool;
}

void CCanvas::StoreObject()
{
	// If we want to store new object, it should be at least bigger then the minimum size
	//		and also not in selecting or deleting mode.
	if (!IsTooSmall()&& mToolType != TOOL_SELECT && mToolType != TOOL_DELETE){

		OBJECT* obj = (OBJECT*)malloc(sizeof(OBJECT));
		obj->type = mToolType;
		obj->point[0] = mLClickPos;
		obj->point[1] = mCurrentPos;
		obj->selected = false;
		mObjectContainer.push_back(obj);
	}
}

bool CCanvas::LoadFile(char *fileName)
{
	std::ifstream file(fileName);
	char str[20];
	
	if(file.is_open()){
		while(!file.eof()){
			OBJECT* obj = (OBJECT*)malloc(sizeof(OBJECT));
			file.getline(str, 20);
			obj->type = atoi(str);
			file.getline(str, 20);
			obj->point[0].x = atoi(str);
			file.getline(str, 20);
			obj->point[0].y = atoi(str);
			file.getline(str, 20);
			obj->point[1].x = atoi(str);
			file.getline(str, 20);
			obj->point[1].y = atoi(str);
			obj->selected = false;
			if (strcmp(str,""))	mObjectContainer.push_back(obj);
			else free(obj);
		}
		file.close();

		return true;
	}

	return false;
}

bool CCanvas::SaveFile(char *fileName)
{
	if (mObjectContainer.empty())
		return false; // An empty storage

	Zoom(1.0/mZoomFactor);

	std::ofstream file;
	file.open(fileName);

	for (UINT count = 0; count < mObjectContainer.size(); count++)
	{
		OBJECT *obj = mObjectContainer.at(count);
		file << obj->type << "\n";
		file << obj->point[0].x << "\n";
		file << obj->point[0].y << "\n";
		file << obj->point[1].x << "\n";
		file << obj->point[1].y << "\n";
	}

	file.close();

	return true;
}

void CCanvas::ClearAll()
{
	mObjectContainer.clear();
}

void CCanvas::SetMessage(char message[128])
{
	strcpy(mMessage, message);
}
#pragma once

#include <math.h>
#include <vector>

enum
{
	TOOL_RECTANGLE,
	TOOL_CIRCLE,
	TOOL_LINE,
	TOOL_DELETE,
	TOOL_SELECT,
	TOOL_SELECT_ALL,
	TOOL_SPECIAL, // Special object is SELECTABLE only

	STATUS_MOVING,
	STATUS_DRAWING,
	STATUS_RESIZING,
	STATUS_NONE,
	STATUS_DELETING,
};

class CCanvas
{
public:
	typedef struct
	{
		int		type;
		CPoint	point[2]; // Stores coordinate points
		bool	selected;
	} OBJECT;

	CCanvas(void);
	~CCanvas(void);

	bool	LoadFile(char *fileName);
	bool	SaveFile(char *fileName);

	void	SetSnapToGrid(bool setting, int gridsizeX, int gridsizeY);
	void	SetTool(int tool);
	void	SetMessage(char message[128]);

	int		GetTool(){return mToolType;};
	
	void	DrawGrid(CDC *dc, HWND hwnd);
	void	OnDraw(CDC *dc);
	void	OnMouseMove(CPoint pt);
	void	OnLButtonDown(CPoint pt);
	void	OnLButtonUp(CPoint);

	int		Zoom(float zoom_factor);
	void	SelectAll();
	void	ClearAll();

	CPoint 	GetLineIntersectionWithObjects(CPoint& pt1, CPoint &pt2);

private:
	void	SwapLong(LONG &a, LONG &b);
	bool	IsTooSmall();
	void	SnapToGrid(CPoint &point);

	bool	FindObject(CPoint pt);
	void	MoveSelectedObject();
	void	DeselectAllObjects();
	int		DeleteSelectedObject();
	void	StoreObject();

	int		GetResizingDirection();
	void	ResizeObject();

	void	DrawFromContainer(CDC *dc);
	void	DrawHitMark(CDC *dc, int object_index);



	bool	mIsSnapToGrid;
	int		mGridsizeX;
	int		mGridsizeY;

	float	mZoomFactor;

	char	mMessage[128];

	int		mIndex; // Index of a selected object
	int		mStatus; // Drawing, moving, selecting, resizing or deleting?

	int		mToolType; // Selection, rectangle, circle, line, ore delete?

	int		mResizeDirection;

	CPoint  mLClickPos; // Left click position
	CPoint	mCurrentPos; // Current mouse position
	CPoint	mDeltaPos;	// Position shift on mouse pointer
	CPoint	mHitMarkPos[8];

	// Define storage and its iterator
	std::vector<OBJECT *>	mObjectContainer;
};

#define LCENTER		7
#define RCENTER		3
#define TCENTER		1
#define BCENTER		5
#define TLCORNER	0
#define BLCORNER	6
#define TRCORNER	2
#define BRCORNER	4
#define NONE		100

/*

  0------1------2
  |             |
  |             |
  7             3
  |             |
  |             |
  6------5------4
*/
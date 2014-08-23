// Author : Auralius Manurung
// Contact : auralius@lavabit.com

#include "StdAfx.h"
#include "math.h"
#include "Robot.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTORS ///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

CRobot::CRobot(void)
{
	// Default setting when CRobot is created

	mCanDrawTrajectory = false;
	mZoom = 1;

	SetSensorNum(6); // Default: 6 sensors
	SetRadiationCone();
	SetSensorRange();
	SetNoiseProperties(0.0, 0.0);
	SetSimulationTimeStep();
	SetSensorSamplingRate();


	AddRobot(CPoint(200, 200));
	SetActiveRobot(0);

	PrepareToRun(); // Init certain variables for simulation
}

CRobot::~CRobot(void)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

inline float CRobot::UniformNoise()
{
	return ((float)rand()/(float)(RAND_MAX+1));
}

float CRobot::GaussianNoise(const float &mean, const float &deviation)
{
	// Press W.H., Teukolsky S.A., Vetterling W.T., Flannery B.P. Numerical recipes in C, pp 289-290
	if (mean == 0.0 && deviation == 0.0)
		return 0.0;

	float x,v1,v2,r;
	if (mNoiseTime == 0.0){
		do {
			v1 = 2.0 * UniformNoise() - 1.0;
			v2 = 2.0 * UniformNoise() - 1.0;
			r = v1 * v1 + v2 * v2;
		} while (r >= 1.0);

		r = sqrt((-2.0 * log(r)) / r);
		mNoiseTime = v2 * r;
		return (mean + v1 * r * deviation);
	}
		else{
			x = mNoiseTime;
			mNoiseTime = 0.0;
			return (mean + x * deviation);
		}
}

bool CRobot::UpdateSensorValue(float accuracy, bool manual_update)
{
	if (mSimulationCount < mSensorSamplingRate / mSimulationTimeStep && !manual_update)
		return false;

	float step = TWO_PI / mSensorNum;

	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);

		float sensor_dist = robot->Size / 2;

		for (int sensor_index = 0; sensor_index < mSensorNum; sensor_index++){
			float min_dist = mSensorRange[MAX_VAL];
			float pivot_angle = robot->Theta + sensor_index * step;

			// Calculate sensor position 
			CPoint sensor_position = mGeom.Rotate(CPoint(robot->XPos, robot->YPos), sensor_dist, pivot_angle);
							
			// measure to object
			for (float i = 0; i <= 20; i++){
				CPoint pt_max = mGeom.Rotate(sensor_position, mSensorRange[MAX_VAL], pivot_angle - mRadiationCone + i / 10 * mRadiationCone);
				CPoint res = mCanvas->GetLineIntersectionWithObjects(sensor_position,pt_max);
				float dist = mGeom.Calc2PointDist(sensor_position, res);

				// measure to other robot
				for (UINT count_ = 0; count_ < mVectRobot.size(); count_++){
					ROBOT *robot_ = &mVectRobot.at(count_);
					if (robot != robot_){ // find intersection with other robot but itself
						mGeom.DoLineToEllipseIntersect(sensor_position, pt_max, CPoint(robot_->XPos - sensor_dist, robot_->YPos - sensor_dist), CPoint(robot_->XPos + sensor_dist, robot_->YPos + sensor_dist));
						float dist_ = mGeom.GetIntersectDist(sensor_position);
						if (dist_ < dist)
							dist = dist_;
					}
				}

				if (dist < min_dist)
					min_dist = dist;
			}

			// Should not smaller than desired minimum value
			if (min_dist >=  mSensorRange[MIN_VAL])
				robot->SensorValue[sensor_index] = min_dist + GaussianNoise(mMeanNoise,mDevNoise);
			else
				robot->SensorValue[sensor_index] = mSensorRange[MIN_VAL];
		}
	}

	mSimulationCount = 0;
	return true;
}

void CRobot::UpdateTrajectoryData(ROBOT *robot)
{
	CPoint pos = CPoint(robot->XPos, robot->YPos);
	UINT size = robot->Trail.size();
	
	if (size > 0){
		if ( mGeom.Calc2PointDist(pos, robot->Trail.at(size - 1)) > 5.0)
			robot->Trail.push_back(pos);
	}
	else
		robot->Trail.push_back(pos);
}

void CRobot::DrawTrajectory(CDC *pDC, ROBOT *robot)
{
	UINT size = robot->Trail.size();

	// Draw if not empty
	if (size > 1)
	{
		CPen trail_pen(PS_DOT, 2, robot->RobotColor);
		CPen *old_pen = pDC->SelectObject(&trail_pen);
		for (UINT count = 1; count < size; count++)
		{
			CPoint start = robot->Trail.at(count-1);
			CPoint end = robot->Trail.at(count);

			pDC->MoveTo(start);
			if (mGeom.Calc2PointDist(start, end) < 10)
				pDC->LineTo(end);
		}
		pDC->SelectObject(old_pen);
	}
}

void CRobot::DrawSensorBeam(ROBOT *robot, CDC * pDC)
{
	CPen sensor_pen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen *old_pen = pDC->SelectObject(&sensor_pen);

	float sensor_dist = robot->Size / 2 - 3;

	// draw sensor beam
	for (int sensor_index = 0; sensor_index < mSensorNum; sensor_index++){
		float pivot_angle = robot->Theta + sensor_index * TWO_PI / mSensorNum;
		CPoint sensor_position = mGeom.Rotate(CPoint(robot->XPos, robot->YPos), sensor_dist, pivot_angle);

		pDC->MoveTo(sensor_position);
		// To draw sensor beam, we make polygon from sensor value (rotated 3 times) + 1 current sensor position
		for (float i = 0; i <= 2; i = i ++){
			pDC->LineTo(mGeom.Rotate(sensor_position, robot->SensorValue[sensor_index], pivot_angle - mRadiationCone + i * mRadiationCone));
		}
		pDC->LineTo(sensor_position);
	}

	pDC->SelectObject(old_pen);
}

void CRobot::DrawLocationMark(CDC *pDC)
{
	CBrush brush;
	brush.CreateSolidBrush(RGB(255,255,0));
	CBrush *old_brush = pDC->SelectObject(&brush);

	for (UINT count = 0; count < mMark.size(); count++){
		int x = mMark.at(count).x - 10;
		int y = mMark.at(count).y - 10;
		int x_ = mMark.at(count).x + 10;
		int y_ = mMark.at(count).y + 10;

		pDC->Rectangle(x,y,x_,y_);	
		pDC->MoveTo(x,y);
		pDC->LineTo(x_,y_);
		pDC->MoveTo(x_,y);
		pDC->LineTo(x,y_);

	}

	pDC->SelectObject(old_brush);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBERS//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void CRobot::SetCanvasAddr(CCanvas *canvas_addr)
{
	mCanvas = canvas_addr;
}

void CRobot::AddRobot(CPoint &initial_pos)
{
	ROBOT new_robot;
	new_robot.XPos = initial_pos.x;
	new_robot.YPos = initial_pos.y;
	new_robot.Theta = 0;
	new_robot.Size = 40 * mZoom;
	new_robot.LWheelSpeed = 0;
	new_robot.RWheelSpeed = 0;
	new_robot.HittingWall = false;

	// Random initial color
	COLORREF random_color;
	srand(mVectRobot.size()+time(0));
	do{
		random_color = RGB(UniformNoise()*255, UniformNoise()*255, UniformNoise()*255);
		
	} while (random_color < RGB(255,255,255)/2); // Get rid off dark colors

	new_robot.RobotColor = random_color;
	mVectRobot.push_back(new_robot);

	PrepareToRun(); // Do some preparations  :D
}

void CRobot::RemoveRobot()
{
	if (mVectRobot.size() > 1)
		mVectRobot.erase(mVectRobot.end()-1);
}

void CRobot::AddLocationMark(CPoint &pos)
{
	mMark.push_back(pos);
}

void CRobot::RemoveLocationMark()
{
	if (!mMark.empty())
		mMark.erase(mMark.end()-1);
}

void CRobot::SetActiveRobot(UINT index)
{
	if (index < mVectRobot.size()){
		mActiveRobot = index;
		mRobot = &mVectRobot.at(index);
	}
}

void CRobot::DrawRobot(CDC* pDC)
{
	const float deg45 = M_PI / 4;
	const float deg135 = M_PI / 4 * 3;
	const float deg15 = M_PI / 180 * 15;

	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);

		DrawSensorBeam(robot, pDC);
		DrawTrajectory(pDC, robot);

		float robot_radius = robot->Size / 2;
		CPoint pos = CPoint(robot->XPos, robot->YPos);

		pDC->MoveTo(pos);

		// Draw robot
		CPen robot_pen(PS_SOLID, 1, OBSTACLE);
		CPen *old_pen = pDC->SelectObject(&robot_pen);

		CBrush brush;
		brush.CreateSolidBrush(robot->RobotColor);
		CBrush *old_brush = pDC->SelectObject(&brush);

		pDC->Ellipse(robot->XPos - robot_radius, robot->YPos - robot_radius, robot->XPos + robot_radius, robot->YPos + robot_radius);	

		pDC->SelectObject(old_brush);
		
		int l = robot_radius - 4;

		// For wheels
		CPoint pt1 = mGeom.Rotate(pos, l, robot->Theta + deg45);
		CPoint pt2 = mGeom.Rotate(pos, l, robot->Theta + deg135);
		CPoint pt3 = mGeom.Rotate(pos, l, robot->Theta - deg45 );
		CPoint pt4 = mGeom.Rotate(pos, l, robot->Theta - deg135);

		// For Head
		CPoint pt0 = mGeom.Rotate(pos, robot_radius, robot->Theta);
		CPoint pt5 = mGeom.Rotate(pos, l, robot->Theta + deg15 );
		CPoint pt6 = mGeom.Rotate(pos, l, robot->Theta - deg15);
		
		// Draw head
		pDC->MoveTo(pt5);
		pDC->LineTo(pt0);
		pDC->LineTo(pt6);

		// Draw wheels
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		pDC->MoveTo(pt3);
		pDC->LineTo(pt4);

		pDC->SelectObject(old_pen);
	}

	DrawLocationMark(pDC);
}

void CRobot::SetDrawTrajectoryStatus(bool status)
{
	mCanDrawTrajectory = status;

	if (!status)
	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);
		robot->Trail.clear();
	}
}

void CRobot::SetSimulationTimeStep(float time_step)
{
	mSimulationTimeStep = time_step;
}

void CRobot::SetSensorSamplingRate(float sampling_rate)
{
	mSensorSamplingRate = sampling_rate;
}

void CRobot::GoRobotGo()
{
	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);

		if (mSimulationCount < mSensorSamplingRate / mSimulationTimeStep){
			if (mCanDrawTrajectory)
				UpdateTrajectoryData(robot);
			
			robot->HittingWall = SimpleCollisionDetection(robot);
		}

		if (!robot->HittingWall){
			float plus_factor = robot->RWheelSpeed + robot->LWheelSpeed;
			float minus_factor = robot->LWheelSpeed - robot->RWheelSpeed;
			float half_plus_factor = plus_factor / 2;
			float a = robot->Size / 2 * plus_factor / minus_factor;
			float b = robot->Theta + minus_factor * mSimulationTimeStep / robot->Size;
			float cos_theta = cos(robot->Theta);
			float sin_theta = sin(robot->Theta);

			if (minus_factor != 0.0){
				robot->Theta =  b;
				robot->XPos = robot->XPos + a * (sin(b) - sin_theta);
				robot->YPos = robot->YPos - a * (cos(b) - cos_theta);
			}
			else{
				robot->XPos = half_plus_factor * cos_theta * mSimulationTimeStep + robot->XPos;
				robot->YPos = half_plus_factor * sin_theta * mSimulationTimeStep + robot->YPos;
			}
		}
	}

	mSimulationCount ++;
}

void CRobot::SetSpeed(float l_speed, float r_speed)
{
		mRobot->LWheelSpeed = l_speed * mZoom;
		mRobot->RWheelSpeed = r_speed * mZoom;
}

void CRobot::PrepareToRun()
{
	// Random seed
	srand(time(0));
	mNoiseTime = 0.0;
	mSimulationCount = 0;

	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);

		for (int sensor_index = 0; sensor_index < mSensorNum; sensor_index++)
				robot->SensorValue[sensor_index] = 0;
	}
}

void CRobot::SetPosition(CPoint &pt)
{
	mRobot->XPos = pt.x;
	mRobot->YPos = pt.y;
}

void CRobot::SetLocationMarPosition(int index, CPoint &pt)
{
	mMark.at(index) = pt;
}

void CRobot::ResetPosition()
{
	 // Line up the robot horizontally
	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);
		robot->XPos = 100 + (robot->Size + 10) * count;
		robot->YPos = 100;
		robot->Theta = 0;
		robot->HittingWall = false;
	}
}

void CRobot::SetAngle(float theta)
{
	mRobot->Theta = theta;
}

void CRobot::SetRadiationCone(float cone_angle) 
{
	mRadiationCone = cone_angle / 2;
}

void CRobot::SetNoiseProperties(float mean, float dev)
{
	mMeanNoise = mean;
	mDevNoise = dev;
}

void CRobot::SetSensorRange(float min_dist, float max_dist)
{
	if (min_dist >= 0 && max_dist <= 1000){
		mSensorRange[MIN_VAL] = min_dist;
		mSensorRange[MAX_VAL] = max_dist;
	}
}

bool CRobot::IsRobotDetected(CPoint &pt)
{
	for (UINT count = 0; count < mVectRobot.size(); count++){
		mRobot = &mVectRobot.at(count);
		float robot_radius = mRobot->Size / 2;

		if (pt.x > mRobot->XPos - robot_radius && pt.x < mRobot->XPos + robot_radius
			&& pt.y > mRobot->YPos - robot_radius && pt.y < mRobot->YPos + robot_radius){
				SetActiveRobot(count);
				return true;
		}
	}

	return false;
}

int CRobot::IsLocationMarkDetected(CPoint &pt)
{
	for (UINT count = 0; count < mMark.size(); count++){

		if (pt.x > mMark.at(count).x - 10 && pt.x < mMark.at(count).x + 10
			&& pt.y > mMark.at(count).y - 10 && pt.y < mMark.at(count).y + 10){
				return count;
		}
	}

	return -1;
}

void CRobot::Zoom(float zoom_factor)
{
	mZoom = mZoom * zoom_factor;
	for (UINT count = 0; count < mVectRobot.size(); count++){
		ROBOT *robot = &mVectRobot.at(count);
		SetSize(robot->Size * zoom_factor);
		SetPosition(CPoint((int)robot->XPos * zoom_factor + 0.5, (int)robot->YPos * zoom_factor + 0.5));
		SetSensorRange(mSensorRange[MIN_VAL] * zoom_factor, mSensorRange[MAX_VAL] * zoom_factor);
	}
}

bool CRobot::SimpleCollisionDetection(ROBOT *robot)
{
	// We will approach a circle using decagon so we can find the intersection easily.
	// Decagon consists of 10 lines, we find any intersection occurs to these lines.
	// If we count the robot as circle, circle-to-circle intersection is fine, but to 
	//    find intersection between circle (the robot) to an ellipse-shaped obstacle
	//    will be troublesome. :p

	float dist = robot->Size / 2;
	CPoint robot_pos = CPoint(robot->XPos, robot->YPos);
	CPoint pt1 = CPoint(0,0);
	CPoint pt2 = CPoint(0,0);
	CPoint pt3 = CPoint(0,0);

	// Head as normal
	if(robot->LWheelSpeed >= 0 && robot->RWheelSpeed >= 0){
		float count = -HALF_PI;
		for(int i = 0 ; i < 5; i++){ // decagon -> 180 /5 = 45 degress
			pt1 = mGeom.Rotate(robot_pos, dist, robot->Theta + count);
			pt2 = mGeom.Rotate(robot_pos, dist, robot->Theta + count + M_PI / 5);
			pt3 = mCanvas->GetLineIntersectionWithObjects(pt1,pt2);
			if (pt3 != pt2)
				return true;
			count = count + M_PI / 5; 
		}
	}
	
	// Tail as normal
	else if (robot->LWheelSpeed <= 0 && robot->RWheelSpeed <= 0){
		float count = HALF_PI;
		for(int i = 0 ; i < 5; i++){
			pt1 = mGeom.Rotate(robot_pos, dist, robot->Theta + count);
			pt2 = mGeom.Rotate(robot_pos, dist, robot->Theta + count + M_PI / 5);
			pt3 = mCanvas->GetLineIntersectionWithObjects(pt1,pt2);
			if (pt3 != pt2)
				return true;
			count = count + M_PI / 5;
		}
	}


	return false;
}

float CRobot::GetRadiationCone()
{
	return 2 * mRadiationCone;
}

float CRobot::GetSensorRange(int index)
{
	return mSensorRange[index] / mZoom;
}

void CRobot::GetNoiseProperties(float &mean, float &dev)
{
	mean = mMeanNoise;
	dev = mMeanNoise;
}

float CRobot::GetSpeed(int index)
{
	if (index = RIGHT)
		return mRobot->RWheelSpeed / mZoom;
	else if (index = LEFT)
		return mRobot->LWheelSpeed / mZoom;
	else
		return 0;
}

COLORREF CRobot::GetRobotColor(UINT index)
{
	return mVectRobot.at(index).RobotColor;
}

float CRobot::GetSensorValue(int index)
{
	return mRobot->SensorValue[index] / mZoom;
}

CPoint CRobot::GetLocationMarkPosition(int index)
{
	if (mMark.empty())
		return CPoint(-1, -1);
	else 
		return mMark.at(index);
}
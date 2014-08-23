#pragma once

#define MAX_SENSOR_VAL	100
#define LEFT			0
#define RIGHT			1
#define MIN_VAL			0
#define MAX_VAL			1

#include <vector>
#include "canvas.h"
#include "geometry.h"

class CRobot
{
public:
	typedef struct
	{
		float Size;
		float XPos;
		float YPos;
		float Theta;
		float LWheelSpeed;
		float RWheelSpeed;
		float SensorValue[30];	// Max 30 sensors :p
		std::vector<CPoint> Trail;
		COLORREF RobotColor;
		bool HittingWall;
	} ROBOT;

	CRobot(void);
	~CRobot(void);

	/**
	 * Set canvas address where environment objects/obstacles are located
	 * @param canvas_addr Address of canvas
	 */
	void SetCanvasAddr(CCanvas *canvas_addr);

	/**
	 * Add new robot at location of initial_pos
	 * @param inital_pos Initial position where to add the robot
	 */
	void AddRobot(CPoint &initial_pos);
	
	/**
	 * Add new location mark
	 * @param pos Coordinate location where to put the mark
	 */
	void AddLocationMark(CPoint &pos);

	/**
	 * Remove the last robot, but we should have at least one robot
	 */
	void RemoveRobot();

	/**
	 * Remove the last robot, but we should have at least one robot
	 */;
	void RemoveLocationMark();

	/**
	 * Activate a robot
	 * @param index Index of robot to activate
	 */
	void SetActiveRobot(UINT index);


	/**
	 * Prepare necessary parameters before running: 
	 *  such as: variables for noise generator
	 */
	void PrepareToRun();

	/**
	 * Set location mark position
	 * @param pt Desired position for the location mark
	 */
	void SetPosition(CPoint &pt);

	void SetLocationMarPosition(int index, CPoint &pt);

	/**
	 * Reset all robots position position their original position
	 */
	void ResetPosition();

	/**
	 * Set robot angle (in radian)
	 * @param theta Desired rotation angle(Zero is facing to the east, moving clockwise)
	 */
	void SetAngle(float theta);

	/**
	 * Set robot size
	 * @param size Robot diameter
	 */
	void SetSize(float size){mRobot->Size = size;};

	/**
	 * Set left and rightwheel robot speed
	 * @param l_speed Robot left wheel speed
	 * @param r_speed Robot right wheel speed
	 */
	void SetSpeed(float l_speed, float r_speed);

	/**
	 * Set number of sensors
	 * @param num Number of sensors
	 */
	void SetSensorNum(int num){mSensorNum = num;};

	/**
	 * Set angular width of sensor radiation cone
	 * Default is 36 degrees (pi/5), maximum is ?? degrees
	 * @param cone_angle Angular width of the sonar cone
	 */
	void SetRadiationCone(float cone_angle = M_PI/5); 

	/**
	 * Set noise property for distance sensors
	 * @param mean Mean of Gaussian noise
	 * @param dev Standard deviation of Gaussian noise
	 */
	void SetNoiseProperties(float mean, float dev);

	/**
	 * Set min and max value of the distance that sensor can measure
	 * @param min_dist Minimum distance, default value is 0
	 * @param max_dist Maximum distance, default value is 100
	 */
	void SetSensorRange(float min_dist = 0, float max_dist = 100); 

	/**
	 * Toggle and disable trail drawing while robot is moving
	 */
	void SetDrawTrajectoryStatus(bool status);

	/**
	 * Set time step for simulation, default is 0.05s
	 * @param time_step Desired time step value
	 */
	void SetSimulationTimeStep(float time_step = 0.01);

	/**
	 * Set sampling rate for the sensor
	 * @param sampling_rate Sensor sapling rate
	 */
	void SetSensorSamplingRate(float sampling_rate = 0.1);

	/**
	 * Set color of active robot
	 * @param color Desired robot color
	 */
	void SetActiveRobotColor(COLORREF color){mRobot->RobotColor = color;};






	/**
	 * Get number of robot
	 * @return Number of robot
	 */
	int GetNumberOfRobot(){return mVectRobot.size();};

	/**
	 * Get robot position
	 * @return Center mass of the robot
	 */
	CPoint GetPosition(){return CPoint(mRobot->XPos, mRobot->YPos);};

	/**
	 * Get location mark position
	 * @param index Index of desired location marker
	 * @return Location mark position
	 */
	CPoint GetLocationMarkPosition(int index);

	
	/**
	 * Get sensor radiation cone
	 * @return Angular width of the sonar cone
	 */
	float GetRadiationCone(); 

	/**
	 * Get sensor range (min and max distance)
	 * @param index Select: min = 0 or max = 1
	 * @return Min or max distance sensor can measure
	 */
	float GetSensorRange(int index); 

	/**
	 * Get noise property for distance sensors
	 * @param mean Address of mean of Gaussian noise
	 * @param dev Address of standard deviation of Gaussian noise
	 */
	void GetNoiseProperties(float &mean, float &dev);

	/**
	 * Get robot angle
	 * @return Robot angle(Zero is facing to the east, moving clockwise)
	 */
	float GetAngle(){return fmod(mRobot->Theta, (float)TWO_PI);};

	/**
	 * Get robot size
	 * @return Robot diameter
	 */
	float GetSize(){return mRobot->Size / mZoom;};

	/**
	 * Get sensor value
	 * @param index Sensor index
	 * @return Sensor value
	 */
	float GetSensorValue(int index);

	/**
	 * Get number of sensors
	 * @return Number of sensors
	 */
	int GetSensorNum(){return mSensorNum;};

	/**
	 * Get wheel speed
	 * @param index Wheel index: left = 0 or right = 1
	 * @return Wheel speed
	 */
	float GetSpeed(int index);

	/**
	 * Get draw trail status
	 * @return Draw trail status
	 */
	bool GetDrawTrajectoryStatus(){return mCanDrawTrajectory;};

	/**
	 * Get simulation time step
	 * @return simulation time step
	 */
	float GetSimulationTimeStep(){return mSimulationTimeStep;};

	/**
	 * Get sonar sensor sampling rate
	 * @return Sensor sampling ratep (Hz)
	 */
	float GetSenorSamplingRate(){return mSensorSamplingRate;};
	
	/**
	 * Get index of active robot
	 * @return Index of active robot
	 */
	UINT GetIndexOfActiveRobot(){return mActiveRobot;};

	/**
	 * Get color if a certain robot
	 * @return color of the robot
	 */
	COLORREF GetRobotColor(UINT index);

	/**
	 * Draw robot in a shape of a circle
	 */
	void DrawRobot(CDC *pDC);

	/**
	 * Update robot position as a function of time
	 */
	void GoRobotGo();

	/**
	 * Executed when you click on the robot
	 * @param pt Location of mouse click
	 * @return Is it the robot location
	 */
	bool IsRobotDetected(CPoint &pt);

	/**
	 * Executed when you click on location marker
	 * @param pt Location of mouse click
	 * @return Is it the location marker position
	 */
	int IsLocationMarkDetected(CPoint &pt);
	
	/**
	 * Zoom in/out the robot
	 * @param zoom_factor Zooming factor
	 */
	void Zoom(float zoom_factor);

	/**
	 * Conversion from pixet to cm and vice versa, consider 1 cm = 4 pixels
	 */
	int CmToPixel(float cm){return cm * 4;};
	float PixelToCm(int pixel){return (float) pixel / 4;};

	/**
	 * Simple collision detection algorithm
	 * @return True if collision is detected
	 */
	bool SimpleCollisionDetection(ROBOT *robot);

	/**
	 * Draw sensor beams while measuring each value of it
	 * This will update sensorValue[] of m_robot
	 * @param accuracy Sensor accuracy
	 */
	bool UpdateSensorValue(float accuracy = 5.0, bool manual_update = false);



private:

	/**
	 * Generate Gaussian noise
	 * @param mean Noise mean
	 * @param deviation Noise standard deviation
	 * @return Generated noise
	 */
	float GaussianNoise(const float &mean, const float &deviation);

	/**
	 * Generate uniform noise
	 * @return Generated noise
	 */
	float UniformNoise();

	/**
	 * Update trajectory of a robot while moving
	 * @param robot Which robot to update
	 */
	void UpdateTrajectoryData(ROBOT *robot);

	/**
	 * Draw trajectory of a robot while moving
	 * @param robot Which robot trajectory to draw
	 */
	void DrawTrajectory(CDC *pDC, ROBOT *robot);

	/**
	 * Draw sesnsor beam
	 * @param robot Which robot trajectory to draw
	 */
	void DrawSensorBeam(ROBOT *robot, CDC *pDC);

	void DrawLocationMark(CDC *pDC);




	ROBOT				*mRobot;	// Pointer to active robot
	std::vector<ROBOT>	mVectRobot;	// Vector that holds information for all robots
	std::vector<CPoint> mMark;		// Vector that holds target

	int		mSensorNum;			// Number of sensors attached to the robot
	float	mRadiationCone;		// Angular width of the sonar cone
	float	mSensorRange[2];	// Min and max value of the distance that sensor can measure
	float	mMeanNoise;
	float	mDevNoise;
	float	mNoiseTime;

	bool	mCanDrawTrajectory;

	float	mSimulationTimeStep;
	UINT	mSimulationCount;
	float	mSensorSamplingRate;

	UINT	mActiveRobot;

	float	mZoom;

	CCanvas *mCanvas;
	SIMPLE_GEOM::CGeom mGeom;
};

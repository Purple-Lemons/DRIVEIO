/*
This class is used to determine the controls that are required
to navigate the track
*/
#ifndef CARCONTROL_H
#define CARCONTROL_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <chrono>

#include "GameInput.h"
#include "RegressionCurve.h"
#include "SituationDetection.h"
#include "../ComputerVision/SpeedDetection.h"
#include "../ComputerVision/SteeringDetection.h"
#include "../ImageProcessing/ImageProcessing.h"

using namespace std;
using namespace cv;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 580

class CarControl
{
public:
	CarControl();
	CarControl(int, int, int);
	~CarControl();

	void control(vector<Point>, vector<Point>, int, int, Mat*, Mat*);

	void centerSteering(HWND*, int, int);

	void resetCar(Mat);
	void leavePits();

	SpeedDetection getSpeedDet()
	{
		return speedDetection;
	}

	int getDirection()
	{
		return situationDetection.getDirection();
	}

	bool isInCorner()
	{
		return situationDetection.isInCorner();
	}

	RegressionCurve* getOneLineCurve()
	{
		return &steeringCurve;
	}

	RegressionCurve* getSteeringSpeedCurve()
	{
		return &steeringSpeedCurve;
	}

	int getTurnRate()
	{
		return turnRate;
	}

	GameInput* getGameInput()
	{
		return &gameInput;
	}

private:
	GameInput gameInput;

	int turnRate = 0;

	SpeedDetection speedDetection;
	SituationDetection situationDetection;

	vector<Point> arc1;
	vector<Point> arc2;

	RegressionCurve steeringSpeedCurve;//The speed to travel while cornering
	RegressionCurve allignmentCurve;//The ammount to turn to line up on a straight
	RegressionCurve steeringCurve;//For the turn rate when only one line has been found
	RegressionCurve oneLineStraightCurve;//For the turn rate when only one line has been found while on a straight

	void corner(int);
	void straight(int, bool);
	void approachCorner(int, bool);
	
	int getStraightPosition();
};

#endif
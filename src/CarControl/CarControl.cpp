#include "CarControl.h"

CarControl::CarControl(int windowX, int windowY, int mouseX)
{
	gameInput = GameInput(windowX, windowY, mouseX);
	situationDetection = SituationDetection();

	vector<double> oneLineParams = vector<double>();
	oneLineParams.push_back(50.0);
	oneLineParams.push_back(1.5);
	oneLineParams.push_back(1.5);
	steeringCurve = RegressionCurve(oneLineParams);

	vector<double> steerSpeedParams = vector<double>();
	steerSpeedParams.push_back(10000.0);
	steerSpeedParams.push_back(-19.0);
	steeringSpeedCurve = RegressionCurve(steerSpeedParams);

	vector<double> allignmentParams = vector<double>();
	allignmentParams.push_back(0.0);
	allignmentParams.push_back(2.0);
	allignmentCurve = RegressionCurve(allignmentParams);

	vector<double> oneLineStraightParams = vector<double>();
	oneLineStraightParams.push_back(-100.0);
	oneLineStraightParams.push_back(1.0);
	oneLineStraightCurve = RegressionCurve(oneLineStraightParams);
}

CarControl::CarControl()
{
}

CarControl::~CarControl()
{
}

void CarControl::control(vector<Point> _arc1, vector<Point> _arc2, int cornerApproaching, int cornerDepth, Mat* image, Mat* hudImage)
{
	gameInput.releaseKeys();

	arc1.swap(vector<Point>());
	arc2.swap(vector<Point>());

	arc1 = _arc1;
	arc2 = _arc2;

	int speed = speedDetection.getSpeed(hudImage);
	
	situationDetection.detectSituation(arc1, arc2, cornerApproaching, cornerDepth, speed);

	if (situationDetection.getNoLines())
	{
		gameInput.steer(0);
	}
	else if (situationDetection.isOnStraight())
	{
		putText(*image, "Straight", Point(50, 50), 16, 2, Scalar(0, 0, 255), 2);
		straight(speed, situationDetection.getOneLine());
	}
	else if (situationDetection.isApproachingCorner())
	{
		putText(*image, "Approaching Corner", Point(50, 50), 16, 2, Scalar(0, 0, 255), 2);
		approachCorner(speed, situationDetection.getOneLine());
	}
	else if(situationDetection.isInCorner())
	{
		putText(*image, "Corner", Point(50, 50), 16, 2, Scalar(0, 0, 255), 2);
		corner(speed);
	}
}

void CarControl::corner(int speed)
{
	vector<double> vals;
	double idealSpeed = 0;

	vals.push_back(1.0);

	//If arc1 is the closest or only arc1 has been found
	if (arc2.size() == 0 || (arc1.size() > arc2.size() && arc1[0].y > 0.75 * arc2[0].y && situationDetection.getDirection() == 1))
	{
		//Turns based on the distance from the edge of the track
		vals.push_back(arc1[0].y);
		vals.push_back(arc1[0].y);

		turnRate = steeringCurve.run(vals);
	}
	else if (arc1.size() == 0 || (arc2.size() > arc1.size() && arc2[0].y > 0.75 * arc1[0].y && situationDetection.getDirection() == 2))
	{
		vals.push_back(arc2[0].y);
		vals.push_back(arc2[0].y);

		turnRate = -steeringCurve.run(vals);
	}
	else
	{
		turnRate = 0;
	}

	//Finds the turn rate and ideal speed
	if (vals.size() > 1)
	{
		vals.clear();

		vals.push_back(1.0);
		vals.push_back(abs(turnRate));

		idealSpeed = steeringSpeedCurve.run(vals);

		idealSpeed = sqrt(idealSpeed);
	}

	if (speed < idealSpeed)
	{
		gameInput.throttle(1.0);
	}

	gameInput.steer(turnRate);
}

void CarControl::straight(int speed, bool oneLine)
{
	double turnRate;

	//If both lines have been found
	if (!oneLine)
	{
		//Finds where the vehicle is relative to each track line
		int position = getStraightPosition();

		vector<double> vals = vector<double>();
		vals.push_back(1.0);
		vals.push_back(position);
		turnRate = allignmentCurve.run(vals);

		gameInput.limitedSteer(turnRate);
	}
	else if (arc1.size() > 0 && arc2.size() == 0)
	{
		//Turns based on it's distance from the track lines that has been found
		vector<double> vals;
		vals.push_back(1.0);
		vals.push_back(arc1[0].y);
		turnRate = oneLineStraightCurve.run(vals);

		gameInput.limitedSteer(turnRate);
	}
	else if (arc2.size() > 0 && arc1.size() == 0)
	{
		vector<double> vals;
		vals.push_back(1.0);
		vals.push_back(arc2[0].y);

		turnRate = oneLineStraightCurve.run(vals);

		if(turnRate != 0)
			turnRate = -turnRate;

		gameInput.limitedSteer(turnRate);
	}

	if (speed < 75)
	{
		gameInput.throttle(1.0);
	}
	else if (speed > 75)
	{
		gameInput.throttle(0);
	}
}

void CarControl::approachCorner(int speed, bool oneLine)
{
	double turnRate = 0;

	vector<double> vals;
	if (!oneLine)
	{
		int position = getStraightPosition();

		vector<double> vals = vector<double>();
		vals.push_back(1.0);
		vals.push_back(position);
		turnRate = allignmentCurve.run(vals);

		gameInput.limitedSteer(turnRate);
	}
	else if (arc1.size() > 0 && arc2.size() == 0)
	{
		vals.push_back(1.0);
		vals.push_back(arc1[0].y);
		turnRate = oneLineStraightCurve.run(vals);

		gameInput.limitedSteer(turnRate);
	}
	else
	{
		vals.push_back(1.0);
		vals.push_back(arc2[0].y);

		turnRate = oneLineStraightCurve.run(vals);

		if (turnRate != 0)
			turnRate = -turnRate;

		gameInput.limitedSteer(turnRate);
	}

	if (speed > 60)
	{
		gameInput.brake(1.0);
	}
	else if (speed < 50)
	{
		gameInput.throttle(1.0);
	}
	else
	{
		gameInput.throttle(0.0);
	}
}

//Finds where the car is on a straight - effectively left, right or center
int CarControl::getStraightPosition()
{
	int y1 = arc1[0].y;
	int y2 = arc2[0].y;

	int pos = y1 - y2;

	return pos / 2;
}

//Called once at the start of the program to center the steering
void CarControl::centerSteering(HWND* hwnd, int centerX, int centerY)
{
	gameInput.centerSteering(hwnd, centerX, centerY);
}

//Returns the car to the pit lane
void CarControl::resetCar(Mat hudImage)
{
	situationDetection.resetDirection();
	gameInput.resetCar(hudImage);
}

//Leaves the pit lane
void CarControl::leavePits()
{
	gameInput.leavePits();
}

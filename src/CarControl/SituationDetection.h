/*
This class is used to figure out the situation the track, 
whether the car is on a straight, approaching a corner or in a corner
*/
#ifndef SITUATIONDETECTION_H
#define SITUATIONDETECTION_H

#include <vector>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class SituationDetection
{
public:
	SituationDetection();
	~SituationDetection();

	void detectSituation(vector<Point>, vector<Point>, int, int, int);

	bool isOnStraight()
	{
		return onStraight;
	}

	bool isApproachingCorner()
	{
		return approachingCorner;
	}

	bool isInCorner()
	{
		return inCorner;
	}

	bool getNoLines()
	{
		return noLines;
	}

	bool getOneLine()
	{
		return oneLine;
	}

	int getDirection()
	{
		return direction;
	}

	void resetDirection()
	{
		direction = 0;
	}

private:

	int changeDetected = 0;//Counts the number of times a change has been detected

	//The state of the track
	bool onStraight = true;
	bool approachingCorner = false;
	bool inCorner = false;
	bool oneLine = false;
	bool noLines = false;

	int direction = 0;

	vector<Point> arc1;
	vector<Point> arc2;

	double getCornerDegree();
	double getAngle();
	double angleBetweenPoints(Point, Point);
};

#endif
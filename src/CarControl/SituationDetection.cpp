#include "SituationDetection.h"

SituationDetection::SituationDetection()
{
}

SituationDetection::~SituationDetection()
{ 
}

void SituationDetection::detectSituation(vector<Point> _arc1, vector<Point> _arc2, int cornerApproaching, int cornerDepth, int speed)
{
	//Updates the track lines
	arc1 = _arc1;
	arc2 = _arc2;

	noLines = false;
	oneLine = false;

	//If both lines have been found
	if (arc1.size() > 0 && arc2.size() > 0)
	{
		//Determines if a corner has been detected by these three parameters
		int cornerDetectionsTrue = 0;

		if (abs(getCornerDegree()) > 0.02 || cornerDepth > 250)
		{
			cornerDetectionsTrue++;
		}
		if (abs(getAngle()) > 0.15)
		{
			cornerDetectionsTrue++;
		}

		//If the angle is greater than 0.15 and one of the other parameters is true
		//the a corner has been detected
		bool cornerDetected = cornerDetectionsTrue > 1;

		//Effectively an FSM that transitions between track states
		if (onStraight)
		{
			//If a change is detected three times continuously
			if (cornerApproaching != 0)
			{
				changeDetected++;

				if (changeDetected >= 3)
				{
					approachingCorner = true;
					onStraight = false;

					changeDetected = 0;
				}
			}
			else
			{
				changeDetected = 0;
			}
		}
		else if (approachingCorner)
		{
			if (cornerDetected)
			{
				changeDetected++;

				if (changeDetected >= 3)
				{
					inCorner = true;
					approachingCorner = false;
					direction = cornerApproaching;
				}
			}
			else
			{
				changeDetected = 0;
			}
		}
		else
		{
			if (!cornerDetected)
			{
				changeDetected++;

				if (changeDetected >= 3)
				{
					onStraight = true;
					inCorner = false;
					direction = 0;
				}
			}
			else
			{
				changeDetected = 0;
			}
		}
	}
	else if (arc1.size() > 0 && arc2.size() == 0 && inCorner)//If only one track lines has been found
	{
		inCorner = true;
		onStraight = false;
		approachingCorner = false;

		oneLine = true;

		direction = 1;
	}
	else if (arc2.size() > 0 && arc1.size() == 0 && inCorner)
	{
		inCorner = true;
		onStraight = false;
		approachingCorner = false;

		oneLine = true;

		direction = 2;
	}
	else if (arc1.size() > 0 && arc2.size() == 0 && onStraight)
	{
		onStraight = true;
		inCorner = false;
		approachingCorner = false;

		oneLine = true;
	}
	else if (arc2.size() > 0 && arc1.size() == 0 && onStraight)
	{
		onStraight = true;
		inCorner = false;
		approachingCorner = false;

		oneLine = true;
	}
	else if (arc1.size() > 0 && arc2.size() == 0 && approachingCorner)
	{
		//If only one track lines has been found while approaching a corner
		//it's likely due to entering a corner, but could be a mis identified corner
		//no longer being indentified - this figures out which

		double angle1 = angleBetweenPoints(arc1[0], arc1[arc1.size() / 2]);
		double angle2 = angleBetweenPoints(arc1[arc1.size() / 2], arc1[arc1.size() - 1]);

		double dif = angle1 - angle2;

		if (abs(dif) < 0.01)
		{
			onStraight = true;
			inCorner = false;
		}
		else
		{
			onStraight = false;
			inCorner = true;
		}

		approachingCorner = false;

		oneLine = true;
	}
	else if (arc2.size() > 0 && arc1.size() == 0 && approachingCorner)
	{
		double angle1 = angleBetweenPoints(arc2[0], arc2[arc2.size() / 2]);
		double angle2 = angleBetweenPoints(arc2[arc2.size() / 2], arc2[arc2.size() - 1]);

		double dif = angle1 - angle2;

		if (abs(dif) < 0.01)
		{
			onStraight = true;
			inCorner = false;
		}
		else
		{
			onStraight = false;
			inCorner = true;
		}

		approachingCorner = false;

		oneLine = true;
	}
	else
	{
		noLines = true;
		oneLine = true;
	}
}

//Finds the curvature of the track lines
double SituationDetection::getCornerDegree()
{
	double angle1 = angleBetweenPoints(arc1[0], arc1[arc1.size() / 2]);
	double angle2 = angleBetweenPoints(arc1[arc1.size() / 2], arc1[arc1.size() - 1]);

	double dif1 = angle1 - angle2;

	angle1 = angleBetweenPoints(arc2[0], arc2[arc2.size() / 2]);
	angle2 = angleBetweenPoints(arc2[arc2.size() / 2], arc2[arc2.size() - 1]);

	double dif2 = angle1 - angle2;

	return dif1 + dif2;
}

double SituationDetection::angleBetweenPoints(Point p1, Point p2)
{
	double xDist = p1.x - p2.x;
	double yDist = p1.y - p2.y;

	double r = sqrt(xDist * xDist + yDist * yDist);

	return asin(yDist / r);
}

//Finds the angles of the track lines relative to the horizontal
double SituationDetection::getAngle()
{
	double xDist = arc1[arc1.size() - 1].x - arc1[0].x;
	double yDist = arc1[arc1.size() - 1].y - arc1[0].y;
	double r = sqrt(xDist * xDist + yDist * yDist);

	double angle1 = asin(xDist / r);

	xDist = arc2[arc2.size() - 1].x - arc2[0].x;
	yDist = arc2[arc2.size() - 1].y - arc2[0].y;
	r = sqrt(xDist * xDist + yDist * yDist);

	double angle2 = asin(xDist / r);

	double avgAngle = (angle1 + angle2);

	return avgAngle;
}

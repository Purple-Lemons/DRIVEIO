/*
This class is used to find where the steering is by looking at
the section of the window which contains the steering indicator
*/
#ifndef STEERINGDETECTION_H
#define  STEERINGDETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

class SteeringDetection
{
public:
	SteeringDetection();
	~SteeringDetection();

	int getSteering(Mat*);

private:
};

#endif
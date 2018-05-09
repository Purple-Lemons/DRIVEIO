/*
This class is used to find the speed of the vehicle by
looking at the section of gui that displays the speed
and applying simple text recognition to it
*/
#ifndef SPEEDDETECTION_H
#define SPEEDDETECTION_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using namespace std;
using namespace cv;

class SpeedDetection
{
public:
	SpeedDetection();
	~SpeedDetection();

	int getSpeed(Mat*);//Currently just gets the gear

private:
	vector<Mat> numbers = vector<Mat>();
	vector<Point> numPos = vector<Point>();

	int numDigits = 1;

	vector<Point> findDigits(Mat*);
	vector<Point> findDigit(Mat*, Point);
};

#endif
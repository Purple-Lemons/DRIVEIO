/*
This class uses the colour in the image to detect upcoming
corners and when the vehicle exits the track
*/
#ifndef COLOURDETECTION_H
#define COLOURDETECTION_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

using namespace std;
using namespace cv;

class ColourDetection
{
public:
	ColourDetection();
	~ColourDetection();

	int isApproachingCorner(vector<Point>*, vector<Point>*);
	int cornerDepth();

	double leavingTrack();

	void setImage(Mat* _image)
	{
		_image->copyTo(image);
	}

	Mat* getTwoToneImage()
	{
		return &twoTone;
	}

	bool getLeftTrackLeft()
	{
		return leftTrackLeft;
	}

	bool getLeftTrackRight()
	{
		return leftTrackRight;
	}

private:
	Mat image;
	Mat twoTone;//two tone image of the original image

	//Parameters of the straight line abtraction of the track lines
	double arc1Grad;
	double arc1C;

	double arc2Grad;
	double arc2C;

	void toTwoTone();
	void toTwoToneNoLines();
	bool eightConnected(Point, Point);

	int longestLine = 0;
	Point longestStart;
	Point longestEnd;

	bool leftTrackLeft = false;
	bool leftTrackRight = false;
};

#endif

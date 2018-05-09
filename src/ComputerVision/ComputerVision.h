/*
This calls handles all of the computer vision operations
*/
#ifndef COMPUTERVISION_H
#define COMPUTERVISION_H

#include <vector>

#include "TrackEdgeDetection.h"
#include "ColourDetection.h"

using namespace std;

class ComputerVision
{
public:
	ComputerVision();
	~ComputerVision();

	void detectTrack(vector<Point>*, vector<Point>*, int*, int*, double*, Mat*, Mat*);

	void leavingTrack(bool*, bool*);

	Mat* getTwoToneImage()
	{
		return colourDetection.getTwoToneImage();
	}

private:
	TrackEdgeDetection trackEdgeDetection;
	ColourDetection colourDetection;
};

#endif
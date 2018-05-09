#include "ComputerVision.h"

ComputerVision::ComputerVision()
{
	trackEdgeDetection = TrackEdgeDetection(580, 1920);
	colourDetection = ColourDetection();
}

ComputerVision::~ComputerVision()
{

}

void ComputerVision::detectTrack(vector<Point>* arc1, vector<Point>* arc2, int* approachingCorner, int* corner, double* leavingTrack, Mat* sourceImage, Mat* processedImage)
{
	colourDetection.setImage(sourceImage);

	//Finds the track lines
	trackEdgeDetection.getEdges(processedImage, sourceImage, arc1, arc2);

	//Determines if a corner is approaching
	*approachingCorner = colourDetection.isApproachingCorner(arc1, arc2);//0 - no corner, 1 - right, 2 - left

	*corner = 0;

	//If a corner is approaching, an esitmate of distance is found based on the depth
	if (approachingCorner != 0)
	{
		*corner = colourDetection.cornerDepth();
	}

	*leavingTrack = colourDetection.leavingTrack();
}

void ComputerVision::leavingTrack(bool* left, bool* right)
{
	*left = colourDetection.getLeftTrackLeft();
	*right = colourDetection.getLeftTrackRight();
}
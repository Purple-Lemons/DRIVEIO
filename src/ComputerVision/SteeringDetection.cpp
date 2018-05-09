#include "SteeringDetection.h"

SteeringDetection::SteeringDetection()
{

}

SteeringDetection::~SteeringDetection()
{

}

int SteeringDetection::getSteering(Mat* image)
{
	int x = 0;

	//Finds the x value of the first red pixel (where the indicator sits of the lines)
	for (int i = 0; i < image->cols; i++)
	{
		for (int j = 0; j < image->rows; j++)
		{
			Vec4b pix = image->at<Vec4b>(j, i);

			float b = pix.val[0];
			float g = pix.val[1];
			float r = pix.val[2];

			//If the pixel is red
			if (b == 0 && g == 0 && r == 255)
			{
				x = i;
				break;
			}
		}
	}

	return x - 331;//Minus half the width of the image, puts the center equal to 0
}
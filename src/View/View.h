#ifndef  VIEW_H
#define VIEW_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "../CarControl/RegressionCurve.h"

using namespace std;
using namespace cv;

#define GRAPH_WIDTH 440

class View
{
public:
	View();
	~View();

	void displayTrackLineImage(Mat*);
	void displayColourImage(Mat*);
	void displayGraph(RegressionCurve*, bool, int);

private:
	Mat GraphImage;

};

#endif // ! VIEW_H

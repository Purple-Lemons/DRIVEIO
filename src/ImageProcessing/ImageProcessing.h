/*
This class contains a set of helper functions to process
images before they are passed to computer vision
*/
#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Ole2.h>
#include <OleCtl.h>

using namespace std;
using namespace cv;

class ImageProcessing
{
public:
	ImageProcessing();
	~ImageProcessing();

	void hwnd2mat(HWND*, Mat*);
	void prepareImage(Mat*, Mat*, Rect);
	void getImageArea(Mat*, Mat*, Rect);

private:

};

#endif
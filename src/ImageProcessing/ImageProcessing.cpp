#include "ImageProcessing.h"

ImageProcessing::ImageProcessing()
{
}

ImageProcessing::~ImageProcessing()
{
}

//Converts the hwnd image to a Mat image so that it can be used by opencv2 functions
//This function is taken from https://github.com/sturkmen72/opencv_samples/blob/master/Screen-Capturing.cpp
void ImageProcessing::hwnd2mat(HWND* hwnd, Mat* src)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;

	hwindowDC = GetDC(*hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	height = 1080;  //change this to whatever size you want to resize to
	width = 1920;

	src->create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, width, height, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src->data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(*hwnd, hwindowDC);
}

//Blurs, crops and gray scales the main image
void ImageProcessing::prepareImage(Mat* source, Mat* target, Rect area)
{
	*source = (*source)(area);

	GaussianBlur(*source, *source, Size(3, 3), 5, 5);//Creates a blurred version of the source image
	
	cvtColor(*source, *target, CV_BGR2GRAY);//gray scales image

}

void ImageProcessing::getImageArea(Mat* source, Mat* target, Rect area)
{
	source->copyTo(*target);
	*target = (*target)(area);
}
#define _WIN32_WINNT_WIN10 0x0A00

#include <windows.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <vector>

#include "ComputerVision/ComputerVision.h"
#include "CarControl/CarControl.h"
#include "ImageProcessing/ImageProcessing.h"
#include "Trainer/Trainer.h"
#include "View/View.h"

using namespace std;
using namespace cv;

void leavePits(HWND, int, int, Mat, CarControl*);
void getWindowPos(int*, int*, HWND);
int getMouseX();

Mat sourceImage;
Mat processedImage;
Mat speedImage;
Mat steeringImage;

Rect mainArea(0, 500, 1920, 1080 - 800);
Rect speedArea(930, 165, 80, 50);
Rect steeringArea(332, 250, 1200, 20);

ComputerVision computerVision;
CarControl control;
ImageProcessing imgProc;
Trainer trainer;
View view;

bool leftTrackLeft = false;
bool leftTrackRight = false;

vector<Point> arc1 = vector<Point>();
vector<Point> arc2 = vector<Point>();

int main()
{
	Sleep(1000);//Allow user to focus on game window

	//Sets up the game screen capture
	HWND hwnd = FindWindow(0, L"Assetto Corsa");

	if (hwnd == NULL)
	{
		cout << "Window Not Found" << endl;
		return 0;
	}

	//Finds the position of the game window
	int windowX = 0;
	int windowY = 0;
	getWindowPos(&windowX, &windowY, hwnd);

	int mouseCenterX = getMouseX();

	computerVision = ComputerVision();
	control = CarControl(windowX, windowY, mouseCenterX);
	imgProc = ImageProcessing();
	trainer = Trainer(&control);
	view = View();

	trainer.loadFromFile();

	//Start by resseting the vehicle and leaving the pit lane
	imgProc.hwnd2mat(&hwnd, &sourceImage);
	leavePits(hwnd, mouseCenterX, windowY + 300, sourceImage, &control);

	int key = 0;

	bool isTraining = false;
	bool showTwoToneImage = false;
	bool showMainImage = true;
	bool showGraph = false;

	while (key != 27)
	{
		//Release the memory from the previous arcs
		arc1.swap(vector<Point>());
		arc2.swap(vector<Point>());

		//Converts the hwnd image of the screen to a Mat image
		imgProc.hwnd2mat(&hwnd, &sourceImage);

		imgProc.getImageArea(&sourceImage, &steeringImage, steeringArea);
		imgProc.getImageArea(&sourceImage, &speedImage, speedArea);
		imgProc.prepareImage(&sourceImage, &processedImage, mainArea);

		control.getGameInput()->setImage(&steeringImage);
		
		int approachingCorner;//0 - no corner, 1 - right, 2 - left
		int corner;
		double leavingTrack;

		computerVision.detectTrack(&arc1, &arc2, &approachingCorner, &corner, &leavingTrack, &sourceImage, &processedImage);

		control.control(arc1, arc2, approachingCorner, corner, &sourceImage, &speedImage);
		
		//If it has been detected that the car has left the track, or if a user pressed the 'r' keys
		if (leavingTrack > 0.3 || GetKeyState(0x52) == -127)
		{
			computerVision.leavingTrack(&leftTrackLeft, &leftTrackRight);

			if(isTraining)
				trainer.train(leftTrackLeft, leftTrackRight, control.getDirection());

			leavePits(hwnd, mouseCenterX, windowY + 300, sourceImage, &control);
		}

		if(isTraining)
			trainer.increaseSpeed();

		//Display Images
		if(showMainImage)
			view.displayTrackLineImage(&sourceImage);

		if (showTwoToneImage)
			view.displayColourImage(computerVision.getTwoToneImage());

		if(showGraph)
			view.displayGraph(control.getOneLineCurve(), false, 0);

		//Release the memory used to store the images
		sourceImage.release();
		processedImage.release();
		speedImage.release();

		key = waitKey(1);

		if (GetKeyState(0x51) & 0x8000)//Exits the program even when the button is pressed in another window
		{
			break;
		}
	}
	
	return 0;
}

void leavePits(HWND hwnd, int mouseCenterX, int mouseCenterY, Mat image, CarControl *control)
{
	Mat steeringImage;

	image.copyTo(steeringImage);

	Rect steeringArea(332, 250, 1200, 20);

	steeringImage = image(steeringArea);

	control->getGameInput()->setImage(&steeringImage);

	control->centerSteering(&hwnd, mouseCenterX, mouseCenterY);

	Sleep(1000);

	control->resetCar(steeringImage);//Returns the vehicle to the pit lane

	Sleep(1000);

	control->centerSteering(&hwnd, mouseCenterX, mouseCenterY);

	Sleep(1000);

	control->leavePits();//Leaves the pit lane
}

void getWindowPos(int *x, int *y, HWND hwnd)
{
	RECT rect = { NULL };

	if (GetWindowRect(hwnd, &rect))
	{
		*x = rect.left;
		*y = rect.top;
	}
}

int getMouseX()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	return cursorPos.x + 1;
}
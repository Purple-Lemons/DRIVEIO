/*
This class is responsible for actually inputting
controls into the game
*/
#ifndef GAMEINPUT_H
#define GAMEINPUT_H

#include <windows.h>

#include "../ImageProcessing/ImageProcessing.h"
#include "../ComputerVision/SteeringDetection.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 580

using namespace std;

class GameInput
{
public:
	GameInput(int, int, int);
	GameInput();
	~GameInput();

	void throttle(double);
	void brake(double);
	void steer(double);
	void limitedSteer(double);
	void steerNoImage(double);

	void centerSteering(HWND*, int, int);

	void resetCar(Mat);
	void leavePits();

	void releaseKeys();

	void setImage(Mat* _image)
	{
		image = _image;
	}

private:
	INPUT key_ip;//Keyboard input structure
	INPUT mouse_ip;//Mouse input structure

	Mat* image;

	//Absolute position of the center of the screen
	int centerX = 0;
	int centerY = 0;

	//Center position of the mouse
	int mouseCenterX = 0;

	//Timers that allow the throttle and brake to be applied at a percentage
	int throttleTimer = 0;
	int throttleTimerLength = 10;

	int brakeTimer = 0;
	int brakeTimerLength = 10;

	int currentSteering = 0;

	int maxSteerIncrement = 150;

	SteeringDetection steerDet = SteeringDetection();
};

#endif
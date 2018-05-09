#include "GameInput.h"

GameInput::GameInput(int windowX, int windowY, int mouseX)
{
	centerX = windowX + SCREEN_WIDTH / 2;
	centerY = windowY + 1080 / 2;
	mouseCenterX = mouseX;

	// Set up a generic keyboard event.
	key_ip.type = INPUT_KEYBOARD;
	key_ip.ki.wScan = 0; // hardware scan code for key
	key_ip.ki.time = 0;
	key_ip.ki.dwExtraInfo = 0;

	//set up generic mouse event
	mouse_ip.type = INPUT_MOUSE;
	mouse_ip.mi.mouseData = 0;
	mouse_ip.mi.time = 0;
}

GameInput::GameInput()
{
}

GameInput::~GameInput()
{
}

void GameInput::releaseKeys()
{
	key_ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &key_ip, sizeof(INPUT));
}

void GameInput::throttle(double percentage)
{
	if (throttleTimer <= throttleTimerLength * percentage)
	{
		//keyboard input - accelerate
		key_ip.ki.wVk = 0x57;
		key_ip.ki.dwFlags = 0;
		SendInput(1, &key_ip, sizeof(INPUT));
	}

	throttleTimer++;
	if (throttleTimer >= throttleTimerLength)
	{
		throttleTimer = 0;
	}
}

void GameInput::brake(double percentage)
{
	if (brakeTimer <= brakeTimerLength * percentage)
	{
		//keyboard input - brake
		key_ip.ki.wVk = 0x53;
		key_ip.ki.dwFlags = 0;
		SendInput(1, &key_ip, sizeof(INPUT));
	}

	brakeTimer++;
	if (brakeTimer >= brakeTimerLength)
	{
		brakeTimer = 0;
	}
}

void GameInput::steer(double rate)
{
	//Finds the current steering by checking the on screen indicator
	int mouseX = steerDet.getSteering(image) - 280;
	currentSteering = (int)(mouseX / 0.6);

	//Stops the vehicle from steering to the absolute maximum, to stop the indicator becoming unreadable
	if (rate > 950)
	{
		mouse_ip.mi.dx = (mouseCenterX + 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
	else if (rate < -950)
	{
		mouse_ip.mi.dx = (mouseCenterX - 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
	else
	{
		//Sets upt the mouse event
		mouse_ip.mi.dx = (mouseCenterX + rate - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		//Executes the mouse event
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
}

//For steering when no image is present, keeps track of current steering internally, just used for leaving the pit lane
void GameInput::steerNoImage(double rate)
{
	if (rate > 950)
	{
		mouse_ip.mi.dx = (mouseCenterX + 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));

		currentSteering = 950;
	}
	else if (rate < -950)
	{
		mouse_ip.mi.dx = (mouseCenterX - 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));

		currentSteering = -950;
	}
	else
	{
		mouse_ip.mi.dx = (mouseCenterX + rate - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));

		currentSteering = rate;
	}
}

//Steers at limited rate, to stop eratic steering in certain situations
void GameInput::limitedSteer(double rate)
{
	//Finds the current steering by checking the on screen indicator
	int mouseX = steerDet.getSteering(image) - 280;
	currentSteering = (int)(mouseX / 0.6);

	//Smooths the turning rate, doesn't allow massive changes all at once
	if (rate != 0 && abs(rate - currentSteering) > maxSteerIncrement)
	{
		rate = currentSteering + (maxSteerIncrement * (rate / abs(rate)));
	}

	//Stops the vehicle from steering to the absolute maximum, to stop the indicator becoming unreadable
	if (rate > 950)
	{
		mouse_ip.mi.dx = (mouseCenterX + 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
	else if (rate < -950)
	{
		mouse_ip.mi.dx = (mouseCenterX - 950 - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
	else
	{
		mouse_ip.mi.dx = (mouseCenterX + rate - currentSteering) * (65535.0 / (2560 * 2 + 1366));
		mouse_ip.mi.dy = centerY * (65535.0 / 1440);
		mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
		SendInput(1, &mouse_ip, sizeof(mouse_ip));
	}
}

//Called once at the start of the program to center the steering
void GameInput::centerSteering(HWND* hwnd, int centerX, int centerY)
{
	ImageProcessing imgProc = ImageProcessing();
	Mat image;

	imgProc.hwnd2mat(hwnd, &image);

	int mouseX = (612 - steerDet.getSteering(&image)) / 0.6;

	mouse_ip.mi.dx = (centerX + mouseX) * (65535.0 / (2560 * 2 + 1366));
	mouse_ip.mi.dy = centerY * (65535.0 / 1440);
	mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &mouse_ip, sizeof(mouse_ip));
}

//Returns the vehicle to the pit lane
void GameInput::resetCar(Mat hudImage)
{
	//Releases all keys
	key_ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &key_ip, sizeof(INPUT));

	//Opens the in game menu menu
	key_ip.ki.wVk = 0x1B;//"Esc" Key
	key_ip.ki.dwFlags = 0;
	SendInput(1, &key_ip, sizeof(INPUT));

	Sleep(100);

	//Clicks "Return to pits" on the in game menu
	mouse_ip.mi.dx = (centerX) * (65535.0 / (2560 * 2 + 1366));
	mouse_ip.mi.dy = (centerY) * (65535.0 / 1440);
	mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &mouse_ip, sizeof(mouse_ip));

	mouse_ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &mouse_ip, sizeof(mouse_ip));

	Sleep(1000);

	//Clicks the start driving button
	mouse_ip.mi.dx = (centerX - (SCREEN_WIDTH / 2) * 0.95) * (65535.0 / (2560 * 2 + 1366));
	mouse_ip.mi.dy = (centerY - 300) * (65535.0 / 1440);
	mouse_ip.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK | MOUSEEVENTF_ABSOLUTE;
	SendInput(1, &mouse_ip, sizeof(mouse_ip));

	mouse_ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &mouse_ip, sizeof(mouse_ip));
}

//A script of controls for leaving the pits lanes at Silverstone
void GameInput::leavePits()
{
	currentSteering = 0;

	//Throttle for 3 seconds
	throttle(1.0);

	Sleep(3000);

	releaseKeys();

	//Steer to the right for 4 seconds
	steerNoImage(100);

	Sleep(4000);

	//Steer center and throttle for 3 seconds
	steerNoImage(0);

	throttle(1.0);

	Sleep(3000);

	//Coast for 6 seconds
	releaseKeys();

	Sleep(6000);

	//Steer left for 1 second
	steerNoImage(-50);

	Sleep(1000);

	steerNoImage(0);
}
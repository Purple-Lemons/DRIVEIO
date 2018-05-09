/*
This class is used to train the regression curves used to determine the cars controls
*/
#ifndef TRAINER_H
#define TRAINER_H

#include <fstream>
#include <iostream>
#include <string>

#include "../CarControl/CarControl.h"

class Trainer
{
public:
	Trainer(CarControl*);
	Trainer();
	~Trainer();

	void train(bool, bool, int);
	void trainSteering(bool);
	void trainSpeed(bool);

	void increaseSpeed();

	void saveToFile();

	void loadFromFile();

private:
	CarControl* control;

	bool hasCornered = false;
	int numFrames = 0;
};

#endif // !TRAINER_H

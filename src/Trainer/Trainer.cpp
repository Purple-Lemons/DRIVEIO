#include "Trainer.h"

Trainer::Trainer(CarControl* _control)
{
	control = _control;
}

Trainer::Trainer()
{
}

Trainer::~Trainer()
{
}

void Trainer::train(bool leftTrackLeft, bool leftTrackRight, int cornerDirection)
{
	//Informs the user what has happened to vehicle
	if (leftTrackLeft)
	{
		cout << "Left Track Left, ";
	}
	else if (leftTrackRight)
	{
		cout << "Left Track Right, ";
	}

	if (cornerDirection == 0)
	{
		cout << "On a Straight Road." << endl;
	}
	else if (cornerDirection == 1)
	{
		cout << "On a Right Hand Bend." << endl;
	}
	else
	{
		cout << "On a Left Hand Bend." << endl;
	}

	if (leftTrackRight && cornerDirection == 1)
	{
		cout << "Over Steer" << endl;
		trainSteering(true);
	}
	else if (leftTrackRight && cornerDirection == 2)
	{
		cout << "Under Steer" << endl;
		trainSteering(false);
	}
	else if (leftTrackLeft && cornerDirection == 2)
	{
		cout << "Over Steer" << endl;
		trainSteering(true);
	}
	else if (leftTrackLeft && cornerDirection == 1)
	{
		cout << "Under Steer" << endl;
		trainSteering(false);
	}

	saveToFile();
}

void Trainer::trainSteering(bool overSteer)
{
	if (overSteer)
	{
		vector<double> paramChanges;
		paramChanges.push_back(0);
		paramChanges.push_back(-0.01);
		paramChanges.push_back(0);

		control->getOneLineCurve()->modifyParams(paramChanges);
	}
	else
	{
		vector<double> paramChanges;
		paramChanges.push_back(0);
		paramChanges.push_back(0.01);
		paramChanges.push_back(0);

		control->getOneLineCurve()->modifyParams(paramChanges);
	}
}

void Trainer::trainSpeed(bool overSteer)
{
	if (overSteer)
	{
		vector<double> paramChanges;
		paramChanges.push_back(0);
		paramChanges.push_back(0.1);

		control->getSteeringSpeedCurve()->modifyParams(paramChanges);
	}
	else
	{
		vector<double> paramChanges;
		paramChanges.push_back(0);
		paramChanges.push_back(-0.1);

		control->getSteeringSpeedCurve()->modifyParams(paramChanges);
	}

	saveToFile();
}

//Increases the speed at which the vehicle takes corners every 500 frames
//as long as the vehicle has passed through a corner
void Trainer::increaseSpeed()
{
	numFrames++;

	if (control->isInCorner())
	{
		hasCornered = true;
	}

	if (numFrames > 500 && hasCornered)
	{
		numFrames = 0;
		hasCornered = false;

		trainSpeed(true);

		cout << "Speed Increased" << endl;
	}
}

//Saves the current parameters to a text files
void Trainer::saveToFile()
{
	ofstream file;;
	file.open("TrainingData/params.txt");

	string data = "";

	vector<double> params = control->getSteeringSpeedCurve()->getParams();

	for (int i = 0; i < params.size(); i++)
	{
		data += to_string(params[i]) + ",";
	}

	file << data << endl;

	data = "";

	params = control->getOneLineCurve()->getParams();

	for (int i = 0; i < params.size(); i++)
	{
		data += to_string(params[i]) + ",";
	}

	file << data << endl;
}

//Loads previously trained parameters from a files and passes them 
//to car control
void Trainer::loadFromFile()
{
	ifstream file;
	file.open("TrainingData/params.txt");

	if (file.is_open())
	{
		string line = "";

		//Loads steering speed parameters
		getline(file, line);

		int commaPos = line.find(',');
		double param0 = stod(line.substr(0, commaPos));
		double param1 = stod(line.substr(commaPos + 1, line.size() - 1));

		vector<double> params = vector<double>();

		params.push_back(param0);
		params.push_back(param1);

		control->getSteeringSpeedCurve()->updateParams(params);

		line = "";

		//Loads steering parameters
		getline(file, line);

		commaPos = line.find(',');
		int comma2Pos = line.find(',', commaPos+1);
		param0 = stod(line.substr(0, commaPos));
		param1 = stod(line.substr(commaPos + 1, comma2Pos));
		double param2 = stod(line.substr(comma2Pos + 1, line.size() - 1));

		params.clear();

		params.push_back(param0);
		params.push_back(param1);
		params.push_back(param2);

		control->getOneLineCurve()->updateParams(params);
	}
	else
	{
		cout << "FILE NOT OPENED" << endl;
	}
}


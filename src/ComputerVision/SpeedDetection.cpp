#include "SpeedDetection.h"

SpeedDetection::SpeedDetection()
{
	//loads the images of each digit stored on file
	string path = "Images/AssettoNumbers/2/";
	for (int i = 0; i < 10; i++)
	{
		string fullPath = path + to_string(i) + ".png";

		Mat numImg = imread(fullPath);
		cvtColor(numImg, numImg, CV_BGR2GRAY);//Turns the images from color to grayscale

		numbers.push_back(numImg);

		vector<Point> pos = findDigits(&numbers[i]);
		for (int a = 0; a < pos.size(); a++)
		{
			numPos.push_back(pos[a]);
		}
	}
}

SpeedDetection::~SpeedDetection()
{

}

int SpeedDetection::getSpeed(Mat* image)
{
	numDigits = 1;

	cvtColor(*image, *image, CV_BGR2GRAY);//Turns the images from color to grayscale
	Canny(*image, *image, 50, 100, 3);

	int closest = -1;//The number that is closest to the image
	int max = 0;

	//Find the positions of each digit
	vector<Point> digitPos = findDigits(image);

	int speed = 0;

	//Steps through each sub section based on the positions of the digits found
	for (int digit = 0; digit < digitPos.size(); digit += 2)
	{
		//Checks each digit against each digit stored in file
		for (int a = 0; a < numbers.size(); a++)
		{
			int width = 0;
			int dif = 0;

			//Finds the difference in starting points and width of the file image and the sub image
			if (digitPos[digit + 1].x - digitPos[digit].x < numPos[a * 2 + 1].x - numPos[a * 2].x)
			{
				width = digitPos[digit + 1].x - digitPos[digit].x;

				dif = numPos[a * 2].x - digitPos[digit].x;
			}
			else
			{
				width = numPos[a * 2 + 1].x - numPos[a * 2].x;

				dif = numPos[a * 2].x - digitPos[digit].x;
			}

			//Finds the number of pixels that are the same between the two images
			int same = 0;
			for (int i = digitPos[digit].x; i < digitPos[digit].x + width; i++)
			{
				for (int j = 0; j < image->rows; j++)
				{
					Scalar numPix = numbers[a].at<uchar>(Point(i + dif, j));

					Scalar imagePix = image->at<uchar>(Point(i, j));

					float nb = numPix.val[0];

					float ib = imagePix.val[0];

					if (ib == 255 && nb == 255)
					{
						same++;
					}
				}
			}

			if (same > max)
			{
				max = same;
				closest = a;
			}
		}

		//Adds the digit x 10 to the power of it's position
		speed += closest * pow(10, ((digitPos.size() - (digit + 1)) / 2));
	}

	return speed;
}

//Finds all of the digits in the image
vector<Point> SpeedDetection::findDigits(Mat* image)
{
	vector<Point> pos = vector<Point>();
	
	Point lastPos = Point(0, 0);

	bool moreDigits = true;
	while (moreDigits)
	{
		vector<Point> digitPos = findDigit(image, lastPos);

		if (digitPos.size() == 2)//If there is a start and an end point to the found digit
		{
			pos.push_back(digitPos[0]);
			pos.push_back(digitPos[1]);

			lastPos = digitPos[1];
			lastPos.x += 1;
		}
		else
		{
			moreDigits = false;
		}
	}

	return pos;
}

//Finds the next digit after a certain point in the image
vector<Point> SpeedDetection::findDigit(Mat* image, Point start)
{
	//Finds where the digit begins
	Point firstWhitePix;

	bool found = false;
	for (int i = start.x; i < image->cols; i++)
	{
		for (int j = 0; j < image->rows; j++)
		{
			Scalar imagePix = image->at<uchar>(Point(i, j));

			float intensity = imagePix.val[0];

			if (intensity == 255)
			{
				firstWhitePix = Point(i, j);
				found = true;
				break;
			}
		}

		if (found)
		{
			break;
		}
	}

	if (!found)//No more digits to be found
	{
		return vector<Point>();
	}

	//Finds where the digit ends
	Point lastWhitePix;

	for (int i = firstWhitePix.x + 1; i < image->cols; i++)
	{
		bool found = true;
		for (int j = 2; j < image->rows; j++)
		{
			Scalar imagePix = image->at<uchar>(Point(i, j));

			float intensity = imagePix.val[0];

			if (intensity == 255)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			lastWhitePix = Point(i, 0);
			break;
		}
	}

	if (lastWhitePix.x == 0)
	{
		lastWhitePix.x = image->cols;
	}

	vector<Point> pos = vector<Point>();
	pos.push_back(firstWhitePix);
	pos.push_back(lastWhitePix);

	return pos;
}
#include "ColourDetection.h"

ColourDetection::ColourDetection()
{
}

ColourDetection::~ColourDetection()
{
}

int ColourDetection::isApproachingCorner(vector<Point>* arc1, vector<Point>* arc2)
{
	if (arc1->size() > 0 && arc2->size() > 0)
	{
		//Finds the equations of the lines from the start of the arcs to the end
		Point arc1Start = (*arc1)[0];
		Point arc1End = (*arc1)[arc1->size() - 1];

		Point arc2Start = (*arc2)[0];
		Point arc2End = (*arc2)[arc2->size() - 1];

		arc1Grad = (double)(arc1End.y - arc1Start.y) / (double)(arc1End.x - arc1Start.x);
		arc1C = arc1Start.y - arc1Grad * arc1Start.x;

		arc2Grad = (double)(arc2End.y - arc2Start.y) / (double)(arc2End.x - arc2Start.x);
		arc2C = arc2Start.y - arc2Grad * arc2Start.x;

		twoTone = Mat(Size(1920, 580), CV_8UC4);
		toTwoTone();

		longestLine = 0;

		//Finds the longest continuos line of black from the track lines to the edge
		//of the screen
		for (int i = 0; i < twoTone.cols; i++)
		{
			//Ignores the top 50 pixels
			for (int j = 50; (j < arc1End.y || j < arc2End.y); j++)
			{
				Vec4b pix = twoTone.at<Vec4b>(j, i);

				float b = pix.val[0];
				float g = pix.val[1];
				float r = pix.val[2];

				if (abs(arc1C + arc1Grad * i - j) < 1)//If the pixel is below the left track line
				{
					twoTone.at<Vec4b>(j, i) = Vec4b(0, 0, 255, 255);

					//From the edge of the track lines to the left of the screen
					for (int a = i - 2; a >= 0; a--)
					{
						Vec4b pix2 = twoTone.at<Vec4b>(j, a);

						float g2 = pix2.val[1];

						if (g2 == 0)
						{
							if (i - a >= longestLine)
							{
								longestLine = i - a;
								longestStart = Point(i, j);
								longestEnd = Point(a, j);
							}
						}
						else
						{
							break;
						}
					}
				}
				else if (abs(arc2C + arc2Grad * i - j) < 1)//If the pixel is right the left track line
				{
					twoTone.at<Vec4b>(j, i) = Vec4b(0, 0, 255, 255);

					//From the edge of the track lines to the right of the screen
					for (int a = i + 2; a < twoTone.cols; a++)
					{
						Vec4b pix2 = twoTone.at<Vec4b>(j, a);

						float g2 = pix2.val[1];

						if (g2 == 0)
						{
							if (a - i >= longestLine)
							{
								longestLine = a - i;
								longestStart = Point(i, j);
								longestEnd = Point(a, j);
							}
						}
						else
						{
							break;
						}
					}
				}
			}
		}

		line(twoTone, longestStart, longestEnd, Scalar(255, 0, 0), 2);

		//If the line is long enough to contitute an approaching corner
		if (abs(longestEnd.x - longestStart.x) > 200)
		{
			if (longestEnd.x > longestStart.x)//Right
			{
				return 1;
			}
			else//Left
			{
				return 2;
			}
		}
	}
	else
	{
		twoTone = Mat(Size(1920, 580), CV_8UC4);
		toTwoToneNoLines();
	}

	return 0;//No approahing corner found
}

//Finds the depth of the corner
int ColourDetection::cornerDepth()
{
	int longestDist = 0;
	Point start;
	Point end;

	//If approaching corner is a right hand bend
	if (longestEnd.x > longestStart.x)
	{
		//First 200 pixels always return longest possible line so are skipped
		for (int i = longestStart.x + 200; i < longestEnd.x; i++)
		{
			int dist = 0;

			bool found = false;

			int y = longestStart.y + 1;

			//If a green pixel or the bottom of the screen is found the line stops
			while (!found && y < 580)
			{
				Vec4b pix2 = twoTone.at<Vec4b>(y, i);

				if (pix2.val[1] == 255)
				{
					found = true;
				}
				else
				{
					dist++;
					y++;
				}
			}

			if (dist > longestDist)
			{
				longestDist = dist;
				start = Point(i, longestStart.y);
				end = Point(i, y);
			}
		}

		line(twoTone, start, end, Scalar(255, 0, 255), 2);
	}
	else
	{
		for (int i = longestEnd.x; i < longestStart.x - 200; i++)
		{
			int dist = 0;

			bool found = false;

			int y = longestStart.y + 1;

			while (!found && y < 580)
			{
				Vec4b pix2 = twoTone.at<Vec4b>(y, i);

				if (pix2.val[1] == 255)
				{
					found = true;
				}
				else
				{
					dist++;
					y++;
				}
			}

			if (dist > longestDist)
			{
				longestDist = dist;
				start = Point(i, longestStart.y);
				end = Point(i, y);
			}
		}

		line(twoTone, start, end, Scalar(255, 0, 255), 2);
	}

	return longestLine;
}

//Creates a two tone image, black representing grey pixels (the track)
//and green represnting everything else
void ColourDetection::toTwoTone()
{
	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			//Only checking pixels outside of the track lines - that's where upcoming corners will lie
			if (j < arc1C + arc1Grad * i || j < arc2C + arc2Grad * i)
			{
				Vec4b pix = image.at<Vec4b>(j, i);

				float b = pix.val[0];
				float g = pix.val[1];
				float r = pix.val[2];

				if (abs(b - g) > 5 && abs(b - r) > 5)//If the pixel is not grey
				{
					twoTone.at<Vec4b>(j, i) = Vec4b(0, 255, 0, 255);
				}
			}
		}
	}
}

//Creates a two tone image when no track lines have been found
void ColourDetection::toTwoToneNoLines()
{
	for (int i = 0; i < image.cols; i++)
	{
		for (int j = 0; j < image.rows; j++)
		{
			Vec4b pix = image.at<Vec4b>(j, i);

			float b = pix.val[0];
			float g = pix.val[1];
			float r = pix.val[2];

			if (abs(b - g) > 5 && abs(b - r) > 5)
			{
				twoTone.at<Vec4b>(j, i) = Vec4b(0, 255, 0, 255);
			}
		}
	}
}

//If two points are next to each other
bool ColourDetection::eightConnected(Point p1, Point p2)
{
	if (abs(p1.x - p2.x) <= 1 && abs(p1.y - p2.y) <= 1)
	{
		return true;
	}
	return false;
}

//Finds if the vehicle is leaving the track
double ColourDetection::leavingTrack()
{
	double green = 0;

	int left = 0;
	int right = 0;

	//Finds how many green pixels there are and if they're on the right
	//or left of the screen
	for (int i = twoTone.cols / 4; i < twoTone.cols * 0.75; i++)
	{
		for (int j = 0; j < twoTone.rows; j++)
		{
			Vec4b pix = twoTone.at<Vec4b>(j, i);

			if (pix.val[1] == 255)
			{
				green++;
				if (i < twoTone.cols / 2)
				{
					left++;
				}
				else
				{
					right++;
				}
			}
		}
	}

	double proportion = green / ((twoTone.cols / 2) * twoTone.rows);

	if (proportion > 0.3)
	{
		if (left > right)
		{
			leftTrackLeft = true;
			leftTrackRight = false;
		}
		else
		{
			leftTrackRight = true;
			leftTrackLeft = false;
		}
	}

	return proportion;
}
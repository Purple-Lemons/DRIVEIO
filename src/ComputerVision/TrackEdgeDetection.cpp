#include "TrackEdgeDetection.h"

TrackEdgeDetection::TrackEdgeDetection(int rows, int cols)
{
	angles = new double*[cols];

	for (int i = 0; i < cols; i++)
	{
		angles[i] = new double[rows];
		for (int j = 0; j < rows; j++)
		{
			angles[i][j] = 0;
		}
	}
}

TrackEdgeDetection::TrackEdgeDetection()
{
}

TrackEdgeDetection::~TrackEdgeDetection()
{
}

void TrackEdgeDetection::getEdges(Mat* image, Mat* imgSrc, vector<Point> *_arc1, vector<Point> *_arc2)
{
	//Removes arcs from previous run
	arcs1.swap(vector<vector<Point>>());
	arcs2.swap(vector<vector<Point>>());

	//Finds the edges and gradients of the image
	Canny(*image, edges, 50, 100);

	Sobel(*image, dx, CV_64F, 1, 0, 7);
	Sobel(*image, dy, CV_64F, 0, 1, 7);

	getArcs();
	
	getBestLines(image, imgSrc);

	if (arcs2.size() > 0)
	{
		//Reverses the direction of arc2, since it's lines go right to left
		//but are found left to right
		vector<Point> temp = vector<Point>();
		for (int i = arcs2[0].size() - 1; i >= 0; i--)
		{
			temp.push_back(arcs2[0][i]);
		}

		arcs2[0] = temp;

		*_arc2 = arcs2[0];
	}
	else
	{
		_arc2->clear();
	}

	if (arcs1.size() > 0)
	{
		*_arc1 = arcs1[0];
	}
	else
	{
		_arc1->clear();
	}

	//draw track lines
	for (int i = arcs1.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < arcs1[i].size(); j++)
		{
			circle(*imgSrc, arcs1[i][j], 1, Scalar(0, 255, 0));
		}
	}
	for (int i = arcs2.size() - 1; i >= 0; i--)
	{
		for (int j = 0; j < arcs2[i].size(); j++)
		{
			circle(*imgSrc, arcs2[i][j], 1, Scalar(0, 255, 0));
		}
	}
}

//Finds all of the arcs in the image
void TrackEdgeDetection::getArcs()
{
	//Finds arcs in the left half of the image
	for (int i = 0; i < dx.cols / 2; i++)
	{
		for (int j = 0; j < dx.rows; j++)
		{
			//Gets the pixels in the edges image at (i, j)
			Scalar pix = edges.at<uchar>(Point(i, j));

			//If pixel is white
			if (pix[0] != 0)
			{
				//The sobel gradient of the pixel
				Scalar posDY = dy.at<double>(Point(i, j));
				Scalar posDX = dx.at<double>(Point(i, j));

				double gradient = atan2(posDY[0], posDX[0]);

				angles[i][j] = gradient;

				//If this is the first arc found
				if (arcs1.size() == 0)
				{
					Point p = Point(i, j);
					vector<Point> v;
					v.push_back(p);
					arcs1.push_back(v);
				}
				else
				{
					//Searchs for an arc that this pixel can correcspond to
					bool found = false;
					for (int k = 0; k < arcs1.size(); k++)
					{
						Point p = arcs1[k][0];
						if (sameSign(gradient, angles[p.x][p.y]))
						{
							for (int l = 0; l < arcs1[k].size(); l++)
							{
								p = arcs1[k][l];
								//If within 1 pixel distance of the last pixel and is in the same quadrant of an ellipse
								if (eightConnected(Point(i, j), p))// && abs(gradient - angles[p.x][p.y])
								{
									arcs1[k].push_back(Point(i, j));
									found = true;
									break;
								}
							}
						}
						if (found)
						{
							break;
						}
					}

					if (!found)
					{
						Point p = Point(i, j);
						vector<Point> v;
						v.push_back(p);
						arcs1.push_back(v);
					}
				}
			}
		}
	}

	joinArcs(&arcs1);

	//Finds arcs in the right side of the image
	for (int i = dx.cols / 2; i < dx.cols; i++)
	{
		for (int j = 0; j < dx.rows; j++)
		{
			//Gets the pixels in the edges image at (i, j)
			Scalar pix = edges.at<uchar>(Point(i, j));

			//If pixel is white
			if (pix[0] != 0)
			{
				//The sobel gradient of the pixel
				Scalar posDY = dy.at<double>(Point(i, j));
				Scalar posDX = dx.at<double>(Point(i, j));

				double gradient = atan2(posDY[0], posDX[0]);

				angles[i][j] = gradient;

				//If this is the first arc found
				if (arcs2.size() == 0)
				{
					Point p = Point(i, j);
					vector<Point> v;
					v.push_back(p);
					arcs2.push_back(v);
				}
				else
				{
					//Searchs for an arc that this pixel can correcspond to
					bool found = false;
					for (int k = 0; k < arcs2.size(); k++)
					{
						Point p = arcs2[k][0];
						if (sameSign(gradient, angles[p.x][p.y]))
						{
							for (int l = 0; l < arcs2[k].size(); l++)
							{
								p = arcs2[k][l];
								//If within 1 pixel distance of the last pixel and is in the same quadrant of an ellipse
								if (eightConnected(Point(i, j), p))// && abs(gradient - angles[p.x][p.y])
								{
									arcs2[k].push_back(Point(i, j));
									found = true;
									break;
								}
							}
						}
						if (found)
						{
							break;
						}
					}

					if (!found)
					{
						Point p = Point(i, j);
						vector<Point> v;
						v.push_back(p);
						arcs2.push_back(v);
					}
				}
			}
		}
	}

	joinArcs(&arcs2);
}

//Finds the arc form each side of the screen that are most likely to consitute the edges of the track
void TrackEdgeDetection::getBestLines(Mat* image, Mat* imgSrc)
{
	int minLength = 300;
	int minXMargin = 150;
	int minYMargin = 100;

	//Remove useless arcs
	for (int i = arcs1.size() - 1; i >= 0; i--)
	{
		if (arcs1[i].size() < minLength || (arcs1[i][0].x > minXMargin && arcs1[i][0].y < SCREEN_HEIGHT - minXMargin) || arcs1[i][0].y < minYMargin || !isCloseToLast(arcs1[i][0], true) || arcs1[i][0].y < arcs1[i][arcs1[i].size() - 1].y)
		{
			arcs1.erase(arcs1.begin() + i);
		}
	}

	for (int i = arcs2.size() - 1; i >= 0; i--)
	{
		if (arcs2[i].size() < minLength || arcs2[i][arcs2[i].size() - 1].x < SCREEN_WIDTH - minXMargin || arcs2[i][arcs2[i].size() - 1].y < minYMargin || !isCloseToLast(arcs2[i][0], false) || arcs2[i][0].y > arcs2[i][arcs2[i].size() - 1].y)
		{
			arcs2.erase(arcs2.begin() + i);
		}
	}

	//Get best of the remaining arcs
	findLongestArcs(&arcs1);
	findLongestArcs(&arcs2);
}

void TrackEdgeDetection::findLongestArcs(vector<vector<Point>> *arcs)
{
	int max = 0;
	for (int i = arcs->size() - 1; i >= 0; i--)
	{
		int xDist = (*arcs)[i][0].x - (*arcs)[i][(*arcs)[i].size() - 1].x;
		int yDist = (*arcs)[i][0].y - (*arcs)[i][(*arcs)[i].size() - 1].y;
		int dist = xDist * xDist + yDist * yDist;//Avoid sqrt

		if (dist > max)
		{
			max = dist;
			arcs->erase(arcs->begin() + i + 1, arcs->end());//Removes all previous arcs
		}
		else
		{
			arcs->erase(arcs->begin() + i);
		}
	}
}

//Stops track lines jumping around eratically by checking if the new arc starts close to the arc from the previous frame
bool TrackEdgeDetection::isCloseToLast(Point point, bool leftOrRight)
{
	if (lastStartValid)
	{
		if (leftOrRight)
		{
			int leftXDist = abs(point.x - lastStartLeft.x);
			int leftYDist = abs(point.y - lastStartLeft.y);
			int leftDist = (int)sqrt(leftXDist * leftXDist + leftYDist * leftYDist);

			if (leftDist < maxChange)
			{
				return true;
			}
		}
		else
		{
			int rightXDist = abs(point.x - lastStartRight.x);
			int rightYDist = abs(point.y - lastStartRight.y);
			int rightDist = (int)sqrt(rightXDist * rightXDist + rightYDist * rightYDist);

			if (rightDist < maxChange)
			{
				return true;
			}
		}
		return false;
	}
	
	return true;
}

//Finds if pixels are next to each other in any of the eight directions
bool TrackEdgeDetection::eightConnected(Point p1, Point p2)
{
	if (abs(p1.x - p2.x) <= 1 && abs(p1.y - p2.y) <= 1)
	{
		return true;
	}
	return false;
}

//Finds if angles have the same sign
bool TrackEdgeDetection::sameSign(double angle1, double angle2)
{
	if (angle1 < 0 && angle2 < 0)
	{
		return true;
	}
	else if (angle1 >= 0 && angle2 >= 0)
	{
		return true;
	}

	return false;
}

//Finds if pixels are within 3 distance from each other
bool TrackEdgeDetection::close(Point p1, Point p2)
{
	if (abs(p1.x - p2.x) <= 3 && abs(p1.y - p2.y) <= 3)
	{
		return true;
	}
	return false;
}

//Checks if joining two arcs would create a doubled back line - two paralel lines effectively creating a very tight corner
bool TrackEdgeDetection::doublesBack(Point a1p1, Point a1pn, Point a2p1, Point a2pn)
{
	int xDist = a1p1.x - a2p1.x;
	int yDist = a1p1.y - a2p1.y;
	int distFromStart = sqrt(xDist * xDist + yDist * yDist);

	xDist = a1p1.x - a2pn.x;
	yDist = a1p1.y - a2pn.y;
	int distFromEnd = sqrt(xDist * xDist + yDist * yDist);

	return (distFromEnd < distFromStart);
}

//Joins arcs that are close to each other and are in different quadrants of an ellipse 
void TrackEdgeDetection::joinArcs(vector<vector<Point>>* arcs)
{
	for (int i = arcs->size() - 1; i >= 1; i--)
	{
		for (int j = i - 1; j >= 0; j--)
		{
			Point startPointI = (*arcs)[i][0];
			Point endPointI = (*arcs)[i][(*arcs)[i].size() - 1];

			Point startPointJ = (*arcs)[j][0];
			Point endPointJ = (*arcs)[j][(*arcs)[j].size() - 1];

			if (close(startPointI, endPointJ) && !doublesBack(endPointI, startPointI, endPointJ, startPointJ) && !doublesBack(endPointJ, startPointJ, endPointI, startPointI))
			{
				for (int a = 0; a < (*arcs)[i].size(); a++)
				{
					(*arcs)[j].push_back((*arcs)[i][a]);
				}

				arcs->erase(arcs->begin() + i);

				break;
			}
			else if (close(endPointI, startPointJ) && !doublesBack(startPointI, endPointI, startPointJ, endPointJ) && !doublesBack(startPointJ, endPointJ, startPointI, endPointI))
			{
				for (int a = 0; a < (*arcs)[i].size(); a++)
				{
					(*arcs)[j].insert((*arcs)[j].begin() + a, (*arcs)[i][a]);
				}

				arcs->erase(arcs->begin() + i);

				break;
			}
			else if (close(startPointI, startPointJ) && !doublesBack(endPointI, startPointI, startPointJ, endPointJ) && !doublesBack(endPointJ, startPointJ, startPointI, endPointI))
			{
				for (int a = 0; a < (*arcs)[i].size(); a++)
				{
					(*arcs)[j].insert((*arcs)[j].begin(), (*arcs)[i][a]);
				}

				arcs->erase(arcs->begin() + i);

				break;
			}
			else if (close(endPointI, endPointJ) && !doublesBack(startPointI, endPointI, endPointJ, startPointJ) && !doublesBack(startPointJ, endPointJ, endPointI, startPointI))
			{
				for (int a = (*arcs)[i].size() - 1; a >= 0; a--)
				{
					(*arcs)[j].push_back((*arcs)[i][a]);
				}

				arcs->erase(arcs->begin() + i);

				break;
			}
		}
	}
}

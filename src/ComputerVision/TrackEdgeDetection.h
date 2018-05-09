/*
This class is used to find the set of points that consitutes
the arcs that make up the edges of the race track
*/
#ifndef TRACKEDGEDETECTION_H
#define TRACKEDGEDETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <windows.h>
#include <process.h>
#include <chrono>
#include <opencv2/text.hpp>


using namespace cv;
using namespace std;

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 380
#define PI 3.1415926535

class TrackEdgeDetection
{
public:
	TrackEdgeDetection(int, int);
	TrackEdgeDetection();
	~TrackEdgeDetection();

	void getEdges(Mat*, Mat*, vector<Point>*, vector<Point>*);

private:
	Mat dx;
	Mat dy;
	Mat edges;
	double **angles;

	//Stops the arcs jumping around too much
	Point lastStartLeft;
	Point lastStartRight;
	bool lastStartValid = false;
	int maxChange = 25;

	vector<vector<Point>> arcs1;
	vector<vector<Point>> arcs2;

	void getArcs();
	void findLongestArcs(vector<vector<Point>>*);
	void getBestLines(Mat*, Mat*);

	bool isCloseToLast(Point, bool);

	bool eightConnected(Point p1, Point p2);
	bool close(Point p1, Point p2);
	bool sameSign(double angle1, double angle2);
	void joinArcs(vector<vector<Point>>*);
	bool doublesBack(Point, Point, Point, Point);
};

#endif
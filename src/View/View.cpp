#include "View.h"

View::View()
{
	namedWindow("Track Line Image", WINDOW_AUTOSIZE);

	GraphImage = Mat(Size(GRAPH_WIDTH, GRAPH_WIDTH), CV_8UC4);
}

View::~View()
{
}

void View::displayTrackLineImage(Mat* image)
{
	imshow("Track Line Image", *image);
}

void View::displayColourImage(Mat* image)
{
	imshow("Colour Image", *image);
}

void View::displayGraph(RegressionCurve* curve, bool root, int currentX)
{
	vector<double> params = curve->getParams();

	int margin = 40;

	//Draws the axis of the graph
	line(GraphImage, Point(margin, GRAPH_WIDTH - margin), Point(margin, margin), Scalar(255, 255, 255), 2);
	line(GraphImage, Point(margin, GRAPH_WIDTH - margin), Point(GRAPH_WIDTH, GRAPH_WIDTH - margin), Scalar(255, 255, 255), 2);

	//Writes the values onto the axis
	for (int i = 0; i < 400; i += 20)
	{
		line(GraphImage, Point(margin, GRAPH_WIDTH - margin - i), Point(margin / 2, GRAPH_WIDTH - margin - i), Scalar(255, 255, 255));
		putText(GraphImage, to_string(i), Point(1, GRAPH_WIDTH - margin - i), 16, 0.5, Scalar(255, 255, 255));

		line(GraphImage, Point(margin + i, GRAPH_WIDTH - margin), Point(margin + i, GRAPH_WIDTH - margin / 1.5), Scalar(255, 255, 255));
		putText(GraphImage, to_string(i*10), Point(i + margin - 15, GRAPH_WIDTH - ((i/20) % 2) * 16), 16, 0.5, Scalar(255, 255, 255));
	}

	//If the result of the curve needs to be square rooted
	if (root)
	{
		int increment = 100;

		for (int i = 0; i < 2000; i += increment)
		{
			Point p1 = Point(i, sqrt(params[0] + params[1] * i));
			Point p2 = Point(i + 100, sqrt(params[0] + params[1] * (i + 100)));

			p1.x = (p1.x) + margin;
			p1.y = GRAPH_WIDTH - ((p1.y) + margin);

			p2.x = (p2.x) + margin;
			p2.y = GRAPH_WIDTH - ((p2.y) + margin);

			if (p2.y < 0)
			{
				p2.y = GRAPH_WIDTH - margin;
			}
			if (p1.y < 0)
			{
				p1.y = GRAPH_WIDTH - margin;
			}

			line(GraphImage, p1, p2, Scalar(255, 0, 255), 2);

			if (abs(currentX - i) <= increment / 2)
			{
				circle(GraphImage, p1, 2, Scalar(255, 0, 0), 3);
			}
		}
	}
	else if (params.size() == 2)//If the function is a straight line
	{
		Point p1 = Point(0 + margin, params[0] + margin);

		Point p2 = Point(400 + margin, 400 * params[1] + params[0] + margin);

		p1.y = (GRAPH_WIDTH - p1.y);

		p2.y = (GRAPH_WIDTH - p2.y);

		line(GraphImage, p1, p2, Scalar(255, 0, 255), 2);
	}
	else
	{
		int increment = 10;

		for (int i = 0; i < 400; i += increment)
		{
			Point p1 = Point(i, 0);
			Point p2 = Point(i + increment, 0);

			for (int j = 0; j < params.size(); j++)
			{
				p1.y += pow(i, j) * params[j];
				p2.y += pow(i + increment, j) * params[j];
			}

			p1.y = p1.y;
			p2.y = p2.y;

			p1.y = (GRAPH_WIDTH - p1.y) - margin;
			p2.y = (GRAPH_WIDTH - p2.y) - margin;

			p1.x += margin;
			p2.x += margin;

			line(GraphImage, p1, p2, Scalar(255, 0, 255), 2);
		}
	}

	imshow("Graph", GraphImage);
}
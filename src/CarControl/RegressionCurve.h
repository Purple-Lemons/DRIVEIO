/*
This class models regression curves so that
the car control can train it's controls
*/
#ifndef REGRESSIONCURVE_H
#define REGRESSIONCURVE_H

#include <iostream>
#include <vector>

using namespace std;

class RegressionCurve
{
public:
	RegressionCurve();
	RegressionCurve(vector<double>);
	~RegressionCurve();

	double run(vector<double>);
	
	void updateParams(vector<double>);

	void modifyParams(vector<double>);

	vector<double> getParams()
	{
		return params;
	}

private:
	vector<double> params;
};

#endif // !REGRESSIONCURVE_H

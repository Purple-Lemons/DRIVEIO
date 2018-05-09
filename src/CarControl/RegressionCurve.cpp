#include "RegressionCurve.h"

RegressionCurve::RegressionCurve()
{
	params = vector<double>();
}

RegressionCurve::RegressionCurve(vector<double> _params)
{
	params = _params;
}

RegressionCurve::~RegressionCurve()
{

}

//Find the y values of the curve given these x values
double RegressionCurve::run(vector<double> vals)
{
	double y = 0.0;
	
	for (int i = 0; i < vals.size(); i++)
	{
		y += pow(vals[i], i) * params[i];
	}

	return y;
}

//Set a new set of parameters
void RegressionCurve::updateParams(vector<double> _params)
{
	params = _params;
}

//Alter existing parameters
void RegressionCurve::modifyParams(vector<double> paramChanges)
{
	for (int i = 0; i < params.size(); i++)
	{
		params[i] += paramChanges[i];
	}
}
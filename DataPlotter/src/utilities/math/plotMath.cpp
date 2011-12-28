/*===================================================================================
                                    DataPlotter
                           Copyright Kerry R. Loux 2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  plotMath.cpp
// Created:  3/24/2008
// Author:  K. Loux
// Description:  Contains useful functions that don't fit better in another class.  Hopefully this
//				 file will one day be absolved into a real class instead of just being a kludgy
//				 collection of functions.
// History:

// Standard C++ headers
#include <cstdlib>
#include <assert.h>

// Local headers
#include "utilities/math/plotMath.h"
#include "utilities/math/vector.h"
#include "utilities/math/matrix.h"

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//
// Input Arguments:
//		toCheck	= const double& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the number is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const double &toCheck)
{
	if (fabs(toCheck) < NEARLY_ZERO)
		return true;
	else
		return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsZero
//
// Description:		Returns true if a number is small enough to regard as zero.
//					This function checks the magnitude of the VECTOR.
//
// Input Arguments:
//		toCheck	= const Vector& to be checked for being close to zero
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the magnitude is less than NEARLY_ZERO
//
//==========================================================================
bool PlotMath::IsZero(const Vector &toCheck)
{
	// Check each component of the vector
	if (toCheck.Length() < NEARLY_ZERO)
		return true;

	return false;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//
// Input Arguments:
//		toCheck	= const double& to check
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool PlotMath::IsNaN(const double &toCheck)
{
	return toCheck != toCheck;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		IsNaN
//
// Description:		Determines if the specified number is or is not a number.
//					Vector version - returns false if any component is NaN.
//
// Input Arguments:
//		ToCheck	= Vector& to be checked for containing valid numbers
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if the argument is NOT a number
//
//==========================================================================
bool PlotMath::IsNaN(const Vector &toCheck)
{
	return IsNaN(toCheck.x) || IsNaN(toCheck.y) || IsNaN(toCheck.z);
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Clamp
//
// Description:		Ensures the specified value is between the limits.  In the
//					event that the value is out of the specified bounds, the
//					value that is returned is equal to the limit that the value
//					has exceeded.
//
// Input Arguments:
//		value		= const double& reference to the value which we want to clamp
//		lowerLimit	= const double& lower bound of allowable values
//		upperLimit	= const double& upper bound of allowable values
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the clamped value
//
//==========================================================================
double PlotMath::Clamp(const double &value, const double &lowerLimit, const double &upperLimit)
{
	// Make sure the arguments are valid
	assert(lowerLimit < upperLimit);

	if (value < lowerLimit)
		return lowerLimit;
	else if (value > upperLimit)
		return upperLimit;

	return value;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		RangeToPlusMinusPi
//
// Description:		Adds or subtracts 2 * PI to the specified angle until the
//					angle is between -pi and pi.
//
// Input Arguments:
//		angle		= const double& reference to the angle we want to bound
//
// Output Arguments:
//		None
//
// Return Value:
//		double, equal to the re-ranged angle
//
//==========================================================================
double PlotMath::RangeToPlusMinusPi(const double &_angle)
{
	double angle = _angle;
	while (angle <= Pi)
		angle += 2 * Pi;
	while (angle > Pi)
		angle -= 2 * Pi;

	return angle;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Sign
//
// Description:		Returns 1 for positive, -1 for negative and 0 for zero.
//
// Input Arguments:
//		value		= const double&
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotMath::Sign(const double &value)
{
	if (value > 0.0)
		return 1.0;
	else if (value < 0.0)
		return -1.0;
	else
		return 0.0;
}

//==========================================================================
// Namespace:		PlotMath
// Function:		Modulo
//
// Description:		Modulo for doubles.  Returns a value between zero and divisor.
//
// Input Arguments:
//		value		= const double&
//		div			= cosnt double&, divisor
//
// Output Arguments:
//		None
//
// Return Value:
//		double
//
//==========================================================================
double PlotMath::Modulo(const double &value, const double &div)
{
	double modulo(value);
	while (modulo >= fabs(div))
		modulo -= div;
	while (modulo < 0)
		modulo += div;

	return modulo;
}
/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  plotCurve.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Derived from Primitive for creating plot curves objects.
// History:

#ifndef PLOT_CURVE_H_
#define PLOT_CURVE_H_

// Local headers
#include "renderer/primitives/primitive.h"
#include "utilities/managedList.h"
#include "renderer/line.h"

// Local forward declarations
class Axis;
class Dataset2D;

class PlotCurve : public Primitive
{
public:
	PlotCurve(RenderWindow &renderWindow, const Dataset2D& data);
	PlotCurve(const PlotCurve &plotCurve);

	virtual ~PlotCurve();

	inline void SetLineSize(const double &size) { lineSize = size; modified = true; }
	inline void SetMarkerSize(const int &size) { markerSize = size; modified = true; }
	inline void SetPretty(const bool &pretty) { this->pretty = pretty; line.SetPretty(pretty); modified = true; }

	// For setting up the plot
	inline void BindToXAxis(Axis *xAxis) { this->xAxis = xAxis; modified = true; }
	inline void BindToYAxis(Axis *yAxis) { this->yAxis = yAxis; modified = true; }

	inline Axis *GetYAxis() { return yAxis; }

	// Overloaded operators
	PlotCurve& operator=(const PlotCurve &plotCurve);

protected:
	// Mandatory overloads from Primitive - for creating geometry and testing the
	// validity of this object's parameters
	virtual bool HasValidParameters();
	virtual void Update(const unsigned int& i);
	virtual void GenerateGeometry();

private:
	// The axes with which this object is associated
	Axis *xAxis;
	Axis *yAxis;

	const Dataset2D& data;

	Line line;

	bool pretty;
	double lineSize;
	int markerSize;

	bool PointIsValid(const unsigned int &i) const;

	bool NeedsMarkersDrawn() const;
	void BuildMarkers();

	enum RangeSize
	{
		RangeSizeSmall,
		RangeSizeLarge,
		RangeSizeUndetermined
	};

	bool RangeIsSmall() const;
	RangeSize XRangeIsSmall() const;
	RangeSize YRangeIsSmall() const;

	double xScale;
	double yScale;

	void InitializeMarkerVertexBuffer();
};

#endif// PLOT_CURVE_H_
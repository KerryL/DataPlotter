/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  mainFrame.h
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Main frame for the application.
// History:

#ifndef MAIN_FRAME_H_
#define MAIN_FRAME_H_

// Standard C++ headers
#include <vector>

// wxWidgets headers
#include <wx/wx.h>

// Local headers
#include "lp2d/utilities/dataset2D.h"
#include "lp2d/parser/dataFile.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// LibPlot2D forward declarations
namespace LibPlot2D
{

class PlotRenderer;
class CustomFileFormat;
class Color;
class Filter;
class DataFile;
class PlotListGrid;

}

// The main frame class
class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame() = default;

	bool GetXAxisScalingFactor(double &factor, wxString *label = NULL);
	static bool UnitStringToFactor(const wxString &unit, double &factor);

private:
	static const unsigned long long highQualityCurvePointLimit;

	// Functions that do some of the frame initialization and control positioning
	void CreateControls();
	void SetProperties();

	LibPlot2D::PlotRenderer* CreatePlotArea(wxWindow *parent);
	wxBoxSizer* CreateButtons(wxWindow *parent);

	// Controls
	LibPlot2D::PlotListGrid *optionsGrid;
	LibPlot2D::PlotRenderer *plotArea;

	// The event IDs
	enum MainFrameEventID
	{
		idButtonOpen = wxID_HIGHEST + 500,
		idButtonAutoScale,
		idButtonRemoveCurve,
		idButtonReloadData
	};

	// Button events
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);
	void ButtonReloadDataClickedEvent(wxCommandEvent &event);

	void SetMarkerSize(const unsigned int &curve, const int &size);

	void ShowAppropriateXLabel();

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	static wxString ExtractUnitFromDescription(const wxString &description);
	static bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close);

	LibPlot2D::Dataset2D *GetFFTData(const LibPlot2D::Dataset2D* data);

	void AddFFTCurves(const double& xFactor, LibPlot2D::Dataset2D *amplitude,
		LibPlot2D::Dataset2D *phase, LibPlot2D::Dataset2D *coherence,
		const wxString &namePortion);
		
	void UpdateLegend();

	LibPlot2D::Filter* GetFilter(const LibPlot2D::FilterParameters &parameters,
		const double &sampleRate, const double &initialValue) const;

	void SetTitleFromFileName(wxString pathAndFileName);
	void SetXDataLabel(wxString label);
	void SetXDataLabel(const FileFormat &format);

	LibPlot2D::Dataset2D GetXZoomedDataset(const LibPlot2D::Dataset2D &fullData) const;

	void UpdateCurveQuality();

	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations();
};

#endif// MAIN_FRAME_H_
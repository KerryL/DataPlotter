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
#include "lp2d/utilities/managedList.h"
#include "lp2d/utilities/signals/curveFit.h"
#include "lp2d/parser/dataFile.h"

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// Local forward declarations
class PlotRenderer;
struct FilterParameters;
class CustomFileFormat;
class Color;
class Filter;
class DataFile;

// The main frame class
class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame() = default;

	// For getting a open/save file name from the user
	wxArrayString GetFileNameFromUser(wxString dialogTitle, wxString defaultDirectory,
		wxString defaultFileName, wxString wildcard, long style);

	// Load a plot from file
	bool LoadFiles(const wxArrayString &fileList);
	bool LoadText(const wxString &textData);

	enum PlotContext
	{
		PlotContextXAxis,
		PlotContextLeftYAxis,
		PlotContextRightYAxis,
		PlotContextPlotArea
	};

	bool GetXAxisScalingFactor(double &factor, wxString *label = NULL);
	static bool UnitStringToFactor(const wxString &unit, double &factor);

	void CreatePlotContextMenu(const wxPoint &position, const PlotContext &context);
	void DisplayAxisRangeDialog(const PlotContext &axis);

	void UpdateCursorValues(const bool &leftVisible, const bool &rightVisible,
		const double &leftValue, const double &rightValue);

private:
	static const unsigned long long highQualityCurvePointLimit;

	// Functions that do some of the frame initialization and control positioning
	void CreateControls();
	void SetProperties();

	LibPlot2D::PlotRenderer* CreatePlotArea(wxWindow *parent);
	wxGrid* CreateOptionsGrid(wxWindow *parent);
	wxBoxSizer* CreateButtons(wxWindow *parent);

	// Controls
	wxGrid *optionsGrid;
	LibPlot2D::PlotRenderer *plotArea;

	enum Columns
	{
		colName = 0,
		colColor,
		colLineSize,
		colMarkerSize,
		colLeftCursor,
		colRightCursor,
		colDifference,
		colVisible,
		colRightAxis,

		colCount
	};

	// For displaying a menu that was crated by this form
	// NOTE:  When calculating the Position to display this context menu,
	// consider that the coordinates for the calling object might be different
	// from the coordinates for this object!
	void CreateGridContextMenu(const wxPoint &position, const unsigned int &row);

	void ClearAllCurves();
	void AddCurve(wxString mathString);
	void AddCurve(LibPlot2D::Dataset2D *data, wxString name);
	void RemoveCurve(const unsigned int &i);

	LibPlot2D::Color GetNextColor(const unsigned int &index) const;
	void AddTimeRowToGrid();
	unsigned int AddDataRowToGrid(const wxString &name);

	void UpdateCurveProperties(const unsigned int &index,
		const LibPlot2D::Color &color, const bool &visible,
		const bool &rightAxis);
	void UpdateCurveProperties(const unsigned int &index);

	LibPlot2D::ManagedList<const LibPlot2D::Dataset2D> plotList;

	// The event IDs
	enum MainFrameEventID
	{
		idButtonOpen = wxID_HIGHEST + 100,
		idButtonAutoScale,
		idButtonRemoveCurve,
		idButtonReloadData,

		idContextAddMathChannel,
		idContextFRF,

		idContextCreateSignal,

		idContextSetTimeUnits,
		idContextScaleXData,
		idContextPlotDerivative,
		idContextPlotIntegral,
		idContextPlotRMS,
		idContextPlotFFT,
		idContextTimeShift,
		idContextBitMask,

		idContextFilter,
		idContextFitCurve,

		idPlotContextCopy,
		idPlotContextPaste,
		idPlotContextMajorGridlines,
		idPlotContextMinorGridlines,
		idPlotContextShowLegend,
		idPlotContextAutoScale,
		idPlotContextWriteImageFile,
		idPlotContextExportData,

		idPlotContextBGColor,
		idPlotContextGridColor,

		idPlotContextBottomMajorGridlines,// Maintain this order for each axis' context IDs
		idPlotContextBottomMinorGridlines,
		idPlotContextAutoScaleBottom,
		idPlotContextSetBottomRange,
		idPlotContextSetBottomMajorResolution,
		idPlotContextBottomLogarithmic,
		idPlotContextEditBottomLabel,

		/*idPlotContextTopMajorGridlines,
		idPlotContextTopMinorGridlines,
		idPlotContextAutoScaleTop,
		idPlotContextSetTopRange,
		idPlotContextSetTopMajorResolution,
		idPlotContextTopLogarithmic,
		idPlotContextEditTopLabel,*/

		idPlotContextLeftMajorGridlines,
		idPlotContextLeftMinorGridlines,
		idPlotContextAutoScaleLeft,
		idPlotContextSetLeftRange,
		idPlotContextSetLeftMajorResolution,
		idPlotContextLeftLogarithmic,
		idPlotContextEditLeftLabel,

		idPlotContextRightMajorGridlines,
		idPlotContextRightMinorGridlines,
		idPlotContextAutoScaleRight,
		idPlotContextSetRightRange,
		idPlotContextSetRightMajorResolution,
		idPlotContextRightLogarithmic,
		idPlotContextEditRightLabel
	};

	wxMenu *CreateAxisContextMenu(const unsigned int &baseEventId) const;
	wxMenu *CreatePlotAreaContextMenu() const;

	// Event handlers-----------------------------------------------------
	// Buttons
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);
	void ButtonReloadDataClickedEvent(wxCommandEvent &event);

	// Grid events
	void GridRightClickEvent(wxGridEvent &event);
	void GridDoubleClickEvent(wxGridEvent &event);
	void GridLeftClickEvent(wxGridEvent &event);
	void GridCellChangeEvent(wxGridEvent &event);
	void GridLabelRightClickEvent(wxGridEvent &event);

	// Context menu events
	void ContextAddMathChannelEvent(wxCommandEvent &event);
	void ContextFRFEvent(wxCommandEvent &event);
	void ContextCreateSignalEvent(wxCommandEvent &event);
	void ContextSetTimeUnitsEvent(wxCommandEvent &event);
	void ContextScaleXDataEvent(wxCommandEvent &event);
	void ContextPlotDerivativeEvent(wxCommandEvent &event);
	void ContextPlotIntegralEvent(wxCommandEvent &event);
	void ContextPlotRMSEvent(wxCommandEvent &event);
	void ContextPlotFFTEvent(wxCommandEvent &event);
	void ContextTimeShiftEvent(wxCommandEvent &event);
	void ContextBitMaskEvent(wxCommandEvent &event);

	void ContextFilterEvent(wxCommandEvent &event);
	void ContextFitCurve(wxCommandEvent &event);

	void ContextCopy(wxCommandEvent &event);
	void ContextPaste(wxCommandEvent &event);
	void ContextToggleMajorGridlines(wxCommandEvent &event);
	void ContextToggleMinorGridlines(wxCommandEvent &event);
	void ContextToggleLegend(wxCommandEvent &event);
	void ContextAutoScale(wxCommandEvent &event);
	void ContextWriteImageFile(wxCommandEvent &event);
	void ContextExportData(wxCommandEvent &event);

	void ContextPlotBGColor(wxCommandEvent &event);
	void ContextGridColor(wxCommandEvent &event);

	void ContextToggleMajorGridlinesBottom(wxCommandEvent &event);
	void ContextToggleMinorGridlinesBottom(wxCommandEvent &event);
	void ContextAutoScaleBottom(wxCommandEvent &event);
	void ContextSetRangeBottom(wxCommandEvent &event);
	void ContextSetMajorResolutionBottom(wxCommandEvent &event);
	void ContextSetLogarithmicBottom(wxCommandEvent &event);
	void ContextEditBottomLabel(wxCommandEvent &event);

	void ContextToggleMajorGridlinesLeft(wxCommandEvent &event);
	void ContextToggleMinorGridlinesLeft(wxCommandEvent &event);
	void ContextAutoScaleLeft(wxCommandEvent &event);
	void ContextSetRangeLeft(wxCommandEvent &event);
	void ContextSetMajorResolutionLeft(wxCommandEvent &event);
	void ContextSetLogarithmicLeft(wxCommandEvent &event);
	void ContextEditLeftLabel(wxCommandEvent &event);

	void ContextToggleMajorGridlinesRight(wxCommandEvent &event);
	void ContextToggleMinorGridlinesRight(wxCommandEvent &event);
	void ContextAutoScaleRight(wxCommandEvent &event);
	void ContextSetRangeRight(wxCommandEvent &event);
	void ContextSetMajorResolutionRight(wxCommandEvent &event);
	void ContextSetLogarithmicRight(wxCommandEvent &event);
	void ContextEditRightLabel(wxCommandEvent &event);
	// End event handlers-------------------------------------------------

	void SetMarkerSize(const unsigned int &curve, const int &size);

	void ShowAppropriateXLabel();

	void DisplayMathChannelDialog(wxString defaultInput = wxEmptyString);
	LibPlot2D::FilterParameters DisplayFilterDialog();
	void ApplyFilter(const LibPlot2D::FilterParameters &parameters, LibPlot2D::Dataset2D &data);

	bool XScalingFactorIsKnown(double &factor, wxString *label) const;
	static wxString ExtractUnitFromDescription(const wxString &description);
	static bool FindWrappedString(const wxString &s, wxString &contents,
		const wxChar &open, const wxChar &close);

	LibPlot2D::Dataset2D *GetFFTData(const LibPlot2D::Dataset2D* data);
	LibPlot2D::Dataset2D *GetCurveFitData(const unsigned int &order, const LibPlot2D::Dataset2D* data, wxString &name) const;
	wxString GetCurveFitName(const LibPlot2D::CurveFit::PolynomialFit &fitData, const unsigned int &row) const;

	void AddFFTCurves(const double& xFactor, LibPlot2D::Dataset2D *amplitude, LibPlot2D::Dataset2D *phase,
		LibPlot2D::Dataset2D *coherence, const wxString &namePortion);

	void UpdateSingleCursorValue(const unsigned int &row, double value,
		const unsigned int &column, const bool &isVisible);
		
	void UpdateLegend();

	bool GetCurrentAxisRange(const PlotContext &axis, double &min, double &max) const;
	void SetNewAxisRange(const PlotContext &axis, const double &min, const double &max);

	LibPlot2D::Filter* GetFilter(const LibPlot2D::FilterParameters &parameters,
		const double &sampleRate, const double &initialValue) const;

	LibPlot2D::DataFile* GetDataFile(const wxString &fileName);

	enum FileFormat
	{
		FormatBaumuller,
		FormatKollmorgen,
		FormatFrequency,
		FormatGeneric
	};

	FileFormat currentFileFormat;

	void SetTitleFromFileName(wxString pathAndFileName);
	void SetXDataLabel(wxString label);
	void SetXDataLabel(const FileFormat &format);
	wxString genericXAxisLabel;

	wxString ExtractFileNameFromPath(const wxString &pathAndFileName) const;

	wxArrayString lastFilesLoaded;
	LibPlot2D::DataFile::SelectionData lastSelectionInfo;
	wxArrayString lastDescriptions;

	LibPlot2D::Dataset2D GetXZoomedDataset(const LibPlot2D::Dataset2D &fullData) const;

	wxString GenerateTemporaryFileName(const unsigned int &length = 10) const;

	void DoCopy();
	void DoPaste();

	void UpdateCurveQuality();

	static bool ItemIsInArray(const wxArrayInt& a, const int& item);

	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations();
};

#endif// MAIN_FRAME_H_
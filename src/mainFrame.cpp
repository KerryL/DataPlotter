/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2016

                   This code is licensed under the GPLv2 License
                     (http://opensource.org/licenses/GPL-2.0).

===================================================================================*/

// File:  mainFrame.cpp
// Created:  5/2/2011
// Author:  K. Loux
// Description:  Contains the class functionality (event handlers, etc.) for the
//				 MainFrame class.  Uses wxWidgets for the GUI components.
// History:

// Standard C++ headers
#include <algorithm>

// wxWidgets headers
#include <wx/colordlg.h>
#include <wx/splitter.h>

// Local headers
#include "mainFrame.h"
#include "plotterApp.h"

// LibPlot2D headers
#include <lp2d/gui/textInputDialog.h>
#include <lp2d/gui/plotListGrid.h>
#include <lp2d/parser/genericFile.h>
#include <lp2d/parser/baumullerFile.h>
#include <lp2d/parser/kollmorgenFile.h>
#include <lp2d/parser/customFile.h>
#include <lp2d/parser/customXMLFile.h>
#include <lp2d/parser/customFileFormat.h>
#include <lp2d/renderer/plotRenderer.h>
#include <lp2d/renderer/color.h>
#include <lp2d/renderer/primitives/legend.h>
#include <lp2d/utilities/dataset2D.h>
#include <lp2d/utilities/guiUtilities.h>

// *nix Icons
#ifdef __WXGTK__
#include "../../res/icons/plots16.xpm"
#include "../../res/icons/plots24.xpm"
#include "../../res/icons/plots32.xpm"
#include "../../res/icons/plots48.xpm"
#include "../../res/icons/plots64.xpm"
#include "../../res/icons/plots128.xpm"
#endif

// Testing prototypes
//void TestSignalOperations();

//==========================================================================
// Class:			MainFrame
// Function:		Constant declarations
//
// Description:		Constant declarations for MainFrame class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
const unsigned long long MainFrame::highQualityCurvePointLimit(10000);

//==========================================================================
// Class:			MainFrame
// Function:		MainFrame
//
// Description:		Constructor for MainFrame class.  Initializes the form
//					and creates the controls, etc.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString,
	wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
{
	CreateControls();
	SetProperties();

	if (!LibPlot2D::CustomFileFormat::CustomDefinitionsExist())
		wxMessageBox(_T("Warning:  Custom file definitions not found!"),
		_T("Custom File Formats"), wxICON_WARNING, this);

	srand(time(NULL));

	//TestSignalOperations();
}

//==========================================================================
// Class:			MainFrame
// Function:		Constant Declarations
//
// Description:		Constant declarations for the MainFrame class.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*#ifdef __WXGTK__
const wxString MainFrame::pathToConfigFile = _T("dataplotter.rc");
#else
const wxString MainFrame::pathToConfigFile = _T("dataplotter.ini");
#endif*/

//==========================================================================
// Class:			MainFrame
// Function:		CreateControls
//
// Description:		Creates sizers and controls and lays them out in the window.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::CreateControls()
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	topSizer->Add(splitter, 1, wxGROW);

	wxPanel *lowerPanel = new wxPanel(splitter);
	wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
	lowerSizer->Add(CreateButtons(lowerPanel), 0, wxGROW | wxALL, 5);
	lowerSizer->Add(new LibPlot2D::PlotListGrid(lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	CreatePlotArea(splitter);
	splitter->SplitHorizontally(plotArea, lowerPanel, plotArea->GetSize().GetHeight());
	splitter->SetSize(GetClientSize());
	splitter->SetSashGravity(1.0);
	splitter->SetMinimumPaneSize(150);

	SetSizerAndFit(topSizer);
	splitter->SetSashPosition(splitter->GetSashPosition(), false);
}

//==========================================================================
// Class:			MainFrame
// Function:		CreatePlotArea
//
// Description:		Creates the main plot control.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::PlotRenderer* pointing to plotArea
//
//==========================================================================
LibPlot2D::PlotRenderer* MainFrame::CreatePlotArea(wxWindow *parent)
{
	wxGLAttributes displayAttributes;
	displayAttributes.PlatformDefaults().RGBA().DoubleBuffer().SampleBuffers(1).Samplers(4).Stencil(1).EndList();
	assert(wxGLCanvas::IsDisplaySupported(displayAttributes));
	plotArea = new LibPlot2D::PlotRenderer(*parent, wxID_ANY, displayAttributes);

	plotArea->SetMinSize(wxSize(650, 320));
	plotArea->SetMajorGridOn();
	plotArea->SetCurveQuality(LibPlot2D::PlotRenderer::QualityHighWrite);

	return plotArea;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateButtons
//
// Description:		Creates the buttons and returns the sizer pointer.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxBoxSizer*
//
//==========================================================================
wxBoxSizer* MainFrame::CreateButtons(wxWindow *parent)
{
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer->Add(topSizer);

	topSizer->Add(new wxButton(parent, idButtonOpen, _T("&Open")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonAutoScale, _T("&Auto Scale")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonRemoveCurve, _T("&Remove")), 1, wxGROW);
	topSizer->Add(new wxButton(parent, idButtonReloadData, _T("Reload &Data")), 1, wxGROW);

	buttonSizer->AddStretchSpacer(1);
	buttonSizer->Add(new wxStaticText(parent, wxID_ANY, DataPlotterApp::versionString
		+ _T(" (") + DataPlotterApp::gitHash + _T(")")));

	return buttonSizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetProperties
//
// Description:		Sets the window properties for this window.  Includes
//					title, frame size, and default fonts.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetProperties()
{
	SetTitle(DataPlotterApp::dataPlotterTitle);
	SetName(DataPlotterApp::dataPlotterName);
	Center();

#ifdef __WXMSW__
	SetIcon(wxIcon(_T("ICON_ID_MAIN"), wxBITMAP_TYPE_ICO_RESOURCE));
#elif __WXGTK__
	SetIcon(wxIcon(plots16_xpm));
	SetIcon(wxIcon(plots24_xpm));
	SetIcon(wxIcon(plots32_xpm));
	SetIcon(wxIcon(plots48_xpm));
	SetIcon(wxIcon(plots64_xpm));
	SetIcon(wxIcon(plots128_xpm));
#endif

	const int entryCount(5);
	wxAcceleratorEntry entries[entryCount];
	entries[0].Set(wxACCEL_CTRL, (int)'c', idPlotContextCopy);
	entries[1].Set(wxACCEL_CTRL, (int)'v', idPlotContextPaste);
	entries[2].Set(wxACCEL_CTRL, (int)'o', idButtonOpen);
	entries[3].Set(wxACCEL_CTRL, (int)'a', idButtonAutoScale);
	entries[4].Set(wxACCEL_CTRL, (int)'r', idButtonRemoveCurve);
	wxAcceleratorTable accel(entryCount, entries);
	SetAcceleratorTable(accel);
}

//==========================================================================
// Class:			MainFrame
// Function:		Event Table
//
// Description:		Links GUI events with event handler functions.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_BUTTON(idButtonOpen,						MainFrame::ButtonOpenClickedEvent)
	EVT_BUTTON(idButtonAutoScale,					MainFrame::ButtonAutoScaleClickedEvent)
	EVT_BUTTON(idButtonRemoveCurve,					MainFrame::ButtonRemoveCurveClickedEvent)
	EVT_BUTTON(idButtonReloadData,					MainFrame::ButtonReloadDataClickedEvent)
END_EVENT_TABLE();

//==========================================================================
// Class:			MainFrame
// Function:		ButtonOpenClickedEvent
//
// Description:		Displays a dialog asking the user to specify the file to
//					read from.
//
// Input Arguments:
//		event	= wxCommandEvent&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonOpenClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Set up the wildcard specifications (done here for readability)
	wxString wildcard("All files (*.*)|*.*");
	wildcard.append("|Comma Separated (*.csv)|*.csv");
	wildcard.append("|Tab Delimited (*.txt)|*.txt");

	wxArrayString fileList = LibPlot2D::GuiUtilities::GetFileNameFromUser(this,
		_T("Open Data File"), wxEmptyString, wxEmptyString, wildcard,
		wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

	if (fileList.GetCount() == 0)
		return;

	plotInterface.LoadFiles(fileList, this);
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonAutoScaleClickedEvent
//
// Description:		Event fires when user clicks "AutoScale" button.
//
// Input Arguments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonAutoScaleClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	plotArea->AutoScale();
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonRemoveCurveClickedEvent
//
// Description:		Event fires when user clicks "Remove" button.
//
// Input Arguments:
//		event	= &wxCommandEvent
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonRemoveCurveClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	// Workaround for now
	int i;
	for (i = optionsGrid->GetNumberRows() - 1; i > 0; i--)
	{
		if (optionsGrid->IsInSelection(i, 0))
			RemoveCurve(i - 1);
	}

	plotArea->UpdateDisplay();
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonReloadDataClickedEvent
//
// Description:		Event fires when user clicks "Reload Data" button.
//
// Input Arguments:
//		event	= &wxCommandEvent (unused)
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ButtonReloadDataClickedEvent(wxCommandEvent& WXUNUSED(event))
{
	plotInterface.ReloadData();
}

//==========================================================================
// Class:			MainFrame
// Function:		SetTitleFromFileName
//
// Description:		Sets the frame's title according to the specified file name.
//
// Input Arguments:
//		pathAndFileName	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetTitleFromFileName(wxString pathAndFileName)
{
	wxString fileName(ExtractFileNameFromPath(pathAndFileName));
	unsigned int end(fileName.find_last_of(_T(".")));
	SetTitle(fileName.Mid(0, end) + _T(" - ") + DataPlotterApp::dataPlotterTitle);
}

//==========================================================================
// Class:			MainFrame
// Function:		ExtractFileNameFromPath
//
// Description:		Removes the path from the path and file name.
//
// Input Arguments:
//		pathAndFileName	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString
//
//==========================================================================
wxString MainFrame::ExtractFileNameFromPath(const wxString &pathAndFileName) const
{
	unsigned int start;
#ifdef __WXMSW__
	start = pathAndFileName.find_last_of(_T("\\")) + 1;
#else
	start = pathAndFileName.find_last_of(_T("/")) + 1;
#endif
	return pathAndFileName.Mid(start);
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels to the specified string.
//
// Input Arguments:
//		label	= wxString
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetXDataLabel(wxString label)
{
	optionsGrid->SetCellValue(0, colName, label);
	plotArea->SetXLabel(label);
}

//==========================================================================
// Class:			MainFrame
// Function:		SetXDataLabel
//
// Description:		Sets the x-data labels according to the opened file type.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetXDataLabel(const FileFormat &format)
{
	switch (format)
	{
	case FormatFrequency:
		SetXDataLabel(_T("Frequency [Hz]"));
		break;

	default:
	case FormatGeneric:
	case FormatKollmorgen:
	case FormatBaumuller:
		SetXDataLabel(genericXAxisLabel);
	}
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateCurveQuality
//
// Description:		Sets curve quality according to how many lines need to
//					be rendered.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateCurveQuality()
{
	//if (plotArea->GetTotalPointCount() > highQualityCurvePointLimit)
		plotArea->SetCurveQuality(LibPlot2D::PlotRenderer::QualityHighWrite);
	/*else
		plotArea->SetCurveQuality(static_cast<PlotRenderer::CurveQuality>(
		PlotRenderer::QualityHighStatic | PlotRenderer::QualityHighWrite));*/// TODO:  Fix this after line rendering is improved
}

//==========================================================================
// Class:			MainFrame
// Function:		UpdateLegend
//
// Description:		Updates the contents of the legend actor.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::UpdateLegend()
{
	double lineSize;
	long markerSize;
	std::vector<LibPlot2D::Legend::LegendEntryInfo> entries;
	LibPlot2D::Legend::LegendEntryInfo info;
	int i;
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).IsEmpty())
			continue;
			
		optionsGrid->GetCellValue(i, colLineSize).ToDouble(&lineSize);
		optionsGrid->GetCellValue(i, colMarkerSize).ToLong(&markerSize);
		info.color = LibPlot2D::Color(optionsGrid->GetCellBackgroundColour(i, colColor));
		info.lineSize = lineSize;
		info.markerSize = markerSize;
		info.text = optionsGrid->GetCellValue(i, colName);
		entries.push_back(info);
	}
	plotArea->UpdateLegend(entries);
}

//==========================================================================
// Class:			MainFrame
// Function:		ShowAppropriateXLabel
//
// Description:		Updates the x-axis label as necessary.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::ShowAppropriateXLabel()
{
	// If the only visible curves are frequency plots, change the x-label
	int i;
	bool showFrequencyLabel(false);
	for (i = 1; i < optionsGrid->GetNumberRows(); i++)
	{
		if (optionsGrid->GetCellValue(i, colVisible).Cmp(_T("1")) == 0)
		{
			if (optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FFT")) == 0 ||
				optionsGrid->GetCellValue(i, colName).Mid(0, 3).CmpNoCase(_T("FRF")) == 0)
				showFrequencyLabel = true;
			else
			{
				showFrequencyLabel = false;
				break;
			}
		}
	}

	if (showFrequencyLabel)
		SetXDataLabel(FormatFrequency);
	else
		SetXDataLabel(currentFileFormat);
}

//==========================================================================
// Class:			MainFrame
// Function:		GetXAxisScalingFactor
//
// Description:		Attempts to determine the scaling factor required to convert
//					the X-axis into seconds (assuming X-axis has units of time).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&, scaling factor
//		label	= wxString*, pointer to string, to be populated with the unit string
//
// Return Value:
//		bool; true for success, false otherwise
//
//==========================================================================
bool MainFrame::GetXAxisScalingFactor(double &factor, wxString *label)
{
	if (XScalingFactorIsKnown(factor, label))
		return true;

	wxString unit = ExtractUnitFromDescription(genericXAxisLabel);

	unit = unit.Trim().Trim(false);
	if (label)
		label->assign(unit);

	return UnitStringToFactor(unit, factor);
}

//==========================================================================
// Class:			MainFrame
// Function:		XScalingFactorIsKnown
//
// Description:		If the x-axis scaling factor is known, determines its value.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		factor	= double&
//		label	= wxString*
//
// Return Value:
//		bool, true if known, false otherwise
//
//==========================================================================
bool MainFrame::XScalingFactorIsKnown(double &factor, wxString *label) const
{
	if (currentFileFormat == FormatBaumuller)
	{
		factor = 1000.0;
		if (label)
			label->assign(_T("msec"));
		return true;
	}
	else if (currentFileFormat == FormatKollmorgen)
	{
		factor = 1.0;
		if (label)
			label->assign(_T("sec"));
		return true;
	}

	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		ExtractUnitFromDescription
//
// Description:		Parses the description looking for a unit string.  This
//					will recognize the following as unit strings:
//					X Series Name [unit]
//					X Series Name (unit)
//					X Series Name *delimiter* unit
//
// Input Arguments:
//		description	= const wxString&
//
// Output Arguments:
//		None
//
// Return Value:
//		wxString containing the unit porition of the description
//
//==========================================================================
wxString MainFrame::ExtractUnitFromDescription(const wxString &description)
{
	wxString unit;
	if (FindWrappedString(description, unit, '[', ']'))
		return unit;
	else if (FindWrappedString(description, unit, '(', ')'))
		return unit;

	// Check for last string following a delimiter
	wxArrayString delimiters;
	delimiters.Add(_T(","));
	delimiters.Add(_T(";"));
	delimiters.Add(_T("-"));
	delimiters.Add(_T(":"));

	int location;
	unsigned int i;
	for (i = 0; i < delimiters.size(); i++)
	{
		location = description.Find(delimiters[i].mb_str());
		if (location != wxNOT_FOUND && location < (int)description.Len() - 1)
		{
			unit = description.Mid(location + 1);
			break;
		}
	}

	return unit;
}

//==========================================================================
// Class:			MainFrame
// Function:		FindWrappedString
//
// Description:		Determines if the specified string contains a string wrapped
//					with the specified characters.
//
// Input Arguments:
//		s		= const wxString&
//		open	= const wxChar& specifying the opening wrapping character
//		close	= const wxChar& specifying the closing warpping character
//
// Output Arguments:
//		contents	= wxString&
//
// Return Value:
//		bool, true if a wrapped string is found, false otherwise
//
//==========================================================================
bool MainFrame::FindWrappedString(const wxString &s, wxString &contents,
	const wxChar &open, const wxChar &close)
{
	if (s.Len() < 3)
		return false;

	if (s.Last() == close)
	{
		int i;
		for (i = s.Len() - 2; i >= 0; i--)
		{
			if (s.at(i) == open)
			{
				contents = s.Mid(i + 1, s.Len() - i - 2);
				return true;
			}
		}
	}

	return false;
}

//==========================================================================
// Class:			MainFrame
// Function:		UnitStringToFactor
//
// Description:		Converts from a unit string to a factor value.
//
// Input Arguments:
//		unit	= const wxString&
//
// Output Arguments:
//		factor	= double&
//
// Return Value:
//		bool, true if unit can be converted, false otherwise
//
//==========================================================================
bool MainFrame::UnitStringToFactor(const wxString &unit, double &factor)
{
	// We'll recognize the following units:
	// h, hr, hours -> factor = 1.0 / 3600.0
	// m, min, minutes -> factor = 1.0 / 60.0
	// s, sec, seconds -> factor = 1.0
	// ms, msec, milliseconds -> factor = 1000.0
	// us, usec, microseconds -> factor = 1000000.0

	if (unit.CmpNoCase(_T("h")) == 0 || unit.CmpNoCase(_T("hr")) == 0 || unit.CmpNoCase(_T("hours")) == 0)
		factor = 1.0 / 3600.0;
	else if (unit.CmpNoCase(_T("m")) == 0 || unit.CmpNoCase(_T("min")) == 0 || unit.CmpNoCase(_T("minutes")) == 0)
		factor = 1.0 / 60.0;
	else if (unit.CmpNoCase(_T("s")) == 0 || unit.CmpNoCase(_T("sec")) == 0 || unit.CmpNoCase(_T("seconds")) == 0)
		factor = 1.0;
	else if (unit.CmpNoCase(_T("ms")) == 0 || unit.CmpNoCase(_T("msec")) == 0 || unit.CmpNoCase(_T("milliseconds")) == 0)
		factor = 1000.0;
	else if (unit.CmpNoCase(_T("us")) == 0 || unit.CmpNoCase(_T("usec")) == 0 || unit.CmpNoCase(_T("microseconds")) == 0)
		factor = 1000000.0;
	else
	{
		// Assume a factor of 1
		factor = 1.0;
		return false;
	}

	return true;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetFFTData
//
// Description:		Returns a dataset containing an FFT of the specified data.
//
// Input Arguments:
//		data	= const LibPlot2D::Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D* pointing to a dataset contining the new FFT data
//
//==========================================================================
LibPlot2D::Dataset2D* MainFrame::GetFFTData(const LibPlot2D::Dataset2D* data)
{
	double factor;
	if (!GetXAxisScalingFactor(factor))
		// Warn the user if we cannot determine the time units, but create the plot anyway
		wxMessageBox(_T("Warning:  Unable to identify X-axis units!  Frequency may be incorrectly scaled!"),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	LibPlot2D::FFTDialog dialog(this, data->GetNumberOfPoints(),
		data->GetNumberOfZoomedPoints(plotArea->GetXMin(), plotArea->GetXMax()),
		data->GetAverageDeltaX() / factor);

	if (dialog.ShowModal() != wxID_OK)
		return NULL;

	if (!LibPlot2D::PlotMath::XDataConsistentlySpaced(*data))
		wxMessageBox(_T("Warning:  X-data is not consistently spaced.  Results may be unreliable."),
			_T("Accuracy Warning"), wxICON_WARNING, this);

	LibPlot2D::Dataset2D *newData;

	if (dialog.GetUseZoomedData())
		newData = new LibPlot2D::Dataset2D(LibPlot2D::FastFourierTransform::ComputeFFT(GetXZoomedDataset(*data),
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));
	else
		newData = new LibPlot2D::Dataset2D(LibPlot2D::FastFourierTransform::ComputeFFT(*data,
			dialog.GetFFTWindow(), dialog.GetWindowSize(), dialog.GetOverlap(),
			dialog.GetSubtractMean()));

	newData->MultiplyXData(factor);

	return newData;
}

//==========================================================================
// Class:			MainFrame
// Function:		GetXZoomedDataset
//
// Description:		Returns a dataset containing only the data within the
//					current zoomed x-limits.
//
// Input Arguments:
//		fullData	= const LibPlot2D::Dataset2D&
//
// Output Arguments:
//		None
//
// Return Value:
//		LibPlot2D::Dataset2D
//
//==========================================================================
LibPlot2D::Dataset2D MainFrame::GetXZoomedDataset(const LibPlot2D::Dataset2D &fullData) const
{
	unsigned int i, startIndex(0), endIndex(0);
	while (fullData.GetXData(startIndex) < plotArea->GetXMin() &&
		startIndex < fullData.GetNumberOfPoints())
		startIndex++;
	endIndex = startIndex;
	while (fullData.GetXData(endIndex) < plotArea->GetXMax() &&
		endIndex < fullData.GetNumberOfPoints())
		endIndex++;

	LibPlot2D::Dataset2D data(endIndex - startIndex);
	for (i = startIndex; i < endIndex; i++)
	{
		data.GetXPointer()[i - startIndex] = fullData.GetXData(i);
		data.GetYPointer()[i - startIndex] = fullData.GetYData(i);
	}

	return data;
}

//==========================================================================
// Class:			MainFrame
// Function:		SetMarkerSize
//
// Description:		Sets the marker size for the specified curve.
//
// Input Arguments:
//		curve	= const unsigned int&
//		size	= const int&
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void MainFrame::SetMarkerSize(const unsigned int &curve, const int &size)
{
	optionsGrid->SetCellValue(curve + 1, colMarkerSize, wxString::Format("%i", size));
	UpdateCurveProperties(curve);
}

//==========================================================================
// Class:			None
// Function:		TestSignalOperations
//
// Description:		Performs tests on signal operation classes.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
/*void TestSignalOperations()
{
	// Create test data:
	Dataset2D set1(500);
	unsigned int i;
	double dt = 0.01;
	for (i = 0; i < set1.GetNumberOfPoints(); i++)
		set1.GetXPointer()[i] = i * dt;

	double f1 = 5.0 * M_PI * 2.0;
	double f2 = 2.0 * M_PI * 2.0;

	Dataset2D set2(set1);
	Dataset2D set3(set1);
	Dataset2D set4(set1);
	Dataset2D set5(set1);
	double t;
	for (i = 0; i < set1.GetNumberOfPoints(); i++)
	{
		t = set1.GetXData(i);
		set1.GetYPointer()[i] = 2.0 * sin(f1 * t);
		set2.GetYPointer()[i] = 3.5 * sin(f2 * t);
		set3.GetYPointer()[i] = set1.GetYData(i) + set2.GetYData(i);
		set4.GetYPointer()[i] = set1.GetYData(i) * set2.GetYData(i);
		set5.GetYPointer()[i] = 3.0 + 0.45 * t;
	}

	// Save original data to file
	set1.ExportDataToFile(_T("set1.txt"));
	set2.ExportDataToFile(_T("set2.txt"));
	set3.ExportDataToFile(_T("set3.txt"));
	set4.ExportDataToFile(_T("set4.txt"));
	set5.ExportDataToFile(_T("set5.txt"));

	// Integral
	Dataset2D intTest1 = DiscreteIntegral::ComputeTimeHistory(set1);
	Dataset2D intTest2 = DiscreteIntegral::ComputeTimeHistory(set2);
	Dataset2D intTest5 = DiscreteIntegral::ComputeTimeHistory(set5);
	intTest1.ExportDataToFile(_T("integral set 1.txt"));
	intTest2.ExportDataToFile(_T("integral set 2.txt"));
	intTest5.ExportDataToFile(_T("integral set 5.txt"));

	// Derivative
	DiscreteDerivative::ComputeTimeHistory(set1).ExportDataToFile(_T("derivative set 1.txt"));
	DiscreteDerivative::ComputeTimeHistory(set2).ExportDataToFile(_T("derivative set 2.txt"));
	DiscreteDerivative::ComputeTimeHistory(intTest1).ExportDataToFile(_T("dofint1.txt"));

	// Root Mean Square
	Dataset2D rms1 = RootMeanSquare::ComputeTimeHistory(set1);
	Dataset2D rms2 = RootMeanSquare::ComputeTimeHistory(set2);
	Dataset2D rms3 = RootMeanSquare::ComputeTimeHistory(set3);
	Dataset2D rms4 = RootMeanSquare::ComputeTimeHistory(set4);
	rms1.ExportDataToFile(_T("rms1.txt"));
	rms2.ExportDataToFile(_T("rms2.txt"));
	rms3.ExportDataToFile(_T("rms3.txt"));
	rms4.ExportDataToFile(_T("rms4.txt"));

	// Fast Fourier Transform
	Dataset2D fft1 = FastFourierTransform::ComputeFFT(set1);
	Dataset2D fft2 = FastFourierTransform::ComputeFFT(set2);
	Dataset2D fft3 = FastFourierTransform::ComputeFFT(set3);
	Dataset2D fft4 = FastFourierTransform::ComputeFFT(set4);
	fft1.ExportDataToFile(_T("fft1.txt"));
	fft2.ExportDataToFile(_T("fft2.txt"));
	fft3.ExportDataToFile(_T("fft3.txt"));
	fft4.ExportDataToFile(_T("fft4.txt"));

	// Filters
	std::vector<double> num, den;
	double wc(5.0), z(1.0);
	num.push_back(wc * wc);
	den.push_back(1.0);
	den.push_back(2.0 * z * wc);
	den.push_back(wc * wc);
	Filter f(100.0, num, den);
	// Coefficients should be:
	// a[0] = 0.00059488399762046393
	// a[1] = 0.0011897679952409279
	// a[2] = 0.00059488399762046393
	// b[0] = -1.9024390243902431
	// b[1] = 0.90481856038072561

	num.clear();
	num.push_back(1.0);
	num.push_back(0.0);
	num.push_back(0.0);
	Filter g(100.0, num, den);
	// Coefficients should be:
	// a[0] = 0.95181439619274233
	// a[1] = -1.9036287923854847
	// a[2] = 0.95181439619274233
	// b[0] = -1.9024390243902431
	// b[1] = 0.90481856038072561
}//*/

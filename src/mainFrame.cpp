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
#include <lp2d/gui/plotListGrid.h>
#include <lp2d/renderer/plotRenderer.h>
#include <lp2d/utilities/guiUtilities.h>
#include <lp2d/parser/customFileFormat.h>
#include <lp2d/libPlot2D.h>

// *nix Icons
#ifdef __WXGTK__
#include "../res/icons/plots16.xpm"
#include "../res/icons/plots24.xpm"
#include "../res/icons/plots32.xpm"
#include "../res/icons/plots48.xpm"
#include "../res/icons/plots64.xpm"
#include "../res/icons/plots128.xpm"
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
const unsigned long long MainFrame::mHighQualityCurvePointLimit(10000);

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
	wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE), mPlotInterface(this)
{
	CreateControls();
	SetProperties();

	mPlotInterface.RegisterAllBuiltInFileTypes();

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
	lowerSizer->Add(new LibPlot2D::PlotListGrid(mPlotInterface, lowerPanel), 1, wxGROW | wxALL, 5);
	lowerPanel->SetSizer(lowerSizer);

	CreatePlotArea(splitter);
	splitter->SplitHorizontally(mPlotArea, lowerPanel, mPlotArea->GetSize().GetHeight());
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
	mPlotArea = new LibPlot2D::PlotRenderer(mPlotInterface, *parent, wxID_ANY, displayAttributes);

	mPlotArea->SetMinSize(wxSize(650, 320));
	mPlotArea->SetMajorGridOn();
	mPlotArea->SetCurveQuality(LibPlot2D::PlotRenderer::CurveQuality::HighWrite);

	return mPlotArea;
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
	buttonSizer->Add(CreateVersionText(parent));

	return buttonSizer;
}

//==========================================================================
// Class:			MainFrame
// Function:		CreateVersionText
//
// Description:		Builds a static text control containing version
//					information.
//
// Input Arguments:
//		parent	= wxWindow*
//
// Output Arguments:
//		None
//
// Return Value:
//		wxWindow*
//
//==========================================================================
wxWindow* MainFrame::CreateVersionText(wxWindow *parent)
{
	wxString dataPlotterVersionString(DataPlotterApp::versionString
		+ _T(" (") + DataPlotterApp::gitHash + _T(")"));
	wxString lp2dVersionString(LibPlot2D::versionString
		+ _T(" (") + LibPlot2D::gitHash + _T(")"));

	wxStaticText* versionInfo(new wxStaticText(parent, wxID_ANY,
		dataPlotterVersionString));
	versionInfo->SetToolTip(_T("DataPlotter ") + dataPlotterVersionString
		+ _T("\nLibPlot2D ") + lp2dVersionString);

	return versionInfo;
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
	mPlotInterface.SetApplicationTitle(DataPlotterApp::dataPlotterTitle);
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
	entries[0].Set(wxACCEL_CTRL, static_cast<int>('c'), idCopyEvent);
	entries[1].Set(wxACCEL_CTRL, static_cast<int>('v'), idPasteEvent);
	entries[2].Set(wxACCEL_CTRL, static_cast<int>('o'), idButtonOpen);
	entries[3].Set(wxACCEL_CTRL, static_cast<int>('a'), idButtonAutoScale);
	entries[4].Set(wxACCEL_CTRL, static_cast<int>('r'), idButtonRemoveCurve);
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
	EVT_BUTTON(idButtonOpen,			MainFrame::ButtonOpenClickedEvent)
	EVT_BUTTON(idButtonAutoScale,		MainFrame::ButtonAutoScaleClickedEvent)
	EVT_BUTTON(idButtonRemoveCurve,		MainFrame::ButtonRemoveCurveClickedEvent)
	EVT_BUTTON(idButtonReloadData,		MainFrame::ButtonReloadDataClickedEvent)
	EVT_MENU(idCopyEvent,				MainFrame::CopyEvent)
	EVT_MENU(idPasteEvent,				MainFrame::PasteEvent)
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

	mPlotInterface.LoadFiles(fileList);
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
	mPlotArea->AutoScale();
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
	mPlotInterface.RemoveSelectedCurves();
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
	mPlotInterface.ReloadData();
}

//==========================================================================
// Class:			MainFrame
// Function:		CopyEvent
//
// Description:		Event fires when user uses CTRL-C.
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
void MainFrame::CopyEvent(wxCommandEvent& WXUNUSED(event))
{
	mPlotInterface.Copy();
}

//==========================================================================
// Class:			MainFrame
// Function:		ButtonReloadDataClickedEvent
//
// Description:		Event fires when user uses CTRL-v.
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
void MainFrame::PasteEvent(wxCommandEvent& WXUNUSED(event))
{
	mPlotInterface.Paste();
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
	wxString fileName(LibPlot2D::GuiUtilities::ExtractFileNameFromPath(pathAndFileName));
	unsigned int end(fileName.find_last_of(_T(".")));
	SetTitle(fileName.Mid(0, end) + _T(" - ") + DataPlotterApp::dataPlotterTitle);
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

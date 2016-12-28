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

// LibPlot2D headers
#include <lp2d/gui/guiInterface.h>

// wxWidgets headers
#include <wx/wx.h>

// Standard C++ headers
#include <vector>

// wxWidgets forward declarations
class wxGrid;
class wxGridEvent;

// LibPlot2D forward declarations
namespace LibPlot2D
{
class PlotRenderer;
class PlotListGrid;
}

// The main frame class
class MainFrame : public wxFrame
{
public:
	MainFrame();
	~MainFrame() = default;

private:
	static const unsigned long long highQualityCurvePointLimit;

	LibPlot2D::GuiInterface plotInterface;

	// Functions that do some of the frame initialization and control positioning
	void CreateControls();
	void SetProperties();

	LibPlot2D::PlotRenderer* CreatePlotArea(wxWindow *parent);
	wxBoxSizer* CreateButtons(wxWindow *parent);
	wxWindow* CreateVersionText(wxWindow *parent);

	// Controls
	LibPlot2D::PlotRenderer *plotArea;

	// The event IDs
	enum MainFrameEventID
	{
		idButtonOpen = wxID_HIGHEST + 500,
		idButtonAutoScale,
		idButtonRemoveCurve,
		idButtonReloadData,

		idCopyEvent,
		idPasteEvent
	};

	// Button events
	void ButtonOpenClickedEvent(wxCommandEvent &event);
	void ButtonAutoScaleClickedEvent(wxCommandEvent &event);
	void ButtonRemoveCurveClickedEvent(wxCommandEvent &event);
	void ButtonReloadDataClickedEvent(wxCommandEvent &event);

	// Shortcut-only events
	void CopyEvent(wxCommandEvent &event);
	void PasteEvent(wxCommandEvent &event);

	void SetTitleFromFileName(wxString pathAndFileName);

	DECLARE_EVENT_TABLE();

	// Testing methods
	//void TestSignalOperations();
};

#endif// MAIN_FRAME_H_
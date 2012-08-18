/*===================================================================================
                                    DataPlotter
                          Copyright Kerry R. Loux 2011-2012

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  transferFunctionDialog.cpp
// Created:  8/15/2012
// Author:  K. Loux
// Description:  Dialog for selection of transfer function data
// History:

// Local headers
#include "application/transferFunctionDialog.h"

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		TransferFunctionDialog
//
// Description:		Constructor for TransferFunctionDialog class.
//
// Input Arguments:
//		parent		= wxWindow* that owns this object
//		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
TransferFunctionDialog::TransferFunctionDialog(wxWindow *parent, const wxArrayString &descriptions)
									 : wxDialog(parent, wxID_ANY, _T("Transfer Function"), wxDefaultPosition)
{
	CreateControls(descriptions);
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		CreateControls
//
// Description:		Populates the dialog with controls.
//
// Input Arguments:
//		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		None
//
//==========================================================================
void TransferFunctionDialog::CreateControls(const wxArrayString &descriptions)
{
	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	topSizer->Add(mainSizer, 0, wxALL | wxEXPAND, 5);

	mainSizer->Add(CreateSelectionControls(descriptions));
	mainSizer->Add(CreateButtons(), 1, wxGROW);

	// Set the sizer to this dialog
	SetSizerAndFit(topSizer);

	Center();
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		CreateSelectionControls
//
// Description:		Returns a sizer containing the selection controls.
//
// Input Arguments:
///		descriptions	= const wxArrayString& containing curve descriptions
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* TransferFunctionDialog::CreateSelectionControls(const wxArrayString &descriptions)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add(leftSizer, 1, wxGROW);
	sizer->Add(rightSizer, 1, wxGROW);

	wxStaticText *inputText = new wxStaticText(this, wxID_ANY, _T("Specify stimulus data:"));
	inputList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, descriptions, wxLB_SINGLE);
	leftSizer->Add(inputText);
	leftSizer->Add(inputList, 0, wxGROW | wxALL, 5);

	wxStaticText *outputText = new wxStaticText(this, wxID_ANY, _T("Specify response data:"));
	outputList = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, descriptions, wxLB_SINGLE);
	rightSizer->Add(outputText);
	rightSizer->Add(outputList, 0, wxGROW | wxALL, 5);

	return sizer;
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		CreateButtons
//
// Description:		Returns a sizer containing the button controls.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		wxSizer*
//
//==========================================================================
wxSizer* TransferFunctionDialog::CreateButtons(void)
{
	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton *okButton = new wxButton(this, wxID_OK, _T("OK"));
	wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _T("Cancel"));
	sizer->AddStretchSpacer();
	sizer->Add(okButton, 0, wxALIGN_RIGHT | wxALL, 5);
	sizer->Add(cancelButton, 0, wxALIGN_RIGHT | wxALL, 5);

	return sizer;
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		TransferDataFromWindow
//
// Description:		Validates selections (ensures there are selections).
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		bool, true if transfer is valid, false otherwise
//
//==========================================================================
bool TransferFunctionDialog::TransferDataFromWindow(void)
{
	if (inputList->GetSelection() == wxNOT_FOUND || outputList->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox(_T("Please select one stimulus signal and one response signal."),
			_T("Transfer Function"), wxICON_ERROR);
		return false;
	}

	return true;
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		GetInputIndex
//
// Description:		Returns the input data index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int TransferFunctionDialog::GetInputIndex(void) const
{
	return inputList->GetSelection();
}

//==========================================================================
// Class:			TransferFunctionDialog
// Function:		GetOutputIndex
//
// Description:		Returns the output data index.
//
// Input Arguments:
//		None
//
// Output Arguments:
//		None
//
// Return Value:
//		unsigned int
//
//==========================================================================
unsigned int TransferFunctionDialog::GetOutputIndex(void) const
{
	return outputList->GetSelection();
}
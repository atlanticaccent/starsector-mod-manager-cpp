#include "mmSettings.h"

mmSettings::mmSettings(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(400, 300)) {
	SetBackgroundColour(wxColour(*wxWHITE));
	wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* installDirLabel = new wxStaticText(this, wxID_ANY, "");
	installDirLabel->SetLabelMarkup("<b>Install Directory:</b>");
	installDir = new wxTextCtrl(this, wxID_ANY, "");
	wxButton* installDirBrowser = new wxButton(this, MM_INSTALL_DIR_BROWSE, "Browse");

	verticalSizer->Add(installDirLabel, 0, wxBOTTOM, 10);

	topSizer->Add(installDir, 1, wxEXPAND, 0);
	topSizer->Add(installDirBrowser, 0, wxBOTTOM | wxEXPAND, 0);

	verticalSizer->Add(topSizer, 1, wxEXPAND, 0);

	mainSizer->Add(verticalSizer, 1, wxALL | wxFIXED_MINSIZE, 10);

	this->SetSizer(mainSizer);
}

BEGIN_EVENT_TABLE(mmSettings, wxDialog)
	EVT_BUTTON(MM_INSTALL_DIR_BROWSE, mmSettings::onInstallDirectoryBrowse)
END_EVENT_TABLE()

void mmSettings::onInstallDirectoryBrowse(wxCommandEvent& event) {
	wxDirDialog d(this, "Choose a directory", "/", 0, wxDefaultPosition);

	if (d.ShowModal() == wxID_OK) {
		installDir->SetLabel(d.GetPath());
	}
}

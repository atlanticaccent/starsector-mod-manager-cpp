#include "mmSettings.h"

mmSettings::mmSettings(wxWindow* parent, mmConfig& config) : wxDialog(parent, wxID_ANY, "Settings", wxDefaultPosition, wxSize(400, 300)), config(config) {
	SetBackgroundColour(wxColour(*wxWHITE));
	mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* verticalSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* installDirLabel = new wxStaticText(this, wxID_ANY, "");
	installDirLabel->SetLabelMarkup("<b>Install Directory:</b>");
	installDir = new wxTextCtrl(this, MM_INSTALL_DIR, "");
	wxButton* installDirBrowser = new wxButton(this, MM_INSTALL_DIR_BROWSE, "Browse");

	verticalSizer->Add(installDirLabel, 0, wxBOTTOM, 5);

	topSizer->Add(installDir, 1, wxEXPAND);
	topSizer->Add(installDirBrowser, 0, wxEXPAND);

	verticalSizer->Add(topSizer, 0, wxBOTTOM | wxEXPAND, 5);
	verticalSizer->AddStretchSpacer();
	verticalSizer->Add(CreateButtonSizer(wxOK | wxCANCEL | wxAPPLY), 0, wxEXPAND);

	mainSizer->Add(verticalSizer, 1, wxALL | wxEXPAND, 10);
	this->SetSizer(mainSizer);

	installDir->ChangeValue(config["install_dir"].get<std::string>());
	wxWindow::FindWindowById(wxID_APPLY, this)->Enable(false);
}

BEGIN_EVENT_TABLE(mmSettings, wxDialog)
	EVT_BUTTON(MM_INSTALL_DIR_BROWSE, mmSettings::onInstallDirectoryBrowse)

	EVT_TEXT(MM_INSTALL_DIR, mmSettings::onChange)

	EVT_BUTTON(wxID_OK, mmSettings::onApply)
	EVT_BUTTON(wxID_APPLY, mmSettings::onApply)
END_EVENT_TABLE()

void mmSettings::onInstallDirectoryBrowse(wxCommandEvent& event) {
	wxDirDialog d(this, "Choose a directory", "/", 0, wxDefaultPosition);

	if (d.ShowModal() == wxID_OK) {
		installDir->SetValue(d.GetPath());
	}
}

void mmSettings::onChange(wxCommandEvent& event) {
	//wxMessageDialog(this, "firing").ShowModal();
	wxWindow::FindWindowById(wxID_APPLY, this)->Enable(true);
}

void mmSettings::onApply(wxCommandEvent& event) {
	if (fs::exists(installDir->GetValue().ToStdString())) {
		config["install_dir"] = installDir->GetValue();

		config.apply();
		wxWindow::FindWindowById(wxID_APPLY, this)->Enable(false);
	} else {
		wxMessageDialog error(this, "Directory does not exist");
		error.SetWindowStyle(wxICON_ERROR);
		error.ShowModal();
	}

	event.Skip();
}

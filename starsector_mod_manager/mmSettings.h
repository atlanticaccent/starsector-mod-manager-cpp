#ifndef MM_SETTINGS
#define MM_SETTINGS

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/dirdlg.h>

#include "mmConfig.h"

#define SETTINGS_ID wxID_HIGHEST + 100

class mmSettings : public wxDialog {
public:
	mmSettings(wxWindow* parent, mmConfig& config);

	void onInstallDirectoryBrowse(wxCommandEvent& event);

	void onChange(wxCommandEvent& event);

	void onApply(wxCommandEvent& event);

protected:
	DECLARE_EVENT_TABLE()

private:
	wxBoxSizer* mainSizer;
	wxTextCtrl* installDir;

	mmConfig& config;
};

enum {
	MM_INSTALL_DIR_BROWSE = SETTINGS_ID + 1,
	MM_INSTALL_DIR = SETTINGS_ID + 2
};

#endif // !MM_SETTINGS

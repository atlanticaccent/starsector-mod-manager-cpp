#ifndef MM_SETTINGS
#define MM_SETTINGS

#include <wx/wx.h>
#include <wx/button.h>
#include <wx/dirdlg.h>

#define SETTINGS_ID wxID_HIGHEST + 100

class mmSettings : public wxDialog {
public:
	mmSettings(wxWindow* parent);

	void onInstallDirectoryBrowse(wxCommandEvent& event);

protected:
	DECLARE_EVENT_TABLE()

private:
	wxTextCtrl* installDir;
};

enum {
	MM_INSTALL_DIR_BROWSE = SETTINGS_ID + 1
};

#endif // !MM_SETTINGS

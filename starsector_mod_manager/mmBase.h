#ifndef MM_BASE
#define MM_BASE

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/windowptr.h>
#include <wx/datetime.h>

#include "mmSettings.h"

#define BASE_ID wxID_HIGHEST

class mmBase : public wxFrame {
public:
	mmBase();

	void onSettings(wxCommandEvent& event);

	void onListContextMenuDisplay(wxCommandEvent& event);
	void onAddModClick(wxCommandEvent& event);
	void onRemoveModClick(wxCommandEvent& event);
	void onToggleAllClick(wxCommandEvent& event);
	void onListItemDataChange(wxDataViewEvent& event);

protected:
	DECLARE_EVENT_TABLE()

private:
	wxDataViewListCtrl* m_ctrl;
	wxMenuBar* m_pMenuBar;
	wxMenu* m_pFileMenu;

	mmSettings* settings;
};

enum {
	MM_ADD = BASE_ID + 1,
	MM_REMOVE = BASE_ID + 2,
	MM_TOGGLE_ALL = BASE_ID + 3,
	MM_DATA_LIST_CTRL = BASE_ID + 4,
	MM_SETTINGS_MENU = BASE_ID + 5
};

#endif // !MM_BASE

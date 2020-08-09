#ifndef MM_BASE
#define MM_BASE

#include <sstream>
#include <regex>
#include <memory>
#include <filesystem>

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/windowptr.h>
#include <wx/datetime.h>

#include "mmSettings.h"
#include "mmConfig.h"

#define BASE_ID wxID_HIGHEST
#define CURRENT_SS_VERSION "0.9.1a"

namespace fs = std::filesystem;

class mmBase : public wxFrame {
public:
	mmBase();

	bool getAllMods();

	void onSettings(wxCommandEvent& event);

	void onListContextMenuDisplay(wxCommandEvent& event);
	void onAddModClick(wxCommandEvent& event);
	void onAddModFolder(wxCommandEvent& event);
	void onRemoveModClick(wxCommandEvent& event);
	void onToggleAllClick(wxCommandEvent& event);
	void onListItemDataChange(wxDataViewEvent& event);
	void onListRowSelectionChange(wxDataViewEvent& event);

protected:
	DECLARE_EVENT_TABLE()

private:
	wxPanel* mainPane;
	wxBoxSizer* mainSizer;
	wxDataViewListCtrl* m_ctrl;
	wxButton* remove;
	wxMenuBar* m_pMenuBar;
	wxMenu* m_pFileMenu;
	wxMenu* addMenu;
	wxStaticText* mod_description;

	mmConfig config;
};

enum {
	MM_ADD = BASE_ID + 1,
	MM_REMOVE = BASE_ID + 2,
	MM_TOGGLE_ALL = BASE_ID + 3,
	MM_DATA_LIST_CTRL = BASE_ID + 4,
	MM_SETTINGS_MENU = BASE_ID + 5,
	MM_ADD_FOLDER = BASE_ID + 6,
	MM_ADD_ARCHIVE = BASE_ID + 7
};

#endif // !MM_BASE

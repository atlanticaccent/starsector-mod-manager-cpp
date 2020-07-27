#pragma once

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/windowptr.h>
#include <wx/datetime.h>

class mmBase : public wxFrame {
public:
	mmBase();

	void onListContextMenuDisplay(wxCommandEvent& event);
	void onAddModClick(wxCommandEvent& event);
	void onRemoveModClick(wxCommandEvent& event);
	void onToggleAllClick(wxCommandEvent& event);
	void onListItemDataChange(wxDataViewEvent& event);

protected:
	DECLARE_EVENT_TABLE()

private:
	wxDataViewListCtrl* m_ctrl;
};

enum {
	MM_ADD = wxID_HIGHEST + 1,
	MM_REMOVE = wxID_HIGHEST + 2,
	MM_TOGGLEALL = wxID_HIGHEST + 3,
	MM_LISTITEMDATACHANGE = wxID_HIGHEST + 4
};

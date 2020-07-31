#include "mmBase.h"

mmBase::mmBase() : wxFrame(nullptr, wxID_ANY, "Starsector Mod Manager", wxDefaultPosition, wxSize(800, 600)) {
    wxPanel* mainPane = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);

    m_ctrl = new wxDataViewListCtrl(mainPane, MM_DATA_LIST_CTRL);

    m_ctrl->AppendToggleColumn(wxT("Enabled"));
    m_ctrl->AppendTextColumn(wxT("Mod Name"));
    m_ctrl->AppendTextColumn(wxT("Version #"));
    m_ctrl->AppendTextColumn(wxT("Author"));
    m_ctrl->AppendTextColumn(wxT("Game Version #"));
    //m_ctrl->AppendTextColumn(wxT("Last Updated"));

    m_ctrl->Enable();

    topSizer->Add(m_ctrl, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* add = new wxButton(mainPane, MM_ADD, "Add Mod +");
    wxButton* remove = new wxButton(mainPane, MM_REMOVE, "Remove Mod -");
    wxButton* toggle = new wxButton(mainPane, MM_TOGGLE_ALL, "Toggle All");

    buttonSizer->Add(add, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    buttonSizer->Add(remove, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    buttonSizer->Add(toggle, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    topSizer->Add(buttonSizer, 0, wxALL, 5);

    mainSizer->Add(topSizer, 2, wxEXPAND | wxBOTTOM, 5);
    mainSizer->Add(bottomSizer, 1, wxRESERVE_SPACE_EVEN_IF_HIDDEN);

    mainPane->SetSizer(mainSizer);

    m_pMenuBar = new wxMenuBar();

    m_pFileMenu = new wxMenu();
    m_pFileMenu->Append(MM_SETTINGS_MENU, _T("&Settings"));
    m_pFileMenu->AppendSeparator();
    m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
    m_pMenuBar->Append(m_pFileMenu, _T("&File"));
    
    SetMenuBar(m_pMenuBar);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    config = mmConfig();
    config.initialise();

    getAllMods();
}

bool mmBase::getAllMods() {
    fs::path install_dir(config["install_dir"].get<std::string>());
    fs::path mods_dir = fs::path(install_dir) / "mods";

    if (config["install_dir"] == "" || !fs::is_directory(install_dir) || !fs::is_directory(mods_dir)) return false;

    json enabled;
    std::ifstream(fs::path(mods_dir) / "enabled_mods.json") >> enabled;

    bool parse_error = false;
    for (auto& mod : fs::directory_iterator(mods_dir)) {
        auto info = fs::path(mod.path()) / "mod_info.json";
        if (fs::exists(info)) {
            std::ifstream info_file_stream(info);
            std::stringstream buffer;
            buffer << info_file_stream.rdbuf();
            //std::string temp = buffer.str();
            std::string temp = std::regex_replace(buffer.str(), std::regex("#.*\\\n"), "\n");
            std::string info_string = std::regex_replace(temp, std::regex(",(\\s*[\\}\\]])"), "$1");
            if (!json::accept(info_string, true)) wxMessageDialog(this, info_string).ShowModal();

            try {
                json mod_info = json::parse(info_string);
                wxVector<wxVariant> values;
                values.push_back(std::find(enabled["enabledMods"].begin(), enabled["enabledMods"].end(), mod_info["id"]) != enabled["enabledMods"].end());
                values.push_back(mod_info.value("name", "N/A"));
                values.push_back(mod_info.value("version", "N/A"));
                values.push_back(mod_info.value("author", "N/A"));
                values.push_back(mod_info.value("gameVersion", "N/A"));

                auto meta_data = new std::pair<std::string, std::string>(mod_info["id"], mod_info.value("description", "N/A"));

                m_ctrl->AppendItem(values, (wxUIntPtr) meta_data);
            } catch (nlohmann::detail::type_error e) {
                parse_error = true;
            }
        }
    }
    m_ctrl->GetColumn(1)->SetWidth(wxCOL_WIDTH_AUTOSIZE);

    if (parse_error) wxMessageDialog(this, "There was an error parsing one or more mod_info.json files.").ShowModal();

    return true;
}

BEGIN_EVENT_TABLE(mmBase, wxFrame)
    EVT_MENU(MM_SETTINGS_MENU, mmBase::onSettings)

    EVT_DATAVIEW_ITEM_VALUE_CHANGED(MM_DATA_LIST_CTRL, mmBase::onListItemDataChange)
END_EVENT_TABLE()

void mmBase::onSettings(wxCommandEvent& event) {
    wxWindowPtr<mmSettings> settings(new mmSettings(this, config));

    settings->ShowWindowModalThenDo([this, settings](int retcode) {
        getAllMods();
    });
}

void mmBase::onListContextMenuDisplay(wxCommandEvent& event) {
}

void mmBase::onAddModClick(wxCommandEvent& event) {
}

void mmBase::onRemoveModClick(wxCommandEvent& event) {
}

void mmBase::onToggleAllClick(wxCommandEvent& event) {
}

void mmBase::onListItemDataChange(wxDataViewEvent& event) {
    int i = m_ctrl->ItemToRow(event.GetItem());

    fs::path enabled_mods_file = fs::path(config["install_dir"].get<std::string>()) / "mods" / "enabled_mods.json";
    json enabled;
    std::ifstream(enabled_mods_file) >> enabled;
    auto enabled_mods = enabled["enabledMods"];
    auto meta_data = (std::pair<std::string, std::string>*) m_ctrl->GetItemData(event.GetItem());

    bool already_active = std::find(enabled_mods.begin(), enabled_mods.end(), meta_data->first) != enabled_mods.end();
    if (m_ctrl->GetToggleValue(i, 0)) {
        if (!already_active) {
            if (m_ctrl->GetTextValue(i, 2) != CURRENT_SS_VERSION) {
                wxMessageDialog check(this, "This mod is for a different version of Starsector. \nContinue?", wxMessageBoxCaptionStr, wxCENTRE | wxCANCEL | wxYES_NO | wxNO_DEFAULT);
                if (check.ShowModal() != wxID_YES) {
                    m_ctrl->SetToggleValue(false, i, 0);
                    return;
                }
            }
            enabled["enabledMods"].push_back(meta_data->first);
        }
    } else {
        if (already_active) enabled["enabledMods"].erase(std::find(enabled["enabledMods"].begin(), enabled["enabledMods"].end(), meta_data->first));
    }

    std::ofstream(enabled_mods_file) << std::setw(4) << enabled;
}

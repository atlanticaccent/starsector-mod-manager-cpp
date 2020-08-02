#include "mmBase.h"

mmBase::mmBase() : wxFrame(nullptr, wxID_ANY, "Starsector Mod Manager", wxDefaultPosition, wxSize(800, 600)), config("config.json") {
    mainPane = new wxPanel(this);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

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
    remove = new wxButton(mainPane, MM_REMOVE, "Remove Mod -");
    //wxButton* toggle = new wxButton(mainPane, MM_TOGGLE_ALL, "Toggle All");

    remove->Enable(false);

    buttonSizer->Add(add, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    buttonSizer->Add(remove, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    //buttonSizer->Add(toggle, 0, wxEXPAND | wxBOTTOM | wxALIGN_TOP, 5);
    topSizer->Add(buttonSizer, 0, wxALL, 5);
    mainSizer->Add(topSizer, 2, wxEXPAND | wxBOTTOM, 5);

    mod_description = new wxStaticText(mainPane, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    mainSizer->Add(mod_description, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 10);

    mainPane->SetSizer(mainSizer);

    m_pMenuBar = new wxMenuBar();

    m_pFileMenu = new wxMenu();
    m_pFileMenu->Append(MM_SETTINGS_MENU, _T("&Settings"));
    m_pFileMenu->AppendSeparator();
    m_pFileMenu->Append(wxID_EXIT, _T("&Quit"));
    m_pMenuBar->Append(m_pFileMenu, _T("&File"));
    
    SetMenuBar(m_pMenuBar);

    addMenu = new wxMenu();
    addMenu->Append(MM_ADD_FOLDER, "From Folder");
    addMenu->Append(MM_ADD_ARCHIVE, "From Archive");

    Bind(wxEVT_MENU, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    json default_config = {
        {"starsector_mm", "Written by Iain Laird"},
        {"install_dir", ""}
    };
    config.init_or_create("starsector_mm", default_config);

    getAllMods();
}

bool mmBase::getAllMods() {
    fs::path install_dir(config["install_dir"].get<std::string>());
    fs::path mods_dir = install_dir / "mods";

    if (config["install_dir"] == "" || !fs::is_directory(mods_dir)) return false;

    mmConfig enabled(mods_dir / "enabled_mods.json");
    json enabled_default = json::parse("{\"enabledMods\":[]}");
    enabled.init_or_create("enabledMods", enabled_default);

    m_ctrl->DeleteAllItems();

    bool parse_error = false;
    for (auto& mod : fs::directory_iterator(mods_dir)) {
        auto info = fs::path(mod.path()) / "mod_info.json";
        mmConfig mod_info(info);
        
        if (mod_info.initialise()) {
            try {
                wxVector<wxVariant> values;
                values.push_back(std::find(enabled["enabledMods"].begin(), enabled["enabledMods"].end(), mod_info["id"]) != enabled["enabledMods"].end());
                values.push_back(mod_info.value("name", "N/A"));
                values.push_back(mod_info.value("version", "N/A"));
                values.push_back(mod_info.value("author", "N/A"));
                values.push_back(mod_info.value("gameVersion", "N/A"));

                auto meta_data = new std::tuple<std::string, std::string, fs::path>(
                    mod_info["id"],
                    mod_info.value("description", "N/A"),
                    mod.path()
                );

                m_ctrl->AppendItem(values, (wxUIntPtr) meta_data);
            } catch (detail::type_error& e) {
                parse_error = true;
            }
        }
    }
    m_ctrl->GetColumn(1)->SetWidth(wxCOL_WIDTH_AUTOSIZE);

    if (parse_error) wxLogError("There was an error parsing one or more mod_info.json files.");

    return true;
}

// Credit to https://wiki.wxwidgets.org/WxZipInputStream and https://docs.wxwidgets.org/trunk/overview_archive.html#overview_archive_generic
// for the basis of this function.
bool mmBase::decompressArchiveTo(fs::path archive, fs::path targetDir) {
    wxInputStream* in = new wxFFileInputStream(archive.string());

    if (in->IsOk()) {
        // look for a filter handler, e.g. for '.gz'
        const wxFilterClassFactory* fcf = wxFilterClassFactory::Find(archive.string(), wxSTREAM_FILEEXT);
        // pop the extension, so if it was '.tar.gz' it is now just '.tar'
        if (fcf) archive.replace_extension("");
        // look for a archive handler, e.g. for '.zip' or '.tar'
        const wxArchiveClassFactory* acf = wxArchiveClassFactory::Find(archive.string(), wxSTREAM_FILEEXT);
        if (acf) {
            wxArchiveInputStream* arc = acf->NewStream(in);
            std::unique_ptr<wxArchiveEntry> entry;
            // list the contents of the archive
            while ((entry.reset(arc->GetNextEntry())), entry.get() != NULL) {
                auto fname = targetDir / entry->GetName().ToStdString();

                if (entry->IsDir()) {
                    if (!fs::exists(fname)) {
                        std::error_code ec;
                        fs::create_directory(fname, ec);
                        if (ec.value() != 0) {
                            wxLogError(_(ec.message()));
                        }
                    }
                } else if (arc->CanRead()) {
                    wxFileOutputStream fos(fname.string());

                    if (fos.IsOk()) arc->Read(fos);
                    else {
                        wxLogError("Couldn't create the file '%s'.", fname.string());
                        return false;
                    }
                } else {
                    // Read 'zis' to access the 'upZe's' data.
                    wxLogError("Couldn't read the zip entry '%s'.", entry->GetName());
                    return false;
                }
            }
        } else {
            wxLogError("Can't handle '%s'", archive.string());
            return false;
        }
    }

    getAllMods();
    return true;
}

BEGIN_EVENT_TABLE(mmBase, wxFrame)
    EVT_MENU(MM_SETTINGS_MENU, mmBase::onSettings)

    EVT_BUTTON(MM_ADD, mmBase::onAddModClick)
    EVT_MENU(MM_ADD_FOLDER, mmBase::onAddModFolder)
    EVT_MENU(MM_ADD_ARCHIVE, mmBase::onAddModArchive)

    EVT_BUTTON(MM_REMOVE, mmBase::onRemoveModClick)

    EVT_DATAVIEW_ITEM_VALUE_CHANGED(MM_DATA_LIST_CTRL, mmBase::onListItemDataChange)
    EVT_DATAVIEW_SELECTION_CHANGED(MM_DATA_LIST_CTRL, mmBase::onListRowSelectionChange)
END_EVENT_TABLE()

void mmBase::onSettings(wxCommandEvent& event) {
    wxWindowPtr<mmSettings> settings(new mmSettings(this, config));

    settings->ShowWindowModalThenDo([this, settings](int _) {
        getAllMods();
    });
}

void mmBase::onListContextMenuDisplay(wxCommandEvent& event) {
}

void mmBase::onAddModClick(wxCommandEvent& event) {
    auto button = (wxButton*) event.GetEventObject();
    auto bottomLeftCoord = button->GetRect().GetBottomLeft();

    this->PopupMenu(addMenu, bottomLeftCoord);
}

void mmBase::onAddModFolder(wxCommandEvent& event) {
    wxDirDialog d(this, "Choose a directory");

    if (d.ShowModal() == wxID_OK) {
        fs::path mod_dir(d.GetPath().ToStdString());
        if (fs::exists(mod_dir / "mod_info.json")) {
            try {
                //#ifdef __linux__
                //fs::rename(mod_dir, fs::path(config["install_dir"].get<std::string>()) / "mods" / mod_dir.filename());
                //#elif _WIN32
                //a = MoveFile(mod_dir.c_str(), (fs::path(config["install_dir"].get<std::string>()) / "mods" / mod_dir.filename()).c_str());
                //#endif
                fs::copy(mod_dir, fs::path(config["install_dir"].get<std::string>()) / "mods" / mod_dir.filename(), fs::copy_options::recursive);
                fs::remove_all(mod_dir);

                getAllMods();

                std::string mod_name;
                int i = 0;
                for (; i < m_ctrl->GetItemCount(); i++) {
                    mmConfig info(fs::path(config["install_dir"].get<std::string>()) / "mods" / mod_dir.filename() / "mod_info.json");
                    info.initialise();
                    auto item_id = std::get<0>(*(std::tuple<std::string, std::string, fs::path>*) m_ctrl->GetItemData(m_ctrl->RowToItem(i)));
                    if (item_id == info["id"]) {
                        mod_name = info["name"];
                        break;
                    }
                }

                wxMessageDialog check(this, "Activate '" + mod_name + "'?", wxMessageBoxCaptionStr, wxCENTRE | wxCANCEL | wxYES_NO);
                if (check.ShowModal() == wxID_YES) {
                    m_ctrl->SetToggleValue(true, i, 0);
                    return;
                }
            } catch (std::filesystem::filesystem_error e) {
                wxMessageDialog(this, e.what()).ShowModal();
            }
        } else wxMessageDialog(this, "Could not find 'mod_info.json' in the top level of the folder. \nMaybe check its subdirectories?").ShowModal();
    }
}

void mmBase::onAddModArchive(wxCommandEvent& event) {
    wxFileDialog d(this, "Choose an archive");

    if (d.ShowModal() == wxID_OK) {
        auto archive = fs::path(d.GetPath().ToStdString());
        decompressArchiveTo(
            archive,
            fs::path(config["install_dir"].get<std::string>()) / "mods"
        );
    }
}

void mmBase::onRemoveModClick(wxCommandEvent& event) {
    int i = m_ctrl->GetSelectedRow();
    if (i == wxNOT_FOUND) return;

    auto name = m_ctrl->GetTextValue(i, 1);
    wxMessageDialog check(this, "Are you sure you want to delete '" + name + "'? \nThis cannot be undone!", wxMessageBoxCaptionStr, wxCENTRE | wxCANCEL | wxYES_NO | wxNO_DEFAULT);
    if (check.ShowModal() == wxID_YES) {
        auto meta_data = *(std::tuple<std::string, std::string, fs::path>*) m_ctrl->GetItemData(m_ctrl->RowToItem(m_ctrl->GetSelectedRow()));
        auto mod_dir = std::get<2>(meta_data);
        fs::remove_all(mod_dir);

        json enabled = json::parse(std::ifstream(fs::path(config["install_dir"].get<std::string>()) / "mods" / "enabled_mods.json"));
        auto exists_iter = std::find(enabled["enabledMods"].begin(), enabled["enabledMods"].end(), std::get<0>(meta_data));
        if (m_ctrl->GetToggleValue(i, 0)) {
            if (exists_iter != enabled["enabledMods"].end()) {
                enabled["enabledMods"].erase(exists_iter);
                std::ofstream(fs::path(config["install_dir"].get<std::string>()) / "mods" / "enabled_mods.json") << std::setw(4) << enabled;
            }
        }
        m_ctrl->DeleteItem(i);
        getAllMods();
    }
}

void mmBase::onListItemDataChange(wxDataViewEvent& event) {
    int i = m_ctrl->ItemToRow(event.GetItem());

    fs::path enabled_mods_file = fs::path(config["install_dir"].get<std::string>()) / "mods" / "enabled_mods.json";
    json enabled;
    std::ifstream(enabled_mods_file) >> enabled;
    auto meta_data = *(std::tuple<std::string, std::string, fs::path>*) m_ctrl->GetItemData(event.GetItem());

    auto exists_iter = std::find(enabled["enabledMods"].begin(), enabled["enabledMods"].end(), std::get<0>(meta_data));
    if (m_ctrl->GetToggleValue(i, 0)) {
        if (exists_iter == enabled["enabledMods"].end()) {
            if (m_ctrl->GetTextValue(i, 4) != CURRENT_SS_VERSION) {
                wxMessageDialog check(this, "This mod may be for a different version of Starsector. \nContinue?", wxMessageBoxCaptionStr, wxCENTRE | wxCANCEL | wxYES_NO | wxNO_DEFAULT);
                if (check.ShowModal() != wxID_YES) {
                    m_ctrl->SetToggleValue(false, i, 0);
                    return;
                }
            }
            enabled["enabledMods"].push_back(std::get<0>(meta_data));
        }
    } else if (exists_iter != enabled["enabledMods"].end()) {
        enabled["enabledMods"].erase(exists_iter);
    }

    std::ofstream(enabled_mods_file) << std::setw(4) << enabled;
}

void mmBase::onListRowSelectionChange(wxDataViewEvent& event) {
    remove->Enable(true);

    auto meta_data = *(std::tuple<std::string, std::string, fs::path>*) m_ctrl->GetItemData(event.GetItem());

    mod_description->SetLabel(std::get<1>(meta_data));
    mainSizer->Layout();
}

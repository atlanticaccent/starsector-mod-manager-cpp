#include "mmApp.h"

wxIMPLEMENT_APP(mmApp);

bool mmApp::OnInit() {
    mmBase* base = new mmBase();
    base->Show(true);
    
    return true;
}

// This file is part of fityk program. Copyright (C) Marcin Wojdyr

// wxwindows headers, see wxwindows samples for description
#ifdef __GNUG__
#pragma implementation
#endif
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "common.h"
RCSID ("$Id$")

#include <wx/laywin.h>
#include <wx/splitter.h>
#include <wx/filedlg.h>
#include <wx/resource.h>
#include <wx/valtext.h>
#include <wx/textdlg.h>
#if wxUSE_TOOLTIPS
    #include <wx/tooltip.h>
#endif
#include <wx/cmdline.h> 
#include <wx/tipdlg.h>
#include <wx/statline.h>
#include <wx/html/htmlwin.h>
#include <wx/fs_zip.h>
#include <wx/printdlg.h>
#include <wx/image.h>
#include <wx/config.h>
#include <algorithm>

#include "wx_plot.h"
#include "wx_gui.h"
#include "wx_dlg.h"
#include "wx_pane.h"
#include "other.h"
#include "v_fit.h"
#include "data.h"
#include "sum.h"
#include "pcore.h"
#include "fzgbase.h" 
#include "ffunc.h"
#include "manipul.h"
#include "v_IO.h"
#ifdef USE_XTAL
    #include "crystal.h"
#endif //USE_XTAL

#ifndef __WXMSW__
#include "img/fityk.xpm"
#endif

//toolbars icons
#include "img/clear.xpm"     
#include "img/plusbg.xpm"  
//#include "img/spline.xpm"
#include "img/autoadd.xpm"     
#include "img/tree.xpm" 
#include "img/zoom_t.xpm"
#include "img/range_t.xpm"
#include "img/baseline_t.xpm"
#include "img/addpeak_t.xpm"
#include "img/manual.xpm"
#include "img/openfile.xpm"
#include "img/fitrun.xpm"
#include "img/fitcont.xpm"
#include "img/fitundo.xpm"
#include "img/execute.xpm"
#include "img/log.xpm"
#include "img/reset.xpm"
#include "img/dump.xpm"
#include "img/whole.xpm"
#include "img/vertic.xpm"
#include "img/leftscroll.xpm"
#include "img/rightscroll.xpm"
#include "img/backv.xpm"
#include "img/dpane.xpm"
#include "img/mouse_l.xpm"
#include "img/mouse_r.xpm"

#ifndef VERSION
#   define VERSION "unknown"
#endif

char* app_name = "fityk"; 
char* about_html = 
"<html> <body bgcolor=white text=white>                                  "
"  <table cellspacing=3 cellpadding=4 width=100%>                        "
"   <tr bgcolor=#101010 align=center> <td>                               "
"      <h1> fityk </h1>                                                  "
"      <h3> ver. " VERSION "</h3>                                        "
"      <h6> $Date$</h6>                            "
"   </td> </tr>                                                          "
"   <tr> <td bgcolor=#73A183>                                            "
"     <font color=black>                                                 "
"      <b><font size=+1> <p align = center>                              "
"       Copyright (C) 2001 - 2004 Marcin Wojdyr                          "
"      </font></b><p>                                                    "
"      <font size=-1>                                                    "
"       This program is free software; you can redistribute it           "
"       and/or modify it under the terms of the GNU General Public       "
"       License, version 2, as published by the Free Software Foundation;"
"      </font> <p>                                                       "
"       Feel free to send comments, questions, patches, bugreports       "
"       and feature requests to author                                   "
"       or to <i>fityk-users</i> mailing-list.                           "
"      <p align = center> WWW: <b>fityk.sf.net</b>                       "
"     </font>                                                            "
"   </td> </tr>                                                          "
"</table> </body> </html>                                                ";

using namespace std;
FFrame *frame = NULL;
std::vector<fp> params4plot; 
gui_IO my_gui_IO;

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, _T("h"), _T("help"), "show this help message",
                                wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose", wxCMD_LINE_VAL_NONE, 0 },
    { wxCMD_LINE_SWITCH, "q", "quiet",   "be quiet", wxCMD_LINE_VAL_NONE, 0 },
    { wxCMD_LINE_OPTION, "o", "output", "output file", wxCMD_LINE_VAL_NONE, 0 },
    { wxCMD_LINE_PARAM,  0, 0, "input file", wxCMD_LINE_VAL_STRING, 
                        wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE },
    { wxCMD_LINE_NONE, 0, 0, 0,  wxCMD_LINE_VAL_NONE, 0 }
};  

enum {
    ID_QUIT             = 44001,
    ID_H_MANUAL                ,
    ID_H_TIP                   ,
    ID_D_LOAD                  ,
    ID_D_XLOAD                 ,
    ID_D_RECENT         = 44010, //and next ones
    ID_D_INFO           = 44130,
    ID_D_DEVIATION             ,
    ID_D_RANGE                 ,
    ID_D_B_INFO                ,
    ID_D_B_CLEAR               ,
    ID_D_BACKGROUND            ,
    ID_D_CALIBRATE             ,
    ID_D_EXPORT                ,
    ID_D_SET                   ,
    ID_S_ADD                   ,
    ID_S_HISTORY               ,
    ID_S_INFO                  ,
    ID_S_REMOVE                ,
    ID_S_CHANGE                ,
    ID_S_VALUE                 ,
    ID_S_EXPORT                ,
    ID_S_SET                   ,
    ID_M_FINDPEAK              ,
    ID_M_SET                   ,
    ID_F_METHOD                ,
    ID_F_RUN                   ,
    ID_F_CONTINUE              ,
    ID_F_INFO                  ,
    ID_F_SET                   ,
    ID_F_M             = 44160 , // and a few next IDs
    ID_C_WAVELENGTH    = 44171 ,
    ID_C_ADD                   ,
    ID_C_INFO                  ,
    ID_C_REMOVE                ,
    ID_C_ESTIMATE              ,
    ID_C_SET                   ,
    ID_O_PLOT                  ,
    ID_O_LOG                   ,
    ID_O_RESET                 ,
    ID_PRINT                   ,
    ID_PRINT_SETUP             ,
    ID_PRINT_PREVIEW           ,
    ID_O_INCLUDE               ,
    ID_O_REINCLUDE             ,
    ID_O_WAIT                  ,
    ID_O_DUMP                  ,
    ID_O_SET                   ,
    ID_G_MODE                  ,
    ID_G_M_ZOOM                ,
    ID_G_M_RANGE               ,
    ID_G_M_BG                  ,
    ID_G_M_ADD                 ,
    ID_G_M_PEAK                ,
    ID_G_M_PEAK_N      = 44220 ,
    ID_G_SHOW          = 44260 ,
    ID_G_S_TOOLBAR             ,
    ID_G_S_STATBAR             ,
    ID_G_S_DPANE               ,
    ID_G_S_IO                  ,
    ID_G_V_ALL                 ,
    ID_G_V_VERT                ,
    ID_G_V_SCROLL_L            ,
    ID_G_V_SCROLL_R            ,
    ID_G_V_ZOOM_PREV   = 44302 ,
    ID_G_LCONF1        = 44342 ,
    ID_G_LCONF2                ,
    ID_G_LCONFB                ,
    ID_G_SCONF                 ,
    ID_G_SCONF1                ,
    ID_G_SCONF2                ,

    ID_ft_m_zoom               ,
    ID_ft_m_range              ,
    ID_ft_m_bg                 ,
    ID_ft_m_add                ,
    ID_ft_v_pr                 ,
    ID_ft_b_with               ,
    ID_ft_f_run                ,
    ID_ft_f_cont               ,
    ID_ft_f_undo               ,
    ID_ft_s_aa                 ,
    ID_ft_dpane                ,
    ID_ft_peakchoice
};

IMPLEMENT_APP(FApp)
string get_full_path_of_help_file (const string &name);

bool FApp::OnInit(void)
{
    AL = new ApplicationLogic; 

    //Parsing command line
    wxCmdLineParser cmdLineParser(cmdLineDesc, argc, argv);
    if (cmdLineParser.Parse(false) != 0) {
        cmdLineParser.Usage();
        return false;
    }
    if (cmdLineParser.Found("v"))
        verbosity++;
    if (cmdLineParser.Found("q"))
        verbosity--;
    wxString fityk_dir = wxGetHomeDir() + wxFILE_SEP_PATH + config_dirname;
    if (!wxDirExists(fityk_dir))
        wxMkdir(fityk_dir);
    wxString startup_file_path = fityk_dir + wxFILE_SEP_PATH 
                                + startup_commands_filename;
    if (wxFileExists(startup_file_path)) {
        exec_commands_from_file(startup_file_path.c_str());
    }
    for (unsigned int i = 0; i < cmdLineParser.GetParamCount(); i++) {
        wxString par = cmdLineParser.GetParam(i);
        exec_commands_from_file(par.c_str());
    }

    //global settings
#if wxUSE_TOOLTIPS
    wxToolTip::Enable (true);
    wxToolTip::SetDelay (500);
#endif
    wxConfig::DontCreateOnDemand();
    wxString pre = wxString(config_dirname) + wxFILE_SEP_PATH;
    conf_filename = pre + "config";
    alt_conf_filename = pre + "alt-config";
    wxConfigBase *config = new wxConfig("", "", pre+"wxoptions", "", 
                                        wxCONFIG_USE_LOCAL_FILE);
    wxConfig::Set (config);

    // Create the main frame window
    frame = new FFrame(NULL, -1, app_name, wxDEFAULT_FRAME_STYLE);

    frame->plot_pane->set_mouse_mode(mmd_zoom);
    clear_buffered_sum();
    wxConfigBase *cf = new wxConfig("", "", conf_filename, "", 
                                    wxCONFIG_USE_LOCAL_FILE);
    frame->read_all_settings(cf);
    frame->Show(true);
    frame->io_pane->read_settings(cf); //it does not work earlier (wxGTK)
    delete cf;
    SetTopWindow(frame);
    my_IO = &my_gui_IO;

    if (read_bool_from_config(wxConfig::Get(), "/TipOfTheDay/ShowAtStartup", 
                              false)) {
        frame->OnTipOfTheDay(dummy_cmd_event);
    }

    return true;
}

int FApp::OnExit()
{ 
    delete AL; 
    delete wxConfigBase::Set((wxConfigBase *) NULL);
    return 0;
}

BEGIN_EVENT_TABLE(FFrame, wxFrame)
    EVT_IDLE (FFrame::OnIdle)

    EVT_MENU (ID_D_LOAD,        FFrame::OnDLoad)   
    EVT_MENU (ID_D_XLOAD,       FFrame::OnDXLoad)   
    EVT_MENU_RANGE (ID_D_RECENT+1, ID_D_RECENT+100, FFrame::OnDRecent)
    EVT_MENU (ID_D_INFO,        FFrame::OnDInfo)
    EVT_MENU (ID_D_DEVIATION  , FFrame::OnDDeviation)  
    EVT_MENU (ID_D_RANGE,       FFrame::OnDRange)
    EVT_MENU (ID_D_B_INFO,      FFrame::OnDBInfo)
    EVT_MENU (ID_D_B_CLEAR,     FFrame::OnDBClear)
    EVT_MENU (ID_D_CALIBRATE,   FFrame::OnDCalibrate)  
    EVT_MENU (ID_D_EXPORT,      FFrame::OnDExport) 
    EVT_MENU (ID_D_SET,         FFrame::OnDSet) 

    EVT_MENU (ID_S_HISTORY,     FFrame::OnSHistory)    
    EVT_MENU (ID_S_INFO,        FFrame::OnSInfo)    
    EVT_MENU (ID_S_ADD,         FFrame::OnSAdd)    
    EVT_MENU (ID_S_REMOVE,      FFrame::OnSRemove)    
    EVT_MENU (ID_S_CHANGE,      FFrame::OnSChange)    
    EVT_MENU (ID_S_VALUE,       FFrame::OnSValue)    
    EVT_MENU (ID_S_EXPORT,      FFrame::OnSExport)    
    EVT_MENU (ID_S_SET,         FFrame::OnSSet)    

    EVT_MENU (ID_M_FINDPEAK,    FFrame::OnMFindpeak)
    EVT_MENU (ID_M_SET,         FFrame::OnMSet)

    EVT_MENU (ID_F_METHOD,      FFrame::OnFMethod)    
    EVT_UPDATE_UI (ID_F_METHOD, FFrame::OnFMethod)
    EVT_MENU_RANGE (ID_F_M+0, ID_F_M+8, FFrame::OnFOneOfMethods)    
    EVT_MENU (ID_F_RUN,         FFrame::OnFRun)    
    EVT_MENU (ID_F_CONTINUE,    FFrame::OnFContinue)    
    EVT_MENU (ID_F_INFO,        FFrame::OnFInfo)    
    EVT_MENU (ID_F_SET,         FFrame::OnFSet)    

#ifdef USE_XTAL
    EVT_MENU (ID_C_WAVELENGTH,  FFrame::OnCWavelength)    
    EVT_MENU (ID_C_ADD,         FFrame::OnCAdd)    
    EVT_MENU (ID_C_INFO,        FFrame::OnCInfo)    
    EVT_MENU (ID_C_REMOVE,      FFrame::OnCRemove)    
    EVT_MENU (ID_C_ESTIMATE,    FFrame::OnCEstimate)    
    EVT_MENU (ID_C_SET,         FFrame::OnCSet)    
#endif

    EVT_MENU (ID_O_PLOT,        FFrame::OnOPlot)    
    EVT_MENU (ID_O_LOG,         FFrame::OnOLog)    
    EVT_MENU (ID_O_RESET,       FFrame::OnO_Reset)    
    EVT_MENU (ID_O_INCLUDE,     FFrame::OnOInclude)    
    EVT_MENU (ID_O_REINCLUDE,   FFrame::OnOReInclude)    
    EVT_MENU (ID_PRINT,         FFrame::OnPrint)
    EVT_MENU (ID_PRINT_SETUP,   FFrame::OnPrintSetup)
    EVT_MENU (ID_PRINT_PREVIEW, FFrame::OnPrintPreview)
    EVT_MENU (ID_O_WAIT,        FFrame::OnOWait)    
    EVT_MENU (ID_O_DUMP,        FFrame::OnODump)    
    EVT_MENU (ID_O_SET,         FFrame::OnOSet)    

    EVT_MENU (ID_G_M_ZOOM,      FFrame::OnChangeMouseMode)
    EVT_MENU (ID_G_M_RANGE,     FFrame::OnChangeMouseMode)
    EVT_MENU (ID_G_M_BG,        FFrame::OnChangeMouseMode)
    EVT_MENU (ID_G_M_ADD,       FFrame::OnChangeMouseMode)
    EVT_MENU (ID_G_M_PEAK,      FFrame::OnModePeak)
    EVT_UPDATE_UI (ID_G_M_PEAK, FFrame::OnModePeak)
    EVT_MENU_RANGE (ID_G_M_PEAK_N+0, ID_G_M_PEAK_N+30, FFrame::OnChangePeakType)
    EVT_MENU (ID_G_S_DPANE,     FFrame::OnSwitchDPane)
    EVT_MENU (ID_G_S_IO,        FFrame::OnSwitchIOPane)
    EVT_MENU (ID_G_S_TOOLBAR,   FFrame::OnSwitchToolbar)
    EVT_MENU (ID_G_S_STATBAR,   FFrame::OnSwitchStatbar)
    EVT_MENU (ID_G_V_ALL,       FFrame::OnGViewAll)
    EVT_MENU (ID_G_V_VERT,      FFrame::OnGFitHeight)
    EVT_MENU (ID_G_V_SCROLL_L,  FFrame::OnGScrollLeft)
    EVT_MENU (ID_G_V_SCROLL_R,  FFrame::OnGScrollRight)
    EVT_MENU (ID_G_V_ZOOM_PREV, FFrame::OnShowMenuZoomPrev)
    EVT_UPDATE_UI (ID_G_V_ZOOM_PREV, FFrame::OnShowMenuZoomPrev)
    EVT_MENU_RANGE (ID_G_V_ZOOM_PREV+1, ID_G_V_ZOOM_PREV+20, 
                                FFrame::OnPreviousZoom)    
    EVT_MENU (ID_G_LCONF1,      FFrame::OnConfigRead)
    EVT_MENU (ID_G_LCONF2,      FFrame::OnConfigRead)
    EVT_MENU (ID_G_LCONFB,      FFrame::OnConfigBuiltin)
    EVT_MENU (ID_G_SCONF1,      FFrame::OnConfigSave)
    EVT_MENU (ID_G_SCONF2,      FFrame::OnConfigSave)

    EVT_MENU (ID_H_MANUAL,      FFrame::OnShowHelp)
    EVT_MENU (ID_H_TIP,         FFrame::OnTipOfTheDay)
    EVT_MENU (wxID_ABOUT,       FFrame::OnAbout)
    EVT_MENU (ID_QUIT,          FFrame::OnQuit)

//  EVT_SASH_DRAGGED_RANGE (ID_WINDOW_TOP1, ID_WINDOW_BOTTOM, 
//                            FFrame::OnSashDrag)
END_EVENT_TABLE()

    // Define my frame constructor
FFrame::FFrame(wxWindow *parent, const wxWindowID id, const wxString& title, 
                 const long style)
    : wxFrame(parent, id, title, wxDefaultPosition, wxDefaultSize, style), 
      main_pane(0), data_pane(0), status_bar(0), 
      peak_type_nr(0), toolbar(0), dxload_dialog(0), 
      print_data(new wxPrintData), page_setup_data(new wxPageSetupData),
#ifdef __WXMSW__
      help()
#else
      help(wxHF_TOOLBAR|wxHF_CONTENTS|wxHF_INDEX|wxHF_SEARCH|wxHF_PRINT
           |wxHF_MERGE_BOOKS)
#endif
{
    // Load icon and bitmap
    SetIcon (wxICON (fityk));

    //sizer, splitters, etc.
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    v_splitter = new ProportionalSplitter(this, -1, 0.8);
    main_pane = new ProportionalSplitter(v_splitter, -1, 0.7);
    plot_pane = new PlotPane(main_pane);
    io_pane = new IOPane(main_pane);
    main_pane->SplitHorizontally(plot_pane, io_pane);
    data_pane = new DataPane(v_splitter);
    data_pane->Show(false);
    v_splitter->Initialize(main_pane);
    sizer->Add(v_splitter, 1, wxEXPAND, 0);

    read_recent_data_files();
    set_menubar();

    toolbar = new FToolBar(this, -1);
    SetToolBar(toolbar);

    //status bar
    status_bar = new FStatusBar(this);
    SetStatusBar(status_bar);

    SetAutoLayout (true);
    SetSizer(sizer);
    sizer->Fit(this);
    sizer->SetSizeHints(this);


    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxImage::AddHandler(new wxPNGHandler);
    string help_file = "fitykhelp.htb";
    string help_path = get_full_path_of_help_file(help_file); 
    string help_path_no_exten = help_path.substr(0, help_path.size() - 4);
    help.Initialize(help_path_no_exten.c_str());
    //bool r = help.AddBook(help_path.c_str());
    //if (!r)
    //    wxMessageBox(("Couldn't open help file:\n " + help_path).c_str());
    // help.UseConfig(    );
    // help.SetTempDir(    );
    focus_input();
}

FFrame::~FFrame() 
{
    write_recent_data_files();
    delete print_data;
    delete page_setup_data;
}

void FFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}


void FFrame::read_recent_data_files()
{
    recent_data_files.clear();
    wxConfigBase *c = wxConfig::Get();
    if (c && c->HasGroup("/RecentDataFiles")) {
        for (int i = 0; i < 20; i++) {
            wxString key = wxString("/RecentDataFiles/") + S(i).c_str();
            if (c->HasEntry(key))
                recent_data_files.push_back(wxFileName(c->Read(key, "")));
        }
    }
}

void FFrame::write_recent_data_files()
{
    wxConfigBase *c = wxConfig::Get();
    wxString group("/RecentDataFiles");
    if (c->HasGroup(group))
        c->DeleteGroup(group);
    int counter = 0;
    for (list<wxFileName>::const_iterator i = recent_data_files.begin(); 
         i != recent_data_files.end() && counter < 9; 
         i++, counter++) {
        wxString key = group + "/" + S(counter).c_str();
        c->Write(key, i->GetFullPath());
    }
}

void FFrame::add_recent_data_file(wxString filename)
{
    const int count = data_menu_recent->GetMenuItemCount();
    const wxMenuItemList& mlist = data_menu_recent->GetMenuItems();
    const wxFileName fn = wxFileName(filename);
    recent_data_files.remove(filename);
    recent_data_files.push_front(fn);
    int id_new = 0;
    for (wxMenuItemList::Node *i = mlist.GetFirst(); i; i = i->GetNext()) 
        if (i->GetData()->GetHelp() == fn.GetFullPath()) {
            id_new = i->GetData()->GetId();
            data_menu_recent->Delete(i->GetData());
            break;
        }
    if (id_new == 0) {
        if (count >= 15) {
            wxMenuItem *item = mlist.GetLast()->GetData();
            id_new = item->GetId();
            data_menu_recent->Delete(item);
        }
        else
            id_new = ID_D_RECENT+count+1;
    }
    data_menu_recent->Prepend(id_new, fn.GetFullName(), fn.GetFullPath());
}

void FFrame::read_all_settings(wxConfigBase *cf)
{
    plot_pane->read_settings(cf);
    io_pane->read_settings(cf);
    //data_pane->read_settings(cf);
    read_settings(cf);
}

void FFrame::read_settings(wxConfigBase *cf)
{
    // restore frame position and size
    cf->SetPath("/Frame");
    int x = cf->Read("x", 50),
        y = cf->Read("y", 50),
        w = cf->Read("w", 650),
        h = cf->Read("h", 400);
    Move(x, y);
    SetClientSize(w, h);
    //int bot_height = cf->Read ("BotWinHeight", 100);
    //if (bottom_window) bottom_window->SetDefaultSize(wxSize(-1, bot_height));
}

void FFrame::save_all_settings(wxConfigBase *cf) const
{
    plot_pane->save_settings(cf);
    io_pane->save_settings(cf);
    //data_pane->save_settings(cf);
    save_settings(cf);
}

void FFrame::save_settings(wxConfigBase *cf) const
{
    cf->SetPath ("/Frame");
    int x, y, w, h;
    GetClientSize(&w, &h);
    GetPosition(&x, &y);
    cf->Write("x", (long) x);
    cf->Write("y", (long) y);
    cf->Write("w", (long) w);
    cf->Write("h", (long) h);
    //cf->Write ("BotWinHeight", bottom_window->GetClientSize().GetHeight()); 
}

void FFrame::set_menubar()
{
    // Make a menubar
    wxMenu* data_menu = new wxMenu;
    data_menu->Append (ID_D_LOAD,     "&Load File", "Load data from file");
    data_menu->Append (ID_D_XLOAD,    "&Load File (Custom)", 
                                    "Load data from file, with some options");
    this->data_menu_recent = new wxMenu;
    int rf_counter = 1;
    for (list<wxFileName>::const_iterator i = recent_data_files.begin(); 
         i != recent_data_files.end() && rf_counter < 16; i++, rf_counter++) 
        data_menu_recent->Append(ID_D_RECENT + rf_counter, 
                                 i->GetFullName(), i->GetFullPath());
    data_menu->Append(ID_D_RECENT, "Recent &Files", data_menu_recent); 
    data_menu->AppendSeparator();

    data_menu->Append (ID_D_INFO,     "&Info", "Info about loaded data");
    data_menu->Append (ID_D_DEVIATION  , "Std. &Dev.", 
                                            "Change errors for data points");
    data_menu->Append (ID_D_RANGE,    "&Range", "Change active points");
    wxMenu* data_bg_menu = new wxMenu;
    data_bg_menu->Append(ID_D_B_INFO, "&Info", "Info about baseline points");
    data_bg_menu->Append(ID_D_B_CLEAR, "&Clear", "Clear all baseline points");
    data_menu->Append (ID_D_BACKGROUND, "&Background", data_bg_menu);
    data_menu->Append (ID_D_CALIBRATE, "&Calibrate","Define calibration curve");
    data_menu->Append (ID_D_EXPORT,   "&Export", "Save data to file");
    data_menu->Append (ID_D_SET,      "&Settings", "Preferences and options");

    wxMenu* sum_menu = new wxMenu;
    sum_menu->Append (ID_S_HISTORY,   "&History", "Go back or forward"
                                                    " in change history");      
    sum_menu->Append (ID_S_INFO,      "&Info", "Info about fitted curve");      
    sum_menu->Append (ID_S_ADD,       "&Add", "Add parameter or function"); 
    sum_menu->Append (ID_S_REMOVE,    "&Remove/Freeze",
         "Remove parameter/function or avoid fitting of selected parameters");
    sum_menu->Append (ID_S_CHANGE,    "&Change", "Change value of a-parameter");
    sum_menu->Append (ID_S_VALUE,     "&Value", "Computes value of sum"
                                                    " or selected function");
    sum_menu->Append (ID_S_EXPORT,    "&Export", "Export fitted curve to file");
    sum_menu->Append (ID_S_SET,       "&Settings", "Preferences and options"); 

    wxMenu* manipul_menu = new wxMenu;
    manipul_menu->Append (ID_M_FINDPEAK, "&Find peak", "Search for a peak");
    manipul_menu->Append (ID_M_SET,   "&Settings", "Preferences and options");

    wxMenu* fit_menu = new wxMenu;
    wxMenu* fit_method_menu = new wxMenu;
    fit_method_menu->AppendRadioItem (ID_F_M+0, "&Levenberg-Marquardt", 
                                                "gradient based method");
    fit_method_menu->AppendRadioItem (ID_F_M+1, "Nelder-Mead &simplex", 
                                      "old, slow but reliable method");
    fit_method_menu->AppendRadioItem (ID_F_M+2, "&Genetic Algorithm", 
                                                "almost AI");
    fit_menu->Append (ID_F_METHOD,    "&Method", fit_method_menu, 
                                            "It influences commands below");
    fit_menu->AppendSeparator();
    fit_menu->Append (ID_F_RUN,       "&Run", "Start fitting sum to data");
    fit_menu->Append (ID_F_CONTINUE,  "&Continue", "Continue fitting");      
    fit_menu->Append (ID_F_INFO,      "&Info", "Info about current fit");      
    fit_menu->Append (ID_F_SET,       "&Settings", "Preferences and options"); 

#ifdef USE_XTAL
    wxMenu* crystal_menu = new wxMenu;
    crystal_menu->Append (ID_C_WAVELENGTH, "&Wavelength", 
                                                "Defines X-rays wavelengths");
    crystal_menu->Append (ID_C_ADD,      "&Add", "Add phase or plane");      
    crystal_menu->Append (ID_C_INFO,     "&Info", "Crystalography info");      
    crystal_menu->Append (ID_C_REMOVE,   "&Remove", "Remove phase or plane");
    crystal_menu->Append (ID_C_ESTIMATE, "&Estimate peak", 
                                            "Guess peak parameters");      
    crystal_menu->Append (ID_C_SET,    "&Settings", "Preferences and options");
#endif //USE_XTAL

    wxMenu* gui_menu = new wxMenu;
    wxMenu* gui_menu_mode = new wxMenu;
    gui_menu_mode->AppendRadioItem (ID_G_M_ZOOM, "&Normal", 
                                    "Use mouse for zooming, moving peaks etc.");
    gui_menu_mode->AppendRadioItem (ID_G_M_RANGE, "&Range", 
                             "Use mouse for activating and disactivating data");
    gui_menu_mode->AppendRadioItem (ID_G_M_BG, "&Baseline", 
                                    "Use mouse for substracting background");
    gui_menu_mode->AppendRadioItem (ID_G_M_ADD, "&Peak-Add", 
                                    "Use mouse for adding new peaks");
    wxMenu* gui_menu_mode_peak = new wxMenu;
    vector<const z_names_type*> all_t = V_fzg::all_types(fType);
    for (vector<const z_names_type*>::const_iterator i = all_t.begin();
                                                         i != all_t.end(); i++)
        gui_menu_mode_peak->AppendRadioItem(ID_G_M_PEAK_N + (i - all_t.begin()),
                                            (*i)->name.c_str());
    gui_menu_mode->AppendSeparator();
    gui_menu_mode->Append (ID_G_M_PEAK, "Peak &type", gui_menu_mode_peak);
    gui_menu->Append(ID_G_MODE, "&Mode", gui_menu_mode);
    gui_menu->AppendSeparator();
    wxMenu* gui_menu_show = new wxMenu;
    gui_menu_show->AppendCheckItem (ID_G_S_TOOLBAR, "&Toolbar", 
                                    "Show/hide toolbar");
    gui_menu_show->Check(ID_G_S_TOOLBAR, true);
    gui_menu_show->AppendCheckItem (ID_G_S_STATBAR, "&Status Bar", 
                                    "Show/hide status bar");
    gui_menu_show->Check(ID_G_S_STATBAR, true);
    gui_menu_show->AppendCheckItem (ID_G_S_DPANE, "&Datasets Pane", 
                                    "Show/hide datasets pane");  
    gui_menu_show->AppendCheckItem (ID_G_S_IO, "&Input/Output Text Pane", 
                                    "Show/hide text input/output");  
    gui_menu_show->Check(ID_G_S_IO, true);
    gui_menu->Append(ID_G_SHOW, "S&how", gui_menu_show);
    gui_menu->AppendSeparator();
    gui_menu->Append (ID_G_V_ALL, "Zoom &All", "View whole data");
    gui_menu->Append (ID_G_V_VERT, "Fit &vertically", "Adjust vertical zoom");
    gui_menu->Append (ID_G_V_SCROLL_L, "Scroll &Left", "Scroll view left");
    gui_menu->Append (ID_G_V_SCROLL_R, "Scroll &Right", "Scroll view right");

    wxMenu* gui_menu_zoom_prev = new wxMenu;
    gui_menu->Append(ID_G_V_ZOOM_PREV, "&Previous Zooms", gui_menu_zoom_prev);
    gui_menu->AppendSeparator();
    gui_menu->Append(ID_G_LCONF1, "Load &default config", 
                                                "Default configuration file");
    gui_menu->Append(ID_G_LCONF2, "Load &alt. config", 
                                            "Alternative configuration file");
    gui_menu->Append(ID_G_LCONFB, "Load &built-in config", 
                                                   "Built-in configuration");
    wxMenu* gui_menu_sconfig = new wxMenu;
    gui_menu_sconfig->Append(ID_G_SCONF1, "as default", 
                           "Save current configuration to default config file");
    gui_menu_sconfig->Append(ID_G_SCONF2, "as alternative",
                       "Save current configuration to alternative config file");
    gui_menu->Append(ID_G_SCONF, "&Save current config", gui_menu_sconfig);

    wxMenu* other_menu = new wxMenu;
    other_menu->Append (ID_O_INCLUDE,   "&Include file", 
                                            "Execute commands from a file");
    other_menu->Append (ID_O_REINCLUDE, "&Re-Include file", 
                "Reset & execute commands from the file included last time");
    other_menu->Enable (ID_O_REINCLUDE, false);
    other_menu->Append (ID_O_RESET,     "&Reset", "Reset current session");
    other_menu->AppendSeparator();
    other_menu->Append (ID_O_LOG,       "&Log to file", 
                                            "Start/stop logging to file");
    other_menu->Append (ID_O_DUMP,      "&Dump to file", 
                                         "Save current program state to file");
    other_menu->AppendSeparator();
    other_menu->Append (ID_PRINT,       "&Print...", "Print plots");
    other_menu->Append (ID_PRINT_SETUP, "Print Se&tup",
                                                    "Printer and page setup");
    other_menu->Append (ID_PRINT_PREVIEW, "Print Pre&view", "Preview"); 
    other_menu->AppendSeparator();
    other_menu->Append (ID_O_PLOT,      "&Replot", "Plot data and sum");
    other_menu->Append (ID_O_WAIT,      "&Wait", "What for?");
    other_menu->AppendSeparator();
    other_menu->Append (ID_O_SET,       "&Settings", "Preferences and options");

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(ID_H_MANUAL, "&Manual", "User's Manual");
    help_menu->Append(ID_H_TIP, "&Tip of the day", "Show tip of the day");
    help_menu->Append(wxID_ABOUT, "&About...", "Show about dialog");
    //help_menu->AppendSeparator();
    //help_menu->Append(ID_QUIT, "&Exit", "Exit the program");

    wxMenuBar *menu_bar = new wxMenuBar(wxMENU_TEAROFF);
    menu_bar->Append (other_menu, "&File" );
    menu_bar->Append (data_menu, "&Data" );
    menu_bar->Append (sum_menu, "&Sum" );
    menu_bar->Append (manipul_menu, "&Manipulate");
    menu_bar->Append (fit_menu, "Fi&t" );
#ifdef USE_XTAL
    menu_bar->Append (crystal_menu, "&Xtal/Diffr" );
#endif //USE_XTAL
    menu_bar->Append (gui_menu, "&GUI");
    menu_bar->Append (help_menu, "&Help");

    SetMenuBar(menu_bar);
}


    //construct GUI->Previous Zooms menu
void FFrame::OnShowMenuZoomPrev(wxCommandEvent& WXUNUSED(event))
{
    static vector<string> old_zoom_hist;
    const vector<string> &zoom_hist = plot_pane->get_zoom_hist();
    if (old_zoom_hist == zoom_hist) 
        return;
    wxMenu *menu = GetMenuBar()->FindItem(ID_G_V_ZOOM_PREV)->GetSubMenu(); 
    while (menu->GetMenuItemCount() > 0) //clear 
        menu->Delete(menu->GetMenuItems().GetLast()->GetData());
    int last = zoom_hist.size() - 1;
    for (int i = last, j = 1; i >= 0 && i > last - 10; i--, j++) 
        menu->Append(ID_G_V_ZOOM_PREV + j, zoom_hist[i].c_str());
    old_zoom_hist = zoom_hist;
}
           

void FFrame::OnTipOfTheDay(wxCommandEvent& WXUNUSED(event))
{
    string tip_file = "fityk_tips.txt";
    string tip_path = get_full_path_of_help_file(tip_file); 
    int idx = wxConfig::Get()->Read("/TipOfTheDay/idx", 0L); 
    wxTipProvider *tipProvider = wxCreateFileTipProvider(tip_path.c_str(), idx);
    bool showAtStartup = wxShowTip(this, tipProvider);
    idx = tipProvider->GetCurrentTip();
    delete tipProvider;
    wxConfig::Get()->Write("/TipOfTheDay/ShowAtStartup", showAtStartup); 
    wxConfig::Get()->Write("/TipOfTheDay/idx", idx); 
}

void FFrame::OnShowHelp(wxCommandEvent& WXUNUSED(event))
{
        help.DisplayContents();
}

void FFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxDialog* dlg = new wxDialog(this, -1, wxString("About..."));
    wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
    wxHtmlWindow *html = new wxHtmlWindow (dlg, -1, 
                                           wxDefaultPosition, wxSize(300, 160), 
                                           wxHW_SCROLLBAR_NEVER);
    html->SetBorders(0);
    html->SetPage(about_html);
    html->SetSize (html->GetInternalRepresentation()->GetWidth(), 
                   html->GetInternalRepresentation()->GetHeight());
    topsizer->Add (html, 1, wxALL, 10);
    topsizer->Add (new wxStaticLine(dlg, -1), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    wxButton *bu_ok = new wxButton (dlg, wxID_OK, "OK");
    bu_ok->SetDefault();
    topsizer->Add (bu_ok, 0, wxALL | wxALIGN_RIGHT, 15);
    dlg->SetAutoLayout(true);
    dlg->SetSizer(topsizer);
    topsizer->Fit(dlg);
    dlg->ShowModal();
    dlg->Destroy();
}

void FFrame::not_implemented_menu_item (const std::string &command) const 
{
    wxMessageBox (("This menu item is only reminding about command " + command 
                    + ".\n If you don't know how to use this command,\n"
                    " look for it in the manual.").c_str(), 
                  command.c_str(), wxOK|wxICON_INFORMATION);
}

void FFrame::OnDLoad (wxCommandEvent& WXUNUSED(event))
{
    static wxString dir = ".";
    wxFileDialog fdlg (frame, "Load data from a file", dir, "",
                              "x y files (*.dat, *.xy, *.fio)"
                                          "|*.dat;*.DAT;*.xy;*.XY;*.fio;*.FIO" 
                              "|rit files (*.rit)|*.rit;*.RIT"
                              "|cpi files (*.cpi)|*.cpi;*.CPI"
                              "|mca files (*.mca)|*.mca;*.MCA"
                              "|Siemens/Bruker (*.raw)|*.raw;*.RAW"
                              "|all files (*)|*",
                              wxOPEN | wxFILE_MUST_EXIST);
    if (fdlg.ShowModal() == wxID_OK) {
        exec_command ("d.load '" + fdlg.GetPath() + "'");
        add_recent_data_file(fdlg.GetPath());
    }
    dir = fdlg.GetDirectory();
}

void FFrame::OnDXLoad (wxCommandEvent& WXUNUSED(event))
{
    if (!dxload_dialog)
        dxload_dialog = new FDXLoadDlg(this, -1);

    string fname = my_data->get_filename();
    if (fname.empty()) {
        dxload_dialog->OnChangeButton(dummy_cmd_event);
        if (dxload_dialog->filename.empty())
            return;
    }
    else
        dxload_dialog->set_filename(fname);

    if (dxload_dialog->ShowModal() == wxID_OK) {
        exec_command (dxload_dialog->get_command());
        add_recent_data_file(dxload_dialog->filename.c_str());
    }
}

void FFrame::OnDRecent (wxCommandEvent& event)
{
    wxString s = GetMenuBar()->GetHelpString(event.GetId());
    exec_command ("d.load '" + s + "'");
    add_recent_data_file(s);
}

void FFrame::OnDInfo (wxCommandEvent& WXUNUSED(event))
{
    exec_command ("d.info");
}

void FFrame::OnDDeviation  (wxCommandEvent& WXUNUSED(event))
{
    FDStdDevDlg *dialog = new FDStdDevDlg(this, -1);
    if (dialog->ShowModal() == wxID_OK) {
        exec_command (dialog->get_command());
    }
    dialog->Destroy();
}

void FFrame::OnDRange (wxCommandEvent& WXUNUSED(event))
{
    FDRangeDlg *dialog = new FDRangeDlg(this, -1);
    dialog->ShowModal();
    dialog->Destroy();
}

void FFrame::OnDBInfo (wxCommandEvent& WXUNUSED(event))
{
    exec_command(S("d.background")); 
}

void FFrame::OnDBClear (wxCommandEvent& WXUNUSED(event))
{
    exec_command(S("d.background !")); 
}

void FFrame::OnDCalibrate (wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox (("This menu item is only reminding about command d.calibarate"
                   ".\n If you don't know how to use this command,\n"
                   " look for it in the manual.\n"
                   "By the way, is anyone using command d.calibrate?"),
                  "d.calibrate", wxOK|wxICON_INFORMATION);
}

void FFrame::OnDExport (wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fdlg (frame, "Export data to file", "", "",
                       "x y data (*.dat)|*.dat;*.DAT"
                       "|fityk file (*.fit)|*.fit;*.FIT",
                       wxSAVE | wxOVERWRITE_PROMPT);
    if (fdlg.ShowModal() == wxID_OK) 
        exec_command ("d.export '" + fdlg.GetPath() + "'");
}

void FFrame::OnDSet (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Data", 'd');
}

void FFrame::OnSHistory      (wxCommandEvent& WXUNUSED(event))
{
    if (my_sum->pars()->count_a() == 0) {
        wxMessageBox ("no @-parameters -- no history", "no history",
                      wxOK|wxICON_ERROR);
        return;
    }
    SumHistoryDlg *dialog = new SumHistoryDlg (this, -1);
    dialog->ShowModal();
    dialog->Destroy();
}
            
void FFrame::OnSAdd          (wxCommandEvent& WXUNUSED(event))
{
    FuncBrowserDlg *dialog = new FuncBrowserDlg(this, -1, 1);
    dialog->ShowModal();
    dialog->Destroy();
}
        
void FFrame::OnSInfo         (wxCommandEvent& WXUNUSED(event))
{
    FuncBrowserDlg *dialog = new FuncBrowserDlg(this, -1, 0);
    dialog->ShowModal();
    dialog->Destroy();
}
         
void FFrame::OnSRemove       (wxCommandEvent& WXUNUSED(event))
{
    FuncBrowserDlg *dialog = new FuncBrowserDlg(this, -1, 3);
    dialog->ShowModal();
    dialog->Destroy();
}
           
void FFrame::OnSChange       (wxCommandEvent& WXUNUSED(event))
{
    FuncBrowserDlg *dialog = new FuncBrowserDlg(this, -1, 2);
    dialog->ShowModal();
    dialog->Destroy();
}
           
void FFrame::OnSValue        (wxCommandEvent& WXUNUSED(event))
{
    FuncBrowserDlg *dialog = new FuncBrowserDlg(this, -1, 4);
    dialog->ShowModal();
    dialog->Destroy();
}
          
void FFrame::OnSExport       (wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fdlg (frame, "Export curve to file", "", "",
                       "x y data (*.dat)|*.dat;*.DAT"
                       "|XFIT peak listing (*xfit.txt)|*xfit.txt;*XFIT.TXT" 
                       "|mathematic formula (*.formula)|*.formula"
                       "|parameters of peaks (*.peaks)|*.peaks"
                       "|fityk file (*.fit)|*.fit;*.FIT",
                       wxSAVE | wxOVERWRITE_PROMPT);
    if (fdlg.ShowModal() == wxID_OK) 
        exec_command ("s.export '" + fdlg.GetPath() + "'");
}
           
void FFrame::OnSSet          (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Sum", 's');
}
        
void FFrame::OnMFindpeak     (wxCommandEvent& WXUNUSED(event))
{
    exec_command ("m.findpeak ");
}
        
void FFrame::OnMSet          (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Manipulations", 'm');
}
        

void FFrame::OnFMethod       (wxCommandEvent& event)
{
    wxMenuBar* mb = GetMenuBar();
    assert (mb);
    mb->Check (ID_F_M + fitMethodsContainer->current_method_number(), true);
    event.Skip();
}

void FFrame::OnFOneOfMethods (wxCommandEvent& event)
{
    int m = event.GetId() - ID_F_M;
    exec_command ("f.method " + S(fitMethodsContainer->symbol(m)));
}
           
void FFrame::OnFRun          (wxCommandEvent& WXUNUSED(event))
{
    int r = wxGetNumberFromUser ("Run fitting method", 
                                 "Max. number of iterations", "Fit->Run", 
                                 my_fit->default_max_iterations, 0, 9999);
    if (r != -1)
        exec_command (("f.run " + S(r)).c_str());
}
        
void FFrame::OnFContinue     (wxCommandEvent& WXUNUSED(event))
{
    int r = wxGetNumberFromUser ("Continue previous fitting", 
                                 "Max. number of iterations", "Fit->Continue", 
                                 my_fit->default_max_iterations, 0, 9999);
    if (r != -1)
        exec_command (("f.continue " + S(r)).c_str());
}
             
void FFrame::OnFInfo         (wxCommandEvent& WXUNUSED(event))
{
    exec_command ("f.info");
}
         
void FFrame::OnFSet          (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Fit (" + my_fit->method + ")", 'f');
}
        

void FFrame::OnCWavelength   (wxCommandEvent& WXUNUSED(event))
{
    not_implemented_menu_item ("c.wavelength");
}
               
void FFrame::OnCAdd          (wxCommandEvent& WXUNUSED(event))
{
    not_implemented_menu_item ("c.add");
}
        
void FFrame::OnCInfo         (wxCommandEvent& WXUNUSED(event))
{
    not_implemented_menu_item ("c.info");
}
         
void FFrame::OnCRemove       (wxCommandEvent& WXUNUSED(event))
{
    not_implemented_menu_item ("c.remove");
}
           
void FFrame::OnCEstimate     (wxCommandEvent& WXUNUSED(event))
{
    not_implemented_menu_item ("c.estimate");
}
             
void FFrame::OnCSet          (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Crystalography", 'c');
}
        

void FFrame::OnOPlot         (wxCommandEvent& WXUNUSED(event))
{
    exec_command ("o.plot");
}
         
void FFrame::OnOLog          (wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fdlg (frame, "Log to file", "", "",
                       "Log input to file (*.fit)|*.fit;*.FIT"
                       "|Log output to file (*.fit)|*.fit;*.FIT"
                       "|Log input & output (*.fit)|*.fit;*.FIT"
                       "|Stop logging to file |none.none",
                       wxSAVE);
    char m = my_other->get_log_mode();
    if (m == 'i')
        fdlg.SetFilterIndex(0);
    else if (m == 'o')
        fdlg.SetFilterIndex(1);
    else if (m == 'a')
        fdlg.SetFilterIndex(2);
    else if (m == 'n')
        fdlg.SetFilterIndex(3);
    else 
        assert (0);
    if (m != 'n')
        fdlg.SetPath(my_other->get_log_filename().c_str());
    if (fdlg.ShowModal() == wxID_OK) {
        int mode = fdlg.GetFilterIndex();
        char *modes = "ioan";
        assert(mode >= 0 && mode < 4);
        if (mode < 3)
            exec_command ("o.log " + wxString(S(modes[mode]).c_str()) 
                           + " '" + fdlg.GetPath() + "'");
        else
            exec_command ("o.log !"); 
    }
}

void FFrame::OnO_Reset   (wxCommandEvent& WXUNUSED(event))
{
    int r = wxMessageBox ("Do you really want to reset current session \n"
                          "and lose everything you have done in this session?", 
                          "Are you sure?", 
                          wxYES_NO | wxCANCEL | wxCENTRE | wxICON_QUESTION);
    if (r == wxYES)
        exec_command ("o.include !");
}
        
void FFrame::OnOInclude      (wxCommandEvent& WXUNUSED(event))
{
    static wxString dir = ".";
    wxFileDialog fdlg (frame, "Execute commands from file", dir, "",
                              "fityk file (*.fit)|*.fit;*.FIT|all files|*",
                              wxOPEN | wxFILE_MUST_EXIST);
    if (fdlg.ShowModal() == wxID_OK) {
        exec_command ("o.include '" + fdlg.GetPath() + "'");
        last_include_path = fdlg.GetPath();
        GetMenuBar()->Enable(ID_O_REINCLUDE, true);
    }
    dir = fdlg.GetDirectory();
}
            
void FFrame::OnOReInclude    (wxCommandEvent& WXUNUSED(event))
{
    exec_command ("o.include ! '" + last_include_path + "'");
}
            
void FFrame::OnOWait         (wxCommandEvent& WXUNUSED(event))
{
    int r = wxGetNumberFromUser ("Note: Wait command has a sense only "
            "in script", "Number of seconds to wait", "", 5, 0, 60);
    if (r != -1)
        exec_command (("o.wait " + S(r)).c_str());
}
         
void FFrame::OnODump         (wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog fdlg (frame, "Dump current program state to file as script", 
                                "", "", "fityk file (*.fit)|*.fit;*.FIT",
                       wxSAVE | wxOVERWRITE_PROMPT);
    if (fdlg.ShowModal() == wxID_OK) 
        exec_command ("o.dump '" + fdlg.GetPath() + "'");
}
         
void FFrame::OnOSet          (wxCommandEvent& WXUNUSED(event))
{
    OnXSet ("Other", 'o');
}
        
void FFrame::OnXSet (string name, char letter)
{
    DotSet* myset = set_class_p (letter);
    vector<string> ev, vv, tv;
    myset->expanp ("", ev);
    for (vector<string>::iterator i = ev.begin(); i != ev.end(); i++) {
        string v;
        myset->getp_core (*i, v);
        vv.push_back (v);
        string t;
        myset->typep (*i, t);
        tv.push_back (t);
    }
    string nm =  "Preferences - " + name;
    FSetDlg *dialog = new FSetDlg(this, -1, nm.c_str(), ev, vv, tv, myset);
    if (dialog->ShowModal() == wxID_OK) {
        for (unsigned int i = 0; i < ev.size(); i++) {
            string s;
            wxTextCtrl *tctrl = wxDynamicCast (dialog->tc_v[i], wxTextCtrl);
            wxCheckBox *chbox = wxDynamicCast (dialog->tc_v[i], wxCheckBox);
            wxChoice *chic = wxDynamicCast (dialog->tc_v[i], wxChoice);
            wxSpinCtrl *spinc = wxDynamicCast (dialog->tc_v[i], wxSpinCtrl);
            if (tctrl)
                s = tctrl->GetValue().Trim(true).Trim(false) .c_str();
            else if (chbox)
                s = S(chbox->GetValue());
            else if (chic)
                s = chic->GetStringSelection().c_str();
            else if (spinc)
                s = S(spinc->GetValue());
            else
                assert (0);
            if (s != vv[i]) {
                string cm = S(letter) + ".set " + ev[i] + " = " + s;
                exec_command (cm.c_str());
            }
        }
    }
    dialog->Destroy();
}

void FFrame::OnChangeMouseMode (wxCommandEvent& event)
{
    Mouse_mode_enum mode = mmd_zoom;
    switch (event.GetId()) {
        case ID_G_M_ZOOM:   
        case ID_ft_m_zoom:   
            mode = mmd_zoom;   
            GetMenuBar()->Check(ID_G_M_ZOOM, true);
            if (toolbar) toolbar->ToggleTool(ID_ft_m_zoom, true);
            break;
        case ID_G_M_RANGE:   
        case ID_ft_m_range:  
            mode = mmd_range;  
            GetMenuBar()->Check(ID_G_M_RANGE, true);
            if (toolbar) toolbar->ToggleTool(ID_ft_m_range, true);
            break;
        case ID_G_M_BG:   
        case ID_ft_m_bg:     
            mode = mmd_bg;     
            GetMenuBar()->Check(ID_G_M_BG, true);
            if (toolbar) toolbar->ToggleTool(ID_ft_m_bg, true);
            break;
        case ID_G_M_ADD:   
        case ID_ft_m_add:    
            mode = mmd_add;    
            GetMenuBar()->Check(ID_G_M_ADD, true);
            if (toolbar) toolbar->ToggleTool(ID_ft_m_add, true);
            break;  
        default: assert(0);
    }
    if (mode==mmd_bg && toolbar && toolbar->GetToolState(ID_ft_b_with)==false) {
        toolbar->ToggleTool(ID_ft_b_with, true);
        exec_command ("o.plot +");
    }
    plot_pane->set_mouse_mode(mode);
}

void FFrame::OnModePeak(wxCommandEvent& event)
{
    GetMenuBar()->Check(ID_G_M_PEAK_N + peak_type_nr, true);
    event.Skip();
}

void FFrame::OnChangePeakType(wxCommandEvent& event)
{
    peak_type_nr = event.GetId() - ID_G_M_PEAK_N;
    if (toolbar) toolbar->update_peak_type();
}

void FFrame::OnSwitchToolbar (wxCommandEvent& event)
{
    if (event.IsChecked() && !GetToolBar()) {
        toolbar = new FToolBar(this, -1);
        SetToolBar(toolbar);
    }
    else if (!event.IsChecked() && GetToolBar()){
        SetToolBar(0);
        delete toolbar; 
    }
}

void FFrame::OnSwitchStatbar (wxCommandEvent& event)
{
    if (event.IsChecked() && !GetStatusBar()) {
        status_bar = new FStatusBar(this);
        SetStatusBar(status_bar);
        plot_pane->update_mouse_hints();
    }
    else if (!event.IsChecked() && GetStatusBar()) {
        SetStatusBar(0);
        //bug in wxGTK - after SetStatusBar(0) blank gray status bar remains.
        delete status_bar; 
        status_bar = 0;
        //Layout();
        SendSizeEvent();
    }
}

void FFrame::OnSwitchDPane (wxCommandEvent& event)
{
    bool checked = event.IsChecked();
    if (checked && !v_splitter->IsSplit()) {
        data_pane->Show(true);
        v_splitter->SplitVertically(main_pane, data_pane);
    }
    else if (!checked && v_splitter->IsSplit()) {
        v_splitter->Unsplit();
    }
    GetMenuBar()->Check(ID_G_S_DPANE, checked);
    if (toolbar) toolbar->ToggleTool(ID_ft_dpane, checked);
}

void FFrame::OnSwitchIOPane (wxCommandEvent& event)
{
    bool checked = event.IsChecked();
    if (checked && !main_pane->IsSplit()) {
        io_pane->Show(true);
        main_pane->SplitHorizontally(plot_pane, io_pane);
    }
    else if (!checked && main_pane->IsSplit()) {
        main_pane->Unsplit();
    }
    GetMenuBar()->Check(ID_G_S_IO, checked);
    //if (toolbar) toolbar->ToggleTool(ID_ft_..., checked);
}

void FFrame::OnGViewAll (wxCommandEvent& WXUNUSED(event))
{
    change_zoom("[]");
}

void FFrame::OnGFitHeight (wxCommandEvent& WXUNUSED(event))
{
    change_zoom(". []");
}

void FFrame::OnGScrollLeft (wxCommandEvent & WXUNUSED(event))
{
    scroll_view_horizontally(-0.5);
}

void FFrame::OnGScrollRight (wxCommandEvent & WXUNUSED(event))
{
    scroll_view_horizontally(+0.5);
}


void FFrame::OnPreviousZoom(wxCommandEvent& event)
{
    int id = event.GetId();
    string s = plot_pane->zoom_backward(id ? id - ID_G_V_ZOOM_PREV : 1);
    if (s.size()) 
        exec_command("o.plot " + s);
}

void FFrame::change_zoom(const string& s)
{
    plot_pane->zoom_forward();
    string cmd = "o.plot " + s;
    exec_command(cmd);
}

void FFrame::scroll_view_horizontally(fp step)
{
    const Rect &vw = my_core->view;
    fp diff = vw.width() * step;
    fp new_left = vw.left + diff; 
    fp new_right = vw.right + diff;
    change_zoom("[" + S(new_left) + " : " + S(new_right) + "] .");
}


void FFrame::OnConfigSave (wxCommandEvent& event)
{
    wxString name = (event.GetId() == ID_G_SCONF1 ?  wxGetApp().conf_filename 
                                               : wxGetApp().alt_conf_filename);
    wxConfigBase *config =  new wxConfig("", "", name, "", 
                                         wxCONFIG_USE_LOCAL_FILE);
    save_all_settings(config);
    delete config;
}

void FFrame::OnConfigRead (wxCommandEvent& event)
{
    wxString name = (event.GetId() == ID_G_LCONF1 ? wxGetApp().conf_filename 
                                               : wxGetApp().alt_conf_filename);
    wxConfigBase *config = new wxConfig("", "", name, "", 
                                        wxCONFIG_USE_LOCAL_FILE);
    read_all_settings(config);
    delete config;
}

void FFrame::OnConfigBuiltin (wxCommandEvent& WXUNUSED(event))
{
    wxString name = wxString(config_dirname) + wxFILE_SEP_PATH+"builtin-config";
    wxConfigBase *config = new wxConfig("", "", name, "", 
                                        wxCONFIG_USE_LOCAL_FILE);
    if (config->GetNumberOfEntries(true))
        config->DeleteAll();
    read_all_settings(config);
    delete config;
}


class FPreviewFrame : public wxPreviewFrame
{
public:
    FPreviewFrame(wxPrintPreview* preview, wxFrame* parent) 
        : wxPreviewFrame (preview, parent, "Print Preview", 
                          wxDefaultPosition, wxSize(600, 550)) {}
    void CreateControlBar() { 
        m_controlBar = new wxPreviewControlBar(m_printPreview, 
                                        wxPREVIEW_PRINT|wxPREVIEW_ZOOM, this);
        m_controlBar->CreateButtons();
        m_controlBar->SetZoomControl(110);
    }
};

void FFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData print_dialog_data (*print_data);
    wxPrintPreview *preview = new wxPrintPreview (new FPrintout(plot_pane),
                                                  new FPrintout(plot_pane), 
                                                  &print_dialog_data);
    if (!preview->Ok()) {
        delete preview;
        wxMessageBox ("There was a problem previewing.\nPerhaps your current "
                      "printer is not set correctly?", "Previewing", wxOK);
        return;
    }
    FPreviewFrame *frame = new FPreviewFrame (preview, this);
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
}

void FFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
    (*page_setup_data) = *print_data;

    wxPageSetupDialog page_setup_dialog(this, page_setup_data);
    page_setup_dialog.ShowModal();

    (*print_data) = page_setup_dialog.GetPageSetupData().GetPrintData();
    (*page_setup_data) = page_setup_dialog.GetPageSetupData();

}

void FFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    if (plot_pane->is_background_white())
        if (wxMessageBox ("Plots will be printed on white background, \n"
                            "to save ink/toner.\n"
                            "Now background of your plot on screen\n"
                            "is not white, so visiblity of lines and points\n"
                            "can be different.\n Do you want to continue?",
                          "Are you sure?", 
                          wxYES_NO|wxCANCEL|wxICON_QUESTION) != wxYES)
            return;
    wxPrintDialogData print_dialog_data (*print_data);
    wxPrinter printer (&print_dialog_data);
    FPrintout printout(plot_pane);
    bool r = printer.Print(this, &printout, true);
    if (r) 
        (*print_data) = printer.GetPrintDialogData().GetPrintData();
    else if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
        wxMessageBox("There was a problem printing.\nPerhaps your current "
                     "printer is not set correctly?", "Printing", wxOK);
}

const f_names_type& FFrame::get_peak_type() const
{
    return V_f::f_names[peak_type_nr];
}

void FFrame::set_status_hint(const char *left, const char *right)
{
    if (status_bar)
        status_bar->set_hint(left, right);  
}

void FFrame::output_text(const wxString& str, Output_style_enum style)
{
    io_pane->append_text(str, style);
}

void FFrame::refresh_plots(bool update)
{
    plot_pane->refresh_plots(update);
}

void FFrame::focus_input()
{
    io_pane->focus_input();
}

void FFrame::OnIdle(wxIdleEvent &event) 
{
    if (GetToolBar())
        toolbar->OnIdle(event);
    event.Skip();
}


//=======================================================================

void add_peak(fp height, fp ctr, fp hwhm) 
{
    const f_names_type &f = frame->get_peak_type();

    //TODO? function add_peak(f_names_type, map<string,fp>) 
    my_sum->use_param_a_for_value();
    fp center = ctr + my_sum->zero_shift(ctr);
    string cmd = "s.add ^" + S(f.type);
    vector<fp> ini 
          = V_f::get_default_peak_parameters(&f, vector3(height, center, hwhm));
    for (int i = 0; i < f.psize; i++) {
        cmd += " ~" + S(ini[i]);
        const ParDefault &pd = f.pdefaults[i];
        if (pd.lower_set || pd.upper_set)  {
            cmd += " [" + (pd.lower_set ? S(pd.lower) : S()) + ":" 
                   + (pd.upper_set ? S(pd.upper) : S()) + "]";
        }
    }
    string stat = "Height: " + S(height) + " Ctr: " + S(center) 
                   + " HWHM: " + S(hwhm);
//return cmd, stat
    frame->SetStatusText (stat.c_str());
    exec_command (cmd);
}


void add_peak_in_range(fp xmin, fp xmax) 
{
    fp center, height, fwhm;
    my_sum->use_param_a_for_value();
    bool r = my_manipul->estimate_peak_parameters ((xmax + xmin)/2, 
                                                   fabs(xmax - xmin)/2, 
                                                   &center, &height, 0, &fwhm);
    if (r) add_peak (height, center, fwhm/2);
}

//=======================================================================

const char input_prompt[] = "=-> ";

void exec_command (const string& s)
{
    //TODO const int max_lines_in_output_win = 1000;
    string out_s = input_prompt + s + "\n";
    if (!strncmp(s.c_str(), "o.plot", 6))
        frame->SetStatusText((input_prompt + s).c_str());
    else
        frame->output_text(out_s.c_str(), os_ty_input);
    wxBusyCursor wait;
    bool r = parser(s);
    if (!r)
        frame->Close(true);
}

void exec_command (const wxString& s) { exec_command (S(s.c_str())); }
void exec_command (const char* s) { exec_command (wxString(s)); }


//=====================    set  dialog    ==================

FSetDlg::FSetDlg(wxWindow* parent, const wxWindowID id, const wxString& title,
             vector<string>& names, vector<string>& vals, vector<string>& types,
             DotSet* myset)
    :  wxDialog(parent, id, title), opt_names(names), opt_values(vals)
{
    const int max_rows = 10;
    assert (names.size() == vals.size());
    wxBoxSizer *sizer0 = new wxBoxSizer (wxVERTICAL);
    wxStaticText* st = new wxStaticText (this, -1, title);
    sizer0->Add (st, 0, wxALL|wxALIGN_CENTER, 10);
    wxBoxSizer *sizer_H0 = new wxBoxSizer (wxHORIZONTAL);
    sizer0->Add (sizer_H0, 0, wxALL|wxALIGN_CENTER, 0);
    wxBoxSizer *sizer_V0 = 0;
    for (unsigned int i = 0; i < names.size(); i++) { 
        if (i % max_rows == 0) {
            sizer_V0 = new wxBoxSizer (wxVERTICAL);
            sizer_H0->Add (sizer_V0, 0, wxALL|wxALIGN_CENTER, 10);
        }
        wxBoxSizer *sizerH = new wxBoxSizer (wxHORIZONTAL);
        wxControl* tc;
        bool put_static_text = true;
        bool with_tooltip = false;

        if (types[i].find("bool") != string::npos) {//boolean type
            wxCheckBox* tc_ = new wxCheckBox (this, -1, names[i].c_str());
            tc_->SetValue (vals[i] != "0");
            tc = tc_;
            put_static_text = false;
        }
            //enumaration of strings
        else if (types[i].find("enum") != string::npos) {
            vector<string> std_choices;
            myset->expand_enum (names[i], "", std_choices);
            const int max_choice_items = 20;//it should be enough
            assert (size(std_choices) < max_choice_items);
            wxString wx_choice[max_choice_items]; 
            for (unsigned int j = 0; j < std_choices.size(); j++)
                wx_choice[j] = std_choices[j].c_str();
            wxChoice *tc_ = new wxChoice (this, -1, wxDefaultPosition, 
                                          wxDefaultSize,  
                                          std_choices.size(), wx_choice);
            tc_->SetStringSelection (vals[i].c_str());
            tc = tc_;
        }
        else if (types[i].find("integer") != string::npos) {//integer
            tc = new wxSpinCtrl(this, -1, vals[i].c_str(),
                                wxDefaultPosition, wxSize(50, -1), 
                                wxSP_ARROW_KEYS,
                                0, 999999999);
            with_tooltip = true;
        }
        else if (types[i].find("...,") != string::npos) { //integer range
            const string &t = types[i];
            string lower(t.begin(), t.begin() + t.find_first_of(','));
            string upper(t.begin() + t.find_last_of(',') + 1, t.end());
            tc = new wxSpinCtrl(this, -1, vals[i].c_str(),
                                wxDefaultPosition, wxSize(50, -1), 
                                wxSP_ARROW_KEYS,
                                atoi(lower.c_str()), atoi(upper.c_str()));
            with_tooltip = true;
        }
        else { //another type
            tc = new wxTextCtrl (this, -1, vals[i].c_str(), 
                                 wxDefaultPosition, wxSize(50, -1));
            with_tooltip = true;
        }

#if wxUSE_TOOLTIPS
        if (with_tooltip)
            tc->SetToolTip (types[i].c_str()); 
#endif
        if (put_static_text) {
            wxStaticText* st = new wxStaticText (this, -1, names[i].c_str());
            sizerH->Add (st, 0, wxALL|wxALIGN_LEFT, 5);
        }
        sizerH->Add (tc, 0, wxALL|wxALIGN_LEFT, 0);
        tc_v.push_back(tc);
        sizer_V0->Add (sizerH, 0, wxALL|wxALIGN_LEFT, 5);
    }
    sizer0->Add (new wxStaticLine(this, -1), 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
    wxBoxSizer *sizerH = new wxBoxSizer (wxHORIZONTAL);
    wxButton *btOk = new wxButton (this, wxID_OK, "OK");
    sizerH->Add (btOk, 0, wxALL|wxALIGN_CENTER, 5 );
    wxButton *btCancel = new wxButton (this, wxID_CANCEL, "Cancel");
    sizerH->Add (btCancel, 0, wxALL|wxALIGN_CENTER, 5 );
    sizer0->Add (sizerH, 0, wxALL|wxALIGN_CENTER, 5);
    SetAutoLayout (true);
    SetSizer (sizer0);
    sizer0->Fit (this);
    sizer0->SetSizeHints (this);
}

//===============================================================
//                    FToolBar 
//===============================================================

BEGIN_EVENT_TABLE (FToolBar, wxToolBar)
//  EVT_IDLE (FToolBar::OnIdle)  //it doesn't work (why??); 
//                     //FToolBar::OnIdle is now called from FFrame::OnIdle
    EVT_TOOL_RANGE (ID_ft_m_zoom, ID_ft_m_add,  FToolBar::OnChangeMouseMode)
    EVT_TOOL_RANGE (ID_ft_v_pr, ID_ft_s_aa,        FToolBar::OnClickTool)
    EVT_TOOL (ID_ft_dpane, FToolBar::OnSwitchDPane)
    EVT_CHOICE (ID_ft_peakchoice, FToolBar::OnPeakChoice)
END_EVENT_TABLE()

FToolBar::FToolBar (wxFrame *parent, wxWindowID id)
        : wxToolBar (parent, id, wxDefaultPosition, wxDefaultSize,
                     wxNO_BORDER | /*wxTB_FLAT |*/ wxTB_DOCKABLE) 
{
    // mode
    Mouse_mode_enum m = frame ? frame->plot_pane->get_plot()->get_mouse_mode() 
                              : mmd_zoom;
    AddRadioTool(ID_ft_m_zoom, "Zoom", wxBitmap(zoom_t_xpm), wxNullBitmap, 
                 "Normal Mode", "Use mouse for zooming, moving peaks etc."); 
    ToggleTool(ID_ft_m_zoom, m == mmd_zoom);
    AddRadioTool(ID_ft_m_range, "Range", wxBitmap(range_t_xpm), wxNullBitmap, 
         "Data-Range Mode", "Use mouse for activating and disactivating data"); 
    ToggleTool(ID_ft_m_range, m == mmd_range);
    AddRadioTool(ID_ft_m_bg, "Background", wxBitmap(baseline_t_xpm), 
                                                                 wxNullBitmap, 
                 "Baseline Mode", "Use mouse for substracting background"); 
    ToggleTool(ID_ft_m_bg, m == mmd_bg);
    AddRadioTool(ID_ft_m_add, "Add peak", wxBitmap(addpeak_t_xpm), wxNullBitmap,
                 "Add-Peak Mode", "Use mouse for adding new peaks"); 
    ToggleTool(ID_ft_m_add, m == mmd_add);
    AddSeparator();
    // view
    AddTool (ID_G_V_ALL, "Whole", wxBitmap(whole_xpm), wxNullBitmap, 
             wxITEM_NORMAL, "View whole", 
             "Fit data in window");
    AddTool (ID_G_V_VERT, "Fit height", wxBitmap(vertic_xpm), wxNullBitmap,
            wxITEM_NORMAL, "Fit vertically",
            "Set optimal y scale"); 
    AddTool(ID_G_V_SCROLL_L,"<-- scroll",wxBitmap(leftscroll_xpm), wxNullBitmap,
            wxITEM_NORMAL, "Scroll left", "Scroll view left"); 
    AddTool(ID_G_V_SCROLL_R,"scroll -->",wxBitmap(rightscroll_xpm),wxNullBitmap,
            wxITEM_NORMAL, "Scroll right", "Scroll view right"); 
    AddTool (ID_ft_v_pr, "Back", wxBitmap(backv_xpm), wxNullBitmap, 
             wxITEM_NORMAL, "Previous view", 
             "Go to previous View");
    AddSeparator();
    //file
    AddTool (ID_O_INCLUDE, "Execute", wxBitmap(execute_xpm), wxNullBitmap,  
             wxITEM_NORMAL, "Execute script",
             "Execute (include) script from file");
    AddTool (ID_O_LOG, "Log", wxBitmap(log_xpm), wxNullBitmap,  
             wxITEM_NORMAL, "Start/Stop logging",
             "Start/Stop logging commands to file");
    AddTool (ID_O_RESET, "Reset", wxBitmap(reset_xpm), wxNullBitmap,  
             wxITEM_NORMAL, "Reset current session",
             "Reset current session");
    AddTool (ID_O_DUMP, "Dump", wxBitmap(dump_xpm), wxNullBitmap,  
             wxITEM_NORMAL, "Dump session to file",
             "Dump current session to file");
    AddSeparator();
    //data
    AddTool (ID_D_LOAD, "Load", wxBitmap(openfile_xpm), wxNullBitmap,  
             wxITEM_NORMAL, "Load file",
             "Load data from file");
    AddSeparator();
    //background
    AddTool (ID_ft_b_with, "With Bg", wxBitmap(plusbg_xpm), wxNullBitmap,  
             wxITEM_CHECK, "Plot with/without background",
             "Draw plot with added (or not) baseline");
    ToggleTool (ID_ft_b_with, my_core->plus_background);
    AddTool (ID_D_B_CLEAR, "Clear Bg", wxBitmap(clear_xpm), wxNullBitmap, 
             wxITEM_NORMAL, "Clear background", 
             "Delete all baseline points");
    AddSeparator();
    //sum
    AddTool (ID_S_INFO, "Tree", wxBitmap(tree_xpm), wxNullBitmap,
             wxITEM_NORMAL, "Show tree", "Show tree of functions");
    AddSeparator();
    peak_choice = new wxChoice(this, ID_ft_peakchoice); 
    vector<const z_names_type*> all_t = V_fzg::all_types(fType);
    for (vector<const z_names_type*>::const_iterator i = all_t.begin();
                                                         i != all_t.end(); i++)
        peak_choice->Append ((*i)->name.c_str());
    update_peak_type();
    AddControl (peak_choice);
    AddTool (ID_ft_s_aa, "add", wxBitmap(autoadd_xpm), wxNullBitmap, 
             wxITEM_NORMAL, "auto-add", "Add peak automatically");
    AddSeparator();
    //fit
    AddTool (ID_ft_f_run, "Run", wxBitmap(fitrun_xpm), wxNullBitmap,
             wxITEM_NORMAL, "Start fitting", "Start fitting sum to data");
    AddTool (ID_ft_f_cont, "Continue", wxBitmap(fitcont_xpm), wxNullBitmap,
             wxITEM_NORMAL, "Continue fitting", "Continue fitting sum to data");
    AddTool (ID_ft_f_undo, "Continue", wxBitmap(fitundo_xpm), wxNullBitmap,
             wxITEM_NORMAL, "Undo fitting", "Previous set of parameters");
    AddSeparator();
    //help
    AddTool (ID_H_MANUAL, "Help", wxBitmap(manual_xpm), wxNullBitmap,
             wxITEM_NORMAL, "Manual", "Open user manual");
    AddSeparator();
    AddTool (ID_ft_dpane, "Datasets", wxBitmap(dpane_xpm), wxNullBitmap,
             wxITEM_CHECK, "Datasets Pane", "Show/hide datasets pane");

    Realize();
}

void FToolBar::OnIdle(wxIdleEvent &event)
{
    if (GetToolState(ID_ft_b_with) != my_core->plus_background) 
        ToggleTool(ID_ft_b_with, my_core->plus_background);
    event.Skip();
}

void FToolBar::OnPeakChoice(wxCommandEvent &event) 
{
    if (frame) frame->peak_type_nr = event.GetSelection();
}

void FToolBar::update_peak_type() 
{ 
    peak_choice->SetSelection(frame ? frame->peak_type_nr : 0); 
}

void FToolBar::OnChangeMouseMode (wxCommandEvent& event)
{
    frame->OnChangeMouseMode(event);
}

void FToolBar::OnSwitchDPane (wxCommandEvent& event)
{
    frame->OnSwitchDPane(event);
}

void FToolBar::OnClickTool (wxCommandEvent& event)
{
    switch (event.GetId()) {
        case ID_ft_v_pr:  
            frame->OnPreviousZoom(dummy_cmd_event);
            break;
        case ID_ft_b_with: 
            exec_command(S("o.plot ") + (event.IsChecked() ? "+" : "-")); 
            break; 
        case ID_ft_f_run : 
            exec_command("f.run"); 
            break; 
        case ID_ft_f_cont: 
            exec_command("f.continue"); 
            break; 
        case ID_ft_f_undo: 
            exec_command("s.history -1"); 
            break; 
        case ID_ft_s_aa: 
        {
            fp c, h, f;
            bool r = my_manipul->estimate_peak_parameters(0.,+INF,&c,&h,0,&f);
            if (r) 
                add_peak(h, c, f/2);
            break; 
        }
        default: assert(0);
    }
}


#ifndef HELP_DIR
#    define HELP_DIR "."
#endif

string get_full_path_of_help_file (const string &name)
{
    vector<string> possible_prefixes;
    possible_prefixes.push_back("");
    const char *argv0 = wxGetApp().argv[0];
    possible_prefixes.push_back(wxPathOnly(argv0).c_str());
    char *possible_paths[] = {
        HELP_DIR,
        ".",
        "..",
        "doc",
        "../doc",
        0
    };
    for (vector<string>::const_iterator pref = possible_prefixes.begin(); 
            pref != possible_prefixes.end(); pref++) {
        string pref_path = *pref;
        if (!pref_path.empty() && *(pref_path.end()-1) != wxFILE_SEP_PATH)
                pref_path += wxFILE_SEP_PATH;
        for (int i = 0; possible_paths[i]; i++) {
            string path = pref_path + possible_paths[i];
            if (!path.empty() && *(path.end()-1) != wxFILE_SEP_PATH)
                path += wxFILE_SEP_PATH;
            //mesg("Looking for " + name + " in " + path);
            path += name;
            if (wxFileExists(path.c_str())) return path;
        }
    }
    return name;
}


//===============================================================
//                    FStatusBar   
//===============================================================
BEGIN_EVENT_TABLE(FStatusBar, wxStatusBar)
    EVT_SIZE(FStatusBar::OnSize)
END_EVENT_TABLE()

FStatusBar::FStatusBar(wxWindow *parent)
        : wxStatusBar(parent, -1), statbmp2(0)
{
    int widths[sbf_max] = { -1, 100, 100, 120 };
    SetFieldsCount (sbf_max, widths);
    SetMinHeight(15);
    statbmp1 = new wxStaticBitmap(this, -1, wxIcon(mouse_l_xpm));
    statbmp2 = new wxStaticBitmap(this, -1, wxIcon(mouse_r_xpm));
}

void FStatusBar::OnSize(wxSizeEvent& event)
{
    if (!statbmp2) return; 
    wxRect rect;
    GetFieldRect(sbf_hint1, rect);
    wxSize size = statbmp1->GetSize();
    int bmp_y = rect.y + (rect.height - size.y) / 2;
    statbmp1->Move(rect.x + 1, bmp_y);
    GetFieldRect(sbf_hint2, rect);
    statbmp2->Move(rect.x + 1, bmp_y);
    event.Skip();
}

void FStatusBar::set_hint(const char *left, const char *right)
{
    wxString space = "    "; //TODO more sophisticated solution 
    if (left)  SetStatusText(space + wxString(left),  sbf_hint1);
    if (right) SetStatusText(space + wxString(right), sbf_hint2);
}




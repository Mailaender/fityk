// This file is part of fityk program. Copyright (C) Marcin Wojdyr

// wxwindows headers, see wxwindows samples for description
#include <wx/wxprec.h>
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "common.h"
RCSID ("$Id$")

#include "ui.h"
#include "wx_plot.h"
#include "wx_gui.h"
#include "wx_pane.h"
#include "other.h"

using namespace std;

void UserInterface::showMessage (OutputStyle style, const string& s)
{
    frame->output_text(style, s + "\n");
}

void UserInterface::doDrawPlot(bool now, const std::vector<fp>& a)
{
        params4plot = a;
        frame->refresh_plots(true, now);
}


void UserInterface::wait (float seconds) 
{  
    wxUsleep (static_cast<int>(seconds*1e3)); //wxUsleep takes mili-seconds
}

void UserInterface::execCommand(const string& s)
{
    //FIXME should I limit number of displayed lines?
    //const int max_lines_in_output_win = 1000;
    string prompted_s = "=-> " + s;
    //don't output o.plot command - it is generated by every zoom in/out etc.
    bool output = strncmp(s.c_str(), "o.plot", 6) != 0;
    if (output)
        frame->output_text(os_input, prompted_s + "\n");
    else
        frame->set_status_text(s.c_str());
    wxBusyCursor wait;
    bool r = parser(s);
    if (!r)
        close();
    frame->refresh_plots(false, true);
}

void UserInterface::close()
{
        frame->Close(true);
}


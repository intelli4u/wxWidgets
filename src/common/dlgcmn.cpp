/////////////////////////////////////////////////////////////////////////////
// Name:        common/dlgcmn.cpp
// Purpose:     common (to all ports) wxDialog functions
// Author:      Vadim Zeitlin
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "dialogbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dialog.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/button.h"
    #include "wx/containr.h"
#endif

//--------------------------------------------------------------------------
// wxDialogBase
//--------------------------------------------------------------------------

// FIXME - temporary hack in absence of wxtopLevelWindow, should be always used
#ifdef wxTopLevelWindowNative
BEGIN_EVENT_TABLE(wxDialogBase, wxTopLevelWindow)
    WX_EVENT_TABLE_CONTROL_CONTAINER(wxDialogBase)
END_EVENT_TABLE()

WX_DELEGATE_TO_CONTROL_CONTAINER(wxDialogBase)
#endif

void wxDialogBase::Init()
{
    m_returnCode = 0;

    // the dialogs have this flag on by default to prevent the events from the
    // dialog controls from reaching the parent frame which is usually
    // undesirable and can lead to unexpected and hard to find bugs
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);

#ifdef wxTopLevelWindowNative // FIXME - temporary hack, should be always used!
    m_container.SetContainerWindow(this);
#endif
}

#if wxUSE_STATTEXT && wxUSE_TEXTCTRL

wxSizer *wxDialogBase::CreateTextSizer( const wxString& message )
{
    wxBoxSizer *box = new wxBoxSizer( wxVERTICAL );

    // get line height for empty lines
    int y = 0;
    wxFont font( GetFont() );
    if (!font.Ok())
        font = *wxSWISS_FONT;
    GetTextExtent(_T("H"), (int*)NULL, &y, (int*)NULL, (int*)NULL, &font);

    wxString line;
    for ( size_t pos = 0; pos < message.length(); pos++ )
    {
        switch ( message[pos] )
        {
            case _T('\n'):
                if (!line.IsEmpty())
                {
                    wxStaticText *s1 = new wxStaticText( this, -1, line );
                    box->Add( s1 );
                    line = wxT("");
                }
                else
                {
                    box->Add( 5, y );
                }
                break;

            case _T('&'):
                // this is used as accel mnemonic prefix in the wxWindows
                // controls but in the static messages created by
                // CreateTextSizer() (used by wxMessageBox, for example), we
                // don't want this special meaning, so we need to quote it
                line += _T('&');

                // fall through to add it normally too

            default:
                line += message[pos];
        }
    }

    // remaining text behind last '\n'
    if (!line.IsEmpty())
    {
        wxStaticText *s2 = new wxStaticText( this, -1, line );
        box->Add( s2 );
    }

    return box;
}

#endif // wxUSE_STATTEXT && wxUSE_TEXTCTRL

#if wxUSE_BUTTON

wxSizer *wxDialogBase::CreateButtonSizer( long flags )
{
    bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
    
    wxBoxSizer *box = NULL;
    
    // If we have a PDA screen, put yes/no button over 
    // all other buttons, otherwise on the left side.
    if (is_pda)
        box = new wxBoxSizer( wxVERTICAL );
    else
        box = new wxBoxSizer( wxHORIZONTAL );
        
    wxBoxSizer *inner_yes_no = NULL;
    
    // Only create sizer containing yes/no
    // if it is actually required
    if ( (flags & wxYES_NO) != 0 )
    {
        inner_yes_no = new wxBoxSizer( wxHORIZONTAL );
        box->Add( inner_yes_no, 0, wxBOTTOM, 10 );
    }
    
    wxBoxSizer *inner_rest = new wxBoxSizer( wxHORIZONTAL );
    box->Add( inner_rest, 0, 0, 0 );

#if defined(__WXMSW__) || defined(__WXMAC__)
    static const int margin = 6;
#else
    static const int margin = 10;
#endif

    wxButton *ok = (wxButton *) NULL;
    wxButton *cancel = (wxButton *) NULL;
    wxButton *yes = (wxButton *) NULL;
    wxButton *no = (wxButton *) NULL;

    // always show an OK button, unless we have both YES and NO
    if ( (flags & wxYES_NO) != wxYES_NO )
        flags |= wxOK;

    if (flags & wxYES)
    {
        yes = new wxButton( this, wxID_YES, _("Yes"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS );
        inner_yes_no->Add( yes, 0, wxLEFT|wxRIGHT, margin );
    }
    if (flags & wxNO)
    {
        no = new wxButton( this, wxID_NO, _("No"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS );
        inner_yes_no->Add( no, 0, wxLEFT|wxRIGHT, margin );
    }

    if (flags & wxOK)
    {
        ok = new wxButton( this, wxID_OK, _("OK"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS );
        inner_rest->Add( ok, 0, wxLEFT|wxRIGHT, margin );
    }

    if (flags & wxFORWARD)
        inner_rest->Add( new wxButton( this, wxID_FORWARD, _("Forward"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxBACKWARD)
        inner_rest->Add( new wxButton( this, wxID_BACKWARD, _("Backward"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxSETUP)
        inner_rest->Add( new wxButton( this, wxID_SETUP, _("Setup"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxMORE)
        inner_rest->Add( new wxButton( this, wxID_MORE, _("More..."),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxHELP)
        inner_rest->Add( new wxButton( this, wxID_HELP, _("Help"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS  ), 0, wxLEFT|wxRIGHT, margin );

    if (flags & wxCANCEL)
    {
        cancel = new wxButton( this, wxID_CANCEL, _("Cancel"),wxDefaultPosition,wxDefaultSize,wxCLIP_SIBLINGS );
        inner_rest->Add( cancel, 0, wxLEFT|wxRIGHT, margin );
    }

    // choose the default button
    if (flags & wxNO_DEFAULT)
    {
        if (no)
        {
            no->SetDefault();
            no->SetFocus();
        }
    }
    else
    {
        if (ok)
        {
            ok->SetDefault();
            ok->SetFocus();
        }
        else if (yes)
        {
            yes->SetDefault();
            yes->SetFocus();
        }
    }

    return box;
}

#endif // wxUSE_BUTTON

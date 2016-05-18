/***************************************************************
 * Name:      alphaindex.h
 * Purpose:   header for wxAlphaIndex
 * Author:     ()
 * Created:   2016-05-15
 * Copyright:  ()
 * License:   wxWindows licence
 **************************************************************/

#ifndef WX_ALPHAINDEX_ALPHAINDEX_H_INCLUDED
#define WX_ALPHAINDEX_ALPHAINDEX_H_INCLUDED

#ifdef WXUSING_ALPHAINDEX_SOURCE
    #define WXDLLIMPEXP_ALPHAINDEX
#elif WXMAKINGDLL_ALPHAINDEX
    #define WXDLLIMPEXP_ALPHAINDEX WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_ALPHAINDEX WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_ALPHAINDEX
#endif

#include <wx/overlay.h>
#include <wx/bitmap.h>
#include <wx/window.h>
#include <vector>
#include <map>

// Event IDs
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_ALPHAINDEX, wxEVT_ALPHA_INDEX_CLICK, wxCommandEvent);
#define EVT_ALPHA_INDEX_CLICK(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_ALPHA_INDEX_CLICK, winid, wxCommandEventHandler(fn))

// style flags
enum wxAlphaIndexOption
{
    wxALPHA_INDEX_RIGHT = 1 << 0
};

class WXDLLIMPEXP_ALPHAINDEX wxAlphaIndex: public wxWindow
{
    wxDECLARE_CLASS(wxAccordion);

    public:
        //housekeeping
        wxAlphaIndex(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name="wxAlphaIndex");
        wxAlphaIndex();
        bool Create(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxString &name="wxAlphaIndex");

        //virtual overrides
        virtual bool SetFont (const wxFont &font);

        //appearance accessor methods
        void SetPadding(int);
        void SetBorderColour(const wxColour&);
        void SetSeparatorColour(const wxColour&);
        void SetTextColour(const wxColour&);
        void SetHLColour(const wxColour&);
        void SetHLTextColour(const wxColour&);

        int GetPadding();
        wxColour GetBorderColour();
        wxColour GetSeparatorColour();
        wxColour GetTextColour();
        wxColour GetHLColour();
        wxColour GetHLTextColour();

        //letter handling
        void AddLetter(const wxString&,int,int);
        void RealizeLetters();
        void ClearLetters();
        void AddDefaultLetters();

    protected:
        void Init();

    private:
        //event handles
        void OnPaint( wxPaintEvent& event );
        void OnLeftUp( wxMouseEvent& event );
        void OnSize( wxSizeEvent& event );
        void OnMotion( wxMouseEvent& event );
        void OnLeaveWindow( wxMouseEvent& event );

        //helper functions
        void ComputeLetterSizes();
        void SetSizes();
        void Cache();
        int GetSlot(int);
        void DrawLetter(wxDC* dc, int cur_letter, int start, int endd, int wd, int right_adjust);

        //private data
        std::vector<int> letter_widths;
        std::vector<wxString> letters;
        std::vector<int> frequencies;
        std::vector<int> available_letters;
        std::vector<int> ends;
        std::map<int,wxString> letter_builder;
        std::map<int,int> freq_builder;

        wxColour m_separatorColour;
        wxColour m_borderColour;
        wxColour m_textColour;
        wxColour m_hlTextColour;
        wxColour m_hlColour;
        wxBitmap m_cachedBG;
        wxOverlay m_overlay;
        int m_padding;
        int m_letterHt;
        int m_letterWd;
        int m_lastSlot;
};

#endif // WX_ALPHAINDEX_ALPHAINDEX_H_INCLUDED

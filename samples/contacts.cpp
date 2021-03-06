/////////////////////////////////////////////////////////////////////////////
// Name:        contacts.cpp
// Purpose:     wxWidgets wxAlphaIndex sample
// Author:
// Modified by:
// Created:     2016-06-07
// Copyright:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/app.h>
    #include <wx/frame.h>
    #include <wx/menu.h>
    #include <wx/statusbr.h>
    #include <wx/sizer.h>
    #include <wx/stattext.h>
    #include <wx/button.h>
    #include <wx/dcclient.h>
    #include <wx/dcmemory.h>
    #include <wx/msgdlg.h>
#endif

#include <wx/alphaindex/alphaindex.h>
#include <wx/srchctrl.h>
#include <wx/graphics.h>
#include <wx/vlbox.h>
#include <set>
#include <map>

///////////////////////////////////////////////////////////////////////////////
/// Class myVListBox
///////////////////////////////////////////////////////////////////////////////
class myVListBox: public wxVListBox
{
    public:
        myVListBox(wxWindow *parent, wxWindowID id=wxID_ANY,
                   const wxPoint &pos=wxDefaultPosition,
                   const wxSize &size=wxDefaultSize,
                   long style=0, const wxString &name=wxVListBoxNameStr);

        virtual void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;
        virtual wxCoord OnMeasureItem (size_t n) const;

        void InitData(const std::vector<wxString>&);
        void ScrollToLetter(const wxString&);
        const wxBitmap& GetProfilePicture(size_t);
        wxString GetName(size_t) const;

    private:
        //We need to know where each letter starts in the control, so that:
        //1) we know which items to draw with a letter header, and
        //2) we know where to scroll to when a letter in the alpha index is
        //   clicked.
        std::set<size_t> m_startsSet;
        std::map<wxString,size_t> m_scrollTos;

        //a real control would,presumably, be tied to a database.  For
        //this simple sample, we'll just use the following static data for the
        //names and images
        wxBitmap MakeBitmap(int sz,const wxColour& c);
        static size_t m_totalItems;
        static wxString m_firstNames[120];
        static wxString m_lastNames[120];
        static int m_bgColours[120];

        static wxColour m_profileColour1;
        static wxColour m_profileColour2;
        static wxColour m_profileColour3;
        static wxColour m_profileColour4;
        wxBitmap m_b1;
        wxBitmap m_b2;
        wxBitmap m_b3;
        wxBitmap m_b4;
        wxBitmap* m_bmps[4];

        wxBitmap m_bLrg1;
        wxBitmap m_bLrg2;
        wxBitmap m_bLrg3;
        wxBitmap m_bLrg4;
        wxBitmap* m_bmpsLrg[4];

        //items that can be set to control the appearance
        wxFont m_letterHdrFont;
        wxFont m_regularFont;
        int m_bmpMargin;
        int m_bmpSz;
        int m_letterHeaderHt;
        wxColour m_selectionColour;
        wxColour m_selectionBorderColour;
        wxColour m_headerColour;
        wxColour m_headerBorderColour;
        wxColour m_sepLineColour;

        //computed items used while drawing
        int m_regCharHt;
        int m_hdrCharHt;
        int m_regCharWd;
        int m_hdrCharWd;
        int m_lrgSize;
        int m_regSize;
        int m_textOffset;
};

myVListBox::myVListBox(wxWindow *parent, wxWindowID id,
                       const wxPoint &pos, const wxSize &size, long style,
                       const wxString &name)
                       :wxVListBox(parent, id, pos, size,  style, name)
{
    //Set the background to be the same as the parent's background
    SetBackgroundColour( GetParent()->GetBackgroundColour() );

    //Set the colours and fonts
    m_selectionColour=wxColour(167,202,221);
    m_selectionBorderColour=wxColour(167,202,221);
    m_headerColour=wxColour(179,179,179);
    m_headerBorderColour=wxColour(190,190,190);
    m_sepLineColour=wxColour(199,199,199);

    m_regularFont=GetFont();
    m_letterHdrFont=m_regularFont.Smaller();
    m_letterHdrFont.MakeBold();

    //compute the character sizes
    wxClientDC dc(this);
    dc.SetFont(m_regularFont);
    dc.GetTextExtent("A",&m_regCharWd,&m_regCharHt);
    dc.SetFont(m_letterHdrFont);
    dc.GetTextExtent("A",&m_hdrCharWd,&m_hdrCharHt);

    //set the rest of the setable items
    m_bmpMargin=3;
    m_bmpSz=3*m_regCharHt;
    m_letterHeaderHt=2*m_hdrCharHt;

    //compute the items needed for drawing
    int totalbmpht=m_bmpSz+2*m_bmpMargin;
    m_lrgSize=totalbmpht+m_letterHeaderHt;
    m_regSize=totalbmpht+1;
    m_textOffset=(totalbmpht-m_regCharHt)/2;
}

void myVListBox::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
    wxString name=m_lastNames[n]+", "+m_firstNames[n];
    wxDCFontChanger dcfc(dc);
    int l=rect.GetLeft();
    int t=rect.GetTop();
    int w=rect.GetWidth();
    int h=rect.GetHeight();
    int hdrht;

    if(h==m_lrgSize)
    {
        //hdrht is the full header size
        hdrht=m_letterHeaderHt;

        //draw the header box
        dc.SetPen(wxPen(m_headerBorderColour));
        dc.SetBrush(wxBrush(m_headerColour));
        dc.DrawRectangle(l,t,w,hdrht);
        dcfc.Set(m_letterHdrFont);
        dc.DrawText(m_lastNames[n].Left(1),l+m_hdrCharWd,t+m_hdrCharHt/2+m_hdrCharHt%2);
        //h2%2 since if it can't be split evenly, i'd rather have the
        //extra pixel at the top.
    }
    else
    {
        //hdrht is only the single pixel for the separator line
        hdrht=1;

        //draw the separator line:
        dc.SetPen(wxPen(m_sepLineColour));
        dc.DrawLine(l,t,w,t);
    }

    int drawingtop=t+hdrht;

    //if selected, draw the selection rectangle
    if(IsSelected(n))
    {
        dc.SetPen(wxPen(m_selectionBorderColour));
        dc.SetBrush(wxBrush(m_selectionColour));
        dc.DrawRectangle(l,drawingtop,w,h-hdrht);
    }

    //draw the bitmap and name
    //  -draw the bitmap with its margin on the left, top, and bottom
    //  -draw the text centered in the rectangle and 1 character width to the
    //   right of the bitmap
    int bmpleft = l         +m_bmpMargin;
    int bmptop  = drawingtop+m_bmpMargin;
    int textleft= bmpleft   +m_bmpSz+m_regCharWd;
    int texttop = drawingtop+m_textOffset;

    dc.DrawBitmap(*m_bmps[m_bgColours[n]],bmpleft,bmptop);
    dcfc.Set(m_regularFont);
    dc.DrawText(name,textleft,texttop);
}

void myVListBox::InitData(const std::vector<wxString>& letters)
{
    //here we'll do 3 things.
    //1) set the total of the the items in the vlist
    //2) set up some bitmaps to use as thumbnails and profile images
    //3) set up the set of starter letters and the scrollto map.
    //a real contacts program would set up these things based on info in a
    //database.  Here, we'll just use the static data made up for this samlpe.

    //part 1:
    SetItemCount(m_totalItems);

    //part 2:
    m_b1=MakeBitmap(m_bmpSz,m_profileColour1);
    m_b2=MakeBitmap(m_bmpSz,m_profileColour2);
    m_b3=MakeBitmap(m_bmpSz,m_profileColour3);
    m_b4=MakeBitmap(m_bmpSz,m_profileColour4);

    m_bmps[0]=&m_b1;
    m_bmps[1]=&m_b2;
    m_bmps[2]=&m_b3;
    m_bmps[3]=&m_b4;

    m_bLrg1=MakeBitmap(220,m_profileColour1);
    m_bLrg2=MakeBitmap(220,m_profileColour2);
    m_bLrg3=MakeBitmap(220,m_profileColour3);
    m_bLrg4=MakeBitmap(220,m_profileColour4);

    m_bmpsLrg[0]=&m_bLrg1;
    m_bmpsLrg[1]=&m_bLrg2;
    m_bmpsLrg[2]=&m_bLrg3;
    m_bmpsLrg[3]=&m_bLrg4;

    //part 3:
    //the following assumes that LastNames is sorted and that the letters
    //vector contains letters that will be found in LastNames.

    //run over the last names keeping track of where the letters start and end
    std::map<wxString,size_t> letterends;
    wxString a="",b;

    for(size_t i=0;i<120;++i)
    {
        b=m_lastNames[i].Left(1);

        if(b!=a)
        {
            m_startsSet.insert(i);
            m_scrollTos.insert(std::pair<wxString,size_t>(b,i));
            if(!a.IsEmpty())
            {
                letterends.insert(std::pair<wxString,size_t>(a,i-1));
            }
            a=b;
        }
    }

    //run over the letters and if a letter is not in the m_scrollTos map,
    //look at the previous letters until we find one in the lettersend map
    //and use that value for the scrollto.
    for(std::vector<wxString>::const_iterator it=letters.begin();it!=letters.end();++it)
    {
        std::map<wxString,size_t>::iterator mit=m_scrollTos.find(*it);

        if(mit==m_scrollTos.end())
        {
            std::vector<wxString>::const_reverse_iterator rit(it);
            int newsc(0);

            while(rit!=letters.rend())
            {
                mit=letterends.find(*rit);

                if(mit!=letterends.end())
                {
                    newsc=mit->second;
                    break;
                }

                ++rit;
            }

            m_scrollTos.insert(std::pair<wxString,size_t>(*it,newsc));
        }
    }
}

wxBitmap myVListBox::MakeBitmap(int sz,const wxColour& c)
{
    //the profile picture is composed of a circle and an ellipse.  For a 85x85
    //picture, good results are obtained with the circle starting at (14,39)
    //with diameter 39 and the ellipse starting at (11,50) with width 63 and
    //height 74.  For an arbitrary size, we'll just scale those values.

    double scalefactor=static_cast<double>(sz)/85.0;

    double circle_x=23*scalefactor;
    double circle_y=14*scalefactor;
    double circle_diam=39*scalefactor;

    double ellipse_x=11*scalefactor;
    double ellipse_y=50*scalefactor;
    double ellipse_w=63*scalefactor;
    double ellipse_h=74*scalefactor;

    wxBitmap b(sz,sz);
    wxMemoryDC dc(b);

    dc.SetBackground(wxBrush(c));
    dc.Clear();

    if( wxGraphicsContext* gc = wxGraphicsContext::Create(dc) )
    {
        gc->SetPen(*wxTRANSPARENT_PEN);
        gc->SetBrush( wxBrush(wxColour(112,112,112)) );
        gc->DrawEllipse(circle_x, circle_y, circle_diam, circle_diam);
        gc->DrawEllipse(ellipse_x, ellipse_y, ellipse_w, ellipse_h);
        delete gc;
    }

    dc.SelectObject(wxNullBitmap);
    return b;
}

wxCoord myVListBox::OnMeasureItem(size_t n) const
{
    return m_startsSet.find(n)==m_startsSet.end()?m_regSize:m_lrgSize;
}

void myVListBox::ScrollToLetter(const wxString& s)
{
    std::map<wxString,size_t>::iterator it=m_scrollTos.find(s);
    if(it!=m_scrollTos.end())
    {
        ScrollToRow(it->second);
    }
}

wxString myVListBox::GetName(size_t i) const
{
    return i<m_totalItems?(m_firstNames[i] + " " + m_lastNames[i]):wxEmptyString;
}

const wxBitmap& myVListBox::GetProfilePicture(size_t i)
{
    return i<m_totalItems?*m_bmpsLrg[m_bgColours[i]]:wxNullBitmap;
}


///////////////////////////////////////////////////////////////////////////////
/// Class ContactsFrame
///////////////////////////////////////////////////////////////////////////////

class ContactsFrame: public wxFrame
{
    public:
        ContactsFrame( wxWindow* parent, wxWindowID id = wxID_ANY,
                      const wxString& title = wxT("wxAlphaIndex Demo"),
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxSize( 481,466 ),
                      long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );;
        ~ContactsFrame();

    private:
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnB1(wxCommandEvent& event);
        void OnAlphaIndex(wxCommandEvent& event);
		void OnSearchCancelButton( wxCommandEvent& event );
		void OnSearch( wxCommandEvent& event );
		void OnList(wxCommandEvent& event);
		void OnPaintProfilePic(wxPaintEvent& event);

		void SearchHelper(bool);

        wxColour m_bgColour;
        wxColour m_brdrColour;
        wxColour m_alIndTextColour;
        wxColour m_alIndHLColour;

		wxPanel* m_vlistPanel;
		wxSearchCtrl* m_searchCtrl;
		myVListBox*  m_vlist;
		wxPanel* m_searchResults;
		wxPanel* m_profilePicPanel;
		wxTextCtrl* m_nameTextCtrl;
};

ContactsFrame::ContactsFrame( wxWindow* parent, wxWindowID id,
                             const wxString& title, const wxPoint& pos,
                             const wxSize& size, long style )
              : wxFrame( parent, id, title, pos, size, style )
{
    m_bgColour=wxColour(227,227,227);
    m_brdrColour=wxColour(191,191,191);
    m_alIndTextColour=wxColour(151,151,151);
    m_alIndHLColour=wxColour(49,106,197);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	//create the menu and status bar
	wxMenu* fileMenu = new wxMenu();
	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* menuFileQuit = new wxMenuItem( fileMenu, wxID_ANY,
                wxString( "&Quit" ) + "\t" + "Alt+F4",
                "Quit the application", wxITEM_NORMAL );
	wxMenuItem* menuHelpAbout = new wxMenuItem( helpMenu, wxID_ANY,
                wxString( "&About" ) + "\t" + "F1",
                "Show info about this application", wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
    helpMenu->Append( menuHelpAbout );
	wxMenuBar* mbar = new wxMenuBar( 0 );

	mbar->Append( fileMenu, wxT("&File") );
	mbar->Append( helpMenu, wxT("&Help") );

	this->SetMenuBar( mbar );
	this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );

	//create the main panel
	wxPanel* mainpanel = new wxPanel(this);
	wxBoxSizer* mainizer = new wxBoxSizer( wxHORIZONTAL );

	//Set up the left side of the frame:
	wxAlphaIndex* alphaindex = new wxAlphaIndex(mainpanel,wxID_ANY);
	alphaindex->AddDefaultLetters();
	alphaindex->SetBorderColour(m_brdrColour);
	alphaindex->SetBackgroundColour(m_bgColour);
	alphaindex->SetHLColour(m_alIndHLColour);
	alphaindex->SetTextColour(m_alIndTextColour);
	mainizer->Add( alphaindex, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxPanel* leftborder = new wxPanel(mainpanel);
	leftborder->SetOwnBackgroundColour(m_brdrColour);
	wxBoxSizer* leftbordersizer = new wxBoxSizer( wxVERTICAL );

	m_vlistPanel = new wxPanel( leftborder );
	m_vlistPanel->SetBackgroundColour(m_bgColour);
	wxBoxSizer* vlistpanelsizer = new wxBoxSizer( wxVERTICAL );

	m_searchCtrl = new wxSearchCtrl( m_vlistPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	#ifndef __WXMAC__
	m_searchCtrl->ShowSearchButton( true );
	#endif
	m_searchCtrl->ShowCancelButton( false );
	vlistpanelsizer->Add( m_searchCtrl, 0, wxEXPAND|wxALL, 5 );

	m_vlist = new myVListBox(m_vlistPanel,wxID_ANY, wxDefaultPosition, wxDefaultSize,wxBORDER_NONE) ;
	m_vlist->InitData( alphaindex->GetLetters() );
	vlistpanelsizer->Add( m_vlist, 1, wxEXPAND, 5 );

	m_searchResults = new wxPanel( m_vlistPanel );
	m_searchResults->Hide();
	wxBoxSizer* searchresultssizer = new wxBoxSizer( wxVERTICAL );
	searchresultssizer->Add( 0, 0, 1, wxEXPAND, 5 );
	wxString resulttext="No results found\n\n(The search control is \n"
	"only from demonstration \npurposes.  It won't \nreturn any results.)";
	wxStaticText* searchresult = new wxStaticText( m_searchResults, wxID_ANY,
                 resulttext, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	searchresultssizer->Add( searchresult, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	searchresultssizer->Add( 0, 0, 1, wxEXPAND, 5 );
	m_searchResults->SetSizer( searchresultssizer );

	vlistpanelsizer->Add( m_searchResults, 1, wxEXPAND | wxALL, 5 );
	m_vlistPanel->SetSizer( vlistpanelsizer );
	leftbordersizer->Add( m_vlistPanel, 1, wxEXPAND | wxALL, 1 );
	leftborder->SetSizer( leftbordersizer );

	mainizer->Add( leftborder, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

    //Set up the right side of the frame:
	wxPanel* rightborder = new wxPanel(mainpanel);
	rightborder->SetOwnBackgroundColour(m_brdrColour);
	wxBoxSizer* rightbordersizer = new wxBoxSizer( wxVERTICAL );

	wxPanel* rightpanel = new wxPanel( rightborder );
	rightpanel->SetBackgroundColour(m_bgColour);
	wxBoxSizer* rightpanelsizer = new wxBoxSizer( wxVERTICAL );

	m_profilePicPanel = new wxPanel( rightpanel, wxID_ANY, wxDefaultPosition, wxSize( 220,220 ), wxTAB_TRAVERSAL );
	rightpanelsizer->Add( m_profilePicPanel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxScrolledWindow* scrwin = new wxScrolledWindow( rightpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	scrwin->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText* namestatictest = new wxStaticText( scrwin, wxID_ANY, "Full Name:");
	m_nameTextCtrl = new wxTextCtrl( scrwin, wxID_ANY );
	wxStaticText* phonestatictest = new wxStaticText( scrwin, wxID_ANY, "Phone:" );
	wxTextCtrl* phonetextctrl = new wxTextCtrl( scrwin, wxID_ANY );
	wxStaticText* emailstatictest = new wxStaticText( scrwin, wxID_ANY, "Email:" );
	wxTextCtrl* emailtextctrl = new wxTextCtrl( scrwin, wxID_ANY);
	wxButton* m_updateButton = new wxButton( scrwin, wxID_ANY, "Update" );

    fgSizer1->Add( namestatictest, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxLEFT, 5 );
	fgSizer1->Add( m_nameTextCtrl, 0, wxALL|wxEXPAND, 5 );
	fgSizer1->Add( phonestatictest, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxLEFT, 5 );
    fgSizer1->Add( phonetextctrl, 0, wxALL|wxEXPAND, 5 );
	fgSizer1->Add( emailstatictest, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxTOP|wxBOTTOM|wxLEFT, 5 );
	fgSizer1->Add( emailtextctrl, 0, wxALL|wxEXPAND, 5 );
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	fgSizer1->Add( m_updateButton, 0, wxALL|wxALIGN_RIGHT, 5 );

	scrwin->SetSizer( fgSizer1 );
	rightpanelsizer->Add( scrwin, 1, wxEXPAND | wxALL, 5 );

	rightpanel->SetSizer( rightpanelsizer );
	rightbordersizer->Add( rightpanel, 1, wxEXPAND | wxALL, 1 );
	rightborder->SetSizer( rightbordersizer );

	mainizer->Add( rightborder, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	//finish setting up the main panel
	mainpanel->SetSizer( mainizer );
	mainpanel->Layout();

    //bind the event handlers
	this->Bind( wxEVT_COMMAND_MENU_SELECTED, &ContactsFrame::OnQuit, this, menuFileQuit->GetId() );
	this->Bind( wxEVT_COMMAND_MENU_SELECTED, &ContactsFrame::OnAbout, this, menuHelpAbout->GetId() );
	m_searchCtrl->Bind( wxEVT_COMMAND_SEARCHCTRL_CANCEL_BTN, &ContactsFrame::OnSearchCancelButton, this );
	m_searchCtrl->Bind( wxEVT_COMMAND_SEARCHCTRL_SEARCH_BTN, &ContactsFrame::OnSearch, this );
	m_searchCtrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &ContactsFrame::OnSearch, this );
	m_profilePicPanel->Bind( wxEVT_PAINT, &ContactsFrame::OnPaintProfilePic, this );
	m_updateButton->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &ContactsFrame::OnB1 , this );
    alphaindex->Bind( wxEVT_ALPHA_INDEX_CLICK,  &ContactsFrame::OnAlphaIndex , this );
    m_vlist->Bind( wxEVT_LISTBOX, &ContactsFrame::OnList , this );
}

ContactsFrame::~ContactsFrame()
{
}

void ContactsFrame::OnPaintProfilePic(wxPaintEvent& WXUNUSED(event))
{
    int i=m_vlist->GetSelection();

    if(i!=wxNOT_FOUND)
    {
        wxPaintDC dc(m_profilePicPanel);
        dc.DrawBitmap(m_vlist->GetProfilePicture(i),0,0);
    }
}

void ContactsFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Destroy();
}

void ContactsFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("A wxAlphaIndex demo.", _("Welcome to..."));
}

void ContactsFrame::OnList(wxCommandEvent& event)
{
    m_nameTextCtrl->SetValue(m_vlist->GetName(event.GetInt()));
    m_profilePicPanel->Refresh();
}

void ContactsFrame::SearchHelper(bool b)
{
    m_vlistPanel->Freeze();
    m_searchCtrl->ShowCancelButton(!b);
    m_searchResults->Show(!b);
    m_vlist->Show(b);
    m_vlistPanel->Layout();
    m_vlistPanel->Thaw();
}

void ContactsFrame::OnSearchCancelButton( wxCommandEvent& WXUNUSED(event))
{
    SearchHelper(true);
}

void ContactsFrame::OnSearch( wxCommandEvent& WXUNUSED(event))
{
    SearchHelper(false);
}

void ContactsFrame::OnAlphaIndex(wxCommandEvent& event)
{
    m_vlist->ScrollToLetter(event.GetString());
}

void ContactsFrame::OnB1(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("The update button doesn't do anything.", _("Sorry..."));
}


///////////////////////////////////////////////////////////////////////////////
/// Class ContactsFrame
///////////////////////////////////////////////////////////////////////////////

class myApp : public wxApp
{
    public:
        virtual bool OnInit()
        {
            ContactsFrame* frame = new ContactsFrame(0L);
            frame->SetIcon(wxICON(aaaa)); // To Set App Icon
            frame->Show();

            return true;
        }
};

wxIMPLEMENT_APP(myApp);


///////////////////////////////////////////////////////////////////////////////
// static data used for the vlist
// names genetated from listofrandomnames.com
///////////////////////////////////////////////////////////////////////////////

size_t myVListBox::m_totalItems=120;

wxColour myVListBox::m_profileColour1 = wxColour(171,183,199);
wxColour myVListBox::m_profileColour2 = wxColour(199,179,146);
wxColour myVListBox::m_profileColour3 = wxColour(160,166,140);
wxColour myVListBox::m_profileColour4 = wxColour(207,173,164);

wxString myVListBox::m_firstNames[120]={
    "Audry",
    "Marx",
    "Sheridan",
    "Marietta",
    "Carmina",
    "Carmela",
    "Gretta",
    "Terisa",
    "Ike",
    "Kami",
    "Anjanette",
    "Adeline",
    "Raymon",
    "Tammara",
    "Karyn",
    "Martin",
    "Adaline",
    "Galina",
    "Loree",
    "Alpha",
    "Katherine",
    "Fe",
    "Fatimah",
    "Margit",
    "Betty",
    "Patrice",
    "Elissa",
    "Barbie",
    "Brett",
    "Jolyn",
    "Lon",
    "Tommy",
    "Jessie",
    "Mila",
    "Stevie",
    "Charlena",
    "Felicia",
    "Lisette",
    "Sang",
    "Parthenia",
    "Ilse",
    "Brock",
    "Malorie",
    "Eufemia",
    "Denny",
    "Jacalyn",
    "Lashawn",
    "Millard",
    "Tanner",
    "Alexander",
    "Ayesha",
    "Willow",
    "Dalton",
    "Coletta",
    "Estela",
    "Renee",
    "Allison",
    "Hosea",
    "Carolina",
    "Johanna",
    "Winford",
    "Latasha",
    "Sanda",
    "Antonia",
    "Felton",
    "Aleida",
    "Hayden",
    "Hannelore",
    "Rikki",
    "Hortencia",
    "Loralee",
    "Zulema",
    "Catrina",
    "Teofila",
    "Theressa",
    "Burl",
    "Felicidad",
    "Giselle",
    "Shantel",
    "Stephania",
    "Cole",
    "Warren",
    "Kristen",
    "Garnett",
    "Marguerita",
    "Shelby",
    "Myrle",
    "Cristie",
    "Ladawn",
    "Brande",
    "Betsey",
    "Germaine",
    "Lakenya",
    "Theodore",
    "Carlos",
    "Roxie",
    "Burma",
    "Romelia",
    "Joellen",
    "Billy",
    "Ione",
    "Daria",
    "Tatum",
    "Livia",
    "Helene",
    "Dominica",
    "Bridgette",
    "Clora",
    "Bryon",
    "Milissa",
    "Lillian",
    "Erica",
    "Sharice",
    "Naida",
    "Christie",
    "Eleni",
    "Hosea",
    "Peggie",
    "Kindra",
    "Mi"
};

wxString myVListBox::m_lastNames[120]={
    "Abad",
    "Ackley",
    "Addario",
    "Albarran",
    "Alber",
    "Albertson",
    "Antoine",
    "Beauchesne",
    "Belden",
    "Bookout",
    "Braga",
    "Browne",
    "Busse",
    "Cancel",
    "Capron",
    "Cerrato",
    "Chaffins",
    "Claar",
    "Clark",
    "Collis",
    "Coria",
    "Defalco",
    "Diller",
    "Dingee",
    "Dizon",
    "Donadio",
    "Duer",
    "Dugan",
    "Dukes",
    "Edds",
    "Edmisten",
    "Eidem",
    "Fedor",
    "Fontes",
    "Gebo",
    "Gott",
    "Hackney",
    "Hagerman",
    "Haider",
    "Hannold",
    "Havens",
    "Haycock",
    "Hayford",
    "Hettinger",
    "Holtz",
    "Honda",
    "Hudock",
    "Hulings",
    "Irvine",
    "Isabell",
    "Jerez",
    "Jessee",
    "Justiniano",
    "Keep",
    "Kempker",
    "Kim",
    "Kleiner",
    "Lahman",
    "Liberatore",
    "Lockridge",
    "Lovejoy",
    "Lush",
    "Maclaren",
    "Maire",
    "Malachi",
    "Mast",
    "Mathias",
    "Mattia",
    "Mccardle",
    "Mccaskill",
    "Mccaskill",
    "Mcmartin",
    "Menges",
    "Moran",
    "Narducci",
    "Obryant",
    "Oestreich",
    "Ort",
    "Pazos",
    "Pendergast",
    "Petry",
    "Pettiway",
    "Polinsky",
    "Procopio",
    "Ray",
    "Reader",
    "Remington",
    "Reta",
    "Reynaga",
    "Ritchie",
    "Rochon",
    "Rohan",
    "Savoy",
    "Shahid",
    "Sidwell",
    "Simcox",
    "Southard",
    "Sprowl",
    "Steinhauser",
    "Stillings",
    "Stockard",
    "Strum",
    "Sublett",
    "Syed",
    "Tews",
    "Towns",
    "Umstead",
    "Valenti",
    "Vanderploeg",
    "Vicario",
    "Vidaurri",
    "Waldman",
    "Waldrop",
    "Weatherford",
    "Wehrle",
    "Whitney",
    "Widner",
    "Wilt",
    "Zielke",
    "Zoeller"
};

int myVListBox::m_bgColours[120]={
    1,
    1,
    3,
    3,
    1,
    3,
    0,
    0,
    0,
    2,
    2,
    3,
    0,
    0,
    3,
    2,
    0,
    2,
    2,
    3,
    0,
    0,
    1,
    2,
    2,
    0,
    0,
    3,
    0,
    0,
    0,
    0,
    2,
    3,
    2,
    0,
    3,
    3,
    2,
    2,
    0,
    1,
    2,
    0,
    0,
    0,
    0,
    3,
    2,
    3,
    0,
    0,
    1,
    0,
    0,
    2,
    0,
    0,
    0,
    3,
    1,
    0,
    3,
    3,
    2,
    3,
    3,
    1,
    1,
    0,
    1,
    3,
    1,
    2,
    1,
    1,
    1,
    3,
    0,
    2,
    0,
    2,
    3,
    1,
    1,
    0,
    0,
    1,
    3,
    1,
    0,
    0,
    1,
    0,
    0,
    0,
    2,
    0,
    2,
    2,
    1,
    3,
    1,
    2,
    0,
    3,
    2,
    0,
    2,
    1,
    0,
    2,
    0,
    1,
    3,
    0,
    0,
    0,
    1,
    1
};

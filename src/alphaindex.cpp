/***************************************************************
 * Name:      alphaindex.cpp
 * Purpose:   code for wxAlphaIndex
 * Author:     ()
 * Created:   2016-05-15
 * Copyright:  ()
 * License:   wxWindows licence
 **************************************************************/

#include <wx/alphaindex/alphaindex.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>

wxIMPLEMENT_CLASS(wxAlphaIndex, wxWindow)
wxDEFINE_EVENT(wxEVT_ALPHA_INDEX_CLICK, wxCommandEvent);

wxAlphaIndex::wxAlphaIndex(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    Create(parent, id, pos, size, style,name);
}

wxAlphaIndex::wxAlphaIndex()
{
}

bool wxAlphaIndex::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    if(!wxWindow::Create(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE,name))
    {
        return false;
    }

    Init();
    return true;
}

void wxAlphaIndex::Init()
{
    m_separatorColour=wxColour(192,192,192);
    m_borderColour=*wxBLACK;
    m_textColour=*wxBLACK;
    m_hlTextColour=*wxWHITE;
    m_hlColour=*wxBLACK;

    m_padding=2;
    m_letterHt=0;
    m_letterWd=0;
    m_lastSlot=-1;

    SetBackgroundStyle( wxBG_STYLE_PAINT);

    SetSizes();
    Cache();

	Bind( wxEVT_LEAVE_WINDOW, &wxAlphaIndex::OnLeaveWindow, this );
	Bind( wxEVT_LEFT_UP, &wxAlphaIndex::OnLeftUp, this );
	Bind( wxEVT_MOTION, &wxAlphaIndex::OnMotion, this );
	Bind( wxEVT_PAINT, &wxAlphaIndex::OnPaint, this );
	Bind( wxEVT_SIZE, &wxAlphaIndex::OnSize, this );
}

bool wxAlphaIndex::SetFont(const wxFont &font)
{
    bool b = wxWindow::SetFont(font);

    ComputeLetterSizes();
    SetSizes();
    Cache();
    return b;
}

void wxAlphaIndex::SetPadding(int i)
{
    m_padding=i;
    SetSizes();
    Cache();
}

void wxAlphaIndex::SetBorderColour(const wxColour& c)
{
    m_borderColour=c;
}

void wxAlphaIndex::SetSeparatorColour(const wxColour& c)
{
    m_separatorColour=c;
}

void wxAlphaIndex::SetTextColour(const wxColour& c)
{
    m_textColour=c;
}

void wxAlphaIndex::SetHLColour(const wxColour& c)
{
    m_hlColour=c;
}

void wxAlphaIndex::SetHLTextColour(const wxColour& c)
{
    m_hlTextColour=c;
}

int wxAlphaIndex::GetPadding()
{
    return m_padding;
}

wxColour wxAlphaIndex::GetBorderColour()
{
    return m_borderColour;
}

wxColour wxAlphaIndex::GetSeparatorColour()
{
    return m_separatorColour;
}

wxColour wxAlphaIndex::GetTextColour()
{
    return m_textColour;
}

wxColour wxAlphaIndex::GetHLColour()
{
    return m_hlColour;
}

wxColour wxAlphaIndex::GetHLTextColour()
{
    return m_hlTextColour;
}

void wxAlphaIndex::AddLetter(const wxString& s, int order, int freq)
{
    //order and freq need to be unique.  If not, the letter is rejected.
    if(letter_builder.find(order)!=letter_builder.end())
    {
        wxASSERT_MSG(false,"order was not unique" );
        return;
    }

    if(freq_builder.find(freq)!=freq_builder.end())
    {
        wxASSERT_MSG(false,"freq was not unique" );
        return;
    }

    letter_builder[order]=wxString(s);
    freq_builder[freq]=order;
}

void wxAlphaIndex::RealizeLetters()
{
    letters.clear();
    frequencies.clear();

    for(std::map<int,wxString>::iterator it=letter_builder.begin();it!=letter_builder.end();it++)
    {
        letters.push_back(it->second);
    }

    for(std::map<int,int>::iterator it=freq_builder.begin();it!=freq_builder.end();it++)
    {
        frequencies.push_back(it->second);
    }

    ComputeLetterSizes();
    SetSizes();
    Cache();

    letter_builder.clear();
    freq_builder.clear();
}

void wxAlphaIndex::ClearLetters()
{
    letter_widths.clear();
    letters.clear();
    frequencies.clear();
    available_letters.clear();
    ends.clear();
    letter_builder.clear();
    freq_builder.clear();

    m_letterHt=-1;
    m_letterWd=-1;
}

void wxAlphaIndex::AddDefaultLetters()
{
    ClearLetters();
    AddLetter("#",0,0);
    AddLetter("A",1,2);
    AddLetter("B",2,8);
    AddLetter("C",3,11);
    AddLetter("D",4,13);
    AddLetter("E",5,16);
    AddLetter("F",6,10);
    AddLetter("G",7,17);
    AddLetter("H",8,4);
    AddLetter("I",9,6);
    AddLetter("J",10,22);
    AddLetter("K",11,23);
    AddLetter("L",12,12);
    AddLetter("M",13,9);
    AddLetter("N",14,15);
    AddLetter("O",15,7);
    AddLetter("P",16,14);
    AddLetter("Q",17,24);
    AddLetter("R",18,18);
    AddLetter("S",19,3);
    AddLetter("T",20,1);
    AddLetter("U",21,20);
    AddLetter("V",22,21);
    AddLetter("W",23,5);
    AddLetter("X",24,26);
    AddLetter("Y",25,19);
    AddLetter("Z",26,25);
    RealizeLetters();
}

void wxAlphaIndex::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC(this).DrawBitmap(m_cachedBG,0,0,true);
}

void wxAlphaIndex::OnLeftUp( wxMouseEvent& event )
{
    //if no letters have been added or the space available
    //isn't large enough to show any letters, there's no need to continue
    if(letters.size()>0 && available_letters.size()>0)
    {
        int i=GetSlot(event.GetPosition().y);

        if(i!=-1)
        {
            wxCommandEvent event( wxEVT_ALPHA_INDEX_CLICK, GetId() );
            event.SetEventObject(this);
            event.SetInt( available_letters[i] );
            event.SetExtraLong( i );
            event.SetString( letters[available_letters[i]] );
            ProcessWindowEvent(event);
        }
    }
}

void wxAlphaIndex::DrawLetter(wxDC* dc, int slot, int start, int endd, int wd, int right_adjust)
{
    //we need to draw a letter both to the bitmap in the cache function and
    //to the overlay in the motion function.  Putting this here ensures the
    //drawing is done the same way in both places.
    int cur_letter=available_letters[slot];
    int offset_y = (endd-start-m_letterHt)/2;
    int offset_x = (wd-letter_widths[cur_letter])/2;

    dc->DrawText(letters[cur_letter],offset_x+right_adjust,start+offset_y);
}

void wxAlphaIndex::OnMotion( wxMouseEvent& event )
{
    int slot=GetSlot(event.GetPosition().y);

    if( slot!=-1 && slot!=m_lastSlot )
    {
        wxClientDC dc( this );
        wxDCOverlay overlaydc( m_overlay, &dc );
        overlaydc.Clear();

        dc.SetTextForeground(m_hlTextColour);
        dc.SetBrush( wxBrush(m_hlColour) );
        dc.SetPen( wxPen(m_hlColour) );

        int right_adjust = HasFlag(wxALPHA_INDEX_RIGHT)?0:1;
        int start = (slot==0)?0:ends[slot-1];
        int endd = ends[slot];
        int wd = GetSize().GetWidth();

        //This should really be:
        //
        //dc.DrawRectangle(wxPoint(right_adjust,start+1),wxSize(wd-1,endd-start-1));
        //
        //when on the left, this will leave the rightmost pixels unhighlighted
        //but if we draw those pixels, the clear call to the overlay will not
        //clear them.  That might be a bug in wxWidgets.
        dc.DrawRectangle(wxPoint(right_adjust,start+1),wxSize(wd-1-right_adjust,endd-start-1));
        DrawLetter( &dc, slot, start, endd, wd, right_adjust);
        m_lastSlot=slot;
    }
}

void wxAlphaIndex::OnLeaveWindow( wxMouseEvent& WXUNUSED(event) )
{
    if(m_lastSlot!=-1)
    {
        wxClientDC dc( this );
        wxDCOverlay overlaydc( m_overlay, &dc );
        overlaydc.Clear();
        //m_overlay.Reset();
    }

    m_lastSlot=-1;
}

void wxAlphaIndex::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    Cache();
}

void wxAlphaIndex::ComputeLetterSizes()
{
    int t_width,t_height;
    letter_widths.clear();
    m_letterWd=-1;
    m_letterHt=-1;

    for(std::vector<wxString>::iterator it =letters.begin();it!=letters.end();it++ )
    {
        GetTextExtent( *it, &t_width, &t_height );
        if(t_height>m_letterHt) m_letterHt=t_height;
        if(t_width>m_letterWd) m_letterWd=t_width;
        letter_widths.push_back(t_width);
    }
}

void wxAlphaIndex::SetSizes()
{
    int width = m_letterWd+2*m_padding+1;
    SetMaxSize(wxSize(width, -1));
    SetMinSize(wxSize(width, -1));
    SetSize(width, -1);
}

int wxAlphaIndex::GetSlot(int y)
{
    int i(0);
    bool found(false);

    //we could maybe speed this up by doing a binary search, but there
    //shouldn't be all that many letters anyway.
    for(std::vector<int>::iterator it=ends.begin();it!=ends.end();it++)
    {
        if(*it>=y)
        {
            found=true;
            break;
        }

        i++;
    }

    return found?i:-1;
}

void wxAlphaIndex::Cache()
{
    int wd,ht;
    GetSize(&wd,&ht);

    if(wd<1 || ht<1)
    {
        return;
    }

    bool on_right = HasFlag(wxALPHA_INDEX_RIGHT);
    int total_letters = letters.size();

    m_cachedBG = wxBitmap(wd,ht);
    wxMemoryDC dc(m_cachedBG);

    dc.SetBackground( wxBrush(GetBackgroundColour()) );
    dc.Clear();
    dc.SetFont(GetFont());

    //if no letters have been added, there's no need to draw anything but the border
    if(total_letters>0)
    {
        //lim is the number of letters that can be drawn in the space available.
        int lim = (ht-1)/(m_letterHt+1);  //only ht-1 pixels should really be available
        if(lim>total_letters)
        {
            lim=total_letters;
        }

        ends.clear();
        if(lim!=0)
        {
            //if the number of letters to be drawn has changed, rebuild
            //the available_letters vec.
            if( lim!=static_cast<int>(available_letters.size()) )
            {
                available_letters.clear();

                for(int i=0;i<lim;i++)
                {
                     available_letters.push_back(frequencies[i]);
                }

                std::sort( available_letters.begin(), available_letters.end() );
            }

            double part_dist=static_cast<double>(ht-1)/static_cast<double>(lim);
            int right_adjust = on_right?0:1;
            int last = 0;
            int next;

            dc.SetPen(m_separatorColour);
            dc.SetTextForeground(m_textColour);

            for(int i=0;i<lim;i++)
            {
                next = wxRound(part_dist*(i+1));
                DrawLetter( &dc, i, last, next, wd, right_adjust);
                dc.DrawLine(1+right_adjust,next,wd-2+right_adjust,next);

                ends.push_back(next);
                last = next;
            }
        }
        else
        {
            //if we can't draw any letters, clear the available_letters
            //vec.  This will block any clicks from being processed.
            available_letters.clear();
        }
    }

    //draw the border
    dc.SetPen(m_borderColour);
    dc.DrawLine(0,0,wd,0);
    int border_loc=on_right?wd-1:0;
    dc.DrawLine(border_loc,0,border_loc,ht-1);
    dc.DrawLine(0,ht-1,wd,ht-1);
}

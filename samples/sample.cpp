/***************************************************************
 * Name:      sample.cpp
 * Purpose:   sample for wxAlphaIndex
 * Author:     ()
 * Created:   2016-05-15
 * Copyright:  ()
 * License:   wxWindows licence
 **************************************************************/

#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/alphaindex/alphaindex.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/app.h>


///////////////////////////////////////////////////////////////////////////////
/// Class LetterPanel - a self destructing popup window that shows a letter
///////////////////////////////////////////////////////////////////////////////

class LetterPanel: public wxPanel
{
    public:
        LetterPanel(wxWindow* parent,const wxString& letter,
                    int fontsize=12,int padding=2);

    private:
        void OnPaint( wxPaintEvent& event );
        void OnTimer(wxTimerEvent& event);
        wxString m_letter;
        wxTimer m_timer;
        int m_padding;
};

LetterPanel::LetterPanel( wxWindow* parent, const wxString& letter,
                         int fontsize, int padding )
:wxPanel(parent,wxID_ANY),m_timer(this),m_letter(letter),m_padding(padding)
{
    int ht,wd;
    wxFont f = GetFont();

    f.SetPointSize(fontsize);
    SetFont(f);
    GetTextExtent(letter,&wd,&ht);
    SetSize(wd+2*padding,ht+2*padding);

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind( wxEVT_PAINT, &LetterPanel::OnPaint, this );
    Bind( wxEVT_TIMER, &LetterPanel::OnTimer, this);

    CentreOnParent();
    m_timer.Start(1500, wxTIMER_ONE_SHOT);
}

void LetterPanel::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    dc.SetBackground(*wxRED_BRUSH);
    dc.Clear();
    dc.DrawText(m_letter,m_padding,m_padding);
}

void LetterPanel::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    Destroy();
}

///////////////////////////////////////////////////////////////////////////////
/// Class SampleFrame
///////////////////////////////////////////////////////////////////////////////

class SampleFrame: public wxFrame
{
    public:
        SampleFrame( wxWindow* parent, wxWindowID id = wxID_ANY,
                    const wxString& title = "wxAlphaIndex Sample",
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxSize( 481,466 ),
                    long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    private:
        //event handlers
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnAlphaIndex2(wxCommandEvent& event);
        void OnAlphaIndex1(wxCommandEvent& event);
        void OnPaintPanel( wxPaintEvent& event );

        //helpers
        void SetUpListBox();

        //private data
		wxListBox* m_listBox;
		wxPanel* m_panel;
        int m_scrollto[27];
};

SampleFrame::SampleFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, long style )
: wxFrame( parent, id, title, pos, size, style )
{
	//Set up the menu and the status bar
	wxMenuBar* mbar = new wxMenuBar( 0 );

	wxMenu* fileMenu = new wxMenu();
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, wxID_ANY, "&Quit\tAlt+F4",
                                   "Quit the application", wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
	mbar->Append( fileMenu, "&File" );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, wxID_ANY,  "&About\tF1",
                            "Show info about this application", wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );
	mbar->Append( helpMenu, "&Help" );

	this->SetMenuBar( mbar );

	this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );

	//add a panel to serve as the frame's background
	wxPanel* bg = new wxPanel( this, wxID_ANY);
	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );

	//add the first alpha index and a list control
	wxAlphaIndex* alphaindex1 = new wxAlphaIndex(bg,wxID_ANY);
    alphaindex1->AddDefaultLetters();
    alphaindex1->SetBorderColour( wxColour(127,157,185) );
    alphaindex1->SetBackgroundColour( wxColour(252,252,252) );
    alphaindex1->SetHLColour( wxColour(49,106,197) );
	bSizer->Add( alphaindex1, 0, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	m_listBox = new wxListBox( bg, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	SetUpListBox();
	bSizer->Add( m_listBox, 1, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	//add a panel and the second alpha index to the right.
	m_panel = new wxPanel( bg, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE );
	m_panel->SetBackgroundStyle(wxBG_STYLE_PAINT);
	bSizer->Add( m_panel, 1, wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );

	wxAlphaIndex* alphaindex2 = new wxAlphaIndex(bg, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALPHA_INDEX_RIGHT);

    // This will add the greek letters with random frequencies.
    // The frequencies are litterally a random permutation.
    // I have no clue what a good order would be.
    int freqs[]={22,4,7,16,15,0,2,6,14,19,9,11,5,17,8,12,21,1,23,20,24,18,13,3,10};
    int c = 0x03b1;
    for(int i=0;i<25;i++)
    {
        //U+03C2 is an alternate version of sigma, we'll skip it so that there
        //is only 1 sigma in the list
        if(i<17)
        {
            alphaindex2->AddLetter(wxUniChar(c+i),i,freqs[i]);
        }
        else if(i>17)
        {
            alphaindex2->AddLetter(wxUniChar(c+i),i-1,freqs[i-1]);
        }
    }
    alphaindex2->RealizeLetters();
    alphaindex2->SetBorderColour(wxColour(255,0,0));
    alphaindex2->SetTextColour(wxColour(0,255,0));
    alphaindex2->SetSeparatorColour(wxColour(192,192,192));
    alphaindex2->SetBackgroundColour(wxColour(0,0,255));

	bSizer->Add( alphaindex2, 0, wxEXPAND|wxTOP|wxBOTTOM|wxRIGHT, 5  );

	bg->SetSizer( bSizer );
	bg->Layout();

	// Connect Events
	this->Bind( wxEVT_COMMAND_MENU_SELECTED, &SampleFrame::OnQuit, this, menuFileQuit->GetId() );
	this->Bind( wxEVT_COMMAND_MENU_SELECTED,  &SampleFrame::OnAbout, this, menuHelpAbout->GetId() );
	m_panel->Bind( wxEVT_PAINT, &SampleFrame::OnPaintPanel , this );
    alphaindex1->Bind( wxEVT_ALPHA_INDEX_CLICK,  &SampleFrame::OnAlphaIndex1 , this );
    alphaindex2->Bind( wxEVT_ALPHA_INDEX_CLICK,  &SampleFrame::OnAlphaIndex2 , this );
}

void SampleFrame::SetUpListBox()
{
    m_listBox->Append("1st Ray");
    m_listBox->Append("2-Faced");
    m_listBox->Append("8 eye");
    m_listBox->Append("Acrophies");
    m_listBox->Append("Adamantaimai");
    m_listBox->Append("Aero Combatant");
    m_listBox->Append("Air Buster");
    m_listBox->Append("Allemagne");
    m_listBox->Append("Ancient Dragon");
    m_listBox->Append("Aps");
    m_listBox->Append("Ark Dragon");
    m_listBox->Append("Armored Golem");
    m_listBox->Append("Attack Squad");
    m_listBox->Append("Bad Rap");
    m_listBox->Append("Bad Rap Sample");
    m_listBox->Append("Bagnadrana");
    m_listBox->Append("Bagrisk");
    m_listBox->Append("Bahba Velamyu");
    m_listBox->Append("Bandersnatch");
    m_listBox->Append("Bandit");
    m_listBox->Append("Battery Cap");
    m_listBox->Append("Beachplug");
    m_listBox->Append("Behemoth");
    m_listBox->Append("Bizarre Bug");
    m_listBox->Append("Bizarro*Sephiroth");
    m_listBox->Append("Black Bat");
    m_listBox->Append("Bloatfloat");
    m_listBox->Append("Blood Taste");
    m_listBox->Append("Blue Dragon");
    m_listBox->Append("Blugu");
    m_listBox->Append("Bomb");
    m_listBox->Append("Bottomswell");
    m_listBox->Append("Boundfat");
    m_listBox->Append("Brain Pod");
    m_listBox->Append("Bullmotor");
    m_listBox->Append("Cactuar");
    m_listBox->Append("Cactuer");
    m_listBox->Append("Capparwire");
    m_listBox->Append("Captain");
    m_listBox->Append("Carry Armor");
    m_listBox->Append("Castanets");
    m_listBox->Append("Ceasar");
    m_listBox->Append("Chekov");
    m_listBox->Append("Chocobo");
    m_listBox->Append("Christopher");
    m_listBox->Append("Chuse Tank");
    m_listBox->Append("CMD.Grand Horn");
    m_listBox->Append("Cokatolis");
    m_listBox->Append("Corneo's Lackey");
    m_listBox->Append("Corvette");
    m_listBox->Append("Crawler");
    m_listBox->Append("Crazy Saw");
    m_listBox->Append("Cripshay");
    m_listBox->Append("Cromwell");
    m_listBox->Append("Crown Lance");
    m_listBox->Append("Crysales");
    m_listBox->Append("Cuahl");
    m_listBox->Append("Custom Sweeper");
    m_listBox->Append("Dark Dragon");
    m_listBox->Append("Dark Nation");
    m_listBox->Append("Death Claw");
    m_listBox->Append("Death Dealer");
    m_listBox->Append("Death Machine");
    m_listBox->Append("Deenglow");
    m_listBox->Append("Demons Gate");
    m_listBox->Append("Desert Sahagin");
    m_listBox->Append("Devil Ride");
    m_listBox->Append("Diablo");
    m_listBox->Append("Diamond Weapon");
    m_listBox->Append("Diver Nest");
    m_listBox->Append("Doorbull");
    m_listBox->Append("Dorky Face");
    m_listBox->Append("Dragon");
    m_listBox->Append("Dragon Rider");
    m_listBox->Append("Dragon Zombie");
    m_listBox->Append("Dual Horn");
    m_listBox->Append("Dyne");
    m_listBox->Append("Eagle Gun");
    m_listBox->Append("Edgehead");
    m_listBox->Append("Elfadunk");
    m_listBox->Append("Eligor");
    m_listBox->Append("Emerald Weapon");
    m_listBox->Append("Epiolnis");
    m_listBox->Append("Evilhead");
    m_listBox->Append("Eye");
    m_listBox->Append("Flapbeat");
    m_listBox->Append("Flower Prong");
    m_listBox->Append("Formula");
    m_listBox->Append("Foulander");
    m_listBox->Append("Frozen Nail");
    m_listBox->Append("Gagighandi");
    m_listBox->Append("Gargoyle");
    m_listBox->Append("Garuda");
    m_listBox->Append("Gas Ducter");
    m_listBox->Append("Ghirofelgo");
    m_listBox->Append("Ghost");
    m_listBox->Append("Ghost Ship");
    m_listBox->Append("Gi Nattak");
    m_listBox->Append("Gi Spector");
    m_listBox->Append("Gigas");
    m_listBox->Append("Gighee");
    m_listBox->Append("Goblin");
    m_listBox->Append("Godo");
    m_listBox->Append("Golem");
    m_listBox->Append("Gorkii");
    m_listBox->Append("Grand Horn");
    m_listBox->Append("Grangalan");
    m_listBox->Append("Grangalan Jr.");
    m_listBox->Append("Grangalan Jr.Jr.");
    m_listBox->Append("Grashstrike");
    m_listBox->Append("Gremlin");
    m_listBox->Append("Grenade");
    m_listBox->Append("Grenade Combatant");
    m_listBox->Append("Griffin");
    m_listBox->Append("Grimguard");
    m_listBox->Append("Grosspanzer*Big");
    m_listBox->Append("Grosspanzer*Mobile");
    m_listBox->Append("Grosspanzer*Small");
    m_listBox->Append("Grunt");
    m_listBox->Append("Guard Hound");
    m_listBox->Append("Guard Scorpion");
    m_listBox->Append("Guard System");
    m_listBox->Append("Guardian");
    m_listBox->Append("Gun Carrier");
    m_listBox->Append("Hammer Blaster");
    m_listBox->Append("Hard Attacker");
    m_listBox->Append("Harpy");
    m_listBox->Append("Head Hunter");
    m_listBox->Append("Headbomber");
    m_listBox->Append("Heavy Tank");
    m_listBox->Append("Hedgehog Pie");
    m_listBox->Append("Heg");
    m_listBox->Append("Heli Gunner");
    m_listBox->Append("Hell House");
    m_listBox->Append("Hell Rider VR2");
    m_listBox->Append("Helletic Hojo");
    m_listBox->Append("Hippogriff");
    m_listBox->Append("Ho-chu");
    m_listBox->Append("Hojo");
    m_listBox->Append("Hundred Gunner");
    m_listBox->Append("Hungry");
    m_listBox->Append("Ice Golem");
    m_listBox->Append("Icicle");
    m_listBox->Append("Iron Man");
    m_listBox->Append("Ironite");
    m_listBox->Append("Jamar Armor");
    m_listBox->Append("Jayjujayme");
    m_listBox->Append("Jemnezmy");
    m_listBox->Append("Jenova*BIRTH");
    m_listBox->Append("Jenova*DEATH");
    m_listBox->Append("Jenova*LIFE");
    m_listBox->Append("Jenova*SYNTHESIS");
    m_listBox->Append("Jersey");
    m_listBox->Append("Joker");
    m_listBox->Append("Jumping");
    m_listBox->Append("Kalm Fang");
    m_listBox->Append("Kelzmelzer");
    m_listBox->Append("Killbin");
    m_listBox->Append("Kimara Bug");
    m_listBox->Append("King Behemoth");
    m_listBox->Append("Kyuvilduns");
    m_listBox->Append("Land Worm");
    m_listBox->Append("Laser Cannon");
    m_listBox->Append("Left Arm");
    m_listBox->Append("Left Arm");
    m_listBox->Append("Lessaloploth");
    m_listBox->Append("Levrikon");
    m_listBox->Append("Lifeform-Hojo N");
    m_listBox->Append("Lost Number");
    m_listBox->Append("Machine Gun");
    m_listBox->Append("Madouge");
    m_listBox->Append("Magic Pot");
    m_listBox->Append("Magnade");
    m_listBox->Append("Malboro");
    m_listBox->Append("Malldancer");
    m_listBox->Append("Mandragora");
    m_listBox->Append("Manhole");
    m_listBox->Append("Marine");
    m_listBox->Append("Master Tonberry");
    m_listBox->Append("Materia Keeper");
    m_listBox->Append("Maximum Kimaira");
    m_listBox->Append("Midgar Zolom");
    m_listBox->Append("Mighty Grunt");
    m_listBox->Append("Mirage");
    m_listBox->Append("Mono Drive");
    m_listBox->Append("Moth Slasher");
    m_listBox->Append("Motor Ball");
    m_listBox->Append("Mover");
    m_listBox->Append("MP");
    m_listBox->Append("Mu");
    m_listBox->Append("Mystery Ninja");
    m_listBox->Append("Needle Kiss");
    m_listBox->Append("Nerosuferoth");
    m_listBox->Append("Nibel Wolf");
    m_listBox->Append("Palmer");
    m_listBox->Append("Parasite");
    m_listBox->Append("Pollensalta");
    m_listBox->Append("Poodler");
    m_listBox->Append("Poodler Sample");
    m_listBox->Append("Proto Machinegun");
    m_listBox->Append("Proud Clod");
    m_listBox->Append("Prowler");
    m_listBox->Append("Quick Machine Gun");
    m_listBox->Append("Rapps");
    m_listBox->Append("Razor Weed");
    m_listBox->Append("Red Dragon");
    m_listBox->Append("Right Arm");
    m_listBox->Append("Right Arm");
    m_listBox->Append("Rilfsak");
    m_listBox->Append("Rocket Launcher");
    m_listBox->Append("Rocket Launcher");
    m_listBox->Append("Roulette Cannon");
    m_listBox->Append("Ruby Weapon");
    m_listBox->Append("Ruby's Tentacle");
    m_listBox->Append("Rufus");
    m_listBox->Append("Safer*Sephiroth");
    m_listBox->Append("Sahagin");
    m_listBox->Append("Sample:H0512");
    m_listBox->Append("Sample:H0512-opt");
    m_listBox->Append("Schizo(Left)");
    m_listBox->Append("Schizo(Right)");
    m_listBox->Append("Scissors");
    m_listBox->Append("Scissors(Lower)");
    m_listBox->Append("Scissors(Upper)");
    m_listBox->Append("Scotch");
    m_listBox->Append("Screamer");
    m_listBox->Append("Scrutin Eye");
    m_listBox->Append("Sculpture");
    m_listBox->Append("Sea Worm");
    m_listBox->Append("Search Crown");
    m_listBox->Append("Senior Grunt");
    m_listBox->Append("Sephiroth");
    m_listBox->Append("Serpent");
    m_listBox->Append("Shadow Maker");
    m_listBox->Append("Shake");
    m_listBox->Append("Shred");
    m_listBox->Append("Skeeskee");
    m_listBox->Append("Slalom");
    m_listBox->Append("Slaps");
    m_listBox->Append("Smogger");
    m_listBox->Append("Sneaky Step");
    m_listBox->Append("Snow");
    m_listBox->Append("SOLDIER:1st");
    m_listBox->Append("SOLDIER:2nd");
    m_listBox->Append("SOLDIER:3rd");
    m_listBox->Append("Sonic Speed");
    m_listBox->Append("Soul Fire");
    m_listBox->Append("Special Combatant");
    m_listBox->Append("Spencer");
    m_listBox->Append("Spiral");
    m_listBox->Append("Staniv");
    m_listBox->Append("Stilva");
    m_listBox->Append("Stinger");
    m_listBox->Append("Submarine Crew");
    m_listBox->Append("Sweeper");
    m_listBox->Append("Sword Dance");
    m_listBox->Append("Tail Vault");
    m_listBox->Append("Thunderbird");
    m_listBox->Append("Tonadu");
    m_listBox->Append("Tonberry");
    m_listBox->Append("Touch Me");
    m_listBox->Append("Toxic Frog");
    m_listBox->Append("Trickplay");
    m_listBox->Append("Turks:Elena");
    m_listBox->Append("Turks:Reno");
    m_listBox->Append("Turks:Reno");
    m_listBox->Append("Turks:Reno");
    m_listBox->Append("Turks:Reno");
    m_listBox->Append("Turks:Rude");
    m_listBox->Append("Turks:Rude");
    m_listBox->Append("Turks:Rude");
    m_listBox->Append("Turks:Rude");
    m_listBox->Append("Twin Brain");
    m_listBox->Append("Ultimate Weapon");
    m_listBox->Append("Under Lizard");
    m_listBox->Append("Underwater MP");
    m_listBox->Append("Unknown");
    m_listBox->Append("Unknown 2");
    m_listBox->Append("Unknown 3");
    m_listBox->Append("Valron");
    m_listBox->Append("Vargid Police");
    m_listBox->Append("Velcher Task");
    m_listBox->Append("Vice");
    m_listBox->Append("Vlakorados");
    m_listBox->Append("Warning Board");
    m_listBox->Append("Whole Eater");
    m_listBox->Append("Wind Wing");
    m_listBox->Append("Wolfmeister");
    m_listBox->Append("XCannon");
    m_listBox->Append("Yang");
    m_listBox->Append("Ying");
    m_listBox->Append("Zemzelett");
    m_listBox->Append("Zenene");
    m_listBox->Append("Zolokalter");
    m_listBox->Append("Zuu");

    m_scrollto[0]=0;
    m_scrollto[1]=3;
    m_scrollto[2]=13;
    m_scrollto[3]=35;
    m_scrollto[4]=58;
    m_scrollto[5]=77;
    m_scrollto[6]=85;
    m_scrollto[7]=90;
    m_scrollto[8]=124;
    m_scrollto[9]=141;
    m_scrollto[10]=145;
    m_scrollto[11]=155;
    m_scrollto[12]=161;
    m_scrollto[13]=169;
    m_scrollto[14]=191;
    m_scrollto[15]=194;
    m_scrollto[16]=194;
    m_scrollto[17]=202;
    m_scrollto[18]=203;
    m_scrollto[19]=215;
    m_scrollto[20]=256;
    m_scrollto[21]=276;
    m_scrollto[22]=279;
    m_scrollto[23]=284;
    m_scrollto[24]=288;
    m_scrollto[25]=289;
    m_scrollto[26]=291;
}

void SampleFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Destroy();
}

void SampleFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox("the wxAlphaIndex demo.", _("Welcome to..."));
}

void SampleFrame::OnAlphaIndex1(wxCommandEvent& event)
{
    m_listBox->SetFirstItem(m_scrollto[event.GetInt()]);
}

void SampleFrame::OnAlphaIndex2(wxCommandEvent& event)
{
    m_panel->Freeze();
    new LetterPanel(m_panel,event.GetString(),32,10);
    m_panel->Thaw();
}

void SampleFrame::OnPaintPanel( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(m_panel);
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.DrawRectangle(wxPoint(0,0),m_panel->GetSize());
}

class sampleApp : public wxApp
{
    public:
        virtual bool OnInit()
        {
            SampleFrame* frame = new SampleFrame(0L);
            frame->SetIcon(wxICON(aaaa)); // To Set App Icon
            frame->Show();

            return true;
        }
};

wxIMPLEMENT_APP(sampleApp);

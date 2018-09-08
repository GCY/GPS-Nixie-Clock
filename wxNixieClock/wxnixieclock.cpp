#include "wxnixieclock.h"

IMPLEMENT_APP(App)
DECLARE_APP(App)

bool App::OnInit()
{
   wxFrame *frame = new Frame(wxT("wxNixieClock"));

   frame->Show(true);

   return true;
}

BEGIN_EVENT_TABLE(Frame,wxFrame)
   EVT_MENU(ID_EXIT,Frame::OnExit)
   EVT_TIMER(ID_TIMER,Frame::OnTimer)
   EVT_BUTTON(ID_SYNC_TIME,Frame::OnSyncTime)
   EVT_MENU(ID_TIME_MODE,Frame::OnTimeMode)
   EVT_MENU(ID_TEMPERATURE_MODE,Frame::OnTemperatureMode)
   EVT_MENU(ID_HUMIDITY_MODE,Frame::OnHumidityMode)
   EVT_MENU(ID_RANDOM_MODE,Frame::OnRandomMode)
   EVT_MENU(ID_COUNTER_MODE,Frame::OnCounterMode)
   EVT_MENU(ID_POLLING_MODE,Frame::OnPollingMode)
   EVT_MENU(ID_CONNECT_DEVICE,Frame::OnConnectDevice)
   EVT_MENU(ID_RELEASE_DEVICE,Frame::OnReleaseDevice)
END_EVENT_TABLE()

Frame::Frame(const wxString &title):wxFrame(NULL,wxID_ANY,title,wxDefaultPosition,wxDefaultSize,wxMINIMIZE_BOX | wxCLOSE_BOX | wxCAPTION),timer(this,ID_TIMER)
{

   wxInitAllImageHandlers();
   wxImage::AddHandler(new wxPNGHandler());
   CreateUI();
   timer.Start(30);

}

void Frame::CreateUI()
{
   wxMenu *file = new wxMenu;
   file->Append(ID_EXIT,wxT("E&xit\tAlt-e"),wxT("Exit"));
   wxMenu *tools = new wxMenu;
   tools->Append(ID_CONNECT_DEVICE,wxT("C&onnect Device\tAlt-c"),wxT("Connect Device"));
   tools->Append(ID_RELEASE_DEVICE,wxT("R&elease Device\tAlt-r"),wxT("Release Device"));
   wxMenu *mode = new wxMenu;
   mode->Append(ID_TIME_MODE,wxT("Time"),wxT("Time"));
   mode->Append(ID_TEMPERATURE_MODE,wxT("Temperature"),wxT("Temperature"));
   mode->Append(ID_HUMIDITY_MODE,wxT("Humidity"),wxT("Humidity"));
   mode->Append(ID_RANDOM_MODE,wxT("Random"),wxT("Random"));
   mode->Append(ID_COUNTER_MODE,wxT("Counter"),wxT("Counter"));
   mode->Append(ID_POLLING_MODE,wxT("Polling"),wxT("Polling"));

   wxMenuBar *bar = new wxMenuBar;
   bar->Append(file,wxT("File"));
   bar->Append(tools,wxT("Tools"));
   bar->Append(mode,wxT("Mode"));
   SetMenuBar(bar);

   wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(top);

   wxBoxSizer *screen_box = new wxBoxSizer(wxHORIZONTAL);
   top->Add(screen_box,0,wxALIGN_CENTER_HORIZONTAL | wxALL,5);

   for(size_t i = 0;i < TUBE_NUM;++i){
      tubes[i] = new wxPanel(this,wxID_ANY,wxDefaultPosition,wxSize(100,200));
      screen_box->Add(tubes[i],0,wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL,5);
   }

   wxBoxSizer *button_box = new wxBoxSizer(wxHORIZONTAL);
   top->Add(button_box,0,wxALIGN_CENTER_HORIZONTAL | wxALL,5);

   sync_button = new wxButton(this,ID_SYNC_TIME,wxT("&Sync Time"),wxDefaultPosition,wxDefaultSize);
   button_box->Add(sync_button,0,wxALIGN_CENTER_VERTICAL | wxALL,5);   

   CreateStatusBar(2);
   SetStatusText(wxT("wxNixieClock"));

   top->Fit(this);
   top->SetSizeHints(this);
   
}

void Frame::OnConnectDevice(wxCommandEvent &event)
{
    ConnectArgsDialog dlg(this,wxID_ANY,wxT("Connect"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);

    if(dlg.ShowModal() == wxID_OK){
       serial.Open(dlg.GetDevicePath().mb_str());
       serial.SetBaudRate(wxAtoi(dlg.GetBaudRate()));
       serial.SetParity(8,1,'N');

    }
}

void Frame::OnTimer(wxTimerEvent &event)
{
   wxDateTime time = wxDateTime::Now();
   unsigned digital[TUBE_NUM];
   digital[0] = time.GetHour() / 10;
   digital[1] = time.GetHour() % 10;
   digital[2] = time.GetMinute() / 10;
   digital[3] = time.GetMinute() % 10;
   digital[4] = time.GetSecond() / 10;
   digital[5] = time.GetSecond() % 10;   
   for(size_t i = 0;i < TUBE_NUM;++i){
      wxClientDC dc(tubes[i]);
      wxString str;
      wxStandardPaths& sp = wxStandardPaths::Get();
      str.Printf("%s/in16/%d.png",sp.GetResourcesDir(),digital[i]);
      wxImage *image = new wxImage(str,wxBITMAP_TYPE_PNG);
      wxBitmap *bitmap = new wxBitmap(*image);      
      int x,y,width,height;
      dc.GetClippingBox(&x,&y,&width,&height);
      //wxLogDebug(wxT("%s"),wxNow());
      bitmap->SetWidth(width);
      bitmap->SetHeight(height);
      dc.DrawBitmap(*bitmap,x,y);

      delete bitmap;
      delete image;
   }   
}

void Frame::OnReleaseDevice(wxCommandEvent &event)
{
   serial.Close();   
}

void Frame::OnSyncTime(wxCommandEvent &event)
{
   
   wxDateTime time = wxDateTime::Now();
   unsigned digital[TUBE_NUM];
   digital[0] = time.GetHour() / 10;
   digital[1] = time.GetHour() % 10;
   digital[2] = time.GetMinute() / 10;
   digital[3] = time.GetMinute() % 10;
   digital[4] = time.GetSecond() / 10;
   digital[5] = time.GetSecond() % 10;    
   unsigned month[2];
   month[0] = (time.GetMonth()+1) / 10;
   month[1] = (time.GetMonth()+1) % 10;
   unsigned day[2];
   day[0] = time.GetDay() / 10;
   day[1] = time.GetDay() % 10;
   
   wxString str;
   str.Printf("s%d%d%d%d%d%d%d%d%d%d%d",time.GetYear(),month[0],month[1],day[0],day[1],digital[0],digital[1],digital[2],digital[3],digital[4],digital[5]);
   unsigned char data[255] = {""};
   int size = str.length();
   strcpy((char*)data,str.c_str());
   serial.Write(data,size);
}

void Frame::OnTimeMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"1",1);   
}

void Frame::OnTemperatureMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"2",1);   
}

void Frame::OnHumidityMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"3",1);   
}

void Frame::OnRandomMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"4",1);   
}

void Frame::OnCounterMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"5",1);   
}

void Frame::OnPollingMode(wxCommandEvent &event)
{
   serial.Write((unsigned char*)"6",1);   
}

void Frame::OnExit(wxCommandEvent &event)
{
   Close();
}

Frame::~Frame()
{
   for(size_t i = 0;i < TUBE_NUM;++i){
      delete tubes[i];
   }
   
}

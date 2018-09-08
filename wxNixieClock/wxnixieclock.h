#ifndef __WX_ARDUINO__
#define __WX_ARDUINO__

#include <wx/wx.h>
#include <wx/stdpaths.h>

#include "serialport.h"
#include "connectargsdlg.h"

class App:public wxApp
{
   public:
      bool OnInit();
};

class Frame:public wxFrame
{
   public:
      Frame(const wxString&);
      ~Frame();

      void CreateUI();
      void OnTimer(wxTimerEvent&);
      void OnSyncTime(wxCommandEvent&);
      void OnTemperatureMode(wxCommandEvent&);
      void OnHumidityMode(wxCommandEvent&);
      void OnTimeMode(wxCommandEvent&);
      void OnRandomMode(wxCommandEvent&);
      void OnCounterMode(wxCommandEvent&);
      void OnPollingMode(wxCommandEvent&);
      void OnConnectDevice(wxCommandEvent&);
      void OnReleaseDevice(wxCommandEvent&);
      void OnExit(wxCommandEvent&);

   private:

      enum{
	 ID_CONNECT_DEVICE = 100,
	 ID_RELEASE_DEVICE,
	 ID_EXIT,
	 ID_TIMER,
	 ID_SYNC_TIME,
	 ID_TIME_MODE,
	 ID_TEMPERATURE_MODE,
	 ID_HUMIDITY_MODE,
	 ID_RANDOM_MODE,
	 ID_COUNTER_MODE,
	 ID_POLLING_MODE
      };

      static const size_t TUBE_NUM = 6;
      wxPanel *tubes[TUBE_NUM];
      wxTimer timer;

      wxButton *sync_button;

      wxMenu *device_path;
      SerialPort serial;

      DECLARE_EVENT_TABLE()
};

#endif

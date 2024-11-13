#include <WiFi.h>
#include "main.h"
#include "secrets.h"
#include "atask.h"

const char* ssid     = WIFI_SSID;            //Main Router      
const char* password = WIFI_PASS;            //Main Router Password


void wifi_task(void);
extern gw_ctrl_st gw_ctrl;

// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st wifi_handle    = {"WiFi Task      ", 1000,    0,     0,  255,    0,   1, wifi_task};

WiFiClient client;

void wifi_task_initialize(void)
{
  gw_ctrl.wifi_task_index = atask_add_new(&wifi_handle);  
}

void wifi_task(void)
{
    static uint8_t retries = 6;

    printf("WiFi wifi_handle.state: %d\n", wifi_handle.state);
    switch(wifi_handle.state)
    {
        case 0:   // initial
            if (WiFi.status() != WL_CONNECTED){
                Serial.println("Connecting WiFi");       
                WiFi.begin(ssid, password); 
                retries = 6;
                wifi_handle.state++;
            }
            else wifi_handle.state = 2;
            break;
        case 1:   // Check for the connection
            if (WiFi.status() != WL_CONNECTED) 
            {
                // digitalWrite(LED_YELLOW,LOW);
                if (--retries == 0) wifi_handle.state = 3;
                else Serial.println("Waiting for WiFi"); 
            }
            else 
            {
                //digitalWrite(LED_YELLOW,HIGH);
                Serial.println("Connected to WiFi");
                gw_ctrl.wifi_is_connected = true;
                gw_ctrl.radio_is_available = true;
                wifi_handle.state = 2;
            }             
            break;
        case 2:   // 
            if (WiFi.status() != WL_CONNECTED) wifi_handle.state = 0;
            break;
        case 3:   // 
            Serial.println("WiFi Retry limit reached -> WDT reset…");
            break;
    }
}

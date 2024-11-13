#include <WiFi.h>
#include  "main.h"
#include "atask.h"
#include "mqtt_task.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secrets.h"


extern main_ctrl_st main_ctrl;
extern WiFiClient client;

const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish test_feed_1  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-e6-temp");
Adafruit_MQTT_Publish test_feed_2  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-temp");
Adafruit_MQTT_Publish test_feed_3  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-rssi");
Adafruit_MQTT_Publish test_feed_4  = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/villaastrid.ruuvi-ea-bat");

rfm69_publ_st rfm69_publ[AIO_PUBL_NBR_OF ] =
{ //  feed             zone                 name
    { &test_feed_1, "OD1", "Temp", 0.0, false, UNIT_TEMPERATURE, 600000,   0}, 
    { &test_feed_2, "OD2", "Temp", 0.0, false, UNIT_TEMPERATURE, 600000,   0}, 
    { &test_feed_3, "OD3", "Temp", 0.0, false, UNIT_TEMPERATURE, 600000,   0}, 
    { &test_feed_4, "OD4", "Temp", 0.0, false, UNIT_TEMPERATURE, 600000,   0}, 
};

extern gw_ctrl_st gw_ctrl;
void mqtt_task(void);
// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st mqtt_handle    = {"MQTT Task      ", 1000,    0,     0,  255,    0,   1, mqtt_task};

void mqtt_task_initialize(void)
{
    gw_ctrl.mqtt_task_index = atask_add_new(&mqtt_handle);  
}

void mqtt_task(void)
{
    int8_t        ret;
    static uint8_t       sensor_indx = 0;
    float         fval;
    
    printf("MQTT mqtt_handle.state: %d\n", mqtt_handle.state);
    switch(mqtt_handle.state) 
    {
        case 0: // initial
            sensor_indx  = 0;
            mqtt_handle.state++;
            break;
        case 1: //Re-run MQTT action
            mqtt_handle.state++;
            break;
            
        case 2: // waiting for WiFi
            if (gw_ctrl.wifi_is_connected && gw_ctrl.radio_is_available)
            {
                gw_ctrl.radio_is_available = false;
                mqtt_handle.state = 10;
            }                  
            break;


        case 10:
            if (mqtt.connected())
            {
                printf("MQTT was already connected\n");
                gw_ctrl.radio_is_available = false;
                mqtt_handle.state = 20;
            }
            else 
            {
                ret = mqtt.connect();              
                if (ret != 0) {    // connect will return 0 for connected
                    printf("%s\nRetrying MQTT connection…\n",mqtt.connectErrorString(ret));
                    mqtt.disconnect();
                    gw_ctrl.radio_is_available = true;
                    mqtt_handle.state = 2;
                }
                else
                {
                    gw_ctrl.radio_is_available = false;
                    mqtt_handle.state = 20;
                }
            }
            break;    
        case 20:  // MQTT is connected
            if ((millis() > rfm69_publ[sensor_indx].publ_next_ms) && rfm69_publ[sensor_indx].updated)
            {
                rfm69_publ[sensor_indx].publ_next_ms = millis() + rfm69_publ[sensor_indx].publ_interval_ms;
                rfm69_publ[sensor_indx].updated = false;
                switch(rfm69_publ[sensor_indx].unit_type)
                {
                    case UNIT_TEMPERATURE:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_HUMIDITY:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_AIR_PRESSURE:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_LIGHT:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_LDR:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_RSSI:
                        fval = rfm69_publ[sensor_indx].value;
                        break;
                    case UNIT_VOLTAGE:
                        fval = rfm69_publ[sensor_indx].value;
                        break;


                }
                printf("MQTT Publish: %s %f\n", rfm69_publ[sensor_indx].label,  fval);
                rfm69_publ[sensor_indx].ada_mqtt_publ->publish(fval);  
            }    
            if(++sensor_indx >=  AIO_PUBL_NBR_OF) sensor_indx = 0;         
            mqtt_handle.state = 30;
            break;    
        case 30:
            atask_clear_cntr(gw_ctrl.mqtt_task_index);
            mqtt.disconnect(); 
            gw_ctrl.radio_is_available = true;
            mqtt_handle.state = 40;
            break;
        case 40:
            if (atask_get_cntr(gw_ctrl.mqtt_task_index) > 10 )  mqtt_handle.state = 2;
            break;    

        default:
            printf("Fatal error: incorrect MQTT mqtt_handle.state -> WDT reset…%d\n",mqtt_handle.state);
            mqtt_handle.state = 1;
            break;           
    }
}

void mqtt_task_update_feed_value(char *zone, char *label, float value)
{
    uint8_t findx = 0;
    bool    found = false;

    Serial.printf("zone: %s label: %s value: %f", zone, label, value);
    while (!found && (findx < AIO_PUBL_NBR_OF))
    {
        int zone_match = strcmp(rfm69_publ[findx].zone, zone);
        int label_match = strcmp(rfm69_publ[findx].label, label);
        if ((zone_match == 0) && (label_match == 0)) found = true;
        else findx++;
    }
    if (found)
    {
        Serial.printf(" Feed %d \n",findx);
        rfm69_publ[findx].value = value;
        rfm69_publ[findx].updated = true;
    }
    else Serial.println(" No feed was found");
}

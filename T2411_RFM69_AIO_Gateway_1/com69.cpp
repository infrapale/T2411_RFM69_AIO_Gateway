#include "main.h"
#include "com69.h"
#include "atask.h"
#include "json.h"
#include "mqtt_task.h"
#include "supervisor.h"


extern gw_ctrl_st gw_ctrl;

void com69_task(void);

// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st com69_handle   = {"Comm RM69 Task ",   10,    0,     0,  255,    0,   1, com69_task};

void com69_initialize(void)
{
  gw_ctrl.com69_task_index = atask_add_new(&com69_handle);
  super_set_interval(SUPER_COM69, 300000);
  super_activate_alive_check(SUPER_COM69, true );
}

void uart_clr_serial1_rx_buffer(void) {
  while(SERIAL69.available()){
    SERIAL69.read();
  }
}

uint8_t parse_avail_msg(String *Str)
{
   uint8_t n = 0;
   //String SubStr;
   Serial.print("parse: "); Serial.println(*Str);

   if (Str->startsWith(String("<#X1a:")))
   {
      Serial.println(Str->substring(6,7));
      n = Str->substring(6,7).toInt();
   }
   return n;
}

void print_decoded_msg69(msg69_st *msg69)
{
    Serial.print("Decoded: ");
    Serial.print("Zone: "); Serial.print(msg69->zone);
    Serial.print(" Label: "); Serial.print(msg69->label);
    Serial.print(" Value: "); Serial.print(msg69->value);
    Serial.print(" Remark: "); Serial.print(msg69->remark);
    Serial.println();
}  


void save_to_feed(msg69_st *msg69)
{
  char zone[MSG69_ZONE_LEN];
  char label[MSG69_NAME_LEN];

  msg69->zone.toCharArray(zone,MSG69_ZONE_LEN);
  msg69->label.toCharArray(label,MSG69_ZONE_LEN);
  
  mqtt_task_update_feed_value(zone, label, msg69->value.toFloat());
}


void com69_task(void)
{
  static String RxStrNbrMsg;
  static String RxStrMsg;
  uint8_t nbr_avail_msg;
  char    rx_buff[80];
 
  switch( com69_handle.state)
  {
    case 0:
        SERIAL69.setTimeout(5000);
        RxStrNbrMsg.reserve(80);
        RxStrMsg.reserve(80);
        super_activate_alive_check(SUPER_COM69, true);
        com69_handle.state = 10;
        break;
    case 10:
        RxStrNbrMsg = "";
        super_i_am_alive(SUPER_COM69);
        atask_clear_cntr(gw_ctrl.com69_task_index);
        com69_handle.state = 11;
        break;
    case 11:    
        if (atask_get_cntr(gw_ctrl.com69_task_index) > 500) 
        {
          uart_clr_serial1_rx_buffer();
          com69_handle.state = 20;
        }
        break;
    case 20:
        SERIAL69.println("<#X1A:>");
        atask_clear_cntr(gw_ctrl.com69_task_index);
        com69_handle.state = 30;
        break;
    case 30:
        if (SERIAL69.available()) com69_handle.state = 40;
        if (atask_get_cntr(gw_ctrl.com69_task_index) > 100 ) com69_handle.state = 10;
        break;
    case 40:
        RxStrNbrMsg = "";
        RxStrNbrMsg = SERIAL69.readStringUntil('\n');
        Serial.print("@40: ");Serial.println(RxStrNbrMsg);
        if (RxStrNbrMsg.length()> 0) 
        {
          nbr_avail_msg = parse_avail_msg(&RxStrNbrMsg);
          if (nbr_avail_msg >0) com69_handle.state = 50; 
          else com69_handle.state = 10; 
        }
        else  com69_handle.state = 10;
        break;
    case 50:
        uart_clr_serial1_rx_buffer();
        RxStrMsg = ""; 
        SERIAL69.println("<#X1R:>");
        atask_clear_cntr(gw_ctrl.com69_task_index);
        com69_handle.state = 60;
        break;
    case 60:
        if (SERIAL69.available()) com69_handle.state = 70;
        else
        {
          if (atask_get_cntr(gw_ctrl.com69_task_index) > 200 ) com69_handle.state = 10;
        }
        break;
    case 70:
        RxStrMsg = SERIAL69.readStringUntil('\n');
        Serial.print("@70: ");Serial.println(RxStrMsg);
        if (RxStrMsg.length() > 0) com69_handle.state = 80;
        else com69_handle.state = 10;
        break;
    case 80:
        json_pick_data_from_rx(&gw_ctrl.msg69, &RxStrMsg);    
        print_decoded_msg69(&gw_ctrl.msg69);  
        save_to_feed(&gw_ctrl.msg69);  
        com69_handle.state = 100;
        break;    
    case 100:
        uart_clr_serial1_rx_buffer();
        com69_handle.state = 10;
        break;
  } 
}

//* * * * * * DEPRECATED  * * * * * * 

// uint8_t uart_1_read_line(char *rxbuff, uint8_t max_len)
// {
//     memset(rxbuff,0x00,max_len);
//     uint8_t p = 0;
//     while(SERIAL69.available()){
//       char c = SERIAL69.read();
//       rxbuff[p++] = c;
//       Serial.print(c);
//     }
//     Serial.println();
//     Serial.println(rxbuff);   
//     return p; 
// }

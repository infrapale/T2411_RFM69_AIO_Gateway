#include "main.h"
#include "atask.h"
#include "supervisor.h"

#define   DEFAULT_ALIVE_TIMEOUT_MS   2000 

extern gw_ctrl_st gw_ctrl;

super_st super[SUPER_NBR_OF] =
{ //                  01234567
  [ SUPER_COM69] = { "COM69  ", 0, DEFAULT_ALIVE_TIMEOUT_MS, false },
  [ SUPER_WIFI]  = { "WiFi   ", 0, DEFAULT_ALIVE_TIMEOUT_MS, false },
  [ SUPER_MQTT]  = { "MQTT   ", 0, DEFAULT_ALIVE_TIMEOUT_MS, false },
}; 


void super_task(void);

// atask_st:            = {"Label          ", ival, next, state, prev, cntr, run, task_ptr };
atask_st super_handle   = {"Supervisor Task", 1000,    0,     0,  255,    0,   1, super_task};


void super_initialize(void)
{
  gw_ctrl.super_task_index = atask_add_new(&super_handle);  
    for (uint8_t tindx = 0; tindx < SUPER_NBR_OF; tindx++ )
    {
        super[tindx].next_timeout_ms = millis() + super[tindx].timeout_limit_ms;
    }

}

void super_i_am_alive(super_tasks_et task_indx)
{
    super[task_indx].next_timeout_ms = millis() + super[task_indx].timeout_limit_ms;
}

void super_activate_alive_check(super_tasks_et task_indx, bool is_active )
{
    super[task_indx].do_alive_check = is_active;
}

void super_set_interval(super_tasks_et task_indx, uint32_t new_ival)
{
    super[task_indx].timeout_limit_ms = new_ival;
    super_i_am_alive(task_indx);
}

void super_print_status(void)
{
    for (uint8_t tindx = 0; tindx < SUPER_NBR_OF; tindx++ )
    {
      Serial.printf("%s: %d %d %d\n",
        super[tindx].caption, 
        super[tindx].do_alive_check, 
        super[tindx].timeout_limit_ms,
        super[tindx].next_timeout_ms);
    }
}

void super_task(void)
{
  switch(super_handle.state)
  {
    case 0:
      super_handle.state = 10;
      break;
    case 10:
      for (uint8_t tind)
      break;  
    case 20:
      break;  
    case 30:
      break;  
    case 40:
      break;  
    case 50:
      break;  
  }
}

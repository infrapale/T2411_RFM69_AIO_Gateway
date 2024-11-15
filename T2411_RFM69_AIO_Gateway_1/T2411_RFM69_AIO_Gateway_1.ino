#include "main.h"
#include "io.h"
#include "atask.h"
#include "com69.h"
#include "wifi_task.h"
#include "mqtt_task.h"
#include "supervisor.h"



void debug_print_task(void);
void read_rfm69_task(void);

gw_ctrl_st gw_ctrl;
atask_st debug_print_handle        = {"Debug Print    ", 5000,0, 0, 255, 0, 1, debug_print_task};
//atask_st clock_handle              = {"Clock Task     ", 1000,0, 0, 255, 0, 1, run_1000ms};


void initialize_tasks(void)
{
  atask_initialize();
  atask_add_new(&debug_print_handle);
  super_initialize();
  com69_initialize();
  wifi_task_initialize();
  mqtt_task_initialize();
  Serial.print("Tasks initialized "); Serial.println(ATASK_RESERVED_NBR);
}



void setup() {
  io_initialize_serial();
  initialize_tasks();
}


void loop() {
  atask_run(); 
}

void debug_print_task(void)
{
  atask_print_status(true);
  super_print_status();
}


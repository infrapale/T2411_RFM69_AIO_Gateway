#ifndef __SUPERVISOR_H__
#define __SUPERVISOR_H__

typedef enum
{
    SUPER_COM69 = 0,
    SUPER_WIFI,
    SUPER_MQTT,
    SUPER_NBR_OF
} super_tasks_et;

typedef struct
{
    char      caption[8];
    uint32_t  next_timeout_ms;
    uint32_t  timeout_limit_ms;
    bool      do_alive_check;
} super_st;


void super_initialize(void);

void super_i_am_alive(super_tasks_et task_indx);

void super_activate_alive_check(super_tasks_et task_indx, bool is_active );

void super_set_interval(super_tasks_et task_indx, uint32_t new_ival);

void super_print_status(void);


#endif
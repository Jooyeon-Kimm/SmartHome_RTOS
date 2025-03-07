/* generated thread header file - do not edit */
#ifndef THREAD_TIMER_H_
#define THREAD_TIMER_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void thread_timer_entry(void * pvParameters);
                #else
extern void thread_timer_entry(void *pvParameters);
#endif
#include "r_gpt.h"
#include "r_timer_api.h"
FSP_HEADER
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef timer_callback
void timer_callback(timer_callback_args_t *p_args);
#endif
FSP_FOOTER
#endif /* THREAD_TIMER_H_ */

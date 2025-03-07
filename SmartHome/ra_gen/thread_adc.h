/* generated thread header file - do not edit */
#ifndef THREAD_ADC_H_
#define THREAD_ADC_H_
#include "bsp_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void thread_adc_entry(void * pvParameters);
                #else
extern void thread_adc_entry(void *pvParameters);
#endif
#include "r_adc.h"
#include "r_adc_api.h"
FSP_HEADER
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_channel_cfg_t g_adc0_channel_cfg;

#ifndef adc_callback
void adc_callback(adc_callback_args_t *p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  2
#endif
FSP_FOOTER
#endif /* THREAD_ADC_H_ */

#include <thread_adc.h>
#include "common.h"
#include <event_groups.h>

// ADC 변환 완료 세마포어
StaticSemaphore_t xAdcCompleteSemaphoreBuffer;
SemaphoreHandle_t xAdcCompleteSemaphore = NULL;


void adc_init();


/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/* Thread_adc entry function */
/* pvParameters contains TaskHandle_t */
void thread_adc_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    adc_init();
    ring_buf_init(&g_adc_buffer);

    /* TODO: add your own code here */
    while (1)
    {
        // (1) 데이터 읽기 : READ ADC data
        int adc_avg = adc_read();

        // (2) 자동 조명 ON/OFF : Auto Light On/Off
        auto_on_off(adc_avg);

        vTaskDelay (1000 / portTICK_PERIOD_MS * 60); // 1분 대기
    }
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

// ■ ADC 초기화
void adc_init(){
    xAdcCompleteSemaphore = xSemaphoreCreateBinaryStatic(&xAdcCompleteSemaphoreBuffer);
    ring_buf_init(&g_adc_buffer);

    // ADC OPEN
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    if(err == FSP_SUCCESS) uart_write("\033[34mADC OPEN 성공", NO_VAR);
    else uart_write("\033[37;41mADC OPEN 실패", err);

    // ADC Scan Config
    err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    if(err == FSP_SUCCESS) uart_write("\033[34m ADC SCAN 설정 성공했습니다.", NO_VAR);
    else uart_write("\033[37;41mADC SCAN 설정 실패했습니다.", err); // \033[37;41m: 빨간배경 흰색 글씨

}

// ■ ADC 콜백 함수 구현
void adc_callback(adc_callback_args_t * p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (p_args->event == ADC_EVENT_SCAN_COMPLETE) {
        xSemaphoreGiveFromISR(xAdcCompleteSemaphore, &xHigherPriorityTaskWoken);
    }
}


// ■ ADC 조도센서 읽어오기 (반환값: adc 평균 데이터)
int adc_read(){
    // 변환 완료 대기를 위해, 세마포어 초기화 (블로킹)
    xSemaphoreTake(xAdcCompleteSemaphore, 0);

    // ADC SCAN
    err = R_ADC_ScanStart(&g_adc0_ctrl);
//    if(err == FSP_SUCCESS) uart_write("ADC SCAN START 성공했습니다.", NO_VAR);
//    else uart_write("\033[37;41mADC SCAN START실패했습니다.", err);

    // 변환 완료될 때까지 대기
    if (xSemaphoreTake(xAdcCompleteSemaphore, portMAX_DELAY) == pdFALSE)
    {
        uart_write("ADC 변환 완료 이벤트 없음, 타임아웃!", NO_VAR); // 타임아웃 (Deadlock?)
    }


    // ADC READ
    // [참고] adc_data  -> 전압 으로 바꾸고 싶으면, 4095.0으로 나누고 5 곱하기
    err = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &g_adc_data);
    if(err == FSP_SUCCESS) {
        if (g_adc_data != 0) {
            ring_buf_push(&g_adc_buffer, g_adc_data);
            uart_write("ADC 데이터", (uint16_t)g_adc_data); // 흰색: \033[0m
        }
        uint16_t average = ring_buf_avg(&g_adc_buffer);
        return average;
    }
    else {
        uart_write("\033[37;41mADC READ 실패했습니다", err);
        return 0;
    }

}


// ■ 밝기에 따라 자동 RGB LED 점등
void auto_on_off(int adc_avg){
    if(g_manual_control) return; // 수동제어 활성화 동안, 자동제어 비활성화
    if(adc_avg == 0) {
        RGB_LED_ON();
//        uart_write("밝기:0, LED_ON", NO_VAR);
    }

    switch(is_RGB_LED_ON()){
        // LED가 켜져있을 때
        case true:
            // 주변이 밝으면,
            if(adc_avg >= ADC_THRESHOLD_HIGH) {
                uart_write("\033[31mLED를 꺼야 해요", (uint16_t)adc_avg); // \033[31m : 빨강
                RGB_LED_OFF();
            }
            // 주변이 어두우면,
            else if(adc_avg <= ADC_THRESHOLD_LOW){
                // 유지
            }
            // 밝음과 어두움 중간,
            else {
                uart_write("\033[31mLED를 조금 어둡게 켜야 해요.", (uint16_t)adc_avg); // \033[31m : 빨강
                RGB_HALF_ON();
            }
            break;

            // LED가 꺼져있을 때
        case false:
            // 주변이 어두우면,
            if(adc_avg <= ADC_THRESHOLD_LOW){
                uart_write("\033[31mLED를 켜야 해요", (uint16_t)adc_avg);
                RGB_LED_ON();
            }
            // 주변이 밝으면,
            else if(adc_avg >= ADC_THRESHOLD_HIGH){
                // 유지
            }
            // 밝음과 어두움 중간,
            else {
                uart_write("\033[31mLED를 조금 어둡게 켜야 해요.", (uint16_t)adc_avg); // \033[31m : 빨강
                RGB_HALF_ON();
            }
            break;
    }
}


void ring_buf_init(ring_buf_t *rb) {
    memset(rb->buffer, 0, sizeof(rb->buffer));
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->sum = 0;
}

void ring_buf_push(ring_buf_t *rb, uint16_t data) {
    // 새 데이터를 버퍼에 추가하고 합계 업데이트
    if(data != 0){
        // 기존 위치의 데이터를 합계에서 제거
        if (rb->count == ADC_BUFFER_SIZE) {
            rb->sum -= rb->buffer[rb->head];
            rb->head = (rb->head + 1) % ADC_BUFFER_SIZE;
        }

        rb->buffer[rb->tail] = data;
            rb->sum += data;
            rb->tail = (rb->tail + 1) % ADC_BUFFER_SIZE;

            // 버퍼 카운트 조정
            if (rb->count < ADC_BUFFER_SIZE) {
                rb->count++;
            }
    }
}

uint16_t ring_buf_avg(ring_buf_t *rb) {
    // 평균을 계산하여 반환
    return rb->count > 0 ? (uint16_t)(rb->sum / rb->count) : (uint16_t)0;
}


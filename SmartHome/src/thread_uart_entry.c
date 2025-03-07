#include <thread_uart.h>
#include "common.h"
#include <event_groups.h>
#include <stdio.h>

// 접두어 x는 FreeRTOS API의 일부임을 나타내는 관례임
// UART TX 완료 세마포어
StaticSemaphore_t xTxCompleteSemaphoreBuffer;
StaticSemaphore_t xRxCompleteSemaphoreBuffer;
StaticSemaphore_t xUartTickSemaphoreBuffer;


// UART TX 보호용 뮤텍스
StaticSemaphore_t xMutexBuffer;





/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/* Thread_uart entry function */
/* pvParameters contains TaskHandle_t */
void thread_uart_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    uart_init();
    /* TODO: add your own code here */
    while (1)
    {
        // 타이머 설정 시, 시간 출력하는 함수
        write_time();
        vTaskDelay(1000 / portTICK_PERIOD_MS); // [TASK 지연] 1초 대기 : 1000 / portTICK_PERIOD_MS
        // portTICK_PERIOD_MS : 시스템 틱이 몇 ms 인지
    }
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/


// ■ UART 초기화
void uart_init() {
    // 뮤텍스 및 세마포어 생성
    xUartMutex = xSemaphoreCreateMutexStatic(&xMutexBuffer);
    g_uart_tick_semaphore = xSemaphoreCreateBinaryStatic(&xUartTickSemaphoreBuffer);
    xTxCompleteSemaphore = xSemaphoreCreateBinaryStatic(&xTxCompleteSemaphoreBuffer);
    xRxCompleteSemaphore = xSemaphoreCreateBinaryStatic(&xRxCompleteSemaphoreBuffer);
    xSemaphoreGive(g_uart_tick_semaphore);
    xSemaphoreGive(xTxCompleteSemaphore);

    // UART 드라이버 초기화
    R_SCI_UART_Open(&g_uart0_ctrl, &g_uart0_cfg);
    R_SCI_UART_CallbackSet(&g_uart0_ctrl, uart_callback, NULL, NULL); // 콜백 함수 등록
    g_rx_index = 0;
}

// ■ UART 송수신 콜백
void uart_callback(uart_callback_args_t *p_args){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // ISR 동안 높은 우선순위의 태스크가 준비 상태가 되었는지

    switch(p_args->event){
        // 데이터 송신 (Transmit)
        case UART_EVENT_TX_COMPLETE:
        {
            // TX 완료 시 세마포어/뮤텍스
            xSemaphoreGiveFromISR(xTxCompleteSemaphore, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // pdTRUE로 설정된 경우, ISR 종료 후 즉시 높은 우선순위의 태스크로 스위치할 준비
            break;
        }

        // 데이터 수신 (Receive)
        // 한 문자 수신 event
        case UART_EVENT_RX_CHAR:
        {
            g_rx_buffer[g_rx_index++] = (uint8_t)p_args->data;
            if ((uint8_t)p_args->data == END_CHARACTER || g_rx_index >= UART_RX_BUF_SIZE) {
                g_rx_buffer[g_rx_index] = '\0'; // 종료 문자 추가
                g_rx_index = 0;  // 인덱스 초기화
                xSemaphoreGiveFromISR(xRxCompleteSemaphore, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
            break;
        }

        // 아래 event 는 버퍼가 모두 찼을 때만 호출된다.
        case UART_EVENT_RX_COMPLETE:
//            xSemaphoreGiveFromISR(xRxCompleteSemaphore, &xHigherPriorityTaskWoken);
//            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            g_rx_index = 0;
            break;

        // 이후 사용 안 할 이벤트 (컴파일 경고-e2studio 노란 줄-때문에 추가함)
        case UART_EVENT_ERR_PARITY:
            break;
        case UART_EVENT_ERR_FRAMING:
            break;
        case UART_EVENT_ERR_OVERFLOW:
            break;
        case UART_EVENT_BREAK_DETECT:
            break;
        case UART_EVENT_TX_DATA_EMPTY:
            break;
    }
}

// UART 송신 (MCU > PC, Transmit)
// ■ UART 송신 (MCU > PC, Transmit)
void uart_write(char *message, uint16_t var)
{
    if (xSemaphoreTake(xUartMutex, portMAX_DELAY) == pdTRUE)
    {
        if(var == NO_VAR) sprintf(g_tx_buffer, "%s\r\n\033[0m", message);
        else snprintf(g_tx_buffer, UART_TX_BUF_SIZE, "%s: %u\r\n\033[0m", message, var);

        // TX 완료 대기를 위해 세마포어 초기화 (0 → 초기화)
        xSemaphoreTake(xTxCompleteSemaphore, 0);

        // UART 전송 시작
        R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)g_tx_buffer, strlen(g_tx_buffer));

        // TX 완료될 때까지 대기 (최대 100ms 대기, Deadlock 방지)
        if (xSemaphoreTake(xTxCompleteSemaphore, pdMS_TO_TICKS(100)) == pdFALSE){}

        xSemaphoreGive(xUartMutex);
    }
}

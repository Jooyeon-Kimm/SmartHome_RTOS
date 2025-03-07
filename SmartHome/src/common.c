/*
 * common.c
 *
 *  Created on: 2025. 3. 5.
 *      Author: User
 */
#include "common.h"
#include "hal_data.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h> // 가변인자 함수
#include <ctype.h> // isdigit()

/* 전역변수■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
fsp_err_t err;

/*** UART (Serial 통신) ***/
#define UART_TX_BUF_SIZE 50
#define UART_RX_BUF_SIZE 30
char g_tx_buffer[UART_TX_BUF_SIZE];
volatile uint8_t g_uart_index = 0;  // 버퍼에 데이터가 쌓이는 위치

#define END_CHARACTER   '\r'        // 명령어 종료를 나타내는 문자
#define HEADER          "HDR"       // 헤더: 패킷 시작
#define TAIL            "TAIL"      // 테일: 패킷 끝
volatile uint8_t g_rx_buffer[UART_TX_BUF_SIZE] = {0};
volatile uint16_t g_rx_index = 0;
volatile uint8_t g_big_rx_buffer[UART_TX_BUF_SIZE * 2] = {0};

volatile _Bool g_uart_tx_complete = false;  // 비동기 전송 플래그 (초기값: true)
volatile _Bool g_uart_rx_complete = false;  // 비동기 전송 플래그 (초기값: true)
volatile _Bool g_scan_complete = false;     // ADC SCAN 완료 플래그

volatile _Bool g_uart_tick_output_flag = false;
volatile int g_new_minutes = 0;
volatile int g_new_seconds = 0;
#define NO_VAR 65535 // UART 변수 출력 여부 (uint16_t 의 최댓값:65535 이면 변수 출력 X)

/*** ADC (Analog to Digital Converter ***/
uint16_t g_adc_data; // ADC 조도센서 데이터
#define ADC_BUFFER_SIZE 60
#define ADC_THRESHOLD_HIGH 3000
#define ADC_THRESHOLD_LOW 1000


ring_buf_t g_adc_buffer;

/*** USER BUTTON ***/
bsp_io_level_t g_color_btn_level; // 색깔 변경 버튼 상태 (HIGH or LOW)
bsp_io_level_t g_brightness_btn_level; // 밝기 변경 버튼 상태 (HIGH or LOW)

ButtonState g_color_btn_state = WAITING_FOR_PRESS; // 누르지 않은 상태
ButtonState g_brightness_btn_state = WAITING_FOR_PRESS; // 누르지 않은 상태

int g_color_btn_cnt = 0; // 색상 변경 버튼 클릭 횟수
int g_brightness_btn_cnt = 0;   // 밝기 변경 버튼 클릭 횟수

/*** RGB LED : GPT ***/
uint32_t g_R_LED_duty_cycle = 0;
uint32_t g_G_LED_duty_cycle = 0;
uint32_t g_B_LED_duty_cycle = 0;
_Bool g_is_RGB_LED_ON_by_cmd = false;
#define GAMMA 2.2 // 감마 보정 의 일반적인 감마 값

// [데이터 시트] RGB LED 0V ~ 5V
// http://wiki.sunfounder.cc/index.php?title=RGB_LED_Module
//#define MAX_VOLTAGE 5.0

#define RGB_PWM_PERIOD 1000 // 단위: micro seconds (FSP Configuration)
volatile _Bool g_timer_set = false;
volatile uint64_t g_timer_target_ticks = 0;
volatile uint64_t g_new_tick = 0; // 타이머 설정 틱
volatile uint64_t g_old_tick = 0; // 프로그램 실행부터 지금까지 전체 틱
#define TICK_PER_ONE_SEC 100000
#define TICK_PER_ONE_MIN TICK_PER_ONE_SEC * 60
/*** hal_entry() delay 값 ***/
#define HAL_ENTRY_DELAY 100 // hal_entry() 내부 while 문의 delay
#define SEC_UNIT 1000 / HAL_ENTRY_DELAY // 반복문 내에서 1초 단위로 맞춰주기 위함

/*** 버튼/명령어 수동 제어 여부 ***/
volatile _Bool g_manual_control = false;
/* 소자들■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*** Light Sensor : 조도 센서 ***/
#define LIGHT_SENSOR_PIN BSP_IO_PORT_00_PIN_00

/*** RGB LED ***/
#define RGB_LED_R_PIN BSP_IO_PORT_01_PIN_11
#define RGB_LED_G_PIN BSP_IO_PORT_02_PIN_05
#define RGB_LED_B_PIN BSP_IO_PORT_04_PIN_07

/* 공유자원 접근■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
// 뮤텍스 및 세마포어 생성
SemaphoreHandle_t xTxCompleteSemaphore = NULL;
SemaphoreHandle_t xRxCompleteSemaphore = NULL;
SemaphoreHandle_t g_uart_tick_semaphore = NULL;

// UART TX 보호용 뮤텍스
SemaphoreHandle_t xUartMutex = NULL;


/* 함수들■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/


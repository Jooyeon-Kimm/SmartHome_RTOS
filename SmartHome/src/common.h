/*
 * common.h
 *
 *  Created on: 2025. 3. 5.
 *      Author: User
 */
// 전역변수 정의

// 같은 이름 가진 헤더 있는지 확인
#ifndef COMMON_H_
#define COMMON_H_
#include "hal_data.h"
#include <stdint.h>
#include <string.h>
#include "r_timer_api.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* 전역변수■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
// [volatile] https://luna-archive.tistory.com/2
// volatile 변수 사용 이유 : 컴파일러 최적화 하지 않음
// 멀티스레드 환경이나 인터럽트 서비스 루틴, 하드웨어와 직접 상호작용할 때
// 변수 값이 예측할 수 없이 변경될 수 있는 상황에서 사용됨

// (1) 같은 메모리 주소에 반복 write 할 때,
// 컴파일러가 최적화 수행하면, 마지막 write 만 실행
// mcu 의 경우, 모든 write 수행 해야 함
// (메모리 주소에 연결된 하드웨어 레지스터에 값을 쓰는 프로그램의 경우 이므로)
// 각각의 쓰기가 하드웨어에 특정 명령을 전달하는 것이므로,
// 주소가 같다는 이유만으로 중복되는 쓰기 명령을 없애 버리면, 하드웨어가 오동작

// volatile 키워드는 크게 3가지 경우에 흔히 사용된다.
//(1) MIMO(Memory-mapped I/O)
//(2) 인터럽트 서비스 루틴(Interrupt Service Routine)의 사용
//(3) 멀티 쓰레드 환경

#define BUTTON_S1 (BSP_IO_PORT_00_PIN_05)
#define BUTTON_S2 (BSP_IO_PORT_00_PIN_06)

// FSP 라이브러리의 함수 반환 타입 (성공: FSP_SUCCESS)
extern fsp_err_t err;
extern volatile _Bool g_uart_tick_output_flag;

/*** UART (Serial 통신) ***/
#define UART_TX_BUF_SIZE 50
#define UART_RX_BUF_SIZE 30
extern char g_tx_buffer[UART_TX_BUF_SIZE];

#define END_CHARACTER   '\r'        // 명령어 종료를 나타내는 문자
#define HEADER          '<'       // 헤더: 패킷 시작
#define TAIL            '>'      // 테일: 패킷 끝
extern volatile uint8_t g_rx_buffer[UART_TX_BUF_SIZE];
extern volatile uint16_t g_rx_index;
extern volatile uint8_t g_big_rx_buffer[UART_TX_BUF_SIZE * 2];

extern volatile int g_new_minutes;
extern volatile int g_new_seconds;
#define NO_VAR 65535 // UART 변수 출력 여부 (uint16_t 의 최댓값:65535 이면 변수 출력 X)

/*** ADC (Analog to Digital Converter ***/
extern uint16_t g_adc_data; // ADC 조도센서 데이터
#define ADC_BUFFER_SIZE 60
#define ADC_THRESHOLD_HIGH 3000
#define ADC_THRESHOLD_LOW 1000

typedef struct {
    uint16_t buffer[ADC_BUFFER_SIZE];
    int head;       // 가장 오래된 데이터의 인덱스
    int tail;       // 다음 데이터가 저장될 인덱스
    int count;      // 현재 버퍼에 저장된 데이터 수
    uint32_t sum;   // 버퍼의 데이터 합계
} ring_buf_t;
extern ring_buf_t g_adc_buffer;

/*** USER BUTTON ***/
extern bsp_io_level_t g_color_btn_level; // 색깔 변경 버튼 상태 (HIGH or LOW)
extern bsp_io_level_t g_brightness_btn_level; // 밝기 변경 버튼 상태 (HIGH or LOW)
typedef enum {
    WAITING_FOR_PRESS,  // Click 을 기다림 : 버튼을 누른 상태       (LOW)
    WAITING_FOR_RELEASE, // 손 떼기를 기다림  : 버튼을 누르지 않은 상태 (HIGH)
} ButtonState;

typedef struct {
    uint32_t last_debounce_time;
    ButtonState state;
    bsp_io_level_t last_read_level;
} ButtonControl;


extern ButtonState g_color_btn_state; // 누르지 않은 상태
extern ButtonState g_brightness_btn_state; // 누르지 않은 상태

extern int g_color_btn_cnt; // 색상 변경 버튼 클릭 횟수
extern int g_brightness_btn_cnt;   // 밝기 변경 버튼 클릭 횟수

/*** RGB LED : GPT ***/
extern uint32_t g_R_LED_duty_cycle;
extern uint32_t g_G_LED_duty_cycle;
extern uint32_t g_B_LED_duty_cycle;
extern _Bool g_is_RGB_LED_ON_by_cmd;
#define GAMMA 2.2 // 감마 보정 의 일반적인 감마 값

// [데이터 시트] RGB LED 0V ~ 5V
// http://wiki.sunfounder.cc/index.php?title=RGB_LED_Module
//#define MAX_VOLTAGE 5.0

#define RGB_PWM_PERIOD 1000 // 단위: micro seconds (FSP Configuration)
extern volatile _Bool g_timer_set;
extern volatile uint64_t g_timer_target_ticks;
extern volatile uint64_t g_new_tick; // 타이머 설정 틱
extern volatile uint64_t g_old_tick; // 프로그램 실행부터 지금까지 전체 틱
#define TICK_PER_ONE_SEC 100000
#define TICK_PER_ONE_MIN TICK_PER_ONE_SEC * 60
/*** hal_entry() delay 값 ***/
#define HAL_ENTRY_DELAY 100 // hal_entry() 내부 while 문의 delay
#define SEC_UNIT 1000 / HAL_ENTRY_DELAY // 반복문 내에서 1초 단위로 맞춰주기 위함

/*** 버튼/명령어 수동 제어 여부 ***/
extern volatile _Bool g_manual_control;


/* 소자들■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/*** Light Sensor : 조도 센서 ***/
#define LIGHT_SENSOR_PIN BSP_IO_PORT_00_PIN_00

/*** RGB LED ***/
#define RGB_LED_R_PIN BSP_IO_PORT_01_PIN_11
#define RGB_LED_G_PIN BSP_IO_PORT_02_PIN_05
#define RGB_LED_B_PIN BSP_IO_PORT_04_PIN_07

/* 공유자원 접근■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
// 접두어 x는 FreeRTOS API의 일부임을 나타내는 관례임
// UART TX 완료 세마포어
extern StaticSemaphore_t xTxCompleteSemaphoreBuffer;
extern SemaphoreHandle_t xTxCompleteSemaphore;
extern StaticSemaphore_t xRxCompleteSemaphoreBuffer;
extern SemaphoreHandle_t xRxCompleteSemaphore;
extern StaticSemaphore_t xUartTickSemaphoreBuffer;

// UART TX 보호용 뮤텍스
extern StaticSemaphore_t xMutexBuffer;
extern SemaphoreHandle_t xUartMutex;
extern SemaphoreHandle_t g_uart_tick_mutex;
extern SemaphoreHandle_t g_uart_tick_semaphore;

/* 함수들■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
void delay_us(uint32_t delay);
void delay_ms(uint32_t ms);
_Bool is_RGB_LED_ON();
void RGB_LED_OFF();
void RGB_LED_ON();
void RGB_HALF_ON();
void Device_Init();
void uart_init();
void adc_init();

void ring_buf_init(ring_buf_t *rb);
void ring_buf_push(ring_buf_t *rb, uint16_t data);
uint16_t ring_buf_avg(ring_buf_t *rb);
int adc_read();
void gpt_open();

void pwm_init();

void uart_write(char *message, uint16_t var);
void set_brightness(int level);
void auto_on_off();
uint32_t gamma_correct_duty_cycle(uint32_t duty_cycle);
void set_duty_cycle(timer_ctrl_t * const p_ctrl, uint32_t const duty_cycle);
void set_duty_cycles_by_ratio(int n);
void handle_btn_click(uint16_t btn_num);
void write_duty_cycle();
void check_btn_clicked(bsp_io_port_pin_t BUTTON, bsp_io_level_t g_btn_level, ButtonState* g_btn_state);
uint32_t convert_brightness_to_duty_cycle(uint32_t brightness);
void process_command();
void command_err_handle();

void init_timer();
void set_period(timer_ctrl_t * const p_ctrl, uint32_t const period_counts);
void set_timer(uint32_t minutes, _Bool led_on);
void start_gpt(timer_ctrl_t * const p_ctrl);
void timer_callback(timer_callback_args_t *p_args);
void set_timer_callback(timer_ctrl_t * const p_ctrl, void (*callback)(timer_callback_args_t *pargs));
void uart_init();
void gpt_set_callback(timer_ctrl_t * const p_ctrl, void (*callback)(timer_callback_args_t *pargs));void gpt_set_callback(timer_ctrl_t * const p_ctrl, void (*callback)(timer_callback_args_t *pargs));

void write_time();

void process_single_command(uint8_t* command);
void process_commands();
void receive_command();

#endif /* COMMON_H_ */

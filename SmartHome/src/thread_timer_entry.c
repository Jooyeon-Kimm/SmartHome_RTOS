#include <thread_gpt_.h>
#include <thread_timer.h>
#include "common.h"
#include "thread_uart.h"
#include <stdio.h>
#include <ctype.h> // isdigit()

// 명령어 입력 받아서 처리하는 쓰레드
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/* Thread_timer entry function */
/* pvParameters contains TaskHandle_t */
void thread_timer_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    init_timer();

    /* TODO: add your own code here */
    while (1)
    {
        // UART 한 문자 수신 완료 이벤트 에서 GIVE한 세마포어 여기서 TAKE
        if (xSemaphoreTake(xRxCompleteSemaphore, portMAX_DELAY) == pdTRUE) {
            receive_command(); // 명령어 입력받아 parsing
            process_commands(); // parsing한 명령어들을 처리 (내부적으로 명령어 하나씩 처리)
        }
        vTaskDelay (1);
    }
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/



// ■ timer 드라이버 초기화
void init_timer(){
    gpt_open(&g_timer0_ctrl, &g_timer0_cfg);
    set_period(&g_timer0_ctrl, 1000);
    set_timer_callback(&g_timer0_ctrl, timer_callback);
    start_gpt(&g_timer0_ctrl);
}

// ■ timer 콜백 설정
void set_timer_callback(timer_ctrl_t * const p_ctrl, void (*callback)(timer_callback_args_t *pargs)){
    err = R_GPT_CallbackSet(p_ctrl, callback , NULL, NULL);
    if(err == FSP_SUCCESS) uart_write("GPT Callback SET 성공", NO_VAR);
    else uart_write("GPT Callback SET 실패", NO_VAR);
}

// ■ 타이머 콜백 함수
void timer_callback(timer_callback_args_t *p_args) {
    (void)p_args; // 이벤트 사용하지 않음

    // 타이머가 설정되어 있고, 남은 시간이 있는 경우
    if (g_timer_set && (g_new_minutes > 0 || g_new_seconds > 0)) {
        g_new_tick++; // 타이머 카운트 증가

        // 1초마다
        if (g_new_tick >= TICK_PER_ONE_SEC) {
            g_new_tick = 0;
            if (g_new_seconds > 0) {
                g_new_seconds--;
            } else if (g_new_minutes > 0) {
                g_new_minutes--;
                g_new_seconds = 59;
            }
            g_uart_tick_output_flag = true; // UART 출력을 위한 플래그 설정
        }
    }

    // 남은 시간이 0이 되었을 때
    if (g_new_minutes == 0 && g_new_seconds == 0 && g_timer_set) {
        g_timer_set = false; // 타이머 종료
        if (g_is_RGB_LED_ON_by_cmd) {
            RGB_LED_ON(); // LED 점등
        } else {
            RGB_LED_OFF(); // LED 소등
        }
    }
}

// ■ 타이머 설정을 초기화하는 함수
void set_timer(uint32_t minutes, _Bool led_on) {
    uart_write(led_on ? "[타이머] LED ON 예약" : "[타이머] LED OFF 예약", (uint16_t)minutes);
    g_timer_target_ticks = minutes * TICK_PER_ONE_MIN;  // 타이머 목표 설정
    g_new_tick = 0; // 타이머 카운트 초기화
    g_timer_set = true;
    g_is_RGB_LED_ON_by_cmd = led_on;

    // 타이머 설정 시 초기 시간 설정
    g_new_minutes = (int)minutes;  // 분으로 설정
    g_new_seconds = 0;        // 초는 0으로 시작
}



/***
예)
입력                  : abcdeabc<R10>xyz<R20>def
출력 (g_big_rx_buffer): R10|R20|
***/
// ■ 명령어를 입력받아 처리하는 함수
void receive_command(){
    int recording = 0; // <(헤더)를 만나면 기록 시작하므로 1, >(테일)을 만나면 기록 종료하므로 0
    int index = 0; // 추출할 데이터를 담을 위치 (인덱스)
    uint8_t temp_buffer[UART_TX_BUF_SIZE];

    // 버퍼에서 한 글자씩 살펴보기
    for(int i=0; g_rx_buffer[i] != END_CHARACTER ; i++){
        uint8_t c = g_rx_buffer[i];

        // 만약 HEADER 이면,
        if(c == HEADER){
            recording = 1;
            index = 0;
            continue;
        }

        // 만약 TAIL 이면,
        else if ((c == TAIL) & (recording)){
            recording = 0;
            temp_buffer[index] = '|'; // 문자열의 끝
            strcat((char*)g_big_rx_buffer, (char*)temp_buffer); // 추출한 문자열을, 큰 버퍼에 추가
            continue;
        }

        // recording 중이면, 임시 버퍼에 저장
        if(recording){
            if(index < UART_TX_BUF_SIZE){
                temp_buffer[index++] = c; // 임시 버퍼에 저장하기
            }
        }
    }
    // 추출한 문자열 출력해보기
//    uart_write(g_big_rx_buffer, NO_VAR);
}


// ■ 명령어들을 처리하는 함수
void process_commands(){
    int index = 0;
    int start = 0; // 명령어 첫 글자 인덱스
    int length = (int)strlen(g_big_rx_buffer); // 전체 명령어 문자열의 길이

    while (index < length) {
        if (g_big_rx_buffer[index] == '|') {
            if(index > start) process_single_command(g_big_rx_buffer + start); // 하나의 명령어 처리
            start = index + 1; // 다음 명령어 시작 위치 업데이트
        }
        index++;
    }

    // 명령어 flush<
    memset(g_big_rx_buffer, 0, sizeof(g_big_rx_buffer));

}

// ■ 명령어 하나를 처리하는 함수
void process_single_command(uint8_t* command) {
    uint8_t first_cmd = command[0]; // R | G | B | T | A
    uint8_t* secnd_cmd = command + 1;

    uint32_t brightness; // 밝기
    uint32_t minutes;    // 시간

    switch(first_cmd){
        case 'R': case 'r':
            g_manual_control = true;  // 수동 제어 활성화
            brightness = convert_brightness_to_duty_cycle((uint32_t)atoi(secnd_cmd));
            uart_write("\033[33mR LED 밝기 변경 명령어", (uint16_t)brightness);
            set_duty_cycle(&g_timer3_ctrl, brightness);
            write_duty_cycle();
            break;
        case 'G': case 'g':
            g_manual_control = true;  // 수동 제어 활성화
            brightness = convert_brightness_to_duty_cycle((uint32_t)atoi(secnd_cmd));
            uart_write("\033[33mG LED 밝기 변경 명령어", (uint16_t)brightness);
            set_duty_cycle(&g_timer4_ctrl, brightness);
            write_duty_cycle();
            break;
        case 'B': case 'b':
            g_manual_control = true;  // 수동 제어 활성화
            brightness = convert_brightness_to_duty_cycle((uint32_t)atoi(secnd_cmd)); // "B" 다음 숫자 추출
            uart_write("\033[33mB LED 밝기 변경 명령어", (uint16_t)brightness);
            set_duty_cycle(&g_timer6_ctrl, brightness);
            write_duty_cycle();
            break;

        case 'T': case 't': {
            g_manual_control = true;  // 수동 제어 활성화
            uint8_t temp[3] = {0};  // 임시 버퍼 for storing number (max "99")
            int i = 0;
            while (isdigit(secnd_cmd[i]) && i < 2) {
                temp[i] = secnd_cmd[i];
                i++;
            }
            temp[i] = '\0';
            minutes = (uint32_t)atoi(temp);

            uint8_t* on_or_off = secnd_cmd + i;

            // ON/OFF
            if (strncmp(on_or_off, "ON", 2) == 0) {
                if (is_RGB_LED_ON()) uart_write("\033[37;41m이미 LED가 켜져 있습니다.", NO_VAR);
                else set_timer(minutes, true);
            }

            else if (strncmp(on_or_off, "OFF", 3) == 0) {
                if (!is_RGB_LED_ON()) uart_write("\033[37;41m이미 LED가 꺼져 있습니다.", NO_VAR);
                else set_timer(minutes, false);
            }

            // ON / OFF 가 아니면,
            else command_err_handle();

            break;
        }

        case 'S': case 's':
            g_timer_set = false;
            g_new_tick = 0;
            R_GPT_Reset(&g_timer3_ctrl);
            R_GPT_Reset(&g_timer4_ctrl);
            R_GPT_Reset(&g_timer6_ctrl);
            uart_write("타이머가 리셋되었습니다.", NO_VAR);
            for (uint8_t i = 0; i < UART_RX_BUF_SIZE; i++) g_rx_buffer[i] = 0;
            break;

        case 'A': case 'a':
        {
            if(strncmp(secnd_cmd, "ON", 2) == 0){
                uart_write("\033[35m자동모드로 변환합니다.", NO_VAR);
                g_manual_control = false; // auto mode ON
            }
            else if(strncmp(secnd_cmd, "OFF", 3) == 0){
                uart_write("\033[35m수동모드로 변환합니다.", NO_VAR);
                g_manual_control = true; // auto mode OFF
            }
            else command_err_handle();

            break;
        }

        // LED ON / OFF (백색)
        case 'O': case 'o':
            if(strncmp(secnd_cmd, "N", 1) == 0) {
                RGB_LED_ON();
                g_is_RGB_LED_ON_by_cmd = true;
                uart_write("\033[33mLED ON", NO_VAR);
            }
            else if(strncmp(secnd_cmd, "FF", 2)==0) {
                RGB_LED_OFF();
                g_is_RGB_LED_ON_by_cmd = false;
                uart_write("\033[33mLED OFF", NO_VAR);
            }
            else command_err_handle();
            break;

            // 프로그램 종료 (EXIT)
        case 'E': case 'e':
            if(strncmp(secnd_cmd, "XIT", 3) == 0) while(1);
            else command_err_handle();
            break;


        default:
            command_err_handle();
            break;
    }
}


// ■ 명령어 에러 처리: 형식에 맞지 않는 명령어 처리
void command_err_handle() {
    uart_write("\033[37;41m명령어 형식을 확인해주세요.", NO_VAR);
    uart_write("\033[37현재 입력된 명령어 : ", NO_VAR);
    uart_write(g_big_rx_buffer, NO_VAR);
    uart_write("\033[37m<명령어>", NO_VAR);
    uart_write("\033[37m[LED제어] R50 | G10 | B40", NO_VAR);
    uart_write("\033[37m[타이머] T10", NO_VAR);
    uart_write("\033[37m[타이머 리셋] S", NO_VAR);
    uart_write("\033[37m[모드 변경] AON | AOFF", NO_VAR);
    uart_write("\033[37m[LED 제어] ON | OFF", NO_VAR);
    uart_write("\033[37m[프로그램 종료] EXIT", NO_VAR);
    g_rx_index = 0;  // 인덱스 초기화
}

// ■ 시간 출력 함수
void write_time() {
    if (g_uart_tick_output_flag) {
        snprintf(g_tx_buffer, sizeof(g_tx_buffer), "\033[A\033[K%02d:%02d", g_new_minutes, g_new_seconds);
        uart_write(g_tx_buffer, NO_VAR);
        g_uart_tick_output_flag = false;
    }
}


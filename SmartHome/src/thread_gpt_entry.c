//#include <thread_gpt.h>
//#include "hal_data.h"
//#include "common.h"
//
//_Bool is_RGB_LED_ON();
//void RGB_LED_OFF();
//void RGB_LED_ON();
//void RGB_HALF_ON();
//void RGB_HALF_ON();
//
///*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
///*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
///*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
///* New Thread entry function */
///* pvParameters contains TaskHandle_t */
//void thread_gpt_entry(void *pvParameters)
//{
//    FSP_PARAMETER_NOT_USED (pvParameters);
//    pwm_init();
//
//    /* TODO: add your own code here */
//    while (1)
//    {
//        vTaskDelay (1);
//    }
//}
///*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
//
//
//// ■ RGB_LED 점등 여부
//_Bool is_RGB_LED_ON(){
//    if(g_R_LED_duty_cycle>0 || g_G_LED_duty_cycle >0 || g_B_LED_duty_cycle >0) return true;
//    else return false;
//}
//
//// ■ RGB_LED 끄기 (모든 LED OFF)
//void RGB_LED_OFF(){
//    g_R_LED_duty_cycle = 0;
//    g_G_LED_duty_cycle = 0;
//    g_B_LED_duty_cycle = 0;
//
//    set_duty_cycle(&g_timer3_ctrl, g_R_LED_duty_cycle);
//    set_duty_cycle(&g_timer4_ctrl, g_G_LED_duty_cycle);
//    set_duty_cycle(&g_timer6_ctrl, g_B_LED_duty_cycle);
//}
//
//// ■ RGB_LED 켜기 (모든 LED ON)
//void RGB_LED_ON(){
//    g_R_LED_duty_cycle = RGB_PWM_PERIOD;
//    g_G_LED_duty_cycle = RGB_PWM_PERIOD;
//    g_B_LED_duty_cycle = RGB_PWM_PERIOD;
//
//    set_duty_cycle(&g_timer3_ctrl, g_R_LED_duty_cycle);
//    set_duty_cycle(&g_timer4_ctrl, g_G_LED_duty_cycle);
//    set_duty_cycle(&g_timer6_ctrl, g_B_LED_duty_cycle);
//}
//
//// ■ RGB_LED 반만 켜기 (약간 어둡게)
//void RGB_HALF_ON(){
//    g_R_LED_duty_cycle = gamma_correct_duty_cycle(RGB_PWM_PERIOD/2);
//    g_G_LED_duty_cycle = gamma_correct_duty_cycle(RGB_PWM_PERIOD/2);
//    g_B_LED_duty_cycle = gamma_correct_duty_cycle(RGB_PWM_PERIOD/2);
//
//    set_duty_cycle(&g_timer3_ctrl, g_R_LED_duty_cycle);
//    set_duty_cycle(&g_timer4_ctrl, g_G_LED_duty_cycle);
//    set_duty_cycle(&g_timer6_ctrl, g_B_LED_duty_cycle);
//}
//
//// ■ GPT Duty Cycle 변경
//void set_duty_cycle(timer_ctrl_t * const p_ctrl, uint32_t const duty_cycle) {
//    err = R_GPT_DutyCycleSet(p_ctrl, duty_cycle, GPT_IO_PIN_GTIOCA);
////    if (err == FSP_SUCCESS) uart_write("\033[35mDuty Cycle 설정 성공", (uint16_t)duty_cycle); // \033[35m : 보라색
////    else uart_write("\033[37;41mDuty Cycle 설정 실패", err);
//
//    if (p_ctrl == &g_timer3_ctrl) g_R_LED_duty_cycle = duty_cycle;
//    else if (p_ctrl == &g_timer4_ctrl) g_G_LED_duty_cycle = duty_cycle;
//    else if (p_ctrl == &g_timer6_ctrl) g_B_LED_duty_cycle = duty_cycle;
//}
//
//// ■ 감마 보정
//uint32_t gamma_correct_duty_cycle(uint32_t duty_cycle) {
//    // 듀티 사이클을 [0, 1] 범위로 정규화
//    double normalized_duty_cycle = (double)duty_cycle / RGB_PWM_PERIOD;
//
//    // 감마 보정
//    double corrected_intensity = pow(normalized_duty_cycle, 1/GAMMA);
//
//    // 보정된 듀티 사이클을 다시 0-255 범위로 조정
//    uint32_t corrected_duty_cycle = (uint32_t)(corrected_intensity * RGB_PWM_PERIOD);
//    return corrected_duty_cycle;
//}
//
//// ■ GPT START
//void start_gpt(timer_ctrl_t * const p_ctrl) {
//    err = R_GPT_Start(p_ctrl);
//    if (err == FSP_SUCCESS) uart_write("START GPT 성공", NO_VAR); // 성공 메시지
//    else uart_write("\033[37;41mSTART GPT 실패", err); // 실패 메시지와 오류 코드
//}
//
//
//// ■ PWM 초기화
//void pwm_init(){
//    // (1) GPT OPEN : 3, 4, 6
//    gpt_open(&g_timer3_ctrl, &g_timer3_cfg);
//    gpt_open(&g_timer4_ctrl, &g_timer4_cfg);
//    gpt_open(&g_timer6_ctrl, &g_timer6_cfg);
//
//    // (2) SET Period
//    set_period(&g_timer3_ctrl, RGB_PWM_PERIOD);
//    set_period(&g_timer4_ctrl, RGB_PWM_PERIOD);
//    set_period(&g_timer6_ctrl, RGB_PWM_PERIOD);
//
//    // (3) SET Duty Cycle
//    set_duty_cycle(&g_timer3_ctrl, RGB_PWM_PERIOD);
//    set_duty_cycle(&g_timer4_ctrl, RGB_PWM_PERIOD);
//    set_duty_cycle(&g_timer6_ctrl, RGB_PWM_PERIOD);
//
//    // (4) GPT Start
//    start_gpt(&g_timer3_ctrl);
//    start_gpt(&g_timer4_ctrl);
//    start_gpt(&g_timer6_ctrl);
//
//}
//
//
//// ■ GPT OPEN
//void gpt_open(timer_ctrl_t * const p_ctrl, timer_cfg_t const * const p_cfg) {
//    err = R_GPT_Open(p_ctrl, p_cfg);
//    if (err == FSP_SUCCESS) {
//        uart_write("GPT OPEN 성공", NO_VAR);
//        R_BSP_PinWrite(BSP_IO_PORT_01_PIN_00, BSP_IO_LEVEL_HIGH);
//        R_BSP_PinWrite(BSP_IO_PORT_01_PIN_00, BSP_IO_LEVEL_LOW);
//    }
//    else {
//        uart_write("\033[37;41mGPT OPEN 실패", err);
//        R_BSP_PinWrite(BSP_IO_PORT_01_PIN_00, BSP_IO_LEVEL_HIGH);
//        R_BSP_PinWrite(BSP_IO_PORT_01_PIN_00, BSP_IO_LEVEL_LOW);
//    }
//
//}
//
//// ■ GPT SET Period
//void set_period(timer_ctrl_t * const p_ctrl, uint32_t const period_counts) {
//    err = R_GPT_PeriodSet(p_ctrl, period_counts);
//    if (err == FSP_SUCCESS) uart_write("\033[35mPeriod Set 성공", NO_VAR);
//    else uart_write("\033[37;41mPeriod Set 실패", err);
//}
//
//// ■ GPT 콜백 함수
//void g_timer_callback(timer_callback_args_t *p_args) {
//    (void)p_args; // 이벤트 사용하지 않음
//
//    // 타이머가 설정되어 있고, 남은 시간이 있는 경우
//    if (g_timer_set && (g_new_minutes > 0 || g_new_seconds > 0)) {
//        g_new_tick++; // 타이머 카운트 증가
//
//        // 1초마다
//        if (g_new_tick >= TICK_PER_ONE_SEC) {
//            g_new_tick = 0;
//            if (g_new_seconds > 0) {
//                g_new_seconds--;
//            } else if (g_new_minutes > 0) {
//                g_new_minutes--;
//                g_new_seconds = 59;
//            }
//        }
//    }
//
//    // 남은 시간이 0이 되었을 때
//    if (g_new_minutes == 0 && g_new_seconds == 0 && g_timer_set) {
//        g_timer_set = false; // 타이머 종료
//        if (g_is_RGB_LED_ON_by_cmd) {
//            RGB_LED_ON(); // LED 점등
//        } else {
//            RGB_LED_OFF(); // LED 소등
//        }
//    }
//}
//
//// ■ RGB LED Duty Cycle 출력
//void write_duty_cycle() {
//    uart_write("R DutyCycle", (uint16_t)g_R_LED_duty_cycle);
//    uart_write("G DutyCycle", (uint16_t)g_G_LED_duty_cycle);
//    uart_write("B DutyCycle", (uint16_t)g_B_LED_duty_cycle);
//}
//
//
//// 이 함수는 thread_gpt_entry()에서 미사용.
//// 명령어 입력 thread 에서 사용
//// ■ RGB_LED Duty Cycle 변경을 통한, 밝기 조절
//void set_duty_cycles_by_ratio(int n) {
////    // 전체 듀티 사이클 합을 계산
////    uint32_t total_duty = g_R_LED_duty_cycle + g_G_LED_duty_cycle + g_B_LED_duty_cycle;
////    uart_write("total duty", (uint16_t)total_duty);
//    // LED OFF 상태이면,
//    if(g_R_LED_duty_cycle == 0 && g_G_LED_duty_cycle == 0 && g_B_LED_duty_cycle == 0){
//        // 아직 생각 중
//        uart_write("No duty cycle set", NO_VAR);
//        set_duty_cycle(&g_timer3_ctrl, RGB_PWM_PERIOD);
//        set_duty_cycle(&g_timer6_ctrl, RGB_PWM_PERIOD);
//    }
//    else{
//        // 새로운 듀티 사이클 계산
//        uint32_t new_r_duty = gamma_correct_duty_cycle(g_R_LED_duty_cycle/ 3 * (uint32_t)n);
//        uint32_t new_g_duty = gamma_correct_duty_cycle(g_G_LED_duty_cycle/ 3 * (uint32_t)n);
//        uint32_t new_b_duty = gamma_correct_duty_cycle(g_B_LED_duty_cycle/ 3 * (uint32_t)n);
//
//
//        // 새로운 듀티 사이클로 설정
//        set_duty_cycle(&g_timer3_ctrl, new_r_duty);
//        set_duty_cycle(&g_timer4_ctrl, new_g_duty);
//        set_duty_cycle(&g_timer6_ctrl, new_b_duty);
//
//        // 새로운 듀티 사이클을 전역 변수에 업데이트
//        g_R_LED_duty_cycle = new_r_duty;
//        g_G_LED_duty_cycle = new_g_duty;
//        g_B_LED_duty_cycle = new_b_duty;
//    }
//}
//
//
//// 이 함수는 thread_gpt_entry()에서 미사용.
//// 명령어 입력 thread 에서 사용
//// ■ 밝기를 Duty Cycle로 변경 (밝기 명령어 범위: 0~100), (DutyCycle 범위: 0~RGB_PWM_PERIOD)
//uint32_t convert_brightness_to_duty_cycle(uint32_t brightness) {
//    // 범위 초과 방지
//    if (brightness > 100) brightness = 100;
//
//    // 밝기 > Duty Cycle 은 선형 변환, 이후 감마 보정
//    uint32_t duty_cycle = (brightness * RGB_PWM_PERIOD / 100);
//    return gamma_correct_duty_cycle(duty_cycle);
//}
//

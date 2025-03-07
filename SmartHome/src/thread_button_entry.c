#include "thread_button.h"
#include "common.h"
#include "hal_data.h"

/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/
/* Thread_button entry function */
/* pvParameters contains TaskHandle_t */
void thread_button_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* TODO: add your own code here */
    while (1)
    {
        // 색상 변경 버튼 클릭?
        check_btn_clicked(BUTTON_S1, g_color_btn_level, &g_color_btn_state);

        // 밝기 조절 버튼 클릭?
        check_btn_clicked(BUTTON_S2, g_brightness_btn_level, &g_brightness_btn_state);
        vTaskDelay (1);
    }
}
/*■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■*/

// ■ 색상 변경 클릭 여부 확인
void check_btn_clicked(bsp_io_port_pin_t BUTTON, bsp_io_level_t g_btn_level, ButtonState* g_btn_state){
    uint16_t btn_num = BUTTON - 4; // S1은 핀 5번, S2는 핀 6번 (BUTTON은 핀 번호)

    // 버튼 상태 읽어오기
    err = R_IOPORT_PinRead(&g_ioport_ctrl, BUTTON, &g_btn_level);
    if(err == FSP_SUCCESS){
        _Bool button_pressed = !g_btn_level; // PRESS 되면, BTN_LEVEL = 0
        switch(*g_btn_state){
            case WAITING_FOR_PRESS:
                if(button_pressed){
                    uart_write("\033[32m버튼 PRESS", btn_num); // 초록: \033[32m
                    *g_btn_state = WAITING_FOR_RELEASE;
                }
                else {
//                    uart_write("\033[32m버튼 WAIT FOR PRESS", btn_num); // 초록: \033[32m
                }
                break;


                // PRESS 해제 되기를 기다리는 상태 (PRESS된 상태)
            case WAITING_FOR_RELEASE:
                if(!button_pressed){
                    uart_write("\033[32m버튼 RELEASE", btn_num);
                    *g_btn_state = WAITING_FOR_PRESS;
                    // 버튼 클릭 시, on/off 처리
                    g_manual_control = true;  // 수동 제어 활성화
                    handle_btn_click(btn_num);
                }
                else {
//                    uart_write("\033[32m버튼 WAIT FOR RELEASE", btn_num); // 초록: \033[32m
                }
                break;
        }
    }

}


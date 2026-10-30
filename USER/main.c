#include "main.h"
#include <stdio.h>

/*
 * STM32F407ZGT6 智能桌面机器人主程序。
 * 1. 上电后自动逐帧播放并轮播 21 组小智动态表情。
 * 2. USART6 接收动作编号并执行对应动作。
 * 3. 当前语音模块映射：2=坐下，坐下后显示 DHT11 温湿度 8 秒。
 * 4. 7=身体前后摇摆两次，10=趴下。
 */

/*
 * 读取 DHT11 并显示 8 秒。
 * 读取失败时显示 READ ERR，8 秒后自动恢复小智动态表情。
 */
static void DHT11_ShowFor8Seconds(void)
{
    DHT11_Data_t data;
    char line[20];

    OLED_Clear();

    if (DHT11_Read(&data) != 0u)
    {
        OLED_ShowString(44, 0, "DHT11", 1);

        sprintf(line, "TEMP:%2d.%d C",
                (int)data.temperature_int, (int)data.temperature_dec);
        OLED_ShowString(16, 24, line, 1);

        sprintf(line, "HUMI:%2d.%d %%",
                (int)data.humidity_int, (int)data.humidity_dec);
        OLED_ShowString(16, 48, line, 1);
    }
    else
    {
        OLED_ShowString(44, 8, "DHT11", 1);
        OLED_ShowString(32, 34, "READ ERR", 1);
    }

    OLED_Refresh();
    delay_s(8u);   /* delay_ms 单次最多约 798ms，使用秒函数保证完整 8 秒 */
}

int main(void)
{
    uint8_t emoji = XIAOZHI_EMOJI_NEUTRAL;  /* 当前表情组 */
    uint8_t frame = 0;                      /* 当前表情组内的帧号 */
    uint8_t command;                        /* 本次从 USART6 取出的命令 */
    uint8_t auto_play = 1;                  /* 1：一轮表情结束后自动切换下一组 */

    NVIC_SetPriorityGrouping(3);

    LED_Init();
    PWM_Init();
    OLED_Init();
    DHT11_Init();
    Uart6_Init(9600);
    delay_ms(100);

    /* 上电先显示 neutral 第一帧，随后由主循环连续播放。 */
    LED_ShowXiaozhiEmojiFrame(emoji, frame);

    while (1)
    {
        /* 每帧停留 80 ms，与上游 GIF 的帧间隔一致。 */
        delay_ms(XIAOZHI_FRAME_DELAY_MS);

        frame++;
        if (frame >= LED_GetXiaozhiEmojiFrameCount(emoji))
        {
            frame = 0;

            /* 一组动画播放完后自动进入下一组，形成完整表情轮播。 */
            if (auto_play)
            {
                emoji++;
                if (emoji >= LED_GetXiaozhiEmojiCount())
                {
                    emoji = XIAOZHI_EMOJI_NEUTRAL;
                }
            }
        }

        LED_ShowXiaozhiEmojiFrame(emoji, frame);

        /* 读取并清除本轮命令，避免动作函数执行期间重复触发。 */
        command = Usart6_Recv_data;
        if (command == 0u)
        {
            continue;
        }
        Usart6_Recv_data = 0;

        switch (command)
        {
            case 1: /* 起立 */
                emoji = XIAOZHI_EMOJI_NEUTRAL;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Up();
                break;

            case 2: /* 坐下：动作完成后显示温湿度 8 秒 */
                emoji = XIAOZHI_EMOJI_HAPPY;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Sit();
                DHT11_ShowFor8Seconds();
                break;

            case 3: /* 开灯 */
                LED_ON();
                break;

            case 4: /* 前进 */
                emoji = XIAOZHI_EMOJI_CONFIDENT;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Advance();
                break;

            case 5: /* 关灯 */
                LED_OFF();
                break;

            case 6: /* 后退 */
                emoji = XIAOZHI_EMOJI_NEUTRAL;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Forward();
                break;

            case 7: /* 身体前后摇摆两次（扭屁股） */
                emoji = XIAOZHI_EMOJI_SILLY;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Rock_Twice();
                break;

            case 8: /* 叫：切换为惊讶表情 */
                emoji = XIAOZHI_EMOJI_SURPRISED;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                break;

            case 9: /* 滚 */
                emoji = XIAOZHI_EMOJI_CRYING;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Getdowm();
                break;

            case 10: /* 趴下 */
                emoji = XIAOZHI_EMOJI_SLEEPY;
                frame = 0;
                LED_ShowXiaozhiEmojiFrame(emoji, frame);
                Action_Getdowm();
                break;

            default:
                /* 未定义命令忽略，继续播放当前动态表情。 */
                break;
        }
    }
}

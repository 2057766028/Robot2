#include "dht11.h"
#include "delay.h"

#define DHT11_TIMEOUT_US  300u

/* 读取 DAT 的当前电平。 */
static uint8_t DHT11_ReadPin(void)
{
    return (GPIO_ReadInputDataBit(DHT11_PORT, DHT11_PIN) == Bit_SET) ? 1u : 0u;
}

/* 将 DAT 配置为开漏输出，用于主机发送起始信号。 */
static void DHT11_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/* 将 DAT 切换为高阻输入，由外部或内部上拉拉高。 */
static void DHT11_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_SetBits(DHT11_PORT, DHT11_PIN);
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);
}

/* 等待 DAT 离开指定电平，超时返回 0。 */
static uint8_t DHT11_WaitForLevel(uint8_t level, uint16_t timeout_us)
{
    while (DHT11_ReadPin() == level)
    {
        if (timeout_us == 0u)
        {
            return 0u;
        }
        delay_us(1u);
        timeout_us--;
    }
    return 1u;
}

/* 初始化 DHT11 数据引脚。 */
void DHT11_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    DHT11_SetInput();
}

/*
 * 读取 DHT11 的 40 位数据。
 * 位 0 的高电平约 26~28us，位 1 的高电平约 70us；
 * 释放总线后等待 40us，再读取电平即可区分 0 和 1。
 */
uint8_t DHT11_Read(DHT11_Data_t *data)
{
    uint8_t raw[5] = {0u, 0u, 0u, 0u, 0u};
    uint8_t bit_index;
    uint8_t byte_index;
    uint8_t ok = 1u;
    uint32_t primask;

    if (data == 0)
    {
        return 0u;
    }

    /* 主机发送至少 18ms 低电平起始信号。 */
    DHT11_SetOutput();
    GPIO_ResetBits(DHT11_PORT, DHT11_PIN);
    delay_ms(20u);
    GPIO_SetBits(DHT11_PORT, DHT11_PIN);

    /* 释放总线，等待 DHT11 的 80us 低电平、80us 高电平响应。 */
    DHT11_SetInput();
    delay_us(30u);

    /* 读取 40 位期间关闭中断，避免 USART 中断拉长时序。 */
    primask = __get_PRIMASK();
    __disable_irq();

    if (DHT11_WaitForLevel(1u, DHT11_TIMEOUT_US) == 0u)
    {
        ok = 0u;
    }
    if ((ok != 0u) && (DHT11_WaitForLevel(0u, DHT11_TIMEOUT_US) == 0u))
    {
        ok = 0u;
    }
    if ((ok != 0u) && (DHT11_WaitForLevel(1u, DHT11_TIMEOUT_US) == 0u))
    {
        ok = 0u;
    }

    if (ok != 0u)
    {
        for (bit_index = 0u; bit_index < 40u; bit_index++)
        {
            /*
             * 进入循环时总线处于本位 50us 低电平阶段。
             * 先等待低电平结束（上升沿），再从高电平起点延时 40us 采样：
             *   - 0 位高电平约 26us，40us 后已经变低；
             *   - 1 位高电平约 70us，40us 后仍为高。
             * 最后等待下降沿，进入下一位的低电平阶段。
             */
            if (DHT11_WaitForLevel(0u, DHT11_TIMEOUT_US) == 0u)
            {
                ok = 0u;
                break;
            }

            delay_us(40u);

            byte_index = (uint8_t)(bit_index / 8u);
            raw[byte_index] <<= 1;
            if (DHT11_ReadPin() != 0u)
            {
                raw[byte_index] |= 0x01u;
            }

            if (DHT11_WaitForLevel(1u, DHT11_TIMEOUT_US) == 0u)
            {
                ok = 0u;
                break;
            }
        }
    }

    __set_PRIMASK(primask);

    if (ok == 0u)
    {
        return 0u;
    }

    if ((uint8_t)(raw[0] + raw[1] + raw[2] + raw[3]) != raw[4])
    {
        return 0u;
    }

    /*
     * 增加有效范围检查。全零数据的校验和也是 0，若不检查会被误判为成功；
     * DHT11 的湿度范围为 0~100%RH，温度范围为 0~60℃。
     */
    if ((raw[0] > 100u) || (raw[1] > 9u) ||
        (raw[2] > 60u) || (raw[3] > 9u) ||
        ((raw[0] == 0u) && (raw[1] == 0u) &&
         (raw[2] == 0u) && (raw[3] == 0u)))
    {
        return 0u;
    }

    data->humidity_int = raw[0];
    data->humidity_dec = raw[1];
    data->temperature_int = raw[2];
    data->temperature_dec = raw[3];
    return 1u;
}

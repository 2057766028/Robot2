#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"

/* 小智表情资源参数。 */
#define XIAOZHI_EMOJI_COUNT       21u
#define XIAOZHI_FRAME_DELAY_MS    80u

/* 小智标准表情索引，顺序与上游 GIF 文件及 LED.c 帧表一致。 */
typedef enum
{
    XIAOZHI_EMOJI_NEUTRAL = 0,      /* neutral：平静 */
    XIAOZHI_EMOJI_HAPPY,            /* happy：开心 */
    XIAOZHI_EMOJI_LAUGHING,         /* laughing：大笑 */
    XIAOZHI_EMOJI_FUNNY,            /* funny：滑稽 */
    XIAOZHI_EMOJI_SAD,              /* sad：难过 */
    XIAOZHI_EMOJI_ANGRY,            /* angry：生气 */
    XIAOZHI_EMOJI_CRYING,           /* crying：哭泣 */
    XIAOZHI_EMOJI_LOVING,           /* loving：喜爱 */
    XIAOZHI_EMOJI_EMBARRASSED,      /* embarrassed：害羞 */
    XIAOZHI_EMOJI_SURPRISED,        /* surprised：惊讶 */
    XIAOZHI_EMOJI_SHOCKED,          /* shocked：震惊 */
    XIAOZHI_EMOJI_THINKING,         /* thinking：思考 */
    XIAOZHI_EMOJI_WINKING,          /* winking：眨眼 */
    XIAOZHI_EMOJI_COOL,             /* cool：酷 */
    XIAOZHI_EMOJI_RELAXED,          /* relaxed：放松 */
    XIAOZHI_EMOJI_DELICIOUS,        /* delicious：馋 */
    XIAOZHI_EMOJI_KISSY,            /* kissy：亲吻 */
    XIAOZHI_EMOJI_CONFIDENT,        /* confident：自信 */
    XIAOZHI_EMOJI_SLEEPY,           /* sleepy：困倦 */
    XIAOZHI_EMOJI_SILLY,            /* silly：调皮 */
    XIAOZHI_EMOJI_CONFUSED          /* confused：疑惑 */
} XIAOZHI_Emoji_t;

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);

/* 查询表情组数量、指定表情帧数和英文名称。 */
uint8_t LED_GetXiaozhiEmojiCount(void);
uint8_t LED_GetXiaozhiEmojiFrameCount(uint8_t emoji);
const char *LED_GetXiaozhiEmojiName(uint8_t emoji);

/* 显示指定表情组的指定帧，非法索引会自动回落到有效范围。 */
void LED_ShowXiaozhiEmojiFrame(uint8_t emoji, uint8_t frame);

#endif

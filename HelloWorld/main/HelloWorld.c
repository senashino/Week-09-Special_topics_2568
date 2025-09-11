// ====== โค้ดเต็มที่ถูกต้อง (วางแทนทั้งไฟล์) ======
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "esp_random.h"

static const char *TAG = "LED_BREATH_PATTERNS";

#define LED1_GPIO    GPIO_NUM_2
#define LED2_GPIO    GPIO_NUM_4
#define LED3_GPIO    GPIO_NUM_5

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL_BASE       LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY          5000
#define DUTY_MAX                ((1 << LEDC_DUTY_RES) - 1)

#define BREATH_STEPS            64
#define BREATH_STEP_DELAY_MS    15
#define HOLD_BETWEEN_MS         600

static const gpio_num_t LED_PINS[] = { LED1_GPIO, LED2_GPIO, LED3_GPIO };
static const ledc_channel_t LED_CHS[] = {
    LEDC_CHANNEL_BASE + 0,
    LEDC_CHANNEL_BASE + 1,
    LEDC_CHANNEL_BASE + 2
};
#define NUM_LEDS (sizeof(LED_PINS)/sizeof(LED_PINS[0]))

static void leds_pwm_init(void) {
    ledc_timer_config_t tcfg = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK( ledc_timer_config(&tcfg) );

    for (int i = 0; i < NUM_LEDS; ++i) {
        ledc_channel_config_t ccfg = {
            .gpio_num   = LED_PINS[i],
            .speed_mode = LEDC_MODE,
            .channel    = LED_CHS[i],
            .intr_type  = LEDC_INTR_DISABLE,
            .timer_sel  = LEDC_TIMER,
            .duty       = 0,
            .hpoint     = 0,
            .flags.output_invert = 0,
        };
        ESP_ERROR_CHECK( ledc_channel_config(&ccfg) );
    }
}

static inline void led_set_brightness_idx(int idx, uint32_t duty) {
    if (idx < 0 || idx >= NUM_LEDS) return;
    if (duty > DUTY_MAX) duty = DUTY_MAX;
    ledc_set_duty(LEDC_MODE, LED_CHS[idx], duty);
    ledc_update_duty(LEDC_MODE, LED_CHS[idx]);
}

static inline void leds_all_off(void) {
    for (int i = 0; i < NUM_LEDS; ++i) led_set_brightness_idx(i, 0);
}

static inline void leds_set_group_brightness(uint8_t mask, uint32_t duty) {
    for (int i = 0; i < NUM_LEDS; ++i) {
        if (mask & (1U << i)) led_set_brightness_idx(i, duty);
        else                  led_set_brightness_idx(i, 0);
    }
}

static void breathe_once(uint8_t mask) {
    for (int s = 0; s <= BREATH_STEPS; ++s) {
        float x = (float)s / (float)BREATH_STEPS;
        float b = 0.5f * (1.0f - cosf((float)M_PI * x));
        uint32_t duty = (uint32_t)(b * DUTY_MAX);
        leds_set_group_brightness(mask, duty);
        vTaskDelay(pdMS_TO_TICKS(BREATH_STEP_DELAY_MS));
    }
    for (int s = BREATH_STEPS; s >= 0; --s) {
        float x = (float)s / (float)BREATH_STEPS;
        float b = 0.5f * (1.0f - cosf((float)M_PI * x));
        uint32_t duty = (uint32_t)(b * DUTY_MAX);
        leds_set_group_brightness(mask, duty);
        vTaskDelay(pdMS_TO_TICKS(BREATH_STEP_DELAY_MS));
    }
    leds_all_off();
}

// 1) Knight Rider (breath)
static void pattern_knight_rider_breath(int cycles) {
    ESP_LOGI(TAG, "Pattern: Knight Rider (breath)");
    for (int c = 0; c < cycles; ++c) {
        // ไปทางขวา
        for (int i = 0; i < NUM_LEDS; ++i) {
            breathe_once(1U << i);
        }
        // กลับทางซ้าย (ไม่ซ้ำปลาย)
        for (int i = NUM_LEDS - 2; i >= 1; --i) {
            breathe_once(1U << i);
        }
    }
    leds_all_off();
}

// 2) Binary Counter (breath)
static void pattern_binary_counter_breath(int rounds) {
    ESP_LOGI(TAG, "Pattern: Binary Counter (breath)");
    uint8_t max_val = (1U << NUM_LEDS); // 0..7 สำหรับ 3 ดวง
    for (int r = 0; r < rounds; ++r) {
        for (uint8_t val = 0; val < max_val; ++val) {
            // หายใจตามหน้ากากบิตของค่า val
            breathe_once(val);
        }
    }
    leds_all_off();
}

// 3) Random (breath)
static void pattern_random_breath(int flashes) {
    ESP_LOGI(TAG, "Pattern: Random (breath)");
    for (int i = 0; i < flashes; ++i) {
        uint32_t rnd = esp_random();
        uint8_t mask = (uint8_t)(rnd & ((1U << NUM_LEDS) - 1));
        if (mask == 0) mask = 1U << (rnd % NUM_LEDS); // ให้ติดอย่างน้อย 1 ดวง
        breathe_once(mask);
    }
    leds_all_off();
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32 LED Breathing Patterns Started");
    leds_pwm_init();

    while (1) {
        pattern_knight_rider_breath(/*cycles=*/3);
        vTaskDelay(pdMS_TO_TICKS(HOLD_BETWEEN_MS));

        pattern_binary_counter_breath(/*rounds=*/2);
        vTaskDelay(pdMS_TO_TICKS(HOLD_BETWEEN_MS));

        pattern_random_breath(/*flashes=*/6);
        vTaskDelay(pdMS_TO_TICKS(HOLD_BETWEEN_MS));
    }
}

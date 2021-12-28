#include "irtx.h"

// #include <M5StickCPlus.h>
#include <driver/rmt.h>
#include "esp_pm.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_0
const gpio_num_t RMT_TX_GPIO_NUM = GPIO_NUM_9;

void irtx_init()
{
    rmt_config_t rmt_tx;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_tx.channel = RMT_TX_CHANNEL;
    rmt_tx.gpio_num = RMT_TX_GPIO_NUM;

    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = 80; // 80 MHz / 80 = 1 MHz

    rmt_tx.tx_config.loop_en = false;
    rmt_tx.tx_config.carrier_en = true;
    rmt_tx.tx_config.carrier_duty_percent = 50;
    rmt_tx.tx_config.carrier_freq_hz = 38000;
    rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    rmt_tx.tx_config.idle_output_en = true;

    ESP_ERROR_CHECK(rmt_config(&rmt_tx));
    ESP_ERROR_CHECK(rmt_driver_install(rmt_tx.channel, 0, 0));
}

void irtx_transmit(const uint8_t src)
{
    static const rmt_item32_t bit0 = {{{565, 1, 565, 0}}};  //Logical 0
    static const rmt_item32_t bit1 = {{{565, 1, 1671, 0}}}; //Logical 1

    static const size_t n_bits = 8 * 4 + 1;
    static rmt_item32_t tx_buffer[n_bits];

    // starting sequence
    size_t cnt = 0;
    tx_buffer[cnt].duration0 = 8985;
    tx_buffer[cnt].level0 = 1;
    tx_buffer[cnt].duration1 = 4487;
    tx_buffer[cnt].level1 = 0;
    cnt++;

    uint8_t bytes[] = {0x00, 0xEF, src, (uint8_t)~src};
    for (size_t ib = 0; ib < 4; ib++)
    {
        uint8_t data = bytes[ib];
        for (size_t i = 0; i < 8; i++)
        {
            // lsb first
            if (data & 0x01)
                tx_buffer[cnt].val = bit1.val;
            else
                tx_buffer[cnt].val = bit0.val;
            data >>= 1;
            cnt++;
        }
    }

    // print bytes for debug
    for (size_t i = 0; i < n_bits; i++)
    {
        Serial.printf("%08X ", tx_buffer[i].val);
        if (i % 8 == 7)
            Serial.println();
    }

    ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CHANNEL, tx_buffer, n_bits, false));
}

/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#if defined(ARDUINO) && (TOUCH_DRIVER == 0x826)
#include <Arduino.h>
#include "ArduinoLog.h"
#include "hasp_conf.h"
#include "touch_driver_cst826.h"

#include <Wire.h>

#include "touch_driver.h" // base class
#include "touch_helper.h" // i2c scanner

#include "../../hasp/hasp.h" // for hasp_sleep_state
extern uint8_t hasp_sleep_state;

namespace {

/**
 * @brief CST826 I2C register holding the active point count and first point.
 *
 * The layout is taken from the vendor demo:
 * byte 0: point count, byte 1/2: X high nibble and low byte,
 * byte 3/4: Y high nibble and low byte.
 */
constexpr uint8_t CST826_REG_TOUCH_DATA = 0x02;

/**
 * @brief CST826 chip ID register used only as a startup probe.
 */
constexpr uint8_t CST826_REG_CHIP_ID = 0xAA;

/**
 * @brief Number of bytes required to read the first touch point.
 */
constexpr uint8_t CST826_TOUCH_DATA_LEN = 5;

/**
 * @brief Read a CST826 register block over the shared touch I2C bus.
 *
 * @param reg Register address to read from.
 * @param data Destination buffer for the returned bytes.
 * @param len Number of bytes to read into @p data.
 * @return true when the register transaction completed with the requested
 * byte count, otherwise false.
 */
bool cst826_read_register(uint8_t reg, uint8_t* data, uint8_t len)
{
    if(data == nullptr || len == 0) return false;

    Wire.beginTransmission(I2C_TOUCH_ADDRESS);
    Wire.write(reg);
    if(Wire.endTransmission(false) != 0) return false;

    const uint8_t received = Wire.requestFrom((uint8_t)I2C_TOUCH_ADDRESS, (size_t)len, (bool)true);
    if(received != len) {
        while(Wire.available()) Wire.read();
        return false;
    }

    for(uint8_t i = 0; i < len; i++) {
        data[i] = Wire.read();
    }

    return true;
}

/**
 * @brief Convert raw CST826 bytes into a single coordinate pair.
 *
 * @param raw Five-byte register payload starting at register 0x02.
 * @param x Output raw X coordinate.
 * @param y Output raw Y coordinate.
 * @return true when the payload reports at least one active point.
 */
bool cst826_parse_first_point(const uint8_t raw[CST826_TOUCH_DATA_LEN], uint16_t& x, uint16_t& y)
{
    const uint8_t point_count = raw[0];
    if(point_count == 0) return false;

    x = (((uint16_t)raw[1] & 0x0F) << 8) | raw[2];
    y = (((uint16_t)raw[3] & 0x0F) << 8) | raw[4];
    return true;
}

} // namespace

namespace dev {

IRAM_ATTR bool TouchCst826::read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
    uint8_t raw[CST826_TOUCH_DATA_LEN] = {0};
    uint16_t raw_x                    = 0;
    uint16_t raw_y                    = 0;

    if(_initialized && cst826_read_register(CST826_REG_TOUCH_DATA, raw, sizeof(raw)) &&
       cst826_parse_first_point(raw, raw_x, raw_y)) {

        if(hasp_sleep_state != HASP_SLEEP_OFF) hasp_update_sleep_state(); // update Idle

#ifdef TOUCH_WIDTH
        raw_x         = constrain(raw_x, 0, TOUCH_WIDTH - 1);
        data->point.x = map(raw_x, 0, TOUCH_WIDTH - 1, 0, TFT_WIDTH - 1);
#else
        data->point.x = raw_x;
#endif

#ifdef TOUCH_HEIGHT
        raw_y         = constrain(raw_y, 0, TOUCH_HEIGHT - 1);
        data->point.y = map(raw_y, 0, TOUCH_HEIGHT - 1, 0, TFT_HEIGHT - 1);
#else
        data->point.y = raw_y;
#endif

        data->state = LV_INDEV_STATE_PR;
        hasp_set_sleep_offset(0); // Reset the offset
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    /* Return false because we are not buffering and no more data is pending. */
    return false;
}

void TouchCst826::init(int w, int h)
{
    Wire.begin(TOUCH_SDA, TOUCH_SCL, (uint32_t)I2C_TOUCH_FREQUENCY);

    uint8_t chip_id = 0;
    _initialized   = cst826_read_register(CST826_REG_CHIP_ID, &chip_id, 1);

    if(_initialized) {
        LOG_INFO(TAG_DRVR, "CST826 %s chipid: 0x%x", D_SERVICE_STARTED, chip_id);
    } else {
        LOG_WARNING(TAG_DRVR, "CST826 %s", D_SERVICE_START_FAILED);
    }

    touch_scan(Wire);
}

} // namespace dev

dev::TouchCst826 haspTouch;

#endif // ARDUINO

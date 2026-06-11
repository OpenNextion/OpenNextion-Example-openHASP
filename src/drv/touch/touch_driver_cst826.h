/* MIT License - Copyright (c) 2019-2024 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_CST826_TOUCH_DRIVER_H
#define HASP_CST826_TOUCH_DRIVER_H

#ifdef ARDUINO
#include "lvgl.h"
#include "touch_driver.h"

namespace dev {

/**
 * @brief CST826 capacitive touch driver using direct I2C polling.
 *
 * The ONX3248G035 board routes the CST826 interrupt through an external
 * expander, so the interrupt-driven CST816 path cannot report touches.
 * This driver reads the CST826 coordinate registers directly on each LVGL
 * touch poll and keeps the integration independent from board expander logic.
 */
class TouchCst826 : public BaseTouch {
  public:
    /**
     * @brief Initialize the CST826 I2C bus and probe the controller.
     *
     * @param w Logical display width passed by the openHASP GUI layer.
     * @param h Logical display height passed by the openHASP GUI layer.
     */
    void init(int w, int h);

    /**
     * @brief Read the current touch state for LVGL.
     *
     * @param indev_driver LVGL input driver descriptor.
     * @param data LVGL output structure receiving the point and state.
     * @return Always false because the driver does not buffer extra samples.
     */
    IRAM_ATTR bool read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

  private:
    bool _initialized = false;
};

} // namespace dev

using dev::TouchCst826;
extern dev::TouchCst826 haspTouch;

#endif // ARDUINO

#endif // HASP_CST826_TOUCH_DRIVER_H

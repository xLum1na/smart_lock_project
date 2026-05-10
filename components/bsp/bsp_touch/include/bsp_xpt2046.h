/*
 * SPDX-FileCopyrightText: 2022 atanisoft (github.com/atanisoft)
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "esp_idf_version.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_panel_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new XPT2046 touch driver
 *
 * @note The SPI communication should be initialized before use this function.
 *
 * @param io: LCD/Touch panel IO handle.
 * @param config: Touch configuration.
 * @param out_touch: XPT2046 instance handle.
 * @return
 *      - ESP_OK                    on success
 *      - ESP_ERR_NO_MEM            if there is insufficient memory for allocating main structure.
 *      - ESP_ERR_INVALID_ARG       if @param io or @param config are null.
 */
esp_err_t esp_lcd_touch_new_spi_xpt2046(const esp_lcd_panel_io_handle_t io,
                                        const esp_lcd_touch_config_t *config,
                                        esp_lcd_touch_handle_t *out_touch);

/**
 * @brief Reads the voltage from the v-bat pin of the XPT2046.
 *
 * @param handle: XPT2046 instance handle.
 * @param out_level: Approximate voltage read in from the v-bat pin.
 * @return
 *      - ESP_OK on success, otherwise returns ESP_ERR_xxx
 *
 * @note The v-bat pin has a voltage range of 0.0 to 6.0 volts.
 */
esp_err_t esp_lcd_touch_xpt2046_read_battery_level(const esp_lcd_touch_handle_t handle, float *out_level);

/**
 * @brief Reads the voltage from the aux pin of the XPT2046.
 *
 * @param handle: XPT2046 instance handle.
 * @param out_level: Approximate voltage read in from the aux pin.
 * @return
 *      - ESP_OK on success, otherwise returns ESP_ERR_xxx
 *
 * @note The aux pin has a voltage range of 0.0 to 2.5 volts.
 */
esp_err_t esp_lcd_touch_xpt2046_read_aux_level(const esp_lcd_touch_handle_t handle, float *out_level);

/**
 * @brief Reads the temperature from the XPT2046 using a one-point reading.
 *        High precision (0.3 degrees C) but low accuracy requires a
 *        calibration offset for accurate results.
 *
 * @param handle: XPT2046 instance handle.
 * @param out_level: Approximate tempreature of the TSC2046 in degrees C
 * @return
 *      - ESP_OK on success, otherwise returns ESP_ERR_xxx
 */
esp_err_t esp_lcd_touch_xpt2046_read_temp0_level(const esp_lcd_touch_handle_t handle, float *output);

/**
 * @brief Reads the temperature from the XPT2046 using a two-point reading.
 *        Low precision (1.6 degrees C) but high accuracy requires no calibration.
 *
 * @param handle: XPT2046 instance handle.
 * @param out_level: Approximate tempreature of the TSC2046 in degrees C
 * @return
 *      - ESP_OK on success, otherwise returns ESP_ERR_xxx
 */
esp_err_t esp_lcd_touch_xpt2046_read_temp1_level(const esp_lcd_touch_handle_t handle, float *output);

#ifdef __cplusplus
}
#endif

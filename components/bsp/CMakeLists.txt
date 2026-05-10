file(GLOB BSP_BSPP4_SRCS "bsp_esp32p4/*.c")
file(GLOB BSP_BSPS3_SRCS "bsp_esp32s3/*.c")
file(GLOB BSP_GPIO_SRCS "bsp_gpio/*.c")
file(GLOB BSP_LEDC_SRCS "bsp_ledc/*.c")
file(GLOB BSP_SPI_SRCS "bsp_spi/*.c")
file(GLOB BSP_DISPLAY_SRCS "bsp_lcd/*.c")
file(GLOB BSP_TOUCH_SRCS "bsp_touch/*.c")
file(GLOB BSP_CAMERA_SRCS "bsp_camera/*.c")
file(GLOB BSP_FP_SRCS "bsp_fp/*.c")
idf_component_register(
    SRCS
        ${BSP_BSPP4_SRCS}
        ${BSP_BSPS3_SRCS}
        ${BSP_GPIO_SRCS}
        ${BSP_LEDC_SRCS}
        ${BSP_SPI_SRCS}
        ${BSP_DISPLAY_SRCS}
        ${BSP_TOUCH_SRCS}
        ${BSP_CAMERA_SRCS}
        ${BSP_FP_SRCS}
    INCLUDE_DIRS 
        "bsp_esp32s3/include"
        "bsp_esp32p4/include"
        "bsp_gpio/include"
        "bsp_ledc/include"
        "bsp_spi/include"
        "bsp_lcd/include"
        "bsp_touch/include"
        "bsp_camera/include"
        "bsp_fp/include"
    REQUIRES
        driver esp_lcd esp32-camera
)
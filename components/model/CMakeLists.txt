idf_component_register(
    SRCS
        "camera_serivce/camera_service.c"
        "face_serivce/face_service.c"
        "fp_serivce/fp_service.c"
        "password_serivce/password_service.c"
        "log_serivce/log_service.c"

    INCLUDE_DIRS
        "camera_serivce"
        "face_serivce"
        "fp_serivce"
        "password_serivce"
        "log_serivce"

    REQUIRES
        bsp
        esp32-camera
        esp_jpeg
        nvs_flash)
idf_component_register(
    SRCS
        "view.c"
        "view_home.c"
        "view_password.c"
        "view_result.c"
        "view_admin.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        lvgl__lvgl bsp esp_timer driver)
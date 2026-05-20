idf_component_register(
    SRCS
        "camera_service/camera_service.c" 
        "display_service/display_service.c"    
    INCLUDE_DIRS 
        "camera_service"
        "display_service"
    REQUIRES
        esp32-camera bsp driver)

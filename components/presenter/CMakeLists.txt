idf_component_register(
    SRCS
        "presenter.c"
        "presenter_home.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        model
        view)
idf_component_register(SRCS "app_main.c"
                           "io_button.c"
                           "io_relay.c"
                           "sip_logic.c"
                           "web_server.c"
                     INCLUDE_DIRS ".")

# Add the cJSON library as a public dependency
set(requires "cjson")
idf_component_get_property(public_reqs ${COMPONENT_NAME} PUBLIC_REQUIRES)
list(APPEND public_reqs ${requires})
idf_component_set_property(${COMPONENT_NAME} PUBLIC_REQUIRES "${public_reqs}")


# Embed the web interface files into the firmware
set(web_files
    "web_root/index.html"
    "web_root/style.css"
    "web_root/script.js"
)

target_embed_file(${COMPONENT_LIB} "${web_files}")

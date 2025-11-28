
file(GLOB_RECURSE app_src       knxApplication/*.c Common/*.c)
file(GLOB_RECURSE exclude_src   Common/eeprom.c)
list(REMOVE_ITEM  app_src       ${exclude_src})

add_library(APP STATIC ${app_src})











set(screenrotator_SRCS displaymanager.cpp orientationsensor.cpp resources/resources.qrc)
add_executable(screenrotator main.cpp ${screenrotator_SRCS})
target_link_libraries(screenrotator Qt5::Gui Qt5::X11Extras Qt5::Sensors Xrandr)


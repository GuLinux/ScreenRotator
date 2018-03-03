qt5_add_resources(screenrotator_SRCS resources.qrc)
add_executable(screenrotator main.cpp ${screenrotator_SRCS})
target_link_libraries(screenrotator Qt5::Qml Qt5::Gui)


# to regenerate icons
if(BUILD_ICONS OR BUILD_CURSOR_ICONS)
    set(svg2png_SRCS svg2png.cpp)
    ecm_add_executable(svg2png ${svg2png_SRCS})
    target_link_libraries(svg2png ${QT_QTXML_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTSVG_LIBRARY})
endif()


cmake_minimum_required(VERSION 3.8)
project(Falcom C)


include_directories(common/include)
include_directories(/Users/asuka/local/vitasdk/arm-vita-eabi/include)
link_directories(common/lib)



set(FONT2PNG_SOURCE_FILES
        font2png/src/font2png.c)

add_executable(font2png ${FONT2PNG_SOURCE_FILES})
target_link_libraries(font2png png z m)


set (MAKEFONT_SOURCE_FILES
        makefont/src/easyfont.c
        makefont/src/easyfont.h
        makefont/src/makefont.c
        makefont/src/makefont.h makefont/src/codeconvert.h makefont/src/codeconvert.c)

add_executable(makefont ${MAKEFONT_SOURCE_FILES})
target_link_libraries(makefont freetype png z m iconv)



set(EDAOPATCH_SOURCE_FILES
        edao-patch/src/hook.c
        edao-patch/src/log.c
        edao-patch/src/main.c
        edao-patch/src/tools.c
        edao-patch/src/translator.c)


add_executable(edao-patch ${EDAOPATCH_SOURCE_FILES})
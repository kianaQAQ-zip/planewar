cmake_minimum_required(VERSION 3.20)
project(PlaneWar CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC：统一用 UTF-8 处理源码中的中文
if(MSVC)
    add_compile_options(/utf-8)
endif()

# ---------------------------------------------------------------------------
# SFML 配置（自动探测 + 明确报错）
# ---------------------------------------------------------------------------
set(_sfml_hints
    "C:/SFML-2.6.2"
    "D:/SFML-2.6.2"
    "$ENV{ProgramFiles}/SFML-2.6.2"
)

# 判断当前 SFML_DIR 是否真的可用（防止上一次失败配置在缓存里残留了无效 SFML_DIR）
set(_sfml_usable FALSE)
if(DEFINED SFML_DIR AND EXISTS "${SFML_DIR}/SFMLConfig.cmake")
    set(_sfml_usable TRUE)
endif()

# 仅在 SFML_DIR 不可用且未用环境变量指定时，才从常见位置自动探测
if(NOT _sfml_usable AND NOT DEFINED ENV{SFML_DIR})
    foreach(_h ${_sfml_hints})
        if(EXISTS "${_h}/lib/cmake/SFML/SFMLConfig.cmake")
            set(SFML_DIR "${_h}/lib/cmake/SFML" CACHE PATH "Auto-detected SFML cmake config" FORCE)
            set(_sfml_usable TRUE)
            message(STATUS "PlaneWar: 自动探测到 SFML -> ${SFML_DIR}")
            break()
        endif()
    endforeach()
endif()

# 仍不可用 -> 明确报错，列出搜索过的路径与排查步骤
if(NOT _sfml_usable AND NOT DEFINED ENV{SFML_DIR})
    message(FATAL_ERROR
        "未能自动找到 SFML。已搜索以下位置（需存在 lib/cmake/SFML/SFMLConfig.cmake）：\n"
        "    ${_sfml_hints}\n"
        "若你曾在 VS 的 CMake 设置里手动填过 SFML_DIR，请确认它指向 "
        "<路径>/lib/cmake/SFML，或先『删除缓存并重新配置』再试。")
endif()

# SFML 2.5+ 即可（2.6 也满足）
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
add_executable(PlaneWar ${SRC})

# 让 #include "core/Window.h" 这类写法能解析
target_include_directories(PlaneWar PRIVATE src)

# SFML 2.6.2 的 config 包导出的导入目标名为小写带连字符（sfml-graphics 等），
# 而非 SFML 3.x 的大写 SFML::Graphics 形式。注意：链接 sfml-graphics 会自动带入
# sfml-window 与 sfml-system，这里显式写出三者以保持清晰。
target_link_libraries(PlaneWar PRIVATE sfml-graphics sfml-window sfml-system)

# ---------------------------------------------------------------------------
# 自动拷贝 SFML 动态库（.dll）到 exe 同目录 —— 仅「预编译 SFML」路线需要
# （若走 vcpkg 路线，删掉下面这段，并在顶部加一行 set(VCPKG_APPLOCAL_DEPS ON) 即可）
# ---------------------------------------------------------------------------
if(WIN32 AND SFML_FOUND)
    # SFML_DIR 形如 <sfml>/lib/cmake/SFML，bin 目录在 <sfml>/bin
    get_filename_component(_sfml_root "${SFML_DIR}/../../.." ABSOLUTE)
    set(_sfml_bin "${_sfml_root}/bin")
    if(EXISTS "${_sfml_bin}")
        file(GLOB _sfml_dlls "${_sfml_bin}/sfml-*-2.dll")
        foreach(_dll ${_sfml_dlls})
            add_custom_command(TARGET PlaneWar POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_dll}" "$<TARGET_FILE_DIR:PlaneWar>"
                COMMENT "PlaneWar: 拷贝 ${_dll} 到输出目录")
        endforeach()
        message(STATUS "PlaneWar: 将在构建后自动拷贝 SFML DLL（来自 ${_sfml_bin}）")
    else()
        message(WARNING "PlaneWar: 未找到 SFML 的 bin 目录（${_sfml_bin}），"
                        "运行时若报缺 dll，请手动拷贝 sfml-*-2.dll 到 exe 旁。")
    endif()
endif()

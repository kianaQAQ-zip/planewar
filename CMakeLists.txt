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

# SFML 2.5+ 即可（2.6 也满足）；audio 组件用于 M3 音效
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp)
add_executable(PlaneWar ${SRC})

# 让 #include "core/Window.h" 这类写法能解析（前缀风格）
# 同时把 src/game 也加进去：src/game/waves、src/game/ui、src/game/events
# 等子目录里的文件用裸名 #include "GameConfig.h" / "ObjectPool.h"
# （这些头文件在 src/game/ 下，不在子目录自身里），需要 src/game 在包含路径上。
target_include_directories(PlaneWar PRIVATE src src/game)

# SFML 2.6.2 的 config 包导出的导入目标名为小写带连字符（sfml-graphics 等），
# 而非 SFML 3.x 的大写 SFML::Graphics 形式。注意：链接 sfml-graphics 会自动带入
# sfml-window 与 sfml-system，这里显式写出三者以保持清晰；sfml-audio 用于 M3 音效。
target_link_libraries(PlaneWar PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

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

    # OpenAL 是 sfml-audio 的运行期依赖，但文件名没有 sfml- 前缀，需单独拷贝。
    set(_openal "${_sfml_bin}/openal32.dll")
    if(EXISTS "${_openal}")
        add_custom_command(TARGET PlaneWar POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_openal}" "$<TARGET_FILE_DIR:PlaneWar>"
            COMMENT "PlaneWar: 拷贝 openal32.dll（SFML 音频依赖）")
    endif()
endif()

# ---------------------------------------------------------------------------
# 自动把 assets/ 目录同步到 exe 输出目录
# ---------------------------------------------------------------------------
# VS「打开文件夹」模式下，exe 输出在 out/build/<cfg>/，而资源在项目根目录 assets/。
# 运行时不一定能以项目根目录为工作目录，因此构建后把资源目录拷贝到 exe 旁边，
# 这样无论通过 VS 调试还是直接双击 exe，都能正确加载图片/字体/音效。
# ---------------------------------------------------------------------------
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    add_custom_command(TARGET PlaneWar POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:PlaneWar>/assets"
        COMMENT "PlaneWar: 拷贝 assets 到输出目录")
    message(STATUS "PlaneWar: 将在构建后自动拷贝 assets 到 exe 输出目录")
else()
    message(WARNING "PlaneWar: 未找到 assets/ 目录，运行时将使用占位渲染与静默音频")
endif()

# ---------------------------------------------------------------------------
# M4：单元测试（纯逻辑层，不依赖 SFML，可脱离游戏 exe 独立构建/运行）
# ---------------------------------------------------------------------------
option(BUILD_TESTS "Build the PlaneWar unit-test target" ON)
if(BUILD_TESTS)
    file(GLOB TEST_SRC CONFIGURE_DEPENDS tests/*.cpp)
    add_executable(PlaneWarTests ${TEST_SRC})
    # 被测单元（Math / ObjectPool / EventBus / Collision / GameConfig / WaveDef）
    # 全部是 SFML-free 头文件，因此测试目标无需链接 SFML，也无需编译游戏 .cpp。
    target_include_directories(PlaneWarTests PRIVATE src src/game)
    if(MSVC)
        target_compile_options(PlaneWarTests PRIVATE /utf-8)
    endif()
endif()

# ---------------------------------------------------------------------------
# M4：AddressSanitizer 开关（零泄漏 + 内存错误检测）
# 用法：cmake -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Release ..
#   - GCC/Clang：-fsanitize=address（含 LeakSanitizer，退出时报告泄漏）
#   - MSVC：/fsanitize=address（含堆越界/use-after-free；MSVC ASan 与 Debug 的
#     /ZI、/RTC、迭代器调试级别冲突，故建议用 Release / RelWithDebInfo 配置跑）
# ---------------------------------------------------------------------------
option(ENABLE_ASAN "Build PlaneWar with AddressSanitizer" OFF)
if(ENABLE_ASAN)
    if(MSVC)
        target_compile_options(PlaneWar PRIVATE /fsanitize=address /RTC-)
        target_link_options(PlaneWar PRIVATE /fsanitize=address /INCREMENTAL:NO)
        target_compile_definitions(PlaneWar PRIVATE _ITERATOR_DEBUG_LEVEL=0)
    else()
        target_compile_options(PlaneWar PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(PlaneWar PRIVATE -fsanitize=address)
    endif()
    message(STATUS "PlaneWar: AddressSanitizer ENABLED")
endif()

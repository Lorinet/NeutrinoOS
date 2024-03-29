﻿cmake_minimum_required (VERSION 3.8)
project(NeutrinoOS)

set(CMAKE_CXX_STANDARD 11)

set(CoreSources "main.cpp" "source/arrayobj.cpp" "source/bitconverter.cpp" "source/components.cpp" "source/disassembler.cpp" "source/dynamiclinker.cpp" "source/event.cpp" "source/filesystem.cpp" "source/input.cpp" "source/instruction.cpp" "source/kernlog.cpp" "source/lvmgr.cpp" "source/ntime.cpp" "source/nvm.cpp" "source/scheduler.cpp" "source/syscall.cpp" "source/timer.cpp" "source/util.cpp" "source/vmmem.cpp" "source/vmmgr.cpp" "source/vt.cpp" "source/config.cpp")
set(TiwazSources "components/TiwazUI/elements.cpp" "components/TiwazUI/gpio.cpp" "components/TiwazUI/graphics.cpp" "components/TiwazUI/inputmgr.cpp" "components/TiwazUI/uiserialization.cpp" "components/TiwazUI/view.cpp" "components/TiwazUI/viewmgr.cpp")
set(EffigySources "components/EffigyWM/element.cpp" "components/EffigyWM/graphics.cpp" "components/EffigyWM/serialization.cpp" "components/EffigyWM/window.cpp" "components/EffigyWM/winmgr.cpp" "components/EffigyWM/sdl/draw/SDL_gfxPrimitives.c" "components/EffigyWM/sdl/draw/SDL_framerate.c" "components/EffigyWM/sdl/draw/SDL_gfxBlitFunc.c" "components/EffigyWM/sdl/draw/SDL_imageFilter.c" "components/EffigyWM/sdl/draw/SDL_rotozoom.c")
set(NFSSources "components/NFS/nfsdimp.cpp")
set(IOManagerSources "components/IOManager/iomgr.cpp")

set(BuildTiwaz FALSE)
set(BuildEffigyWin FALSE)
set(BuildEffigyLin FALSE)

if(BuildTiwaz)
	list(APPEND CoreSources ${TiwazSources})
elseif(BuildEffigyWin OR BuildEffigyLin)
	list(APPEND CoreSources ${EffigySources})
endif()

list(APPEND CoreSources ${NFSSources})
list(APPEND CoreSources ${IOManagerSources})

add_executable (NeutrinoOS ${CoreSources} )

target_include_directories(NeutrinoOS PRIVATE "include")
target_include_directories(NeutrinoOS PRIVATE "components")

target_link_libraries(NeutrinoOS PRIVATE "-lpthread")

if(BuildTiwaz)
	target_link_libraries(NeutrinoOS PRIVATE "u8g2")
	target_link_libraries(NeutrinoOS PRIVATE "wiringPi")
elseif(BuildEffigyWin)
	target_include_directories(NeutrinoOS PRIVATE "components/EffigyWM/sdl/include")
	target_include_directories(NeutrinoOS PRIVATE "components/EffigyWM/sdl/draw")
	target_link_directories(NeutrinoOS PRIVATE "components/EffigyWM/sdl/lib/x64")
	target_link_libraries(NeutrinoOS PRIVATE "SDL")
	target_link_libraries(NeutrinoOS PRIVATE "SDLmain")
	target_link_libraries(NeutrinoOS PRIVATE "SDL_ttf")
	target_link_libraries(NeutrinoOS PRIVATE "SDL_image")
elseif(BuildEffigyLin)
	target_include_directories(NeutrinoOS PRIVATE "components/EffigyWM/sdl/draw")
	target_link_libraries(NeutrinoOS PRIVATE "SDL")
	target_link_libraries(NeutrinoOS PRIVATE "SDLmain")
	target_link_libraries(NeutrinoOS PRIVATE "SDL_ttf")
	target_link_libraries(NeutrinoOS PRIVATE "SDL_image")
	target_compile_options(NeutrinoOS PRIVATE -g)
endif()



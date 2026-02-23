cmake_minimum_required(VERSION 3.28)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Force Release for CommonLibF4" FORCE)
endif()

macro(set_from_environment VARIABLE)
  if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
    set(${VARIABLE} $ENV{${VARIABLE}})
  endif()
endmacro()

set(OutputPath "C:/MO2/Fallout4/mods")

set(CommonLibF4Path "${CMAKE_SOURCE_DIR}/extern/CommonLibF4_${COMMONLIBF4_VARIANT}")

project(${PROJECT_ID} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_subdirectory(${CommonLibF4Path} CommonLibF4)

find_package(spdlog REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)

# single translation unit that provides the F4SE exports
set(SOURCES
  ${CMAKE_SOURCE_DIR}/src/pch.cpp
  ${CMAKE_SOURCE_DIR}/src/main.cpp
)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${SOURCES}
)

add_library(
  ${PROJECT_ID}
  SHARED
  ${SOURCES}
)

target_compile_definitions(
  ${PROJECT_ID}
  PRIVATE
  _UNICODE
)

target_compile_definitions(
  CommonLibF4
  PUBLIC
  F4SE_SUPPORT_XBYAK
  _SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING
)

target_compile_features(
  ${PROJECT_ID}
  PRIVATE
  cxx_std_23
)

target_include_directories(
  ${PROJECT_ID}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/${PROJECT_ID}/include
)

target_precompile_headers(
  ${PROJECT_ID}
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.hpp"
)

target_link_libraries(
  ${PROJECT_ID}
  PRIVATE
  CommonLibF4::CommonLibF4
  # use commonLib provided fmt + spdlog or compiler complains idk
  # fmt::fmt-header-only
  # spdlog::spdlog_header_only
)

if(COMMONLIBF4_VARIANT STREQUAL "OG")
  target_compile_definitions(${PROJECT_ID} PRIVATE "GAME_VERSION=1")
elseif(COMMONLIBF4_VARIANT STREQUAL "AE")
  target_compile_definitions(${PROJECT_ID} PRIVATE "GAME_VERSION=3")
else()
  target_compile_definitions(${PROJECT_ID} PRIVATE "GAME_VERSION=0")
endif()

set_target_properties(${PROJECT_ID} PROPERTIES OUTPUT_NAME "${PROJECT_ID}")

target_compile_definitions(
  ${PROJECT_ID}
  PRIVATE
  WIN32_LEAN_AND_MEAN
  NOMB
  NOMINMAX
  NOSERVICE
)

target_include_directories(
  ${PROJECT_ID}
  SYSTEM
  PRIVATE
  $<TARGET_PROPERTY:CommonLibF4::CommonLibF4,INTERFACE_INCLUDE_DIRECTORIES>
)

find_package(simpleini CONFIG QUIET)
if(TARGET simpleini::simpleini)
  target_link_libraries(${PROJECT_ID} PRIVATE simpleini::simpleini)
else()
  if(DEFINED VCPKG_ROOT AND DEFINED VCPKG_TARGET_TRIPLET)
    find_path(SIMPLEINI_INCLUDE_DIR SimpleIni.h HINTS "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
    if(SIMPLEINI_INCLUDE_DIR)
      target_include_directories(${PROJECT_ID} PRIVATE SYSTEM "${SIMPLEINI_INCLUDE_DIR}")
    endif()
  endif()
endif()

if(EXISTS "${OutputPath}")
  set(POSTBUILD_OUTPUT_DIR "${OutputPath}/${PROJECT_ID} ${COMMONLIBF4_VARIANT}/F4SE/Plugins")
  add_custom_command(TARGET ${PROJECT_ID} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${POSTBUILD_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_ID}> "${POSTBUILD_OUTPUT_DIR}"
  )
else()
  message(WARNING "OutputPath '${OutputPath}' does not exist; skipping post-build copy")
endif()

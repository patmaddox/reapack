if(NOT DEFINED ENV{ARCH})
  message(FATAL_ERROR "The ARCH environment variable is not set.")
endif()

if($ENV{ARCH} STREQUAL "x86_64")
  set(VCPKG_TARGET_ARCHITECTURE x64)
elseif($ENV{ARCH} STREQUAL "i386")
  set(VCPKG_TARGET_ARCHITECTURE x86)
else()
  set(VCPKG_TARGET_ARCHITECTURE $ENV{ARCH})
endif()
set(VCPKG_BUILD_TYPE release)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME ${CMAKE_HOST_SYSTEM_NAME})
set(VCPKG_OSX_ARCHITECTURES $ENV{ARCH})
set(VCPKG_OSX_DEPLOYMENT_TARGET $ENV{DEPLOY_TARGET})

INCLUDE(CMakeForceCompiler)
# CROSS COMPILER SETTING
cmake_minimum_required (VERSION 2.8)
SET(CMAKE_SYSTEM_NAME Generic)

add_definitions(-DTARGET_LIKE_POSIX)
add_definitions(-DMBED_CONF_MBED_CLIENT_EVENT_LOOP_SIZE=8192)

if (${OS_BRAND} MATCHES Linux)
    add_definitions(-DMBED_CONF_NS_HAL_PAL_EVENT_LOOP_THREAD_STACK_SIZE=102400)
else()
    add_definitions(-DMBED_CONF_NS_HAL_PAL_EVENT_LOOP_THREAD_STACK_SIZE=8000)
endif()

add_definitions(-DMBED_CONF_NANOSTACK_EVENTLOOP_EXCLUDE_HIGHRES_TIMER)
add_definitions(-DMBED_CONF_NANOSTACK_EVENTLOOP_USE_PLATFORM_TICK_TIMER)

project(mbedCloudClient)

# mbed-cloud-client

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-cloud-client)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/include)

# mbed-client

SET(MBED_CLIENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client)

include_directories(${MBED_CLIENT_SOURCE_DIR})
include_directories(${MBED_CLIENT_SOURCE_DIR}/source)
include_directories(${MBED_CLIENT_SOURCE_DIR}/source/include)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-c)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-c/nsdl-c)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-c/source/include)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-classic)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-classic/mbed-client-classic)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-mbed-tls)
include_directories(${MBED_CLIENT_SOURCE_DIR}/mbed-client-mbed-tls/mbed-client-mbedtls)

# pal headers

SET(PAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-pal/Source)

include_directories(${PAL_SOURCE_DIR}/PAL-Impl/Services-API)
include_directories(${PAL_SOURCE_DIR}/Port/Platform-API)

# common components

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-coap)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-coap/mbed-coap)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-coap/source/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-trace)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-trace/mbed-trace)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-randlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-randlib/mbed-client-randlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-randlib/mbed-client-randlib/platform)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/mbed-client-libservice)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/mbed-client-libservice/platform)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sal-stack-nanostack-eventloop)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sal-stack-nanostack-eventloop/nanostack-event-loop)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal)

# factory-client

SET(FACTORY_CLIENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/factory-configurator-client)

include_directories(${FACTORY_CLIENT_SOURCE_DIR}/factory-configurator-client/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/factory-configurator-client/factory-configurator-client)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/fc_protocol_handler/fc_protocol_handler)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/fc_protocol_handler/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/secure_store)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/secure_store/secure_store)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/storage)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/storage/storage)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/key-config-manager)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/key-config-manager/key-config-manager)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/key-config-manager/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/utils)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/utils/utils)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/logger)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/logger/logger)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source-pal/api)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source-pal/linux)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/crypto-service/crypto-service)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/crypto-service/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/secsrv-cbor/secsrv-cbor)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/fcc-bundle-handler/fcc-bundle-handler)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/fcc-bundle-handler/source/include)
include_directories(${FACTORY_CLIENT_SOURCE_DIR}/fcc-output-info-handler/fcc-output-info-handler)

# update-client

SET(UPDATE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/update-client-hub)

include_directories(${UPDATE_SOURCE_DIR})
include_directories(${UPDATE_SOURCE_DIR}/update-client-hub)
include_directories(${UPDATE_SOURCE_DIR}/modules/atomic-queue)
include_directories(${UPDATE_SOURCE_DIR}/modules/atomic-queue/atomic-queue)
include_directories(${UPDATE_SOURCE_DIR}/modules/common)
include_directories(${UPDATE_SOURCE_DIR}/modules/common/update-client-common)
include_directories(${UPDATE_SOURCE_DIR}/modules/control-center)
include_directories(${UPDATE_SOURCE_DIR}/modules/control-center/update-client-control-center)
include_directories(${UPDATE_SOURCE_DIR}/modules/device-identity)
include_directories(${UPDATE_SOURCE_DIR}/modules/device-identity/pal4life-device-identity)
include_directories(${UPDATE_SOURCE_DIR}/modules/extensions)		
include_directories(${UPDATE_SOURCE_DIR}/modules/extensions/update-client-extensions)
include_directories(${UPDATE_SOURCE_DIR}/modules/firmware-manager)
include_directories(${UPDATE_SOURCE_DIR}/modules/firmware-manager/update-client-firmware-manager)
include_directories(${UPDATE_SOURCE_DIR}/modules/lwm2m-mbed)
include_directories(${UPDATE_SOURCE_DIR}/modules/lwm2m-mbed/update-client-lwm2m)
include_directories(${UPDATE_SOURCE_DIR}/modules/manifest-manager)
include_directories(${UPDATE_SOURCE_DIR}/modules/manifest-manager/update-client-manifest-manager)
include_directories(${UPDATE_SOURCE_DIR}/modules/manifest-manager/source)
include_directories(${UPDATE_SOURCE_DIR}/modules/monitor)
include_directories(${UPDATE_SOURCE_DIR}/modules/monitor/update-client-monitor)
include_directories(${UPDATE_SOURCE_DIR}/modules/paal-update-api)
include_directories(${UPDATE_SOURCE_DIR}/modules/paal-update-api/paal-update-api)
include_directories(${UPDATE_SOURCE_DIR}/modules/source)
include_directories(${UPDATE_SOURCE_DIR}/modules/source/update-client-source)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-http)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-http/update-client-source-http)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-http-socket)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-http-socket/update-client-source-http-socket)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-manager)
include_directories(${UPDATE_SOURCE_DIR}/modules/source-manager/update-client-source-manager)

FILE(GLOB MBED_CLOUD_CLIENT_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/mbed-coap/source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-randlib/source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sal-stack-nanostack-eventloop/source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/sal-stack-nanostack-eventloop/source/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sal-stack-nanostack-eventloop/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/source/libBits/common_functions.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/source/libList/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/source/nsdynmemLIB/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/nanostack-libservice/source/libip6string/ip6tos.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal/ns_event_loop.c"
if ((${OS_BRAND} MATCHES "FreeRTOS"))
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal/arm_hal_random.c"
endif()
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal/ns_hal_init.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal/arm_hal_interrupt.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ns-hal-pal/arm_hal_timer.cpp"

    "${MBED_CLIENT_SOURCE_DIR}/source/*.cpp"
    "${MBED_CLIENT_SOURCE_DIR}/mbed-client-c/source/*.c"
    "${MBED_CLIENT_SOURCE_DIR}/mbed-client-classic/source/*.cpp"
    "${MBED_CLIENT_SOURCE_DIR}/mbed-client-mbed-tls/source/*.cpp"

    "${FACTORY_CLIENT_SOURCE_DIR}/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/storage/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/secure_store/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/key-config-manager/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/utils/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/logger/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source-pal/linux/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/mbed-client-esfs/source-pal/linux/*.cpp"
    "${FACTORY_CLIENT_SOURCE_DIR}/crypto-service/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/fcc-bundle-handler/source/*.c"
    "${FACTORY_CLIENT_SOURCE_DIR}/fcc-output-info-handler/source/*.c"
    )

if ((${OS_BRAND} MATCHES "Linux"))
    FILE(GLOB UPDATE_SRC
    "${UPDATE_SOURCE_DIR}/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/atomic-queue/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/common/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/control-center/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/device-identity/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/extensions/source/*.c"		
    "${UPDATE_SOURCE_DIR}/modules/extensions/source/*.cpp"
    "${UPDATE_SOURCE_DIR}/modules/firmware-manager/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/lwm2m-mbed/source/*.cpp"
    "${UPDATE_SOURCE_DIR}/modules/manifest-manager/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/source-http/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/source-http-socket/source/*.c"
    "${UPDATE_SOURCE_DIR}/modules/source-manager/source/*.c"
    )
endif()

list(APPEND MBED_CLOUD_CLIENT_SRC ${UPDATE_SRC})
message(status "mbed cloud client sources = \n ${MBED_CLOUD_CLIENT_SRC}")

CREATE_LIBRARY(mbedCloudClient "${MBED_CLOUD_CLIENT_SRC}" "")
add_dependencies(mbedCloudClient mbedtls)

ADDSUBDIRS()

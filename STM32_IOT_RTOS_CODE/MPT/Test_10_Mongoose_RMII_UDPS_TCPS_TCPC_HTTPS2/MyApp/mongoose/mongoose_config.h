// See https://mongoose.ws/documentation/#build-options
#pragma once

#define __RMII 1
//#define __W5500 2

#ifdef __RMII

// <o MG_ARCH> Build environment
//   <MG_ARCH_ARMCC=> Keil MDK
//   <MG_ARCH_FREERTOS=> FreeRTOS
//   <MG_ARCH_CMSIS_RTOS1=> CMSIS-RTOS v1
//   <MG_ARCH_CMSIS_RTOS2=> CMSIS-RTOS v2
//   <MG_ARCH_ARMGCC=> ARM GCC + Newlib
#define MG_ARCH MG_ARCH_FREERTOS

// <q> Enable builtin TCP/IP stack
#define MG_ENABLE_TCPIP 1

// <h> Network Drivers for builtin TCP/IP stack
// <q> Enable STM23Fxx Ethernet driver
#define MG_ENABLE_DRIVER_STM32F 1

// <q> Enable STM32Hxx Ethernet driver
#define MG_ENABLE_DRIVER_STM32H 0

// <q> Enable IMXRT Ethernet driver
#define MG_ENABLE_DRIVER_IMXRT 0

// <q> Enable W5500 Ethernet driver
#define MG_ENABLE_DRIVER_W5500 0

// <q> Enable TI TM4C Ethernet driver
#define MG_ENABLE_DRIVER_TM4C 0

// </h>

// <o MG_TLS> TLS support
//   <MG_TLS_NONE=> None
//   <MG_TLS_BUILTIN=> Built-in TLS 1.3 stack
//   <MG_TLS_MBED=> MbedTLS
#define MG_TLS MG_TLS_BUILTIN

// <q> Enable custom mg_millis() 
#define MG_ENABLE_CUSTOM_MILLIS 0

// <q> Enable custom mg_random() 
#define MG_ENABLE_CUSTOM_RANDOM 1

// <q> Enable packed (embedded) filesystem
#define MG_ENABLE_PACKED_FS 1

// <<< end of configuration section >>>

// Add your customization below this comment
//#define MG_DRIVER_MDC_CR 4   // RMII MDC clock divider, from 0 to 5
#define MG_TCPIP_PHY_ADDR 1  // PHY address
#define MG_TCPIP_IP MG_IPV4(192, 168, 10, 10)     // IP
#define MG_TCPIP_GW MG_IPV4(192, 168, 10, 1)      // Gateway
#define MG_TCPIP_MASK MG_IPV4(255, 255, 255, 0)  // Netmask

//#define MG_ENABLE_POLL 1
#define MG_IO_SIZE 2048
#define MG_ENABLE_TCPIP_DRIVER_INIT 1

//#define MG_IRAM __attribute__((section(".RamFunc")))
//#define MG_ENABLE_CUSTOM_CALLOC	0

#endif

#ifdef __W5500

#include "main.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define MG_ARCH MG_ARCH_FREERTOS
#define MG_ENABLE_TCPIP 1
#define MG_ENABLE_DRIVER_W5500 1
#define MG_ENABLE_TCPIP_PRINT_DEBUG_STATS 0
#define MG_ENABLE_PACKED_FS 1
//#define MG_ENABLE_CUSTOM_MILLIS 1  // We must implement mg_millis()
#define MG_IO_SIZE 256

// For static IP configuration, define MG_TCPIP_{IP,MASK,GW}
// By default, those are set to zero, meaning that DHCP is used
//
//#define MG_TCPIP_IP MG_IPV4(192, 168, 1, 10)
//#define MG_TCPIP_GW MG_IPV4(192, 168, 1, 1)
//#define MG_TCPIP_MASK MG_IPV4(255, 255, 255, 0)

// Set custom MAC address. By default, it is randomly generated
// Using a build-time constant:
#define MG_SET_MAC_ADDRESS(mac) do { uint8_t buf_[6] = {2,0,1,2,3,5};         \
                                memmove(mac, buf_, sizeof(buf_)); } while (0)
//
// Using custom function:
// extern void genmac(unsigned char *mac);
// #define MG_SET_MAC_ADDRESS(mac) genmac(mac)

// Hardware specific SPI support
                                
#define MG_TCPIP_IP MG_IPV4(192, 168, 10, 10)
#define MG_TCPIP_GW MG_IPV4(192, 168, 10, 1)
#define MG_TCPIP_MASK MG_IPV4(255, 255, 255, 0)
                                
//#define MG_ENABLE_TCPIP_DRIVER_INIT 1
                                
#include <stdint.h>
extern void spi_begin(void *);
extern void spi_end(void *);
extern uint8_t spi_txn(void *, uint8_t);
#define MG_TCPIP_DRIVER_INIT(mgr)                                              \
  do {                                                                         \
    static struct mg_tcpip_spi spi_ = {NULL, spi_begin, spi_end, spi_txn};     \
    static struct mg_tcpip_if mif_;                                            \
    mif_.ip = MG_TCPIP_IP;                                                     \
    mif_.mask = MG_TCPIP_MASK;                                                 \
    mif_.gw = MG_TCPIP_GW;                                                     \
    mif_.driver = &mg_tcpip_driver_w5500;                                      \
    mif_.driver_data = &spi_;                                                  \
    MG_SET_MAC_ADDRESS(mif_.mac);                                              \
    mg_tcpip_init(mgr, &mif_);                                                 \
    MG_INFO(("Driver: W5500, MAC: %M", mg_print_mac, mif_.mac));               \
  } while (0)


#endif
 


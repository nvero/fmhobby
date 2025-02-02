/* Copyright (c) 2007 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 4762 $
 */

/** @file
 * @brief Implementation of #hal_nrf_rw for nRF24LU1+
 *
 * #hal_nrf_rw is an SPI function which is hardware dependent. This file
 * contains an implementation for nRF24LU1.
 */

#include <Nordic\reg24lu1.h>
#include <stdint.h>
#include "hal_nrf.h"

uint8_t hal_nrf_rw(uint8_t value)
{
  RFDAT = value;
  RFSPIF = 0;     // ! IMPORTANT ! Clear RF SPI ready flag
                  // after data written to RFDAT..
  while(!RFSPIF); // wait for byte transfer finished
    ;
  return RFDAT;   // return SPI read value
}



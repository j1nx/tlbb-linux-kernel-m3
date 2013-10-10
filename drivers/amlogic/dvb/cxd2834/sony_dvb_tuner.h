/*------------------------------------------------------------------------------

<dev:header>
    Copyright(c) 2011 Sony Corporation.

    $Revision: 3515 $
    $Author: mrushton $

</dev:header>

------------------------------------------------------------------------------*/
/**

 @file    sony_dvb_tuner.h

          This file provides the tuner driver API definition.

*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_DVB_TUNER_H
#define SONY_DVB_TUNER_H

#include "sony_dvb.h"
#include "sony_i2c.h"

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief Definition of the tuner driver API.
*/
typedef struct sony_dvb_tuner_t {

    /**
     @brief Initialize the tuner.

     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*Initialize) (struct sony_dvb_tuner_t * pTuner);

    /**
     @brief Finalize the tuner.

     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*Finalize) (struct sony_dvb_tuner_t * pTuner);

    /**
     @brief Tune to a given frequency with a given bandwidth.
            Tuner driver can block while waiting for PLL locked indication (if required).

     @param pTuner Instance of the tuner driver.
     @param frequency RF frequency to tune too (kHz)
     @param system The type of channel to tune too (DVB-T/C/T2).
     @param bandWidth The bandwidth of the channel in MHz.
            - DVB-C: 8MHz only.
            - DVB-T: 6, 7 or 8.
            - DVB-T2: 5, 6, 7 or 8.

     @return SONY_RESULT_OK if tuner successfully tuned.
    */
    sony_result_t (*Tune) (struct sony_dvb_tuner_t * pTuner,
                               uint32_t frequency, sony_dvb_system_t system, uint8_t bandWidth);

    /**
     @brief Sleep the tuner device (if supported).
     
     @param pTuner Instance of the tuner driver.

     @return SONY_RESULT_OK if successful.
    */
    sony_result_t (*Sleep) (struct sony_dvb_tuner_t * pTuner);

    uint32_t frequency;         /**< Current RF frequency(kHz) tuned. */
    sony_dvb_system_t system;   /**< Current system tuned. */
    uint8_t bandWidth;          /**< Cuurent bandwidth(MHz) tuned. */
    uint32_t xtalFreq;          /**< Tuner Xtal frequency (MHz). */
    sony_i2c_t *pI2c;           /**< I2C driver instance. */
    uint8_t i2cAddress;         /**< I2C address. */
    uint32_t flags;             /**< Flags that can be used by tuner drivers. */
    void *user;                 /**< User defined data. */
} sony_dvb_tuner_t;

#endif /* SONY_DVB_TUNER_H */

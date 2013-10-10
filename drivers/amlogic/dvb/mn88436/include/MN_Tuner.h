/* **************************************************** */
/*!
   @file	MN_Tuner.h
   @brief	Tuner Interface for MN_DMD_Driver
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */

#ifndef MN_TUNER_H
#define MN_TUNER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "MaxLinearDataTypes.h"
/* **************************************************** */
/*  Tuner dependence functions */
/* **************************************************** */
/* these function is defined by MN_DMD_Tuner_(tuner_name).c */
extern DMD_u32_t DMD_Tuner_init(void);
extern DMD_u32_t DMD_Tuner_set_system(DMD_PARAMETER* param);
extern DMD_u32_t DMD_Tuner_tune(DMD_PARAMETER* param);
extern DMD_u32_t DMD_Tuner_term(void);

#ifdef __cplusplus
}
#endif

#endif

/* **************************************************** */
/*!
   @file	MN_DMD_driver.c
   @brief	Panasonic Demodulator Driver
   @author	R.Mori
   @date	2011/6/30
   @param
		(c)	Panasonic
   */
/* **************************************************** */

#include "MN_DMD_driver.h"
#include "MN_DMD_device.h"
#include "MN_DMD_common.h"
#include "MN_I2C.h"
#include "MN_Tuner.h"

/* **************************************************** */
/* Local Functions */
/* **************************************************** */

/* **************************************************** */
/* Definition of Constant Strings */
/* **************************************************** */

/* **************************************************** */
/* Public Functions  */
/* **************************************************** */
/*! Demodulator set device ID */
DMD_u32_t DMD_API DMD_set_devid( DMD_PARAMETER* param , DMD_u32_t id )
{
	param->devid = id;

	return DMD_E_OK;
}


/*!	Demodulator API Initialize & Open */
/*!
	*initialize parameter variable (DMD_PARAMETER* param)
	*open & connect to USB Device for I2C communication
	 (for WINDOWS API only)
*/
DMD_u32_t DMD_API DMD_open( DMD_PARAMETER* param)
{
	int i;

	param->devid = 0;
	param->system= DMD_E_NOT_DEFINED;
	/* Initialize Parameter Variable */
	for(i=0;i<DMD_INFORMATION_MAX;i++){
		param->info[i] = 0;		//NOT_DEFINED
	}
	param->bw		= DMD_E_BW_NOT_SPECIFIED;
	param->freq		= 666000;
	param->funit	= DMD_E_KHZ;
	
	//troy.wang, 20120801, for customer to pass test in lab.( SPEC : A74-2010 )
	param->echo_set = DMD_E_ENHANCE_MINUS_ECHO_DELAY_NO_SIDEEFFECT;
	
	if( DMD_device_open( param ) == DMD_E_ERROR )
	{
		DMD_DBG_TRACE( "ERROR: DMD open" );
		return DMD_E_ERROR;

	}

	if( DMD_I2C_open() == DMD_E_ERROR )
	{
		DMD_DBG_TRACE( "ERROR: I2C open" );
		return DMD_E_ERROR;
	}
	return DMD_E_OK;
}

/*! Demodulator LSI Initialize */
/*!
	*release power down
	*Tuner Initialize
	*transfer common register settings via I2C
	*transfer auto countrol sequence via I2C
*/
DMD_u32_t DMD_API	DMD_init( DMD_PARAMETER* param )
{
    /*IF use DMD to repeat I2C cmd, firstly initializing DMD is better. Troy.wangyx, 120801*/
	/* Demodulator LSI Initialize */
	if( DMD_device_init( param ) == DMD_E_ERROR )
	{
		DMD_DBG_TRACE( "ERROR: DMD Initialize" );
		return DMD_E_ERROR;
	}
	/* Auto Control Sequence Transfer */
	if( DMD_device_load_pseq( param ) == DMD_E_ERROR )
	{
		DMD_DBG_TRACE( "ERROR: DMD Load Autocontrol" );
		return DMD_E_ERROR;
	}

	/* Tuner_init */
/*	if( DMD_Tuner_init() == DMD_E_ERROR )
	{
		DMD_DBG_TRACE( "ERROR: Tuner Initialize" );
		return DMD_E_ERROR;
	}*/
	
	return DMD_E_OK;
}

/*! Select Broadcast Sytem */
/*!
	*Transfer register settings for each system
*/
DMD_u32_t DMD_API DMD_set_system( DMD_PARAMETER* param ){
	if( DMD_Tuner_set_system( param ) == DMD_E_ERROR ){
		DMD_DBG_TRACE("ERROR:Tuner set system");
		return DMD_E_ERROR;
	}

/*	if( DMD_device_set_system( param ) == DMD_E_ERROR ){
		DMD_DBG_TRACE("ERROR:device set system");
		return DMD_E_ERROR;
	}*/

       /* '11/08/29 : OKAMOTO	Select TS output. */
	if( DMD_set_ts_output(param, param->ts_out ) == DMD_E_ERROR ){
		DMD_DBG_TRACE("ERROR:set ts output");
		return DMD_E_ERROR;
	}
	
	/* 20120801, troy, set TCB control mode : Through mode */
	if (DMD_device_set_TCB_mode( param ) == DMD_E_ERROR )
	{
              DMD_DBG_TRACE( "ERROR: DMD set tcb I2C mode" );
		return DMD_E_ERROR;
	}

	/* 20120801, troy, set echo performance */
	if (DMD_device_set_echo_enhance( param, param->echo_set) == DMD_E_ERROR )
	{
              DMD_DBG_TRACE( "ERROR: DMD set echo performace" );
		return DMD_E_ERROR;
	}
	
	return DMD_E_OK;
}

/*! Tune */
DMD_u32_t DMD_API DMD_tune( DMD_PARAMETER* param )
{
	DMD_ERROR_t ret;
	/* Call pre tune process */
	ret = DMD_device_pre_tune( param );
	/* Tuner */
	ret |= DMD_Tuner_tune( param );
	/* Call post tune process */
	ret |= DMD_device_post_tune( param );
	return ret;
}

/*! scan */
DMD_u32_t DMD_API DMD_scan( DMD_PARAMETER* param )
{
	DMD_ERROR_t	ret;
	/* Call pre tune process */
	DMD_device_pre_tune( param );   //default empty 
	
	/* Tuner: RF and BW input*/
	DMD_Tuner_tune( param ); 
	/* DMD software reset */
	DMD_device_post_tune( param );
	
	/* Call Lock/SYNC Status Judgement */
//	ret = DMD_device_scan( param );
	return ret;
}

/*! term */
DMD_u32_t DMD_API DMD_term( DMD_PARAMETER* param )
{
	DMD_ERROR_t	ret;
	ret = DMD_E_OK;

	ret = DMD_device_term(param);
	ret |= DMD_Tuner_term();

	return ret;
}

/*! close */
DMD_u32_t DMD_API DMD_close( DMD_PARAMETER* param )
{
	DMD_ERROR_t	ret;
	ret = DMD_E_OK;

	ret = DMD_device_close(param);

	return ret;
}

/* **************************************************** */
/* Set Information */
/* **************************************************** */
DMD_u32_t DMD_API DMD_set_info( DMD_PARAMETER* param , DMD_u32_t id  , DMD_u32_t val){

	return DMD_device_set_info( param , id , val );
}

/* **************************************************** */
/* Get Information */
/* **************************************************** */
DMD_u32_t DMD_API DMD_get_info( DMD_PARAMETER* param , DMD_u32_t id ){

	return DMD_device_get_info( param , id );
}

DMD_text_t DMD_API DMD_info_title( DMD_SYSTEM_t sys ,DMD_u32_t id )
{

	if( id < DMD_E_INFO_COMMON_END_OF_INFORMATION )
	{
		return DMD_INFO_TITLE_COMMON[id];

	}
	else
		switch( sys )
		{
		case DMD_E_DVBT:
			if( id < DMD_E_INFO_DVBT_END_OF_INFORMATION ){
				id -= DMD_E_INFO_COMMON_END_OF_INFORMATION;
				return DMD_INFO_TITLE_DVBT[id];
			}
			else
			{
				return "Not Defined";
			}
			break;
		case DMD_E_DVBT2:
			if( id < DMD_E_INFO_DVBT2_END_OF_INFORMATION ){
				id -= DMD_E_INFO_COMMON_END_OF_INFORMATION;
				return DMD_INFO_TITLE_DVBT2[id];
			}
			else
			{
				return "Not Defined";
			}
			break;
		case DMD_E_DVBC:
			if( id < DMD_E_INFO_DVBC_END_OF_INFORMATION ){
				id -= DMD_E_INFO_COMMON_END_OF_INFORMATION;
				return DMD_INFO_TITLE_DVBC[id];
			}
			else
			{
				return "Not Defined";
			}
			break;
#ifndef DMD_DISABLE_ISDB
		case DMD_E_ISDBT:
		case DMD_E_ISDBT_BRAZIL:
			if( id < DMD_E_INFO_ISDBT_END_OF_INFORMATION ){
				id -= DMD_E_INFO_COMMON_END_OF_INFORMATION;
				return DMD_INFO_TITLE_ISDBT[id];
			}
			else
			{
				return "Not Defined";
			}
			break;
		case DMD_E_ISDBS:
			if( id < DMD_E_INFO_ISDBS_END_OF_INFORMATION ){
				id -= DMD_E_INFO_COMMON_END_OF_INFORMATION;
				return DMD_INFO_TITLE_ISDBS[id];
			}
			else
			{
				return "Not Defined";
			}
			break;
#endif
		default:
			break;
		}
	return "Not Defined";
}

DMD_text_t DMD_API DMD_info_value( DMD_SYSTEM_t sys ,DMD_u32_t id , DMD_u32_t val )
{


	if( id < DMD_E_INFO_COMMON_END_OF_INFORMATION && id != DMD_E_INFO_STATUS )
	{
		return DMD_info_value_common( id ,val );

	}
	else 
	{
		switch( sys )
		{
		case DMD_E_DVBT2:
			return DMD_info_value_dvbt2( id , val );
			break;
		case DMD_E_DVBT:
			return DMD_info_value_dvbt( id , val );
			break;
#ifndef DMD_DISABLE_ISDB
		case DMD_E_ISDBT:
		case DMD_E_ISDBT_BRAZIL:
			return DMD_info_value_isdbt( id , val );
			break;
		case DMD_E_ISDBS:
			return DMD_info_value_isdbs( id , val );
#endif
		case DMD_E_ATSC:
		case DMD_E_QAMB_256QAM:
		case DMD_E_QAMB_64QAM:
		case DMD_E_QAMC_256QAM:
		case DMD_E_QAMC_64QAM:
			return DMD_info_value_vq( id , val );
		case DMD_E_DVBC:
			return DMD_info_value_dvbc( id , val );
			break;
		default:
			break;
		}
	}

	return "Not Defined";
}
DMD_text_t DMD_API DMD_value_text( DMD_PARAMETER* param , DMD_u32_t id )
{
	return DMD_info_value( param->system , id , param->info[id] );
}

/* **************************************************** */
/* Floating Functions */
/* **************************************************** */
// Get BER
DMD_float_t DMD_API DMD_get_ber( DMD_PARAMETER* param )
{
	DMD_float_t ret;
#ifdef DMD_FLOATING_FUNCTION
	DMD_get_info( param , DMD_E_INFO_BERRNUM );

	if( param->info[DMD_E_INFO_BITNUM] == 0 )
	{
		ret = 1.0;
	}
	else
	{

		ret = (DMD_float_t) param->info[DMD_E_INFO_BERRNUM]
			/ (DMD_float_t) param->info[DMD_E_INFO_BITNUM];
	}
#else
	ret = 1;
#endif
	return ret;

}
DMD_float_t DMD_API DMD_get_per( DMD_PARAMETER* param )
{
	DMD_float_t ret;
#ifdef DMD_FLOATING_FUNCTION
	DMD_get_info( param , DMD_E_INFO_PACKETNUM );

	if( param->info[DMD_E_INFO_PACKETNUM] == 0 )
	{
		ret = 1.0;
	}
	else
	{

		ret = (DMD_float_t) param->info[DMD_E_INFO_PERRNUM]
			/ (DMD_float_t) param->info[DMD_E_INFO_PACKETNUM];
	}
#else
	ret = 1;
#endif
	return ret;

}
//Get CNR
DMD_float_t DMD_API DMD_get_cnr( DMD_PARAMETER* param )
{
	DMD_float_t	ret;
	DMD_get_info( param , DMD_E_INFO_CNR_INT );
#ifdef DMD_FLOATING_FUNCTION

	ret = (DMD_float_t) param->info[DMD_E_INFO_CNR_INT] 
	     +  (DMD_float_t) param->info[DMD_E_INFO_CNR_DEC] / ( (DMD_float_t) 100.0 );
#else
	ret = (DMD_float_t) param->info[DMD_E_INFO_CNR_INT];
#endif


	return ret;
}



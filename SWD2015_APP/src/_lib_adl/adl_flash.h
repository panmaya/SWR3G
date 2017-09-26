/*
 * adl_flash.h
 *
 * Created: 24/9/2557 17:43:55
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_flash.h   -  Copyright Sierra Wireless Inc. (c) 2010														  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/


/**
  @file adl_flash.h
  @brief ADL FLASH Service public interface header file
  @ingroup adl_flash_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_flash_public_IFS ADL FLASH Service Interface Specification
  @endif
  
  @section adl_flash_public_IFS_OVERVIEW Overview
  This document describes the public FLASH interface of the ADL software element.
  
  @version 1.0
  
  @author FDN
  
  @section adl_flash_public_IFS_HISTORY History
  <table>
  <tr>
      <td><b>Version</b></td>
      <td><b>Summary</b></td>
      <td><b>Author</b></td>
      <td><b>Date</b></td>
  </tr>
  <tr>
      <td>1.0</td>
      <td>Initial revision</td>
      <td>FDN</td>
      <td>15/05/2008</td>
  </tr>
  </table>
  
  @section adl_flash_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_flash_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_flash_public_IFS_RELATED_DOCUMENTS Related documents references
  
  Here are the documents referenced in this interface specification.
  @anchor ref1 [1] <> X </A> \n
  
  @subsection adl_flash_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
  - <b>N.A.</b>\n
    Not appliable
  - <b>N.U.</b>\n
    Not used
  - <b>IFS</b>\n
    Interface Specification
  - <b>SE</b>\n
    Software Element
  */

/**
  @ifnot GLOBAL_IFS
  @defgroup adl_flash_public_IFS ADL FLASH Service Interface Specification
  @else
  @addtogroup adl_flash_public_IFS
  @{
  @endif
  
  @section adl_flash_public_IFS_ftr Features
  
  ADL provides this service to handle FLASH objects management.
  <br>An ADL application may subscribe to a set of objects identified by an handle, used by all ADL flash functions.
  This handle is chosen and given by the application at subscription time.
  To access to a particular object, the application gives the handle and the ID of the object to access.
  At first subscription, the Handle and the associated set of IDs are saved in flash. The number of flash object IDs associated to a given handle may be only changed after have erased the flash objects (with the AT+WOPEN=3 command).
  For a particular handle, the flash objects ID take any value, from 0 to the ID range upper limit provided on subscription.
  Important note:
  Due to the internal storage implementation, only up to 2000 object identifiers can exist at the same time.

  <br>The defined operations are:
  - #adl_flhSubscribe
  - #adl_flhSubscribe
  - #adl_flhExist
  - #adl_flhRead
  - #adl_flhWrite
  - #adl_flhErase
  - #adl_flhGetFreeMem
  - #adl_flhGetIDCount
  - #adl_flhGetUsedSize
  
  @if GLOBAL_IFS
  @}
  @endif
  */

#ifndef __ADL_FLASH_H__
#define __ADL_FLASH_H__


/**
  @defgroup adl_flash_public_IFS_Definitions Types and constants definition
  @ingroup adl_flash_public_IFS
  
  */

/**
  * @addtogroup adl_flash_public_IFS_Definitions
  * @{
  */

/**
 * @brief Constant to represent all handle IDs
 */
#define ADL_FLH_ALL_IDS 0xFFFF 

/**
 * @brief Error code for non existing object
 */
#define ADL_FLH_RET_ERR_OBJ_NOT_EXIST   ADL_RET_ERR_SPECIFIC_BASE
/**
 * @brief Error code for no more flash memory
 */
#define ADL_FLH_RET_ERR_MEM_FULL        ADL_RET_ERR_SPECIFIC_BASE - 1
/**
 * @brief Error code for no more IDs
 */
#define ADL_FLH_RET_ERR_NO_ENOUGH_IDS   ADL_RET_ERR_SPECIFIC_BASE - 2
/**
 * @brief Error code for ID out of handle range
 */
#define ADL_FLH_RET_ERR_ID_OUT_OF_RANGE ADL_RET_ERR_SPECIFIC_BASE - 3





s8 adl_flash_init(uint8_t lun_id);
s32 adl_flash_getErr(void);


/**
  * @}
  */

/**
  @defgroup adl_flash_public_IFS_op1 FLASH API access functions
  @ingroup adl_flash_public_IFS
  */

/**
  * @addtogroup adl_flash_public_IFS_op1
  * @{
  */

/** 
 * @brief Flash service subscription function
 * @par Description: This function subscribes to a set of objects identified by the given Handle.
 * @param [in]  Handle 			The Handle of the set of objects to subscribe to.
 * @param [in]  NbObjectRes 	The number of objects related to the given handle. It means that the IDs available for this handle are in the range [ 0 , (NbObjectRes – 1) ].
 * @retval 		OK 								Operation successful
 * @retval 		ADL_RET_ERR_PARAM 				on parameter error
 * @retval 		ADL_RET_ERR_ALREADY_SUBSCRIBED 	if space is already created for this handle
 * @retval 		ADL_FLH_RET_ERR_NO_ENOUGH_IDS 	if there are no more enough object IDs to allocate the handle.
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 		if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhSubscribe ( const ascii Handle[6], u32 NbObjectsRes );
//s8 adl_flhMakeDir ( const ascii * Handle);

/** 
 * @brief Test flash object existence function
 * @par Description: This function checks if a flash object exists from the given Handle at the given ID in the flash memory allocated to the ADL developer.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to investigate (in the range allocated to the provided Handle).
 * @retval 		objectlength						on success the requested Flash object length 
 * @retval 		0K 									if the object does not exist.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s32 adl_flhExist( const ascii Handle[6], u32 ID );
//u32 adl_flhExistTime(const ascii Handle[6], u32 ID );

/** 
 * @brief Read flash object function
 * @par Description: This function reads the flash object from the given Handle at the given ID, for the length provided and stores it in a buffer.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to read.
 * @param [in]  Len 			The length of the flash object to read.
 * @param [in]  ReadData		The string allocated to store the read flash object.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_PARAM 					if one at least of the parameters has a bad value.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_FLH_RET_ERR_OBJ_NOT_EXIST 		if the object does not exist.
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_READ error event will then occur).
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhRead( const ascii Handle[6], u32 ID, u16 Len, u8 * ReadData );

/** 
 * @brief Write flash object function
 * @par Description: This function writes the flash object from the given Handle at the given ID, for the length provided with the buffer provided. A single flash object can use up to 30 Kbytes of memory.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to read.
 * @param [in]  Len 			The length of the flash object to write.
 * @param [in]  WriteData		The provided string to write in the flash object.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_PARAM 					if one at least of the parameters has a bad value.
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_WRITE error event will then occur).
 * @retval 		ADL_FLH_RET_ERR_MEM_FULL 			if flash memory is full.
 * @retval 		ADL_FLH_RET_ERR_NO_ENOUGH_IDS 		if the object can not be created due to the global ID number limitation.
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhWrite( const ascii Handle[6], u32 ID, u16 Len, u8 * WriteData );

/** 
 * @brief Erase flash object function
 * @par Description: This function erases the flash object from the given Handle at the given ID.
 * @param [in]  Handle 			The Handle of the subscribe set of objects.
 * @param [in]  ID 				The ID of the flash object to be erased.
 * @note		If ID is set to ADL_FLH_ALL_IDS, all flash objects related to the provided handle will be erased.
 * @retval 		OK 									on success
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_FLH_RET_ERR_OBJ_NOT_EXIST 		if the object does not exist
 * @retval 		ADL_RET_ERR_FATAL 					if a fatal error occurred (ADL_ERR_FLH_DELETE error event will then be generated)
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
s8 adl_flhErase( const ascii Handle[6], u32 ID );

s8 adl_flhClear( const ascii Handle[6] );

s8 adl_flhReFormat(void);

/** 
 * @brief Get remaining flash memory function
 * @par Description: This function gets the current remaining flash memory size.
 * @retval 		freemem			Current free flash memory size in bytes.
 */
//u32 adl_flhGetFreeMem( void );

/** 
 * @brief Get ID count function
 * @par Description: This function returns the ID count for the provided handle.
 * @param [in]  Handle 			The Handle of the subscribed set of objects.
 * @retval 		IDcount						on success ID count allocated on the provided handle
 * @retval 		ADL_RET_ERR_PARAM           if the handle is NULL
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 	if handle is not subscribed
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 	if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
//s32 adl_flhGetIDCount( ascii * Handle );

/** 
 * @brief Get used size function
 * @par Description: This function returns the used size by the provided ID range from the provided handle. The handle should also be set to NULL to get the whole used size.
 * @param [in]  Handle 			The Handle of the subscribed set of objects. If set to NULL, the whole flash memory used size will be returned
 * @param [in]  StartID 		First ID of the range from which to get the used size ; has to be lower than EndID.
 * @param [in]  EndID 			Last ID of the range from which to get the used size; has to be greater than StartID. To get the used size by all an handle IDs, the [ 0 , ADL_FLH_ALL_IDS ] range may be used
 * @retval 		Usedsize 							on success the used size from the provided Handle (if any) the whole flash memory used size (otherwise)
 * @retval 		ADL_RET_ERR_PARAM 					on parameter error
 * @retval 		ADL_RET_ERR_UNKNOWN_HDL 			if handle is not subscribed
 * @retval 		ADL_FLH_RET_ERR_ID_OUT_OF_RANGE 	if ID is out of handle range
 * @retval 		ADL_RET_ERR_SERVICE_LOCKED 			if the function was called from a low level Interrupt handler (the function is forbidden in this context).
 */
//s32 adl_flhGetUsedSize( ascii * Handle, u16 StartID, u16 EndID );

/**
  * @}
  */





#endif /* ADL_FLASH_H_ */
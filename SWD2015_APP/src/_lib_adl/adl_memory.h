/*
 * adl_memory.h
 *
 * Created: 4/10/2557 10:36:14
 *  Author: pan
 */ 


/**********************************************************************************************************************/
/*  adl_memory.h   -  Copyright Sierra Wireless Inc. (c) 2010														  */
/*                                                                                                                    */
/*                                                                                                                    */
/* LICENSE                                                                                                            */
/* =======                                                                                                            */
/* If no specific agreement has been signed, this Source Code is subject to the following license terms:              */
/* http://www.sierrawireless.com/productsandservices/AirPrime/Sierra_Wireless_Software_Suite/License/Userlicense.aspx */
/* If you modify this file, you are subject to the contribution rules described into this license                     */
/**********************************************************************************************************************/

/**
  @file adl_memory.h
  @brief ADL Memory Service public interface header file
  @ingroup adl_memory_public_IFS
  */

/**
  @ifnot GLOBAL_IFS
  @mainpage
  @else
  @defgroup adl_memory_public_IFS ADL Memory Service Interface Specification
  @endif
  
  @section adl_memory_public_IFS_OVERVIEW Overview
  This document describes the public Memory Service interface of the ADL software element.
  
  @version 02.00

  @author SUB Infosys AT Maintenance

  @section adl_memory_public_IFS_HISTORY History
  <table>
  <tr>
      <td><b>Rev<br>XX.YY</b></td>
      <td><b>FH Rev<br>X</b></td>
	  <td><b>Date<br>YYYY/MM/DD</b></td>
      <td><b>Updates</b></td>
      <td><b>Author</b></td>
  </tr>
  <tr>
      <td>01.00</td>
      <td>N/A</td>	
      <td>xxxx/xx/xx</td>	  
      <td>Creation</td>
      <td>XXXX</td>
  </tr>
  <tr>
      <td>02.00</td>
      <td>N/A</td>	
      <td>2013/03/13</td>	  
      <td>Major Update:<br>
          Updated name and revision table as per records management guidelines.
	  </td>
      <td>SUB Infosys AT Maintenance</td>
  </tr>
  </table>
  
  @section adl_memory_public_IFS_INTRODUCTION Introduction
  
  @subsection adl_memory_public_IFS_REMINDER Reminder
  
  Interfaces are the means by which software elements interact. An interface is a list of operations providing a coherent service.
  Software elements should have as much (independent) interfaces than of managed services.
  Uses the interface specification
  - To software element developers: provides the contract that states what the provider has to implement to meet the services promised by the interface.
  - To software element users: provides the contract that states what the client needs to do to use the interface.
  
  @subsection adl_memory_public_IFS_ABBR_AND_GLOSSARY Abbreviations and glossary
  
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
  @defgroup adl_memory_public_IFS ADL Memory Service Interface Specification
  @else
  @addtogroup adl_memory_public_IFS
  @{
  @endif
  
  @section adl_memory_public_IFS_ftr Features
  
  ADL provides this service to allow applications to handle dynamic memory buffers, and get information about the platform's RAM mapping.
  
  The defined operations are:
  - <b>get & release</b> functions (adl_memGet() & adl_memRelease()) usable to manage dynamic memory buffers
  - An <b>information</b> function (adl_memGetInfo()) usable to retrieve information about the platform's RAM mapping
  
  @section adl_memory_public_IFS_Tool Heap Memory Block Status
A list of the currently reserved heap memory blocks can be displayed at any time using the Target Monitoring Tool "Get RTK Status" command. Please refer to the Tools Manual for more information.
  
  @section adl_memory_public_IFS_sample Code Sample
  
  The code sample below illustrates a nominal use case of the ADL Memory Service public interface (error cases are not handled)
  
  @code
// Somewhere in the application code, used as an event handler
void MyFunction ( void )
{
    // Local variables
    adl_memInfo_t MemInfo;
    u8 * MyByteBuffer

    // Gets Open AT RAM information
    adl_memGetInfo ( &MemInfo );

    // Allocates a 10 bytes memory buffer
    MyByteBuffer = ( u8 * ) adl_memGet ( 10 );

    // Releases the previously allocated memory buffer
    adl_memRelease ( MyByteBuffer );
}
  @endcode
  
  @if GLOBAL_IFS
  @}
  @endif
  */



#ifndef __adl_memory_H__
#define __adl_memory_H__



/**
  @defgroup adl_memory_public_IFS_Definitions Types and constants definition
  @ingroup adl_memory_public_IFS
  
  Memory service related types and constants definition.
  */

/**
  * @addtogroup adl_memory_public_IFS_Definitions
  * @{
  */

/**
  @brief Memory information structure
  
  @par Description:
  This structure contains several fields containing information about the platform's RAM mapping.
  
  @note The RAM dedicated to the Open AT<sup>®</sup> application is divided in three areas (Call stack, Heap memory & Global variables). The adl_memGetInfo() function returns these area current sizes.
  
  @image html adl_memMapping.png Open AT<sup>®</sup> RAM Mapping
 */
typedef struct _adl_memInfo_t
{
    /**
      @brief Total RAM size
      
      @par Description:
      Total RAM size for the Open AT<sup>®</sup> application (in bytes).
      Please refer to the <b>Memory Resources</b> chapter for more information.
      */
    u32 TotalSize;
    
    /**
      @brief Tasks stacks size
      
      @par Description:
      Open AT<sup>®</sup> application call stacks area size (in bytes).
      This size is defined by the Open AT<sup>®</sup> application in the #adl_InitTasks task table, and thanks to the #adl_InitIRQLowLevelStackSize and #adl_InitIRQHighLevelStackSize constants (Please refer to the Mandatory API chapter for more information).
      
      */
    u32 StackSize;
    
    /**
      @brief Heap memory size
      
      @par Description:
      Open AT<sup>®</sup> application total heap memory area size (in bytes).
      This size is the difference between the total Open AT<sup>®</sup> memory size and the Global & Stack areas sizes.
      
      */
    u32 HeapSize;
    
    /**
      @brief Global variables memory size
      
      @par Description:
      Open AT<sup>®</sup> application global variables area size (in bytes).
      This size is defined at the binary link step; it includes the ADL library, plug-in libraries (if any) and Open AT<sup>®</sup> application global variables.
      
      */
    u32 GlobalSize;
} adl_memInfo_t;


/**
  @brief  Uninitialized global variable
  
  @par Description:
  This macro is used to define a global variable in the uninitialized part of RAM, this part is not cleared after a hard or soft reset, only when power supply is off.
  So when an application restarts, global variable defined with this macro keep the last saved value before the last reset.
  
  @param[in]        _X         This parameters corresponds to global variable to define. the type and the name of the variable have to be defined. refer to example to get more information on using
   
  @note be care on the syntax : 
    - at the end of variable declaration, there is no semi-colon
    - global variable can not be initialized with a value when it declared
    
    
  @code
  // Global variable definition
  ADL_MEM_UNINIT( u32 MyGlobal )

  void adl_main ( adl_InitType_e InitType )
  {  
      ...
      MyGlobal = 500;
      ...
  }
  @endcode
  
 */
#ifdef __REMOTETASKS__

#define ADL_MEM_UNINIT(_X)  _X;

#else  // __REMOTETASKS__

#ifdef __CC_ARM

#if __ARMCC_VERSION < 200000

// ADS version
#define ADL_MEM_UNINIT(_X)      _Pragma ( "arm section zidata=\"UNINIT\"" ) \
                                _X; \
                                _Pragma ( "arm section zidata" )

#else   // __ARMCC_VERSION < 200000

// RVDS version
#define ADL_MEM_UNINIT(_X)      _X __attribute__((section("UNINIT"),zero_init));

#endif  // __ARMCC_VERSION < 200000

#else   // __CC_ARM

// GCC version
#define ADL_MEM_UNINIT(_X)      _X __attribute__((section("UNINIT")));

#endif  // __CC_ARM

#endif // __REMOTETASKS__


/**
  * @}
  */



/**
  @defgroup adl_memory_public_IFS_op1 Memory mapping information
  @ingroup adl_memory_public_IFS
  */

/**
  * @addtogroup adl_memory_public_IFS_op1
  * @{
  */

/**
  @brief Memory mapping information function
  
  @par Description:
  This function returns information about the Open AT<sup>®</sup> RAM areas sizes.
  
  @param[out]       Info    Structure updated by the function, using the #adl_memInfo_t type.
  
  @retval       OK                  On success
  @retval       ADL_RET_ERR_PARAM   On parameter error
 */
//s32 adl_memGetInfo ( adl_memInfo_t * Info );

/**
  * @}
  */

/**
  @defgroup adl_memory_public_IFS_op2 Dynamic memory management
  @ingroup adl_memory_public_IFS
  */

/**
  * @addtogroup adl_memory_public_IFS_op2
  * @{
  */

/**
  @brief Dynamic memory allocation function
  
  @par Description:
  This function allocates the memory for the requested size into the client application RAM memory.
  
  @param[in]    Size    The memory buffer requested size (in bytes).
  
  @return       A pointer to the allocated memory buffer on success.
  
  @exception    #ADL_ERR_MEM_GET    If the allocation fails since the platform runs out of memory, this function will lead to a #ADL_ERR_MEM_GET error, which can be handled by the Error Service. If this error is filtered and refused by the error handler, the function will return NULL. Please refer to the Error service for more information.
  
  @note         Memory allocation may also fails due to an unrecoverable corrupted memory state; one of the following exceptions is then generated (these exceptions cannot be filtered by the Error service, and systematically lead to a reset of the Wireless CPU<sup>®</sup>)

  @exception    "RTK Exception 166"     A buffer header or footer data is corrupted: a write overflow has occurred on this block.
 */
void * _adl_memGet ( u32 size, const ascii * , uint32_t  );
#define adl_memGet(_x) _adl_memGet (_x, __FILE__, __LINE__ );

/**
  @brief Dynamic memory release macro
  
  @par Description:
  This macro releases the allocated memory buffer designed by the supplied pointer.
  
  @param[in,out]    _p_ A pointer on the allocated memory buffer.
  
  @retval   TRUE    If the memory was correctly released. In this case, the provided pointer is set to NULL.
  
  @note         If the memory release fails, one of the following exceptions is generated (these exception cannot be filtered by the Error service, and systematically lead to a reset of the Wireless CPU<sup>®</sup>)

  @exception    "RTK exception 155"         The supplied address is out of the heap memory address range.
  @exception    "RTK exception 161 or 166"  The supplied buffer header or footer data is corrupted: a write overflow has occurred on this block
  @exception    "RTK exception 159 or 172"  The heap memory release process has failed due to a global memory corruption in the heap area.
 */
#define adl_memRelease(_p_) _adl_memRelease( ( void ** ) &_p_ )


/**
  @brief Dynamic memory release function
  
  @par Description:
  Internal memory release function, which should not be called directly. The #adl_memRelease macro has to be used in order to release memory buffer.
  
  @param[in,out]    Ptr A pointer on the allocated memory buffer address.
  
  @return  Please refer to the #adl_memRelease macro definition
 */
void _adl_memRelease ( void ** Ptr );



/**
  @brief gives heap memory size information function
  
  @par Description:
  gives free OpenAt heap memory size information 
  
 * @param o_pu32_Heapsize     Initial total heap size (in bytes) 
 * @param o_pu32_HeapFree     total amount of free memory heap left (in bytes) 
 * @param o_pu32_HeapContig   bigest free chunk heap memory (in bytes)
  
  @retval   OK      On success
  @retval   ERROR   On failure
 */
bool adl_memStat ( u32 *o_pu32_HeapSize, u32 *o_pu32_HeapFree, u32 *o_pu32_HeapContig);




/**
  * @}
  */



#endif

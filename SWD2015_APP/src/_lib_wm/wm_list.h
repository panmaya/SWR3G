/*
 * wm_list.h
 *
 * Created: 23/9/2557 13:46:42
 *  Author: pan
 */ 


#ifndef WM_LIST_H_
#define WM_LIST_H_

//#include "adl_global.h"
/*************/
/* Constants */
/*************/

/* List styles */
#define WM_LIST_NONE            0x0000  // No specific style
#define WM_LIST_SORTED          0x0001  // Sorted list
#define WM_LIST_NODUPLICATES    0x0002  // Only "new" elements are added


/*********/
/* Types */
/*********/

/* List callback table type */
typedef struct
{
	s16     ( * CompareItem ) ( void *, void * );
	void    ( * FreeItem ) ( void * );
} wm_lstTable_t;

/* List type */
typedef void * wm_lst_t;

/**************/
/* Prototypes */
/**************/

/***************************************************************************/
/*  Function   : wm_lstCreate                                              */
/*-------------------------------------------------------------------------*/
/*  Object     : Create a list.                                            */
/*                                                                         */
/*  Return     : A pointer to the list created.                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  Attr              | x |   |   | The type of the list from below        */
/*                    |   |   |   |  WM_LIST_NONE                          */
/*                    |   |   |   |  WM_LIST_SORTED                        */
/*                    |   |   |   |  WM_LIST_NODUPLICATES                  */
/*--------------------+---+---+---+----------------------------------------*/
/*  funcTable         | x |   |   | The table holding the compare and the  */
/*                    |   |   |   | free functions to be associated to     */
/*                    |   |   |   | the list created.                      */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
wm_lst_t _wm_lstCreate ( u16 Attr, wm_lstTable_t * funcTable, const char *, uint32_t );
#define wm_lstCreate(a,b)	_wm_lstCreate(a,b,__FILE__,__LINE__)



/***************************************************************************/
/*  Function   : wm_lstDestroy                                             */
/*-------------------------------------------------------------------------*/
/*  Object     : Clear the provided list before destroying it              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list to be destroyed.              */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
void wm_lstDestroy ( wm_lst_t list );



/***************************************************************************/
/*  Function   : wm_lstClear                                               */
/*-------------------------------------------------------------------------*/
/*  Object     : Delete all the list provided items                        */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list to clear                      */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//void wm_lstClear ( wm_lst_t list );



/***************************************************************************/
/*  Function   : wm_lstGetCount                                            */
/*-------------------------------------------------------------------------*/
/*  Object     : Count the items number in the provided list.              */
/*                                                                         */
/*  Return     : The items count.                                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list to count                      */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
u16 wm_lstGetCount ( wm_lst_t list );



/***************************************************************************/
/*  Function   : wm_lstAddItem                                             */
/*-------------------------------------------------------------------------*/
/*  Object     : Add the provided item to the provided list.               */
/*                                                                         */
/*  Note       : The user must have supplied a compare function at the     */
/*               list creation time.                                       */
/*                                                                         */
/*  Return     : The position in the list where the item has been added.   */
/*               (0 is the first position). Or ERROR if an error occurred. */
/*                                                                         */
/*  Note       : If a duplicate is found and the type of the list is       */
/*               NODUPLICATES then the index of the duplicate is returned. */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
s16 _wm_lstAddItem ( wm_lst_t list, void * item, const char *, uint32_t );
#define wm_lstAddItem(a,b)	_wm_lstAddItem(a,b,__FILE__,__LINE__)


/***************************************************************************/
/*  Function   : wm_lstInsertItem                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : Insert the provided item in the provided list at the      */
/*                providedindex.                                           */
/*                                                                         */
/*  Return     : The position in the list where the item is inserted.      */
/*               (0 is the first position). Or ERROR if an error occurred. */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  index             | x |   |   | The index (0 is the first position)    */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//s16 wm_lstInsertItem ( wm_lst_t list, void * item, u16 index );



/***************************************************************************/
/*  Function   : wm_lstGetItem                                             */
/*-------------------------------------------------------------------------*/
/*  Object     : Get the item of the provided list at the provided index.  */
/*                                                                         */
/*  Return     : A pointer to the found item, NULL otherwise.              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  index             | x |   |   | The index (0 is the first position)    */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
void * wm_lstGetItem ( wm_lst_t List, s16 index );



/***************************************************************************/
/*  Function   : wm_lstDeleteItem                                          */
/*-------------------------------------------------------------------------*/
/*  Object     : Delete the item in the provided list at the provided index*/
/*                                                                         */
/*  Return     : The number of remaining item in the list. Or ERROR if an  */
/*               error occurred.                                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  index             | x |   |   | The index (0 is the first position)    */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
s16 wm_lstDeleteItem ( wm_lst_t list, s16 index );



/***************************************************************************/
/*  Function   : wm_lstFindItem                                            */
/*-------------------------------------------------------------------------*/
/*  Object     : Finds the provided item in the provided list.             */
/*                                                                         */
/*  Note       : The user must have supplied a compare function at the     */
/*               list creation time.                                       */
/*                                                                         */
/*  Return     : The position in the list where the item has been found.   */
/*               (0 is the first position). Or ERROR if an error occurred. */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//s16 wm_lstFindItem ( wm_lst_t List, void * item );



/***************************************************************************/
/*  Function   : wm_lstFindAllItem                                         */
/*-------------------------------------------------------------------------*/
/*  Object     : Finds all the item matching the item provided in the list */
/*               provided.                                                 */
/*                                                                         */
/*  Note       : The user must have supplied a compare function at the     */
/*               list creation time.                                       */
/*                                                                         */
/*  Return     : A s16 buffer holding the indexes where the item matching  */
/*               the item provided are (0 is the first position), ERROR    */
/*               will be the last element of the buffer.                   */
/*               This buffer should be released by the application when    */
/*               its processing is done                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//s16 * wm_lstFindAllItem ( wm_lst_t List, void *item );



/***************************************************************************/
/*  Function   : wm_lstFindNextItem                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : Finds the next provided item in the provided list, that   */
/*               was not found by a previous wm_lstFindNextItem call       */
/*                                                                         */
/*  Note       : The user must have supplied a compare function at the     */
/*               list creation time.                                       */
/*                                                                         */
/*  Return     : The position in the list where the item has been found.   */
/*               (0 is the first position). Or ERROR if an error occurred. */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//s16 wm_lstFindNextItem ( wm_lst_t List, void * item );



/***************************************************************************/
/*  Function   : wm_lstFindPrevItem                                        */
/*-------------------------------------------------------------------------*/
/*  Object     : Finds the next provided item in the provided list, that   */
/*               was not found by a previous wm_lstFindPrevItem call       */
/*                                                                         */
/*  Note       : The user must have supplied a compare function at the     */
/*               list creation time.                                       */
/*                                                                         */
/*  Return     : The position in the list where the item has been found.   */
/*               (0 is the first position). Or ERROR if an error occurred. */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//u16 wm_lstFindPrevItem ( wm_lst_t List, void * item );



/***************************************************************************/
/*  Function   : wm_lstResetItem                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : Reset all the items previously found by the               */
/*               wm_lstFindNextItem function for the provided item         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/*  Variable Name     |IN |OUT|GLB|  Utilisation                           */
/*--------------------+---+---+---+----------------------------------------*/
/*  list              | x |   |   | The list                               */
/*--------------------+---+---+---+----------------------------------------*/
/*  item              | x |   |   | The item                               */
/*--------------------+---+---+---+----------------------------------------*/
/***************************************************************************/
//void wm_lstResetItem ( wm_lst_t List, void * item );




#endif //WM_LIST_H_
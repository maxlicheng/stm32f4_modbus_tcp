/*
* FreeModbus Libary: BARE Port
* Copyright (C) 2006 Christian Walter <wolti@sil.at>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
* File: $Id: port.h,v 1.1 2006/08/22 21:35:13 wolti Exp $
*/

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>
#include "sys.h"
#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "os_cfg.h"

/* default */

#define MB_TCP_DEBUG     0

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

//#define  OS_ENTER_CRITICAL()  
//#define  OS_EXIT_CRITICAL()   

#define ENTER_CRITICAL_SECTION(  )   OS_ENTER_CRITICAL()
#define EXIT_CRITICAL_SECTION(  )    OS_EXIT_CRITICAL()

typedef uint8_t BOOL;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef uint16_t USHORT;
typedef int16_t SHORT;
typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#ifdef MB_TCP_DEBUG
typedef enum
{
  MB_LOG_DEBUG,
  MB_LOG_INFO,
  MB_LOG_WARN,
  MB_LOG_ERROR
} eMBPortLogLevel;
#endif

/* ----------------------- Function prototypes ------------------------------*/
#ifdef MB_TCP_DEBUG
void vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule, const CHAR * szFmt, ... );
void prvvMBTCPLogFrame( UCHAR * pucMsg, UCHAR * pucFrame, USHORT usFrameLen );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif

#endif


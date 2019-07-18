/*
* FreeModbus Libary: lwIP Port
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
* File: $Id: porttcp.c,v 1.2 2006/09/04 14:39:20 wolti Exp $
*/

/* ----------------------- System includes ----------------------------------*/
#include <stdio.h>
#include <string.h>

#include "port.h"

/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/api.h"
#include "lwip/tcp.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- MBAP Header --------------------------------------*/
#define MB_TCP_UID          6
#define MB_TCP_LEN          4
#define MB_TCP_FUNC         7

/* ----------------------- Defines  -----------------------------------------*/
#define MB_TCP_DEFAULT_PORT 502 /* TCP listening port. */
#define MB_TCP_BUF_SIZE     ( 256 + 7 ) /* Must hold a complete Modbus TCP frame. */

/* ----------------------- Prototypes ---------------------------------------*/
void vMBPortEventClose( void );
void vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule, const CHAR * szFmt, ... );

/* ----------------------- Static variables ---------------------------------*/
#define MB_TCP_CLIENTS  3
static struct tcp_pcb *pxPCBListen;
static struct tcp_pcb *pxPCBClientToSend;
static struct tcp_pcb *pxPCBClient[MB_TCP_CLIENTS];

static UCHAR    aucTCPBuf[MB_TCP_CLIENTS][MB_TCP_BUF_SIZE];
static USHORT   usTCPBufPos[MB_TCP_CLIENTS];
static USHORT   usTCPBufReceived[MB_TCP_CLIENTS];   

/* ----------------------- Static functions ---------------------------------*/
static err_t    prvxMBTCPPortAccept( void *pvArg, struct tcp_pcb *pxPCB, err_t xErr );
static err_t    prvxMBTCPPortReceive( void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p, err_t xErr );
static void     prvvMBTCPPortError( void *pvArg, err_t xErr );

/* ----------------------- Begin implementation -----------------------------*/
BOOL
xMBTCPPortInit( USHORT usTCPPort )
{
  struct tcp_pcb *pxPCBListenNew, *pxPCBListenOld;
  BOOL            bOkay = FALSE;
  USHORT          usPort;
  
  if( usTCPPort == 0 )
  {
    usPort = MB_TCP_DEFAULT_PORT;
  }
  else
  {
    usPort = ( USHORT ) usTCPPort;
  }
  
  if( ( pxPCBListenNew = pxPCBListenOld = tcp_new(  ) ) == NULL )
  {      
    /* Can't create TCP socket. */
    bOkay = FALSE;
  }
  else if( tcp_bind( pxPCBListenNew, IP_ADDR_ANY, ( u16_t ) usPort ) != ERR_OK )
  {      
    /* Bind failed - Maybe illegal port value or in use. */
    ( void )tcp_close( pxPCBListenOld );
    bOkay = FALSE;
  }
  else if( ( pxPCBListenNew = tcp_listen( pxPCBListenNew ) ) == NULL )
  {      
    ( void )tcp_close( pxPCBListenOld );
    bOkay = FALSE;
  }
  else
  {     
    /* Register callback function for new clients. */
    tcp_accept( pxPCBListenNew, prvxMBTCPPortAccept );
    /* Everything okay. Set global variable. */
    pxPCBListen = pxPCBListenNew;     
    
#if MB_TCP_DEBUG
    vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Protocol stack ready.\r\n" );
#endif
  }
  
  bOkay = TRUE;
  return bOkay;
}

void
prvvMBPortReleaseClient( struct tcp_pcb *pxPCB )
{
  if( pxPCB != NULL )
  {
    if( tcp_close( pxPCB ) != ERR_OK )
    {
      tcp_abort( pxPCB );
    }
    
    for(int i = 0; i < MB_TCP_CLIENTS; i++)
    {
      if( pxPCB == pxPCBClient[i])
      {
#if MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-CLOSE", "Closed connection to %d.%d.%d.%d.\r\n",
                   ip4_addr1( &( pxPCB->remote_ip ) ),
                   ip4_addr2( &( pxPCB->remote_ip ) ),
                   ip4_addr3( &( pxPCB->remote_ip ) ), ip4_addr4( &( pxPCB->remote_ip ) ) );
#endif
        pxPCBClient[i] = NULL;
      }      
    }    
    
    if( pxPCB == pxPCBListen )
    {
      pxPCBListen = NULL;
    }
    
  }
}

void
vMBTCPPortClose(  )
{
  /* Shutdown any open client sockets. */
  for(int i = 0; i < MB_TCP_CLIENTS; i++)
  {
    prvvMBPortReleaseClient( pxPCBClient[i] );
  }  
  
  /* Shutdown or listening socket. */
  prvvMBPortReleaseClient( pxPCBListen );
  
  /* Release resources for the event queue. */
  vMBPortEventClose();
}

void
vMBTCPPortDisable( void )
{
  for(int i = 0; i < MB_TCP_CLIENTS; i++)
  {
    prvvMBPortReleaseClient( pxPCBClient[i] );
  }
}

err_t
prvxMBTCPPortAccept( void *pvArg, struct tcp_pcb *pxPCB, err_t xErr )
{
  err_t error;
  
  if( xErr != ERR_OK )
  {
    return xErr;
  }
  
  /* Find first pxPCBClient whose value is null */
  for(int i = 0; i < MB_TCP_CLIENTS; i++)
  {
    if( pxPCBClient[i] == NULL )
    {
      /* Register the client. */
      pxPCBClient[i] = pxPCB;
      
      /* Set up the receive function prvxMBTCPPortReceive( ) to be called when data
      * arrives.
      */
      tcp_recv( pxPCB, prvxMBTCPPortReceive );
      
      /* Register error handler. */
      tcp_err( pxPCB, prvvMBTCPPortError );
      
      /* Set callback argument later used in the error handler. */
      tcp_arg( pxPCB, pxPCB );
      
      /* Reset the buffers and state variables. */
      usTCPBufPos[i] = 0;
      usTCPBufReceived[i] = 0;
      
#if MB_TCP_DEBUG
      vMBPortLog( MB_LOG_DEBUG, "MBTCP-ACCEPT", "Accepted new client %d.%d.%d.%d\r\n",
                 ip4_addr1( &( pxPCB->remote_ip ) ),
                 ip4_addr2( &( pxPCB->remote_ip ) ),
                 ip4_addr3( &( pxPCB->remote_ip ) ), ip4_addr4( &( pxPCB->remote_ip ) ) );
#endif
      
      error = ERR_OK;
      return error;
    }    
  }
  
  return ERR_CONN;
}

/* Called in case of an unrecoverable error. In any case we drop the client
* connection. */
void
prvvMBTCPPortError( void *pvArg, err_t xErr )
{
  struct tcp_pcb *pxPCB = pvArg;
  
  if( pxPCB != NULL )
  {
#if MB_TCP_DEBUG
    vMBPortLog( MB_LOG_DEBUG, "MBTCP-ERROR", "Error with client connection! Droping it.\r\n" );
#endif
    prvvMBPortReleaseClient( pxPCB );
  }
}

err_t
prvxMBTCPPortReceive( void *pvArg, struct tcp_pcb *pxPCB, struct pbuf *p, err_t xErr )
{
  USHORT usLength;
  USHORT usIndex;
    
  err_t error = xErr;
  
  if( error != ERR_OK )
  {
    return error;
  }
  
  /* If pbuf is NULL then remote end has closed connection. */
  if( p == NULL )
  {
    prvvMBPortReleaseClient( pxPCB );
    return ERR_OK;
  } 
  
  /* Acknowledge that we have received the data bytes. */
  tcp_recved( pxPCB, p->len );
  
  /* Find an index of PCB */
  for(int i=0; i < MB_TCP_CLIENTS; i++)
  {
    if(pxPCBClient[i] == pxPCB)
    {
      usIndex = i;
      break;
    }
  }  
  
  /* Check for internal buffer overflow. In case of an error drop the client.*/
  if( ( usTCPBufPos[usIndex] + p->len ) >= MB_TCP_BUF_SIZE )
  {
    prvvMBPortReleaseClient( pxPCB );
    error = ERR_OK;
  }
  else
  {
    memcpy( &aucTCPBuf[usIndex][usTCPBufPos[usIndex]], p->payload, p->len );
    usTCPBufPos[usIndex] += p->len;
    
    /* If we have received the MBAP header we can analyze it and calculate
    * the number of bytes left to complete the current request. If complete
    * notify the protocol stack.
    */
    if( usTCPBufPos[usIndex] >= MB_TCP_FUNC )
    {
      /* Length is a byte count of Modbus PDU (function code + data) and the
      * unit identifier. */
      usLength = aucTCPBuf[usIndex][MB_TCP_LEN] << 8U;
      usLength |= aucTCPBuf[usIndex][MB_TCP_LEN + 1];
      
      /* Is the frame already complete. */
      if( usTCPBufPos[usIndex] < ( MB_TCP_UID + usLength ) )
      {
      }
      else if( usTCPBufPos[usIndex] == ( MB_TCP_UID + usLength ) )
      {
#if MB_TCP_DEBUG
        prvvMBTCPLogFrame( "MBTCP-RECV", &aucTCPBuf[usIndex][0], usTCPBufPos[usIndex] );
#endif
        /* Set message received flag */
        usTCPBufReceived[usIndex] = 1;
        /* Set pointer for response */
        pxPCBClientToSend = pxPCBClient[usIndex];
        ( void )xMBPortEventPost( EV_FRAME_RECEIVED );
      }
      else
      {
#if MB_TCP_DEBUG
        vMBPortLog( MB_LOG_DEBUG, "MBTCP-ERROR",
                   "Received to many bytes! Droping client.\r\n" );
#endif
        /* This should not happen. We can't deal with such a client and
        * drop the connection for security reasons.
        */
        
        //prvvMBPortReleaseClient( pxPCB );
        
        /* 패킷이 붙어서 오는 경우 발생 확인, 이런 경우 그냥 Packet만 Drop 하도록 수정 */
        usTCPBufPos[usIndex] = 0;        
      }
    }
  }
  pbuf_free( p );
  return error;
}

BOOL
xMBTCPPortGetRequest( UCHAR ** ppucMBTCPFrame, USHORT * usTCPLength )
{  
  for(int i = 0; i < MB_TCP_CLIENTS; i++)
  {
    if(usTCPBufReceived[i] == 1)
    {
      *ppucMBTCPFrame = &aucTCPBuf[i][0];
      *usTCPLength = usTCPBufPos[i];
      
      /* Reset the buffer. */
      usTCPBufPos[i] = 0;
      usTCPBufReceived[i] = 0;
      
      return TRUE;
    }
  }
  
  return FALSE;
}

BOOL
xMBTCPPortSendResponse( const UCHAR * pucMBTCPFrame, USHORT usTCPLength )
{
  BOOL bFrameSent = FALSE;
  
  if( pxPCBClientToSend )
  {
    /* Make sure we can send the packet. */
    assert( tcp_sndbuf( pxPCBClientToSend ) >= usTCPLength );
    
    if( tcp_write( pxPCBClientToSend, pucMBTCPFrame, ( u16_t ) usTCPLength, TCP_WRITE_FLAG_COPY ) == ERR_OK )
    {
#if MB_TCP_DEBUG
      for(int i = 0; i < MB_TCP_CLIENTS; i++)
      {
        if(pxPCBClient[i] == pxPCBClientToSend)
        {
          prvvMBTCPLogFrame( "MBTCP-SENT", &aucTCPBuf[i][0], usTCPLength );          
          break;
        }
      }      
#endif
      /* Make sure data gets sent immediately. */
      ( void )tcp_output( pxPCBClientToSend );
      bFrameSent = TRUE;
    }
    else
    {
      /* Drop the connection in case of an write error. */
      prvvMBPortReleaseClient( pxPCBClientToSend );
    }
  }
  
  pxPCBClientToSend = NULL;  
  return bFrameSent;
}


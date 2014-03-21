/*
 * ANT.cpp
 * 
 * Copyright 2014  <corbamico@163.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


/* Some code slice copy from <dis_framer_ant.hpp/cpp> ANT_LIB MacOS src
 * From http://thisisant.com
 * 
 * Dynastream Innovations Inc.
 * Cochrane, AB, CANADA
 *
 * Copyright � 1998-2008 Dynastream Innovations Inc.
 * All rights reserved. This software may not be reproduced by
 * any means without express written approval of Dynastream
 * Innovations Inc.
 */

#include "ANT.h"



#define TX_FIFO_SIZE                          256

#define ANT_DATA_CHANNEL_NUM_OFFSET           0
#define ANT_DATA_EVENT_ID_OFFSET              1
#define ANT_DATA_EVENT_CODE_OFFSET            2



UCHAR CheckSum_Calc8(const volatile void *pvDataPtr_, USHORT usSize_)
{
   const UCHAR *pucDataPtr = (UCHAR *)pvDataPtr_;
   UCHAR ucCheckSum = 0;
   int i;
   
   // Calculate the CheckSum value (XOR of all bytes in the buffer).
   for (i = 0; i < usSize_; i++)
      ucCheckSum ^= pucDataPtr[i];
   
   return ucCheckSum;
}

typedef struct ANT_MESSAGE
{
   UCHAR ucMessageID;
   UCHAR aucData[MESG_MAX_SIZE_VALUE];
} ANT_MESSAGE;

class CFramerANT
{
public:
	CFramerANT(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false):
		serial_(receivePin,transmitPin,inverse_logic)
		{
		}
	BOOL WriteMessage(void *pvData_, USHORT usMessageSize_)
	{
	   UCHAR aucTxFifo[TX_FIFO_SIZE];
	   UCHAR ucTotalSize;

	   if (usMessageSize_ > MESG_MAX_SIZE_VALUE)
	   {
		  return FALSE;
	   }

	   ucTotalSize = (UCHAR) usMessageSize_ + MESG_HEADER_SIZE;
	   aucTxFifo[0] = MESG_TX_SYNC;
	   aucTxFifo[MESG_SIZE_OFFSET] = (UCHAR) usMessageSize_;
	   aucTxFifo[MESG_ID_OFFSET] = ((ANT_MESSAGE *) pvData_)->ucMessageID;
	   memcpy(&aucTxFifo[MESG_DATA_OFFSET], ((ANT_MESSAGE *) pvData_)->aucData, usMessageSize_);
	   aucTxFifo[ucTotalSize++] = CheckSum_Calc8(aucTxFifo, ucTotalSize);

	   // Pad with two zeros.
	   aucTxFifo[ucTotalSize++] = 0;
	   aucTxFifo[ucTotalSize++] = 0;


	   //if (pclSerial->WriteBytes(aucTxFifo, ucTotalSize))
	   if (serial_.write(aucTxFifo, ucTotalSize))
	   {
		  return TRUE;
	   }
	   return FALSE;

	}
	BOOL SendCommand(ANT_MESSAGE *pstANTMessage_, USHORT usMessageSize_, ULONG ulResponseTime_=0)
	{
		if(ulResponseTime_!=0)
		{
			if (WriteMessage(pstANTMessage_, usMessageSize_))
			{
				return TRUE;
			}
			
		}
		//wait response
		else
		{
		}
		
		return FALSE;
	}
	BOOL ResetSystem()
	{
		ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_SYSTEM_RESET_ID;
	   stMessage.aucData[0] = 0;
	   return WriteMessage(&stMessage, MESG_SYSTEM_RESET_SIZE);		
	}
	
private:
	SoftwareSerial serial_;
};

static CFramerANT * g_pFramerANT = NULL;


//Initializes and opens USB connection to the module
extern "C" EXPORT
BOOL ANT_Init(UCHAR ucUSBDeviceNum, ULONG ulBaudrate)
{
	if(NULL==g_pFramerANT)
	{
		g_pFramerANT = new CFramerANT(10,11);
	}
    return TRUE;
}

extern "C" EXPORT
void ANT_Close()
{
	if(NULL==g_pFramerANT)
	{
		delete g_pFramerANT;
	}	
}

extern "C" EXPORT
BOOL ANT_ResetSystem(void)
{
	if(g_pFramerANT)
	{
		return g_pFramerANT->ResetSystem();
	}
}



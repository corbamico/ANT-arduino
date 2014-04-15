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
   USHORT i;
   
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
			serial_.begin(4800);
			Serial.begin(4800);
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
	   aucTxFifo[ucTotalSize] = CheckSum_Calc8(aucTxFifo, ucTotalSize);
	   ucTotalSize++;

	   // Pad with two zeros.
	   aucTxFifo[ucTotalSize++] = 0;
	   aucTxFifo[ucTotalSize++] = 0;


	   //if (pclSerial->WriteBytes(aucTxFifo, ucTotalSize))
//	   if (serial_.write(aucTxFifo, ucTotalSize))
	   if (Serial.write(aucTxFifo, ucTotalSize))
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
			return WriteMessage(pstANTMessage_, usMessageSize_);
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
	
	///////////////////////////////////////////////////////////////////////
	BOOL SetNetworkKey(UCHAR ucNetworkNumber_, UCHAR *pucKey_, ULONG ulResponseTime_)
	{
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_NETWORK_KEY_ID;
	   stMessage.aucData[0] = ucNetworkNumber_;
	   memcpy(&stMessage.aucData[1], pucKey_, 8);

	   return SendCommand(&stMessage, MESG_NETWORK_KEY_SIZE, ulResponseTime_);
	}
	BOOL AssignChannel(UCHAR ucANTChannel_, UCHAR ucChannelType_, UCHAR ucNetworkNumber_, ULONG ulResponseTime_)
	{
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_ASSIGN_CHANNEL_ID;
	   stMessage.aucData[0] = ucANTChannel_;
	   stMessage.aucData[1] = ucChannelType_;
	   stMessage.aucData[2] = ucNetworkNumber_;

	   return SendCommand(&stMessage, MESG_ASSIGN_CHANNEL_SIZE, ulResponseTime_);
	}
	
	BOOL SetChannelID(UCHAR ucANTChannel_, USHORT usDeviceNumber_, UCHAR ucDeviceType_, UCHAR ucTransmitType_, ULONG ulResponseTime_)
	{
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_CHANNEL_ID_ID;
	   stMessage.aucData[0] = ucANTChannel_;
	   stMessage.aucData[1] = (UCHAR)(usDeviceNumber_ & 0xFF);
	   stMessage.aucData[2] = (UCHAR)((usDeviceNumber_ >>8) & 0xFF);
	   stMessage.aucData[3] = ucDeviceType_;
	   stMessage.aucData[4] = ucTransmitType_;

	   return SendCommand(&stMessage, MESG_CHANNEL_ID_SIZE, ulResponseTime_);
	}
	
	BOOL SetChannelPeriod(UCHAR ucANTChannel_, USHORT usMessagePeriod_, ULONG ulResponseTime_)
	{
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_CHANNEL_MESG_PERIOD_ID;
	   stMessage.aucData[0] = ucANTChannel_;
	   stMessage.aucData[1] = (UCHAR)(usMessagePeriod_ & 0xFF);
	   stMessage.aucData[2] = (UCHAR)((usMessagePeriod_ >>8) & 0xFF);

	   return SendCommand(&stMessage, MESG_CHANNEL_MESG_PERIOD_SIZE, ulResponseTime_);
	}
	BOOL SetChannelRFFrequency(UCHAR ucANTChannel_, UCHAR ucRFFrequency_, ULONG ulResponseTime_)
	{
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_CHANNEL_RADIO_FREQ_ID;
	   stMessage.aucData[0] = ucANTChannel_;
	   stMessage.aucData[1] = ucRFFrequency_;

	   return SendCommand(&stMessage, MESG_CHANNEL_RADIO_FREQ_SIZE, ulResponseTime_);
	}
	
	
	BOOL CloseChannel(UCHAR ucANTChannel_, ULONG ulResponseTime_)
	{
	   BOOL bReturn;
	   ANT_MESSAGE stMessage;

	   stMessage.ucMessageID = MESG_CLOSE_CHANNEL_ID;
	   stMessage.aucData[0]  = ucANTChannel_;
	   bReturn = SendCommand(&stMessage, MESG_CLOSE_CHANNEL_SIZE, ulResponseTime_);
	   return bReturn;
	}
	BOOL Read(uint8_t& ch)
	{
		if (Serial.available() > 0)
		{
			ch = Serial.read();
			return TRUE;
		}
		return FALSE;
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
	return FALSE;
}

extern "C" EXPORT
BOOL ANT_Read(uint8_t& ch)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->Read(ch);
	}
	return FALSE;
}
extern "C" EXPORT 
BOOL ANT_SetNetworkKey(UCHAR ucNetNumber, UCHAR *pucKey)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->SetNetworkKey(ucNetNumber,pucKey,0);
	}
	return FALSE;	
}
extern "C" EXPORT 
BOOL ANT_AssignChannel(UCHAR ucANTChannel_, UCHAR ucChannelType_, UCHAR ucNetworkNumber_)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->AssignChannel(ucANTChannel_,ucChannelType_,ucNetworkNumber_,0/*ulResponseTime_*/);
	}
	return FALSE;	
}

extern "C" EXPORT 
BOOL ANT_SetChannelId(UCHAR ucANTChannel, USHORT usDeviceNumber, UCHAR ucDeviceType, UCHAR ucTransmissionType_)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->SetChannelID(ucANTChannel,usDeviceNumber,ucDeviceType,ucTransmissionType_,0/**/);
	}
	return FALSE;
}
extern "C" EXPORT 
BOOL ANT_SetChannelPeriod(UCHAR ucANTChannel, USHORT usMesgPeriod)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->SetChannelPeriod(ucANTChannel,usMesgPeriod,0 /*ULONG ulResponseTime_*/);
	}
	return FALSE;
}

extern "C" EXPORT 
BOOL ANT_SetChannelRFFreq(UCHAR ucANTChannel, UCHAR ucRFFreq)
{
	if (g_pFramerANT)
	{
		return g_pFramerANT->SetChannelRFFrequency(ucANTChannel,ucRFFreq,0 /*ULONG ulResponseTime_*/);
	}
	return FALSE;
}

extern "C" EXPORT 
BOOL ANT_OpenChannel(UCHAR ucANTChannel) // Opens a Channel
{
	if (g_pFramerANT)
	{
		return FALSE;//
	}
	return FALSE;
}


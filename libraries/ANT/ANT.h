/*
 * ANT.h
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
#ifndef	ANT_H_INCLUDED
#define	ANT_H_INCLUDED
#include "types.h"
#include "antdefines.h"
#include "antmessage.h"

#include <SoftwareSerial.h>

class ANTPlusClass
{
public:
	void begin(long speed);
};




#ifdef __cplusplus
extern "C" {
#endif
#define EXPORT __attribute__((visibility("default")))
// Application callback function pointer
typedef BOOL (*RESPONSE_FUNC)(UCHAR ucANTChannel, UCHAR ucResponseMsgID);
typedef BOOL (*CHANNEL_EVENT_FUNC)(UCHAR ucANTChannel, UCHAR ucEvent);


EXPORT BOOL ANT_Init(UCHAR ucUSBDeviceNum_, ULONG ulBaudrate_);  //Initializes and opens USB connection to the module
EXPORT void ANT_Close();   //Closes the USB connection to the module

EXPORT void ANT_AssignResponseFunction(RESPONSE_FUNC pfResponse, UCHAR* pucResponseBuffer); // pucResponse buffer should be of size MESG_RESPONSE_EVENT_SIZE
EXPORT void ANT_AssignChannelEventFunction(UCHAR ucANTChannel,CHANNEL_EVENT_FUNC pfChannelEvent, UCHAR *pucRxBuffer);
EXPORT void ANT_UnassignAllResponseFunctions();	//Unassigns all response functions


EXPORT BOOL ANT_SetNetworkKey(UCHAR ucNetNumber, UCHAR *pucKey);
EXPORT BOOL ANT_AssignChannel(UCHAR ucANTChannel, UCHAR ucChanType, UCHAR ucNetNumber);
EXPORT BOOL ANT_SetChannelId(UCHAR ucANTChannel, USHORT usDeviceNumber, UCHAR ucDeviceType, UCHAR ucTransmissionType_);
EXPORT BOOL ANT_SetChannelPeriod(UCHAR ucANTChannel, USHORT usMesgPeriod);
EXPORT BOOL ANT_SetChannelRFFreq(UCHAR ucANTChannel, UCHAR ucRFFreq);

EXPORT BOOL ANT_OpenChannel(UCHAR ucANTChannel); // Opens a Channel
EXPORT BOOL ANT_CloseChannel(UCHAR ucANTChannel); // Close a channel

EXPORT BOOL ANT_ResetSystem(void);


#ifdef __cplusplus
}
#endif
#endif  //ANT_H_INCLUDED

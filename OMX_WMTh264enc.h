/*
* Copyright (c) 2011-2012 WonderMedia Technologies, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License"); you may not 
* use this file except in compliance with the License. You may obtain a copy 
* of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software distributed 
* under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
* CONDITIONS OF ANY KIND, either express or implied. See the License for the 
* specific language governing permissions and limitations under the License.
*/
#ifndef OMX_WMTH264ENC_H
/* To assert that only one occurrence is included */
#define OMX_WMTH264ENC_H

/*--- OMX_WMTh264enc.h---------------------------------------------------------------
*   Copyright (C) 2011 WonderMedia Tech. Inc.
*
* MODULE       : OMX_WMTh264enc.h
* AUTHOR       : Andy Cho
* DATE         : 2011/12/22
* DESCRIPTION  : 
*------------------------------------------------------------------------------*/


/*--- History -------------------------------------------------------------------
*Version 0.01 , Andy Cho
*	First version
*
*------------------------------------------------------------------------------*/
/*-------------------- MODULE DEPENDENCY -------------------------------------*/

#include <pthread.h>     /* for h264enc thread */

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "wmt-h264enc.h"

/*-------------------- EXPORTED PRIVATE CONSTANTS ----------------------------*/

#define BUF_COUNT		3
/*-------------------- EXPORTED PRIVATE TYPES---------------------------------*/

typedef enum portNum{
	OMX_INPUT_PORT =0,
	OMX_OUTPUT_PORT,
	OMX_MAX_PORT,
}portNum;

typedef enum kind_of_buffer{
	srcBuffer = 0,
	dstBuffer = 1,
	cmdBuffer = 2,
}kind_of_buffer;

typedef struct{
	OMX_IN OMX_HANDLETYPE hComponent;
	OMX_IN OMX_COMMANDTYPE Cmd;
	OMX_IN OMX_U32 nParam1;
	OMX_IN OMX_PTR pCmdData;
}omx_command_t;

typedef struct{
	void *data[BUF_COUNT];
	int head;
	int tail;
	int isFull;
	pthread_mutex_t mutex;
	pthread_cond_t  notEmpty;
	pthread_cond_t  notFull;
}wmt_bufflist_t;

typedef struct{
	ve_handle_t hve;
	int mb_fd;
	int width;
	int height;
	int pixelfmt;
	int fps;
	int start;
	int enc_start;
	pthread_t  h264enc_thread;
	pthread_t  cmdhandle_thread;
	pthread_mutex_t data_mutex;
	pthread_mutex_t state_mutex;
	pthread_mutex_t h264enc_mutex;
	pthread_cond_t populate_cond;
	pthread_cond_t unpopulate_cond;
	pthread_cond_t state_cond;
	pthread_cond_t h264enc_cond;
	int	state_count;
	int enc_frame_no;
	OMX_U32 tablecount[OMX_MAX_PORT];
	wmt_bufflist_t Buffers[3];
	

	//component information
	OMX_PORT_PARAM_TYPE* 				portParam;
	OMX_VIDEO_PARAM_PORTFORMATTYPE*		portFormat[OMX_MAX_PORT];
	OMX_PARAM_PORTDEFINITIONTYPE* 		portDef[OMX_MAX_PORT];
	OMX_PRIORITYMGMTTYPE* 				piorityMgmt;
	OMX_VIDEO_PARAM_AVCTYPE* 			avc;
	OMX_VIDEO_PARAM_BITRATETYPE* 		bitrate[OMX_MAX_PORT];
	OMX_CONFIG_FRAMERATETYPE*			framerateConfig;	//only output port need
	OMX_VIDEO_CONFIG_BITRATETYPE*		bitrateConfig ;		//only output port need
	OMX_VIDEO_PARAM_QUANTIZATIONTYPE* 	quantization;
	OMX_CALLBACKTYPE					callBack;
	OMX_HANDLETYPE						component;
	OMX_STATETYPE						state;
	OMX_IMAGE_PARAM_QFACTORTYPE* 		QFactor;
	OMX_CONFIG_INTRAREFRESHVOPTYPE*		refreshVOP;
	OMX_BUFFERHEADERTYPE*				buffertable[OMX_MAX_PORT][BUF_COUNT];
	OMX_BOOL							cmpAllocate[OMX_MAX_PORT][BUF_COUNT];
	OMX_STRING							componentName;
	OMX_PARAM_COMPONENTROLETYPE 		componentRole;
		
}component_privatedata;

/*--------------------- EXPORTED PRIVATE MACROS -------------------------------*/

#define WMT_MALLOC(ptr,type,size) 	\
	ptr = (type *)malloc(size); 	\
	if(ptr == NULL)					\								
		return OMX_ErrorInsufficientResources; \
	memset(ptr,0,size)	
	

/*--------------------- EXPORTED PRIVATE FUNCTIONS  ---------------------------*/

#ifdef	__cplusplus
extern "C" {
#endif

static OMX_ERRORTYPE SetCallbacks (OMX_IN OMX_HANDLETYPE hComponent,
                                   	   OMX_IN OMX_CALLBACKTYPE* pCallBacks,
                                       OMX_IN OMX_PTR pAppData);

static OMX_ERRORTYPE GetComponentVersion (OMX_HANDLETYPE hComponent,
                                          		   OMX_STRING  szComponentName,
                                          		   OMX_VERSIONTYPE* pComponentVersion,
                                          		   OMX_VERSIONTYPE* pSpecVersion,
                                          	  	   OMX_UUIDTYPE* pComponentUUID);

static OMX_ERRORTYPE SendCommand (OMX_IN OMX_HANDLETYPE hComponent,
                                  		 OMX_IN OMX_COMMANDTYPE Cmd,
                                  		 OMX_IN OMX_U32 nParam1,
                                  		 OMX_IN OMX_PTR pCmdData);

static OMX_ERRORTYPE GetParameter (OMX_IN OMX_HANDLETYPE hComponent,
                                   	    OMX_IN OMX_INDEXTYPE nParamIndex,
                                   		OMX_INOUT OMX_PTR CompParamStruct);

static OMX_ERRORTYPE SetParameter (OMX_IN OMX_HANDLETYPE hComponent,
                                   	   OMX_IN OMX_INDEXTYPE nParamIndex,
                                   	   OMX_IN OMX_PTR CompParamStruct);

static OMX_ERRORTYPE GetConfig (OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_INDEXTYPE nConfigIndex,
                                   OMX_INOUT OMX_PTR ComponentConfigStructure);

static OMX_ERRORTYPE SetConfig (OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_INDEXTYPE nConfigIndex,
                                   OMX_IN OMX_PTR ComponentConfigStructure);

static OMX_ERRORTYPE EmptyThisBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                                      	   OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE FillThisBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                                     	OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE GetState (OMX_IN OMX_HANDLETYPE hComponent,
                               	 OMX_OUT OMX_STATETYPE* pState);

static OMX_ERRORTYPE ComponentTunnelRequest (OMX_IN OMX_HANDLETYPE hComponent,
                                             			OMX_IN OMX_U32 nPort,
                                             			OMX_IN OMX_HANDLETYPE hTunneledComp,
                                             			OMX_IN  OMX_U32 nTunneledPort,
                                             			OMX_INOUT OMX_TUNNELSETUPTYPE* pTunnelSetup);

static OMX_ERRORTYPE UseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                   OMX_IN OMX_U32 nPortIndex,
                                   OMX_IN OMX_PTR pAppPrivate,
                                   OMX_IN OMX_U32 nSizeBytes,
                                   OMX_IN OMX_U8* pBuffer);

static OMX_ERRORTYPE AllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                                        OMX_INOUT OMX_BUFFERHEADERTYPE** pBuffer,
                                        OMX_IN OMX_U32 nPortIndex,
                                        OMX_IN OMX_PTR pAppPrivate,
                                        OMX_IN OMX_U32 nSizeBytes);

static OMX_ERRORTYPE FreeBuffer (OMX_IN OMX_HANDLETYPE hComponent,
                                    OMX_IN OMX_U32 nPortIndex,
                                    OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE ComponentDeInit (OMX_IN OMX_HANDLETYPE hComponent);


static OMX_ERRORTYPE ExtensionIndex (OMX_IN OMX_HANDLETYPE hComponent,
                                     	  OMX_IN OMX_STRING cParameterName,
                                     	  OMX_OUT OMX_INDEXTYPE* pIndexType);

void *h264enc_thread_entry(void*);
void *cmdhandle_thread_entry(void*);

#ifdef	__cplusplus
}
#endif	

#endif /* ifndef OMX_WMTH264ENC_H */


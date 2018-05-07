
#include "sink_private.h"

#include <stdio.h>
#include "string.h"
#include "sink_spp.h"
#include <connection_no_ble.h>
#include "sink_debug.h"
#include "print.h"
/*
#define DEBUG_SPPS

#ifdef DEBUG_SPPS
#define SPPS_DEBUG(x)   DEBUG(x)
#else
#define SPPS_DEBUG(x) 
#endif
*/
static SppsTaskData theSppServiceTask;

#define SPPS_MESSAGE_SEND_DATA   (SPPS_SHIM_MESSAGE_TOP)

#define BAD_SINK ((Sink) 0xFFFF)
#define BAD_SINK_CLAIM (0xFFFF)

static char test[]="FUCK U";

void sinkSppsSendString(const char *s, uint16 length)
{
    MESSAGE_SPP_MAKE(smsd, SPPS_MESSAGE_SEND_DATA_T);
    smsd->size_data = length;
    smsd->data = (uint8 *)s;
    MessageSend(&(theSppServiceTask.task), SPPS_MESSAGE_SEND_DATA, smsd);
}

static void sppsSendPacket(uint8 *data, uint16 length)
{
    Sink sink = theSppServiceTask.sink;
    
    if (SinkClaim(sink, length) == BAD_SINK_CLAIM)
    {
        /*SPPS_DEBUG(("spps: no sink\n"));*/
        printf("spps: no sink\n");
    }

    else
    {
        uint8 *sink_data = SinkMap(sink);
        memcpy (sink_data, data, length);

        SinkFlush(sink, length);
    }

}

void sinkSppsDisconnect(void)
{
    if (theSppServiceTask.spp != NULL)
        SppsDisconnectRequestTestExtra(theSppServiceTask.spp);
}

void handleSPPsMessage ( Task task, MessageId id, Message message )
{
    bool processed = TRUE;
    
    uint8 *pdu;
    uint16 pdu_size;
    uint16 i;
    
    switch (id)
    {
        case SPPS_MESSAGE_SEND_DATA:
            sppsSendPacket(((SPPS_MESSAGE_SEND_DATA_T *)message)->data, 
                ((SPPS_MESSAGE_SEND_DATA_T *)message)->size_data);
            break;
            
        case SPPS_MESSAGE_MORE_DATA_TEST_EXTRA:
            pdu = (((SPPS_MESSAGE_MORE_DATA_TEST_EXTRA_T *)message)->data);
            pdu_size = (((SPPS_MESSAGE_MORE_DATA_TEST_EXTRA_T *)message)->size_data);
            
            /*SPPS_DEBUG (("pdu_size %d\n", pdu_size));*/
            printf("pdu_size %d\n", pdu_size);
            for (i = 0; i < pdu_size; i++)
                printf("%c", pdu[i]);
                /*SPPS_DEBUG(("%c", pdu[i]));*/
            printf("\n");
            /*SPPS_DEBUG(("\n"));*/
            
            sinkSppsSendString( (char*)pdu, pdu_size);
            
            break;
            
        case SPPS_CONNECT_IND_TEST_EXTRA:                
            SppsConnectResponseTestExtra(task, 
                &((SPPS_CONNECT_IND_TEST_EXTRA_T *)message)->addr, 
                TRUE, 
                ((SPPS_CONNECT_IND_TEST_EXTRA_T *)message)->sink,
                ((SPPS_CONNECT_IND_TEST_EXTRA_T *)message)->server_channel, 
                0);
            
            break;

        case SPPS_SERVER_CONNECT_CFM_TEST_EXTRA:
            theSppServiceTask.spp = ((SPPS_SERVER_CONNECT_CFM_TEST_EXTRA_T *)message)->spp;
            theSppServiceTask.sink = ((SPPS_SERVER_CONNECT_CFM_TEST_EXTRA_T *)message)->sink;
            
            sinkSppsSendString(test,sizeof(test)/sizeof(uint8));
            break;
            
        case SPPS_DISCONNECT_IND_TEST_EXTRA:
            SppsDisconnectResponseTestExtra(theSppServiceTask.spp);
            break;

        case SPPS_DISCONNECT_CFM_TEST_EXTRA:
            theSppServiceTask.spp = NULL;
            break;

        case SPPS_START_SERVICE_CFM_TEST_EXTRA:
            break;

        case SPPS_STOP_SERVICE_CFM_TEST_EXTRA:
            break;

        case CL_RFCOMM_CONTROL_IND:
            break;
            
        default:
            processed = FALSE;
            break;
    }

    if (!processed)
        SppsHandleComplexMessage( task, id, message );
    
}

void sinkSppsInit(void)
{
    theSppServiceTask.task.handler = handleSPPsMessage;
    
    SppsStartServiceTestExtra(&(theSppServiceTask.task));
}



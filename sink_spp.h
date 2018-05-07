#ifndef _SINK_SPP_H_
#define _SINK_SPP_H_

#include <spps_shim.h>

#define MESSAGE_SPP_MAKE(NAME,TYPE) TYPE * const NAME = PanicUnlessNew(TYPE)

typedef struct 
{
 	TaskData    task;
    Sink        sink;
    SPP         *spp;
} SppsTaskData;

typedef struct
{
    uint16  size_data;
    uint8   *data;
} SPPS_MESSAGE_SEND_DATA_T;

#if 1
void handleSPPsMessage ( Task task, MessageId id, Message message );

void sinkSppsInit(void);

void sinkSppsSendString(const char *s, uint16 length);

void sinkSppsDisconnect(void);


#else

#define handleSPPsMessage(task, id, message)  {}

#define sink_spps_init( )

#endif

#endif /* _SINK_SPP_H_ */



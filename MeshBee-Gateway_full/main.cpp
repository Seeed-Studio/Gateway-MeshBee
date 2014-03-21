/*
 * main.c
 * Gateway for SeeedStudio Mesh Bee(Zigbee) module
 *
 * Copyright (c) Seeed Studio. 2014.
 * Author     : Oliver Wang
 * Create Time: 2014/03
 * Change Log :
*/

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "mbed.h"
#include "EthernetInterface.h"
#include "rtos.h"
#include "string.h"

#include "xively.h"
#include "xi_err.h"
#include "common.h"
#include "ringbuf.h"

/****************************************************************************/
/***        Globle Variables                                               ***/
/****************************************************************************/
//USB printf
Serial console(USBTX,USBRX);
//Transmit data between zigbee node and mbed gateway
Serial dataPort(P0_0,P0_1);
//Serial dataPort(P4_28,P4_29);
//led
DigitalOut  run_led(LED1);
//ethernet
EthernetInterface eth;
//A Feed is the collection of channels(datastream)
xi_feed_t feed;
//Datastream contains a series of datapoints
xi_datapoint_t* current_temp;
xi_context_t* xi_context;

//thread vari
osMutexId Rx_Mutex;
osMutexDef(Rx_Mutex);

//ringbuffer
struct ringbuffer rb_rx_uart;
struct ringbuffer rb_tx_uart;

uint8 rb_rx_mempool[RXFIFOLEN*3];
uint8 rb_tx_mempool[RXFIFOLEN*3];

//datastream:A series of data points
typedef struct
{
    uint8 verifyByte;                   //verify,avoiding mistakes when take serial transmission
    uint8 dataType;                     // maybe on-chip temperature or voltage
    uint16 dataPointCnt;                // number of data point
    uint16 dataPoint[DATA_POINT_NUM];   //a series of data points
}tsDataStream;

//remote cmd string
const char *QueryDataCmd = "atqd\r\n";

/****************************************************************************/
/***        Local Functions declaration                                   ***/
/****************************************************************************/
void vErr_Callback(void);
int vTRAN_SerialRead(char* buf, int len);
void vTRAN_SerialCallBack(void);
uint16 vEndianTransform(uint16 src);
/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/*
 **@ fuc name: vMEM_RingBufferInit
 **@ bref: ringbuffer initialize
 **@ retval: none
 **@ input: none
 **@ output: none
*/
void vMEM_RingBufferInit(void){
    init_ringbuffer(&rb_rx_uart, rb_rx_mempool, RXFIFOLEN * 3);
    init_ringbuffer(&rb_tx_uart, rb_tx_mempool, RXFIFOLEN * 3);    
}
/*
 **@ fuc name: vTRAN_SerialInit
 **@ bref: ethernet initialize
 **@ retval: none
 **@ input: none
 **@ output: none
*/
void vTRAN_SerialInit(void){
   console.baud(9600); 
   dataPort.baud(115200);   //is baudrate error?
   
   //dataPort.attach(&vTRAN_SerialCallBack,Serial::RxIrq);          
}
/*
 **@ fuc name: vTRAN_EthernetInit
 **@ bref: ethernet initialize
 **@ retval: none
 **@ input: none
 **@ output: none
*/
void vTRAN_EthernetInit(void){
                
    //default:DHCP
    int ret = eth.init();    
    if(ret != NULL){
        console.printf("Failed to initialize Ethernet.\n");          
        vErr_Callback();
    }
    
    //connect network
    ret = eth.connect();
    if(ret != NULL){
        console.printf("Failed to connect Ethernet.\n");           
        vErr_Callback();
    }
    else{
        console.printf("IP: %s\n", eth.getIPAddress());    
    }    
}
/*
 **@ fuc name: vTRAN_XivelyInit
 **@ bref: Xively initialize
 **@ retval: none
 **@ input: none
 **@ output: none
*/
int vTRAN_XivelyInit(){
    memset(&feed, NULL, sizeof(xi_feed_t));
    feed.feed_id = XI_FEED_ID;      //feed id,visit xively.com for more details
    feed.datastream_count = 1;      //now,we only create one sensor variable "inner_temp"
    feed.datastreams[0].datapoint_count = 1;
    xi_datastream_t* temp_datastream = &feed.datastreams[0];
    strcpy(temp_datastream->datastream_id,"inner_temp");
    current_temp = &temp_datastream->datapoints[0];
    
    xi_context = xi_create_context(XI_HTTP, XI_API_KEY, feed.feed_id);
    if(NULL == xi_context){
        console.printf("Failed to create xi_context.\n");
        return -1;        
    }
    
    return 0;
}

/*
 **@ fuc name: vFrameDecoder
 **@ bref: 
 **@ retval: none
 **@ input: none
 **@ output: none
*/
void vFrameDecoder(uint8 *buffer, int len, tsDataStream *stream, bool *isValid){
    uint8 *pos = buffer;
    while(pos < buffer + len){
        if(*pos == 0x3e){
            *isValid = TRUE;
            if((pos+sizeof(tsDataStream)) <= (buffer + len)){
                memcpy((uint8*)stream, pos, sizeof(tsDataStream));   
                
                //Endian transform
                int i = 0;
                for(; i<DATA_POINT_NUM; i++){
                    stream->dataPoint[i] = vEndianTransform(stream->dataPoint[i]);    
                } 
            }    
            return;
        }    
        pos++;
    }    
}

/*
 **@ fuc name: vEndianTransform
 **@ bref: JN516x unsigned short to LPC1768 unsigned short
 **@ retval: none
 **@ input: none
 **@ output: none
*/
uint16 vEndianTransform(uint16 src){
    uint16 a, b=0, c=0;
    
    a = src;
    b=(a>>8)&Mask;
    c=(a<<8)&(~Mask);
    return b|c;        
}
/*
 **@ fuc name: vThread_SerialPort
 **@ bref: serial call back
 **@ retval: none
 **@ input: none
 **@ output: none
*/
void vThread_SerialPort(const void *args){
    while(true){
        //check how many ring buffer size are availiable  
        osMutexWait(Rx_Mutex, osWaitForever); 
        uint32 free_cnt = ringbuffer_free_space(&rb_rx_uart);
        osMutexRelease(Rx_Mutex);
        
        //if uart buffer have data and ring buffer is free
        while(dataPort.readable() && free_cnt > 1){
            uint8 tmp = dataPort.getc();
            osMutexWait(Rx_Mutex, osWaitForever);
            ringbuffer_push(&rb_rx_uart, &tmp, sizeof(uint8)); 
            osMutexRelease(Rx_Mutex);   
        }                           
    }    
}
osThreadDef(vThread_SerialPort, osPriorityNormal, DEFAULT_STACK_SIZE);

/*
 **@ fuc name: vThread_SensorDataProcess
 **@ bref: decode sensor data frame
 **@ retval: none
 **@ input: none
 **@ output: none
*/

uint8 tmpBuf[RXFIFOLEN];
void vThread_SensorDataProcess(const void *args){
    while(1){
        osMutexWait(Rx_Mutex, osWaitForever);
        uint32 dataCnt = ringbuffer_data_size(&rb_rx_uart);
        osMutexRelease(Rx_Mutex);
        
        if(dataCnt < sizeof(tsDataStream)){
            wait(0.02);
            continue;    
        }    
        
        //read data       
        uint32 readCnt = MIN(dataCnt, THRESHOLD_READ);
        memset(tmpBuf,0,sizeof(tmpBuf));
        
        osMutexWait(Rx_Mutex, osWaitForever);
        ringbuffer_pop(&rb_rx_uart, tmpBuf, readCnt);
        osMutexRelease(Rx_Mutex);
        
        bool isValid = FALSE;
        tsDataStream dataStream;
        memset(&dataStream, 0, sizeof(tsDataStream));
        vFrameDecoder(tmpBuf, readCnt, &dataStream, &isValid);
        if(isValid == TRUE){
            console.printf("verifyByte:%c\n",dataStream.verifyByte);
            console.printf("dataType:%d\n",dataStream.dataType);
            console.printf("dataPoint[0]:%d\n",dataStream.dataPoint[0]);
            
            //push to cloud platform.[xively or something]
            int i = 0;
            for(;i<DATA_POINT_NUM; i++){
                float tmpVal = (float)(dataStream.dataPoint[i]);
                xi_set_value_f32(current_temp,tmpVal);
                xi_feed_update( xi_context, &feed );                    
            }
            console.printf("Push done.\r\n");                 
        }
    }    
}
osThreadDef(vThread_SensorDataProcess, osPriorityNormal, DEFAULT_STACK_SIZE);
/*
 **@ fuc name: main
 **@ bref: main function of gateway
 **@ retval: 
 **@ input: none
 **@ output: none
*/
int main() {
    
    //init transmission layer 
    vMEM_RingBufferInit();
    vTRAN_SerialInit();
    vTRAN_EthernetInit();
    vTRAN_XivelyInit();               
    console.printf("--Gateway for Mesh network--\n");
    
    //create mutex
    Rx_Mutex = osMutexCreate(osMutex(Rx_Mutex));
    
    osThreadCreate(osThread(vThread_SerialPort), NULL);
    osThreadCreate(osThread(vThread_SensorDataProcess), NULL);
         
    //loop 
    while(1){
        if(dataPort.writeable()){
            //send a remote sample command  
            dataPort.printf(QueryDataCmd);                
        }    
        run_led = !run_led;
        wait(2);
    }
}
 
/*
 **@ fuc name: vTRAN_SerialCallBack
 **@ bref: error callback
 **@ retval: 
 **@ input: none
 **@ output: none
*/
void vTRAN_SerialCallBack(){
 
    if(dataPort.readable())
    {
        char c = dataPort.getc();        
        run_led = !run_led;
    }    
    return;
}
 
/*
 **@ fuc name: vTRAN_SerialRead
 **@ bref: Serial Port read string
 **@ retval: 
 **@ input: buf to store the data,data len
 **@ output: read len
*/
int vTRAN_SerialRead(char* buf, int len){
    int readLen = 0;
    char *p = buf;
    while(p - buf < len){
        if(dataPort.readable()){
            *p = dataPort.getc();    
            readLen++;            
        }    
        p++;
    }
    return readLen;
}
/*
 **@ fuc name: vErr_Callback
 **@ bref: error callback
 **@ retval: 
 **@ input: none
 **@ output: none
*/
void vErr_Callback(void){
    while(1){
        run_led = !run_led; 
        wait(0.2);
    }
}

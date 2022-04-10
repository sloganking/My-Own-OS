#include <drivers/vga.h>

using namespace myos::common;
using namespace myos::drivers;


//constructor
VideoGraphicsArray::VideoGraphicsArray() : 
    miscPort(0x3C2),
    crtcIndexPort(0x3d4),   //cathode ray tube controler, index data
    crtcDataPort(0x3d5),
    sequencerIndexPort(0x3c4),
    sequencerDataPort(0x3c5),
    graphicsControllerIndexPort(0x3ce),
    graphicsControllerDataPort(0x3cf),
    attributeControllerIndexPort(0x3c0),
    attributeControllerReadPort(0x3c1),
    attributeControllerWritePort(0x3c0),
    attributeControllerResetPort(0x3da)
{

}

//destructor
VideoGraphicsArray::~VideoGraphicsArray(){

}

//sends initialization codes to corresponding ports
void VideoGraphicsArray::WriteRegisters(uint8_t* registers){

    //>misc
        //write byte to miscPort and then increase pointer by 1. 
        miscPort.Write(*(registers++));

    //<>sequencer
        for(uint8_t i = 0; i < 5; i++){

            //where do you want to write data
            sequencerIndexPort.Write(i);

            //what do you want to write there
            sequencerDataPort.Write(*(registers++));
        }

    //<>cathode ray tube controller

        //unlock, send data, lock it again
        //security measure since CRTs can blow up
        crtcIndexPort.Write(0x03);
        crtcDataPort.Write(crtcDataPort.Read() | 0x80);  //set first bit to 1

        crtcIndexPort.Write(0x11);
        crtcDataPort.Write(crtcDataPort.Read() & ~0x80); //set first bit to 0

        registers[0x03] = registers[0x03] | 0x80;
        registers[0x11] = registers[0x11] & ~0x80;

        for(uint8_t i = 0; i < 25; i++){

            //where do you want to write data
            crtcIndexPort.Write(i);

            //what do you want to write there
            crtcDataPort.Write(*(registers++));
        }

    //<>graphics controller
        for(uint8_t i = 0; i < 9; i++){

            //where do you want to write data
            graphicsControllerIndexPort.Write(i);

            //what do you want to write there
            graphicsControllerDataPort.Write(*(registers++));
        }

    //<>attributeController
        for(uint8_t i = 0; i < 21; i++){

            //must reset attributeController before sending data
            attributeControllerResetPort.Read();

            //where do you want to write data
            attributeControllerIndexPort.Write(i);

            //what do you want to write there
            attributeControllerWritePort.Write(*(registers++));
        }
        attributeControllerResetPort.Read();
        attributeControllerIndexPort.Write(0x20);
    //<
}

bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height, uint32_t colordepth){
    return width == 320 && height == 200 && colordepth == 8;
}

bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height, uint32_t colordepth){

    if(!SupportsMode(width, height, colordepth)){
        return false;
    }
    unsigned char g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00,	0x00
    };

    WriteRegisters(g_320x200x256);
    return true;
}

//gives location of pixel space in memory
uint8_t* VideoGraphicsArray::GetFrameBufferSegment(){
    graphicsControllerIndexPort.Write(0x06);

    //only interested in bits 3-4
    uint8_t segmentNumber = (graphicsControllerDataPort.Read() >> 2) & 0x03;

    switch(segmentNumber){
        default:
        case 0: return (uint8_t*)0x00000;
        case 1: return (uint8_t*)0xA0000;
        case 2: return (uint8_t*)0xB0000;
        case 3: return (uint8_t*)0xB8000;
        break;
    }
}

//8 bit vga mode only has 256 colores. colorIndex selects which one to display
void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint32_t colorIndex){
    if(x < 0 || 320 <= x 
    || y < 0 || 200 <= y){
        return;
    }

    uint8_t* pixelAddress = GetFrameBufferSegment() + 320*y + x;
    *pixelAddress = colorIndex;
}

uint8_t VideoGraphicsArray::GetColorIndex(uint8_t r, uint8_t g, uint8_t b){

    if(r == 0x00 && g == 0x00 && b == 0x00) return 0x00;    //black
    if(r == 0x00 && g == 0x00 && b == 0xA8) return 0x01;    //blue
    if(r == 0x00 && g == 0xA8 && b == 0x00) return 0x02;    //green
    if(r == 0xA8 && g == 0x00 && b == 0x00) return 0x04;    //red
    if(r == 0xFF && g == 0xFF && b == 0xFF) return 0x3F;    //white

    //if incorrect color, return black
    return 0x00;
}

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b){
    PutPixel(x,y, GetColorIndex(r,g,b));
}

void VideoGraphicsArray::FillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b){
    for(uint32_t Y = y; Y < y+h; Y++){
        for(uint32_t X = x; X < x+w; X++){
            PutPixel(X,Y,r,g,b);
        }
    }
}
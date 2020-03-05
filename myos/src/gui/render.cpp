#include <gui/render.h>

using namespace myos;
using namespace common;
using namespace gui;


// class Pixel{
    // public:
        // uint8_t r,
        // uint8_t g,
        // uint8_t b


// class Render : public myos::drivers::VideoGraphicsArray{
    // private:
        // Pixel[][] pixels;

    // public:

        //constructor
        Render::Render(
            common::int32_t w, common::int32_t h
        ){
            // Pixel[w][h] pixels;
        }

        //destructor
        Render::~Render(){

        }

        //displayes pre-rendered frame to screen
        void Render::display(GraphicsContext* gc){
            for(int ix = 0; ix < 320; ix++){
                for(int iy = 0; iy < 200; iy++){
                    gc->PutPixel(ix,iy, pixels[ix][iy].r, pixels[ix][iy].g, pixels[ix][iy].b);
                }
            }
        }

        void Render::PutPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b){
            pixels[x][y].r = r;
            pixels[x][y].g = g;
            pixels[x][y].b = b;
        }

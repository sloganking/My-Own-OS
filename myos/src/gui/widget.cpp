#include <gui/widget.h>

using namespace myos::common;
using namespace myos::gui;

//constructor
Widget(
    Widget* parent, 
    int32_t x, int32_t y, int32_t w, int32_t h, 
    uint8_t r, uint8_t g, uint8_t b
)
{
    this->parent = parent;

    //position
    this->x = x;
    this->y = y;

    //size
    this->w = w;
    this->h = h;

    //rgb color attributes
    this->r = r;
    this->g = g;
    this->b = b;

    this->Focussable = true;

}

//destructor
Widget::~Widget(){

}

void Widget::GetFocus(Widget* widget){

    //pass focus to parent
    //all widgets will pass focus to parents untill a parent contains the functionality to deal with it
    //for example if you click a text box, tell the window containing that text box that it is in focus
    if(parent != 0){
        parent->GetFocus(widget)
    }
}

//to get absolute coordinates
void Widget::ModelToScreen(int32_t &x, int32_t& y){

    //get offset from parent
    if(parent != 0){
        parent->ModelToScreen(x,y);
    }

    //add offset to this widget
    x += this->x;
    y += this->y;
}

void Widget::Draw(GraphicsContext* gc){

    //get absolute coordinates so we know where to draw
    int X = 0;
    int Y = 0;
    ModelToScreen(X,Y);

    gc->FillRectangle(X,Y,w,h, r,g,b);
}

void Widget::OnMouseDown(int32_t x,  int32_t y){
    if(Focussable){
        GetFocus(this);
    }
}
void Widget::OnMouseUp(int32_t x,  int32_t y){

}

void Widget::OnMouseMove(int32_t oldx,  int32_t oldy, int32_t newx,  int32_t newy){

}

void Widget::OnKeyDown(int32_t x,  int32_t y){

}

void Widget::OnKeyUp(int32_t x,  int32_t y){

}
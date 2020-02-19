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

void Widget::OnKeyDown(char* str){

}

void Widget::OnKeyUp(char* str){

}












//constructor
CompositeWidget::CompositeWidget(
    Widget* parent, 
    int32_t x, int32_t y, int32_t w, int32_t h, 
    uint8_t r, uint8_t g, uint8_t b
)
{
    focussedChild = 0;
    numChildren = 0;
}

//destructor
CompositeWidget::~CompositeWidget(){

}

void CompositeWidget::GetFocus(Widget* widget){
    this->focussedChild = widger;
    if(parent != 0){
        parent->GetFocus(this);
    }
}

void CompositeWidget::Draw(GraphicsContext* gc){
    //draw own background
    Widget::Draw(gc);

    //draw all children
    for(int i = numChildren; i >= 0; --i){
        children[i]->Draw(gc);
    }
}

//iterate through children and pass event to the child that contains the coorinate
//pass the click to the one that has been clicked
void CompositeWidget::OnMouseDown(int32_t x,  int32_t y){
    for(int i = 0; i < numChildren; ++i){
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y)){
            children[i]->OnMouseDown(x - this->x, y - this->y);

            //only the top window is clicked on
            break;
        }
    }
}

//iterate through children and pass event to the child that contains the coorinate
//pass the click to the one that has been clicked
void CompositeWidget::OnMouseUp(int32_t x,  int32_t y){
    for(int i = 0; i < numChildren; ++i){
        if(children[i]->ContainsCoordinate(x - this->x, y - this->y)){
            children[i]->OnMouseUp(x - this->x, y - this->y);

            //only the top window is clicked on
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(int32_t oldx,  int32_t oldy, int32_t newx,  int32_t newy){
    
    int firstChild = -1;

    //for object that contains old coordinate
    for(int i = 0; i < numChildren; ++i){
        if(children[i]->ContainsCoordinate(oldx - this->x, oldy - this->y)){

            //subtracting thisx/y turns the coordinates into relative coordinates
            children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);

            firstChild = i;

            //only the top window is clicked on
            break;
        }
    }

    //for object that contains new coordinate
    for(int i = 0; i < numChildren; ++i){
        if(children[i]->ContainsCoordinate(newx - this->x, newy - this->y)){

            if(firstChild != i){
                //subtracting thisx/y turns the coordinates into relative coordinates
                children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            }

            //only the top window is clicked on
            break;
        }
    }
}

void CompositeWidget::OnKeyDown(char* str){
    if(focussedChild != 0){
        focussedChild->OnKeyDown(str);
    }
}
void CompositeWidget::OnKeyUp(char* str){
    if(focussedChild != 0){
        focussedChild->OnKeyUp(str);
    }
}
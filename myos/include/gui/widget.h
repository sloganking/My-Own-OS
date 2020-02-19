#ifndef __MYOS__GUI__WIDGET_H
#define __MYOS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>

namespace myos{
    namespace drivers{
        class Widget{
            protected:

                Widget* parent;
                common::int32_t x;
                common::int32_t y;
                common::int32_t w;
                common::int32_t h;

                common::uint8_t r;
                common::uint8_t g;
                common::uint8_t b;

                bool Focussable;

            public:

                //constructor
                Widget(
                    Widget* parent, 
                    common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, 
                    common::uint8_t r, common::uint8_t g, common::uint8_t b
                );

                //destructor
                ~Widget();

                virtual void GetFocus(Widget* widget);
                virtual void ModelToScreen(common::int32_t &x, common::int32_t& y);

                virtual void Draw(GraphicsContext* gc);
                virtual void OnMouseDown(common::int32_t x, common:: int32_t y);
                virtual void OnMouseUp(common::int32_t x, common:: int32_t y);
                virtual void OnMouseMove(common::int32_t oldx, common:: int32_t oldy, common::int32_t newx, common:: int32_t newy);

                virtual void OnKeyDown(common::int32_t x, common:: int32_t y);
                virtual void OnKeyUp(common::int32_t x, common:: int32_t y);
        };

        //contains array of widgets and the order decides which gets drawn first
        class CompositeWidget : public Widget{
            private: 
                Widget* children[100];
                int numChildren;
                Widget* focussedChild;

            public:

                //constructor
                CompositeWidget(
                    Widget* parent, 
                    common::int32_t x, common::int32_t y, common::int32_t w, common::int32_t h, 
                    common::uint8_t r, common::uint8_t g, common::uint8_t b
                );

                //destructor
                ~CompositeWidget();

                virtual void GetFocus(Widget* widget);

                virtual void Draw(GraphicsContext* gc);
                virtual void OnMouseDown(common::int32_t x, common:: int32_t y);
                virtual void OnMouseUp(common::int32_t x, common:: int32_t y);
                virtual void OnMouseMove(common::int32_t oldx, common:: int32_t oldy, common::int32_t newx, common:: int32_t newy);

                virtual void OnKeyDown(common::int32_t x, common:: int32_t y);
                virtual void OnKeyUp(common::int32_t x, common:: int32_t y);
        };
    }
}

#endif
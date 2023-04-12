#ifndef IVIEW_H
#define IVIEW_H

#include "appModel.h"

/*
**   IView is the observer abstract class, GUI is the concrete observer class
*/

class IView {
    public:
        virtual void updateView(AppModel &appModel) = 0;
        virtual ~IView() = default;

};

#endif 
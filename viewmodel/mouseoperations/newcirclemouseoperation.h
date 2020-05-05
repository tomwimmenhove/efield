#ifndef NEWCIRCLEMOUSEOPERATION_H
#define NEWCIRCLEMOUSEOPERATION_H

#include "newtwonodemousepperation.h"
#include "util/undo/newcircleundoitem.h"

class NewCircleMouseOperation : public NewTwoNodeMouseOperation<CircleElement<float>, NewCircleUndoItem>
{
public:
    using NewTwoNodeMouseOperation::NewTwoNodeMouseOperation;
};

#endif // NEWCIRCLEMOUSEOPERATION_H

#ifndef NEWLINEMOUSEOPERATION_H
#define NEWLINEMOUSEOPERATION_H

#include "newtwonodemousepperation.h"
#include "util/undo/newlineundoitem.h"

class NewLineMouseOperation : public NewTwoNodeMouseOperation<LineElement<float>, NewLineUndoItem>
{
public:
    using NewTwoNodeMouseOperation::NewTwoNodeMouseOperation;
};

#endif // NEWLINEMOUSEOPERATION_H

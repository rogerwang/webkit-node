/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PredictedType.h"

#include "ValueProfile.h"
#include <wtf/BoundsCheckedPointer.h>

namespace JSC {

#ifndef NDEBUG
const char* predictionToString(PredictedType value)
{
    if (value == PredictNone)
        return "None";
    
    static const int size = 96;
    static char description[size];
    BoundsCheckedPointer<char> ptr(description, size);
    
    if (value & PredictObjectUnknown) {
        ASSERT(!(value & (PredictObjectMask & ~PredictObjectUnknown)));
        ptr.strcat("Object");
    }

    if (value & PredictCellOther)
        ptr.strcat("Othercell");
    
    if (value & PredictObjectOther)
        ptr.strcat("Otherobj");
    
    if (value & PredictFinalObject)
        ptr.strcat("Final");

    if (value & PredictArray)
        ptr.strcat("Array");
    
    if (value & PredictString)
        ptr.strcat("String");
    
    if (value & PredictInt32)
        ptr.strcat("Int");
    
    if (value & PredictDouble)
        ptr.strcat("Double");
    
    if (value & PredictBoolean)
        ptr.strcat("Bool");
    
    if (value & PredictOther)
        ptr.strcat("Other");
    
    *ptr++ = 0;
    
    return description;
}
#endif

PredictedType predictionFromClassInfo(const ClassInfo* classInfo)
{
    if (classInfo == &JSFinalObject::s_info)
        return PredictFinalObject;
    
    if (classInfo == &JSArray::s_info)
        return PredictArray;
    
    if (classInfo == &JSString::s_info)
        return PredictString;
    
    if (classInfo->isSubClassOf(&JSObject::s_info))
        return PredictObjectOther;
    
    return PredictCellOther;
}

PredictedType predictionFromStructure(Structure* structure)
{
    return predictionFromClassInfo(structure->classInfo());
}

PredictedType predictionFromCell(JSCell* cell)
{
    return predictionFromStructure(cell->structure());
}

PredictedType predictionFromValue(JSValue value)
{
    if (value.isInt32())
        return PredictInt32;
    if (value.isDouble())
        return PredictDouble;
    if (value.isCell())
        return predictionFromCell(value.asCell());
    if (value.isBoolean())
        return PredictBoolean;
    return PredictOther;
}

} // namespace JSC


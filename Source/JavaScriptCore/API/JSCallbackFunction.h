/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef JSCallbackFunction_h
#define JSCallbackFunction_h

#include "InternalFunction.h"
#include "JSObjectRef.h"

namespace JSC {

class JSCallbackFunction : public InternalFunction {
protected:
    JSCallbackFunction(JSGlobalObject*, JSObjectCallAsFunctionCallback);
    void finishCreation(JSGlobalData&, const Identifier& name);

public:
    typedef InternalFunction Base;

    static JSCallbackFunction* create(ExecState* exec, JSGlobalObject* globalObject, JSObjectCallAsFunctionCallback callback, const Identifier& name)
    {
        JSCallbackFunction* function = new (allocateCell<JSCallbackFunction>(*exec->heap())) JSCallbackFunction(globalObject, callback);
        function->finishCreation(exec->globalData(), name);
        return function;
    }

    static const ClassInfo s_info;
    
    // InternalFunction mish-mashes constructor and function behavior -- we should 
    // refactor the code so this override isn't necessary
    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue proto) 
    { 
        return Structure::create(globalData, globalObject, proto, TypeInfo(ObjectType, StructureFlags), &s_info); 
    }

    static JSValueRef toStringCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);
    static JSValueRef valueOfCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

private:
    virtual CallType getCallDataVirtual(CallData&);
    static CallType getCallData(JSCell*, CallData&);

    static EncodedJSValue JSC_HOST_CALL call(ExecState*);

    JSObjectCallAsFunctionCallback m_callback;
};

} // namespace JSC

#endif // JSCallbackFunction_h

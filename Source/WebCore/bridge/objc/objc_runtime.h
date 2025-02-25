/*
 * Copyright (C) 2004, 2008 Apple Inc. All rights reserved.
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

#ifndef KJS_BINDINGS_OBJC_RUNTIME_H
#define KJS_BINDINGS_OBJC_RUNTIME_H

#include "BridgeJSC.h"
#include "JSDOMBinding.h"
#include "objc_header.h"
#include <runtime/JSGlobalObject.h>
#include <wtf/RetainPtr.h>

namespace JSC {
namespace Bindings {

ClassStructPtr webScriptObjectClass();
ClassStructPtr webUndefinedClass();

class ObjcInstance;

class ObjcField : public Field {
public:
    ObjcField(IvarStructPtr);
    ObjcField(CFStringRef name);
    
    virtual JSValue valueFromInstance(ExecState*, const Instance*) const;
    virtual void setValueToInstance(ExecState*, const Instance*, JSValue) const;

private:
    IvarStructPtr _ivar;
    RetainPtr<CFStringRef> _name;
};

class ObjcMethod : public Method {
public:
    ObjcMethod() : _objcClass(0), _selector(0), _javaScriptName(0) {}
    ObjcMethod(ClassStructPtr aClass, SEL _selector);

    virtual int numParameters() const;

    NSMethodSignature *getMethodSignature() const;
    
    bool isFallbackMethod() const { return _selector == @selector(invokeUndefinedMethodFromWebScript:withArguments:); }
    void setJavaScriptName(CFStringRef n) { _javaScriptName = n; }
    CFStringRef javaScriptName() const { return _javaScriptName.get(); }
    
    SEL selector() const { return _selector; }

private:
    ClassStructPtr _objcClass;
    SEL _selector;
    RetainPtr<CFStringRef> _javaScriptName;
};

class ObjcArray : public Array {
public:
    ObjcArray(ObjectStructPtr, PassRefPtr<RootObject>);

    virtual void setValueAt(ExecState *exec, unsigned int index, JSValue aValue) const;
    virtual JSValue valueAt(ExecState *exec, unsigned int index) const;
    virtual unsigned int getLength() const;
    
    ObjectStructPtr getObjcArray() const { return _array.get(); }

    static JSValue convertObjcArrayToArray(ExecState *exec, ObjectStructPtr anObject);

private:
    RetainPtr<ObjectStructPtr> _array;
};

class ObjcFallbackObjectImp : public JSNonFinalObject {
public:
    typedef JSNonFinalObject Base;

    static ObjcFallbackObjectImp* create(ExecState* exec, JSGlobalObject* globalObject, ObjcInstance* instance, const Identifier& propertyName)
    {
        // FIXME: deprecatedGetDOMStructure uses the prototype off of the wrong global object
        Structure* domStructure = WebCore::deprecatedGetDOMStructure<ObjcFallbackObjectImp>(exec);
        ObjcFallbackObjectImp* fallbackObject = new (allocateCell<ObjcFallbackObjectImp>(*exec->heap())) ObjcFallbackObjectImp(globalObject, domStructure, instance, propertyName);
        fallbackObject->finishCreation(globalObject);
        return fallbackObject;
    }

    static const ClassInfo s_info;

    const Identifier& propertyName() const { return _item; }

    static ObjectPrototype* createPrototype(ExecState*, JSGlobalObject* globalObject)
    {
        return globalObject->objectPrototype();
    }

    static Structure* createStructure(JSGlobalData& globalData, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(globalData, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), &s_info);
    }

protected:
    void finishCreation(JSGlobalObject*);

private:
    ObjcFallbackObjectImp(JSGlobalObject*, Structure*, ObjcInstance*, const Identifier& propertyName);
    static const unsigned StructureFlags = OverridesGetOwnPropertySlot | JSObject::StructureFlags;
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual bool getOwnPropertyDescriptor(ExecState*, const Identifier&, PropertyDescriptor&);
    virtual void put(ExecState*, const Identifier& propertyName, JSValue, PutPropertySlot&);
    virtual CallType getCallDataVirtual(CallData&);
    static CallType getCallData(JSCell*, CallData&);
    virtual bool deleteProperty(ExecState*, const Identifier& propertyName);
    virtual JSValue defaultValue(ExecState*, PreferredPrimitiveType) const;

    virtual bool toBoolean(ExecState*) const;

    RefPtr<ObjcInstance> _instance;
    Identifier _item;
};

} // namespace Bindings
} // namespace JSC

#endif

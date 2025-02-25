/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InspectorPageAgent_h
#define InspectorPageAgent_h

#if ENABLE(INSPECTOR)

#include "Frame.h"
#include "InspectorFrontend.h"
#include "PlatformString.h"
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>

namespace WebCore {

class CachedResource;
class DOMWrapperWorld;
class DocumentLoader;
class Frame;
class Frontend;
class InjectedScriptManager;
class InspectorArray;
class InspectorObject;
class InspectorState;
class InstrumentingAgents;
class KURL;
class Page;
class RegularExpression;
class SharedBuffer;

typedef String ErrorString;

class InspectorPageAgent {
    WTF_MAKE_NONCOPYABLE(InspectorPageAgent);
public:

    enum ResourceType {
        DocumentResource,
        StylesheetResource,
        ImageResource,
        FontResource,
        ScriptResource,
        XHRResource,
        WebSocketResource,
        OtherResource
    };

    static PassOwnPtr<InspectorPageAgent> create(InstrumentingAgents*, Page*, InspectorState*, InjectedScriptManager*);

    static bool cachedResourceContent(CachedResource*, String* result, bool* base64Encoded);
    static bool sharedBufferContent(PassRefPtr<SharedBuffer>, const String& textEncodingName, bool withBase64Encode, String* result);
    static void resourceContent(ErrorString*, Frame*, const KURL&, String* result, bool* base64Encoded);

    static PassRefPtr<SharedBuffer> resourceData(Frame*, const KURL&, String* textEncodingName);
    static CachedResource* cachedResource(Frame*, const KURL&);
    static String resourceTypeString(ResourceType);
    static ResourceType cachedResourceType(const CachedResource&);
    static String cachedResourceTypeString(const CachedResource&);

    // Page API for InspectorFrontend
    void enable(ErrorString*);
    void disable(ErrorString*);
    void addScriptToEvaluateOnLoad(ErrorString*, const String& source);
    void removeAllScriptsToEvaluateOnLoad(ErrorString*);
    void reload(ErrorString*, const bool* const optionalIgnoreCache);
    void open(ErrorString*, const String& url, const bool* const inNewWindow);
    void getCookies(ErrorString*, RefPtr<InspectorArray>* cookies, WTF::String* cookiesString);
    void deleteCookie(ErrorString*, const String& cookieName, const String& domain);
    void getResourceTree(ErrorString*, RefPtr<InspectorObject>*);
    void getResourceContent(ErrorString*, const String& frameId, const String& url, String* content, bool* base64Encoded);
    void searchInResource(ErrorString*, const String& frameId, const String& url, const String&, RefPtr<InspectorArray>*);
    void searchInResources(ErrorString*, const String&, const bool* const caseSensitive, const bool* const isRegex, RefPtr<InspectorArray>*);

    // InspectorInstrumentation API
    void didClearWindowObjectInWorld(Frame*, DOMWrapperWorld*);
    void domContentEventFired();
    void loadEventFired();
    void frameNavigated(DocumentLoader*);
    void frameDetached(Frame*);
    void loaderDetachedFromFrame(DocumentLoader*);

    // Inspector Controller API
    void setFrontend(InspectorFrontend*);
    void clearFrontend();
    void restore();

    // Cross-agents API
    Frame* mainFrame();
    String createIdentifier();
    Frame* frameForId(const String& frameId);
    String frameId(Frame*);
    String loaderId(DocumentLoader*);

private:
    InspectorPageAgent(InstrumentingAgents*, Page*, InspectorState*, InjectedScriptManager*);

    PassRefPtr<InspectorObject> buildObjectForFrame(Frame*);
    PassRefPtr<InspectorObject> buildObjectForFrameTree(Frame*);

    InstrumentingAgents* m_instrumentingAgents;
    Page* m_page;
    InjectedScriptManager* m_injectedScriptManager;
    InspectorState* m_state;
    InspectorFrontend::Page* m_frontend;
    Vector<String> m_scriptsToEvaluateOnLoad;
    HashMap<Frame*, String> m_frameToIdentifier;
    HashMap<String, Frame*> m_identifierToFrame;
    HashMap<DocumentLoader*, String> m_loaderToIdentifier;
};


} // namespace WebCore

#endif // ENABLE(INSPECTOR)

#endif // !defined(InspectorPagerAgent_h)

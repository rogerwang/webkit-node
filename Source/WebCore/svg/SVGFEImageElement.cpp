/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2010 Dirk Schulze <krit@webkit.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#if ENABLE(SVG) && ENABLE(FILTERS)
#include "SVGFEImageElement.h"

#include "Attr.h"
#include "CachedImage.h"
#include "CachedResourceLoader.h"
#include "ColorSpace.h"
#include "Document.h"
#include "Image.h"
#include "RenderObject.h"
#include "RenderSVGResource.h"
#include "SVGElementInstance.h"
#include "SVGImageBufferTools.h"
#include "SVGNames.h"
#include "SVGPreserveAspectRatio.h"

namespace WebCore {

// Animated property definitions
DEFINE_ANIMATED_PRESERVEASPECTRATIO(SVGFEImageElement, SVGNames::preserveAspectRatioAttr, PreserveAspectRatio, preserveAspectRatio)
DEFINE_ANIMATED_STRING(SVGFEImageElement, XLinkNames::hrefAttr, Href, href)
DEFINE_ANIMATED_BOOLEAN(SVGFEImageElement, SVGNames::externalResourcesRequiredAttr, ExternalResourcesRequired, externalResourcesRequired)

BEGIN_REGISTER_ANIMATED_PROPERTIES(SVGFEImageElement)
    REGISTER_LOCAL_ANIMATED_PROPERTY(preserveAspectRatio)
    REGISTER_LOCAL_ANIMATED_PROPERTY(href)
    REGISTER_LOCAL_ANIMATED_PROPERTY(externalResourcesRequired)
    REGISTER_PARENT_ANIMATED_PROPERTIES(SVGFilterPrimitiveStandardAttributes)
END_REGISTER_ANIMATED_PROPERTIES

inline SVGFEImageElement::SVGFEImageElement(const QualifiedName& tagName, Document* document)
    : SVGFilterPrimitiveStandardAttributes(tagName, document)
{
    ASSERT(hasTagName(SVGNames::feImageTag));
    registerAnimatedPropertiesForSVGFEImageElement();
}

PassRefPtr<SVGFEImageElement> SVGFEImageElement::create(const QualifiedName& tagName, Document* document)
{
    return adoptRef(new SVGFEImageElement(tagName, document));
}

SVGFEImageElement::~SVGFEImageElement()
{
    if (m_cachedImage)
        m_cachedImage->removeClient(this);
}

void SVGFEImageElement::requestImageResource()
{
    if (m_cachedImage) {
        m_cachedImage->removeClient(this);
        m_cachedImage = 0;
    }

    Element* hrefElement = SVGURIReference::targetElementFromIRIString(href(), document());
    if (hrefElement && hrefElement->isSVGElement() && hrefElement->renderer())
        return;

    ResourceRequest request(ownerDocument()->completeURL(href()));
    m_cachedImage = ownerDocument()->cachedResourceLoader()->requestImage(request);

    if (m_cachedImage)
        m_cachedImage->addClient(this);
}

bool SVGFEImageElement::isSupportedAttribute(const QualifiedName& attrName)
{
    DEFINE_STATIC_LOCAL(HashSet<QualifiedName>, supportedAttributes, ());
    if (supportedAttributes.isEmpty()) {
        SVGURIReference::addSupportedAttributes(supportedAttributes);
        SVGLangSpace::addSupportedAttributes(supportedAttributes);
        SVGExternalResourcesRequired::addSupportedAttributes(supportedAttributes);
        supportedAttributes.add(SVGNames::preserveAspectRatioAttr);
    }
    return supportedAttributes.contains<QualifiedName, SVGAttributeHashTranslator>(attrName);
}

void SVGFEImageElement::parseMappedAttribute(Attribute* attr)
{
    if (!isSupportedAttribute(attr->name())) {
        SVGFilterPrimitiveStandardAttributes::parseMappedAttribute(attr);
        return;
    }

    const AtomicString& value = attr->value();
    if (attr->name() == SVGNames::preserveAspectRatioAttr) {
        SVGPreserveAspectRatio::parsePreserveAspectRatio(this, value);
        return;
    }

    if (SVGURIReference::parseMappedAttribute(attr)) {
        requestImageResource();
        return;
    }

    if (SVGLangSpace::parseMappedAttribute(attr))
        return;
    if (SVGExternalResourcesRequired::parseMappedAttribute(attr))
        return;

    ASSERT_NOT_REACHED();
}

void SVGFEImageElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (!isSupportedAttribute(attrName)) {
        SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
        return;
    }

    SVGElementInstance::InvalidationGuard invalidationGuard(this);
    
    if (attrName == SVGNames::preserveAspectRatioAttr) {
        invalidate();
        return;
    }

    // FIXME: This can't be correct, I'm just preserving existing code. <feImage> + SVG DOM 'href' changes need testing.
    if (SVGLangSpace::isKnownAttribute(attrName) || SVGExternalResourcesRequired::isKnownAttribute(attrName) || SVGURIReference::isKnownAttribute(attrName))
        return;

    ASSERT_NOT_REACHED();
}

void SVGFEImageElement::notifyFinished(CachedResource*)
{
    if (!inDocument())
        return;

    Element* parent = parentElement();
    ASSERT(parent);

    if (!parent->hasTagName(SVGNames::filterTag) || !parent->renderer())
        return;

    RenderSVGResource::markForLayoutAndParentResourceInvalidation(parent->renderer());
}

PassRefPtr<FilterEffect> SVGFEImageElement::build(SVGFilterBuilder*, Filter* filter)
{
    if (!m_cachedImage && !m_targetImage) {
        Element* hrefElement = SVGURIReference::targetElementFromIRIString(href(), document());
        if (!hrefElement || !hrefElement->isSVGElement())
            return 0;

        RenderObject* renderer = hrefElement->renderer();
        if (!renderer)
            return 0;

        IntRect targetRect = enclosingIntRect(renderer->objectBoundingBox());
        m_targetImage = ImageBuffer::create(targetRect.size(), ColorSpaceLinearRGB);

        AffineTransform contentTransformation;
        SVGImageBufferTools::renderSubtreeToImageBuffer(m_targetImage.get(), renderer, contentTransformation);
    }

    return FEImage::create(filter, m_targetImage ? m_targetImage->copyImage(CopyBackingStore) : m_cachedImage->image(), preserveAspectRatio());
}

void SVGFEImageElement::addSubresourceAttributeURLs(ListHashSet<KURL>& urls) const
{
    SVGFilterPrimitiveStandardAttributes::addSubresourceAttributeURLs(urls);

    addSubresourceURL(urls, document()->completeURL(href()));
}

}

#endif // ENABLE(SVG)

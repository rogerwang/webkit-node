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

#ifndef RenderFlexibleBox_h
#define RenderFlexibleBox_h

#include "RenderBlock.h"

#if ENABLE(CSS3_FLEXBOX)

namespace WebCore {

class RenderFlexibleBox : public RenderBlock {
public:
    RenderFlexibleBox(Node*);
    virtual ~RenderFlexibleBox();

    virtual const char* renderName() const;

    virtual bool isFlexibleBox() const { return true; }

    virtual void layoutBlock(bool relayoutChildren, int pageLogicalHeight = 0, BlockLayoutPass = NormalLayoutPass);

private:
    class TreeOrderIterator;
    class FlexOrderIterator;
    typedef WTF::HashMap<const RenderBox*, LayoutUnit> InflexibleFlexItemSize;

    bool hasOrthogonalFlow(RenderBox* child) const;
    bool isHorizontalFlow() const;
    bool isLeftToRightFlow() const;
    void setFlowAwareLogicalHeight(LayoutUnit);
    LayoutUnit flowAwareLogicalHeightForChild(RenderBox* child);
    LayoutUnit flowAwareLogicalWidthForChild(RenderBox* child);
    LayoutUnit flowAwareLogicalHeight() const;
    LayoutUnit flowAwareLogicalWidth() const;
    LayoutUnit flowAwareContentLogicalWidth() const;
    LayoutUnit flowAwareBorderStart() const;
    LayoutUnit flowAwareBorderBefore() const;
    LayoutUnit flowAwareBorderAfter() const;
    LayoutUnit flowAwarePaddingStart() const;
    LayoutUnit flowAwarePaddingBefore() const;
    LayoutUnit flowAwarePaddingAfter() const;
    LayoutUnit flowAwareMarginStartForChild(RenderBox* child) const;
    LayoutUnit flowAwareMarginBeforeForChild(RenderBox* child) const;
    LayoutUnit flowAwareMarginAfterForChild(RenderBox* child) const;
    void setFlowAwareMarginStartForChild(RenderBox* child, LayoutUnit);
    void setFlowAwareMarginEndForChild(RenderBox* child, LayoutUnit);
    // FIXME: Supporting layout deltas.
    void setFlowAwareLogicalLocationForChild(RenderBox* child, const LayoutPoint&);
    LayoutUnit logicalBorderAndPaddingWidthForChild(RenderBox* child) const;
    LayoutUnit logicalScrollbarHeightForChild(RenderBox* child) const;
    Length marginStartStyleForChild(RenderBox* child) const;
    Length marginEndStyleForChild(RenderBox* child) const;
    LayoutUnit preferredLogicalContentWidthForFlexItem(RenderBox* child) const;

    void layoutInlineDirection(bool relayoutChildren);

    float logicalPositiveFlexForChild(RenderBox* child) const;
    float logicalNegativeFlexForChild(RenderBox* child) const;

    void computePreferredLogicalWidth(bool relayoutChildren, TreeOrderIterator&, LayoutUnit&, float& totalPositiveFlexibility, float& totalNegativeFlexibility);
    bool runFreeSpaceAllocationAlgorithmInlineDirection(FlexOrderIterator&, LayoutUnit& availableFreeSpace, float& totalPositiveFlexibility, float& totalNegativeFlexibility, InflexibleFlexItemSize&, WTF::Vector<LayoutUnit>& childSizes);
    void setLogicalOverrideSize(RenderBox* child, LayoutUnit childPreferredSize);
    void layoutAndPlaceChildrenInlineDirection(FlexOrderIterator&, const WTF::Vector<LayoutUnit>& childSizes, LayoutUnit availableFreeSpace, float totalPositiveFlexibility);
};

} // namespace WebCore

#endif // ENABLE(CSS3_FLEXBOX)

#endif // RenderFlexibleBox_h

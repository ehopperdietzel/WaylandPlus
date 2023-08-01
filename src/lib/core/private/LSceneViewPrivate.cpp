#include <private/LSceneViewPrivate.h>
#include <private/LViewPrivate.h>
#include <LOutput.h>
#include <LCompositor.h>
#include <LPainter.h>
#include <LSurfaceView.h>
#include <LLog.h>
#include <LFramebuffer.h>

void LSceneView::LSceneViewPrivate::clearTmpVariables(ThreadData *oD)
{
    oD->newDamage.clear();
    oD->opaqueTransposedSum.clear();
    oD->foundRenderableSaledView = false;
}

void LSceneView::LSceneViewPrivate::damageAll(ThreadData *oD)
{
    oD->newDamage.clear();
    oD->newDamage.addRect(fb->rect());
    oD->newDamage.addRect(fb->rect());
}

void LSceneView::LSceneViewPrivate::checkRectChange(ThreadData *oD)
{
    if (oD->prevRect.size() != fb->rect().size())
    {
        damageAll(oD);
        oD->prevRect.setSize(fb->rect().size());
    }
}

void LSceneView::LSceneViewPrivate::cachePass(LView *view, ThreadData *oD)
{
    if (view->type() != Scene)
        for (list<LView*>::const_iterator it = view->children().cbegin(); it != view->children().cend(); it++)
            cachePass(*it, oD);

    // Quick view cache handle to reduce verbosity
    LView::LViewPrivate::ViewCache *cache = &view->imp()->cache;

    view->imp()->repaintCalled = false;

    // Quick output data handle
    cache->voD = &view->imp()->threadsMap[std::this_thread::get_id()];
    cache->voD->o = oD->o;

    // Cache mapped call
    cache->mapped = view->mapped();

    // Cache view rect
    cache->rect.setPos(view->pos());
    cache->rect.setSize(view->size());

    cache->scalingVector = view->scalingVector();
    cache->scalingEnabled = (view->scalingEnabled() || view->parentScalingEnabled()) && cache->scalingVector != LSizeF(1.f, 1.f);

    if (cache->mapped && cache->scalingEnabled)
        oD->foundRenderableSaledView = true;
}


void LSceneView::LSceneViewPrivate::calcNewDamage(LView *view, ThreadData *oD)
{
    // Children first
    if (view->type() == Scene)
    {
        LSceneView *sceneView = (LSceneView*)view;
        if (view->imp()->cache.scalingEnabled)
            sceneView->render(nullptr);
        else
            sceneView->render(&oD->opaqueTransposedSum);
    }
    else
    {
        for (list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
            calcNewDamage(*it, oD);
    }

    // Quick view cache handle to reduce verbosity
    LView::LViewPrivate::ViewCache *cache = &view->imp()->cache;

    // Update view intersected outputs
    for (list<LOutput*>::const_iterator it = compositor()->outputs().cbegin(); it != compositor()->outputs().cend(); it++)
    {
        if ((*it)->rect().intersects(cache->rect, false))
            view->enteredOutput(*it);
        else
           view->leftOutput(*it);
    }

    if (!view->isRenderable())
        return;

    cache->opacity = view->opacity();

    if (cache->rect.size().area() == 0 || cache->opacity <= 0.f || cache->scalingVector.w() == 0.f || cache->scalingVector.y() == 0.f)
        cache->mapped = false;

    bool mappingChanged = cache->mapped != cache->voD->prevMapped;

    if (oD->o && !mappingChanged && !cache->mapped)
    {
        if (view->forceRequestNextFrameEnabled())
            view->requestNextFrame(oD->o);
        return;
    }

    bool opacityChanged = cache->opacity != cache->voD->prevOpacity;

    cache->localRect = LRect(cache->rect.pos() - fb->rect().pos(), cache->rect.size());

    bool rectChanged = cache->localRect != cache->voD->prevLocalRect;

    // If rect or order changed (set current rect and prev rect as damage)
    if (mappingChanged || rectChanged || cache->voD->changedOrder || opacityChanged || cache->scalingEnabled)
    {
        cache->damage.addRect(cache->rect);
        cache->voD->changedOrder = false;
        cache->voD->prevMapped = cache->mapped;
        cache->voD->prevRect = cache->rect;
        cache->voD->prevOpacity = cache->opacity;
        cache->voD->prevLocalRect = cache->localRect;

        if (!cache->mapped)
        {
            oD->newDamage.addRegion(cache->voD->prevParentClipping);
            return;
        }
    }
    else if (view->damage())
    {
        cache->damage = *view->damage();

        // Scene views already have their damage transposed
        if (view->type() != Scene)
            cache->damage.offset(cache->rect.pos());
    }
    else
    {
        cache->damage.clear();
    }

    // Calculates the current rect intersected with parents rects (when clipping enabled)

    LRegion currentParentClipping;
    currentParentClipping.addRect(cache->rect);

    if (view->parentClippingEnabled())
        parentClipping(view->parent(), &currentParentClipping);

    // Calculates the new exposed view region if parent clipping has grown
    LRegion newExposedParentClipping = currentParentClipping;
    newExposedParentClipping.subtractRegion(cache->voD->prevParentClipping);
    cache->damage.addRegion(newExposedParentClipping);

    // Add exposed now non clipped region to new output damage
    cache->voD->prevParentClipping.subtractRegion(currentParentClipping);
    oD->newDamage.addRegion(cache->voD->prevParentClipping);

    // Saves current clipped region for next frame
    cache->voD->prevParentClipping = currentParentClipping;

    // Clip current damage to current visible region
    cache->damage.intersectRegion(currentParentClipping);

    // Remove previus opaque region to view damage
    cache->damage.subtractRegion(oD->opaqueTransposedSum);

    // Add clipped damage to new damage
    oD->newDamage.addRegion(cache->damage);

    if (cache->opacity < 1.f || cache->scalingEnabled)
    {
        cache->translucent.clear();
        cache->translucent.addRect(cache->rect);
        cache->opaque.clear();
    }
    else
    {
        // Store tansposed traslucent region
        if (view->translucentRegion())
        {
            cache->translucent = *view->translucentRegion();

            if (view->type() != Scene)
                cache->translucent.offset(cache->rect.pos());
        }
        else
        {
            cache->translucent.clear();
            cache->translucent.addRect(cache->rect);
        }

        // Store tansposed opaque region
        if (view->opaqueRegion())
        {
            cache->opaque = *view->opaqueRegion();

            if (view->type() != Scene)
                cache->opaque.offset(cache->rect.pos());
        }
        else
        {
            cache->opaque = cache->translucent;
            cache->opaque.inverse(cache->rect);
        }
    }

    // Clip opaque and translucent regions to current visible region
    cache->opaque.intersectRegion(currentParentClipping);
    cache->translucent.intersectRegion(currentParentClipping);

    // Check if view is ocludded
    currentParentClipping.subtractRegion(oD->opaqueTransposedSum);

    cache->occluded = currentParentClipping.empty();

    if (oD->o && (!cache->occluded || view->forceRequestNextFrameEnabled()))
        view->requestNextFrame(oD->o);

    // Store sum of previus opaque regions (this will later be clipped when painting opaque and translucent regions)
    cache->opaqueOverlay = oD->opaqueTransposedSum;
    oD->opaqueTransposedSum.addRegion(cache->opaque);
}

void LSceneView::LSceneViewPrivate::drawOpaqueDamage(LView *view, ThreadData *oD)
{
    // Children first
    if (view->type() != Scene)
        for (list<LView*>::const_reverse_iterator it = view->children().crbegin(); it != view->children().crend(); it++)
            drawOpaqueDamage(*it, oD);

    LView::LViewPrivate::ViewCache *cache = &view->imp()->cache;

    if (!view->isRenderable() || !cache->mapped || cache->occluded || cache->opacity < 1.f)
        return;

    cache->opaque.intersectRegion(oD->newDamage);
    cache->opaque.subtractRegion(cache->opaqueOverlay);

    oD->boxes = cache->opaque.rects(&oD->n);

    if (cache->scalingEnabled)
    {
        for (Int32 i = 0; i < oD->n; i++)
        {
            oD->w = oD->boxes->x2 - oD->boxes->x1;
            oD->h = oD->boxes->y2 - oD->boxes->y1;

            view->paintRect(
                oD->p,
                (oD->boxes->x1 - cache->rect.x()) / cache->scalingVector.x(),
                (oD->boxes->y1  - cache->rect.y()) / cache->scalingVector.y(),
                oD->w / cache->scalingVector.x(),
                oD->h / cache->scalingVector.y(),
                oD->boxes->x1,
                oD->boxes->y1,
                oD->w,
                oD->h,
                view->bufferScale(),
                1.f);

            oD->boxes++;
        }
    }
    else
    {
        for (Int32 i = 0; i < oD->n; i++)
        {
            oD->w = oD->boxes->x2 - oD->boxes->x1;
            oD->h = oD->boxes->y2 - oD->boxes->y1;

            view->paintRect(
                oD->p,
                oD->boxes->x1 - cache->rect.x(),
                oD->boxes->y1 - cache->rect.y(),
                oD->w,
                oD->h,
                oD->boxes->x1,
                oD->boxes->y1,
                oD->w,
                oD->h,
                view->bufferScale(),
                1.f);

            oD->boxes++;
        }
    }
}

void LSceneView::LSceneViewPrivate::drawBackground(ThreadData *oD, bool addToOpaqueSum)
{
    LRegion backgroundDamage = oD->newDamage;
    backgroundDamage.subtractRegion(oD->opaqueTransposedSum);
    oD->boxes = backgroundDamage.rects(&oD->n);

    for (Int32 i = 0; i < oD->n; i++)
    {
        oD->p->drawColor(oD->boxes->x1,
                      oD->boxes->y1,
                      oD->boxes->x2 - oD->boxes->x1,
                      oD->boxes->y2 - oD->boxes->y1,
                      clearColor.r,
                      clearColor.g,
                      clearColor.b,
                      clearColor.a);
        oD->boxes++;
    }

    if (addToOpaqueSum)
        oD->opaqueTransposedSum.addRegion(backgroundDamage);
}

void LSceneView::LSceneViewPrivate::drawTranslucentDamage(LView *view, ThreadData *oD)
{
    LView::LViewPrivate::ViewCache *cache = &view->imp()->cache;

    if (!view->isRenderable() || !cache->mapped || cache->occluded)
        goto drawChildrenOnly;

    glBlendFunc(view->imp()->sFactor, view->imp()->dFactor);

    cache->occluded = true;
    cache->translucent.intersectRegion(oD->newDamage);
    cache->translucent.subtractRegion(cache->opaqueOverlay);

    oD->boxes = cache->translucent.rects(&oD->n);

    if (cache->scalingEnabled)
    {
        for (Int32 i = 0; i < oD->n; i++)
        {
            oD->w = oD->boxes->x2 - oD->boxes->x1;
            oD->h = oD->boxes->y2 - oD->boxes->y1;

            view->paintRect(
                oD->p,
                (oD->boxes->x1 - cache->rect.x()) / cache->scalingVector.x(),
                (oD->boxes->y1  - cache->rect.y()) / cache->scalingVector.y(),
                oD->w / cache->scalingVector.x(),
                oD->h / cache->scalingVector.y(),
                oD->boxes->x1,
                oD->boxes->y1,
                oD->w,
                oD->h,
                view->bufferScale(),
                cache->opacity);

            oD->boxes++;
        }
    }
    else
    {
        for (Int32 i = 0; i < oD->n; i++)
        {
            oD->w = oD->boxes->x2 - oD->boxes->x1;
            oD->h = oD->boxes->y2 - oD->boxes->y1;

            view->paintRect(
                oD->p,
                oD->boxes->x1 - cache->rect.x(),
                oD->boxes->y1 - cache->rect.y(),
                oD->w,
                oD->h,
                oD->boxes->x1,
                oD->boxes->y1,
                oD->w,
                oD->h,
                view->bufferScale(),
                cache->opacity);

            oD->boxes++;
        }
    }

    drawChildrenOnly:
    if (view->type() != Scene)
        for (list<LView*>::const_iterator it = view->children().cbegin(); it != view->children().cend(); it++)
            drawTranslucentDamage(*it, oD);
}

void LSceneView::LSceneViewPrivate::parentClipping(LView *parent, LRegion *region)
{
    if (!parent)
        return;

    region->clip(LRect(parent->pos(), parent->size()));

    if (parent->parentClippingEnabled())
        parentClipping(parent->parent(), region);
}
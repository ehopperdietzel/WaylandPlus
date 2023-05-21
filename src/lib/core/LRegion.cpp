#include "LRegion.h"
#include <stdio.h>
#include <pixman.h>

using namespace Louvre;

LRegion::LRegion()
{
    m_region = malloc(sizeof(pixman_region32_t));
    pixman_region32_init((pixman_region32_t*)m_region);
}

LRegion::~LRegion()
{
    pixman_region32_fini((pixman_region32_t*)m_region);
    free((pixman_region32_t*)m_region);
}

LRegion::LRegion(const LRegion &other)
{
    copy(other);
}

Louvre::LRegion &LRegion::operator=(const LRegion &other)
{
    copy(other);
    return *this;
}

void LRegion::clear()
{
    m_changed = true;
    pixman_region32_clear((pixman_region32_t*)m_region);
}

void LRegion::addRect(const LRect &rect)
{
    pixman_region32_union_rect((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, rect.x(), rect.y(), rect.w(), rect.h());
    m_changed = true;
}

void LRegion::addRegion(const LRegion &region)
{
    m_changed = true;
    pixman_region32_union((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, (pixman_region32_t*)region.m_region);
}

void LRegion::subtractRect(const LRect &rect)
{
    pixman_region32_t tmp;

    pixman_region32_init_rect(&tmp, rect.x(), rect.y(), rect.w(), rect.h());

    pixman_region32_subtract((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, &tmp);

    pixman_region32_fini(&tmp);
    m_changed = true;
}

void LRegion::subtractRegion(const LRegion &region)
{
    m_changed = true;
    pixman_region32_subtract((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, (pixman_region32_t*)region.m_region);
}

void LRegion::intersectRegion(const LRegion &region)
{
    m_changed = true;
    pixman_region32_intersect((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, (pixman_region32_t*)region.m_region);
}

void LRegion::multiply(Float32 factor)
{
    if(factor == 1.f)
        return;

    pixman_region32_t tmp;
    pixman_region32_init(&tmp);

    int n;
    pixman_box32_t *rects = pixman_region32_rectangles((pixman_region32_t*)m_region, &n);

    if(factor == 2.f)
    {
        for(int i = 0; i < n; i++)
        {
            pixman_region32_union_rect(
                        &tmp,
                        &tmp,
                        rects[i].x1 << 1,
                        rects[i].y1 << 1,
                        (rects[i].x2 - rects[i].x1) << 1,
                        (rects[i].y2 - rects[i].y1) << 1);
        }
    }
    else
    {
        for(int i = 0; i < n; i++)
        {
            int x = rects[i].x1 * factor;
            int y = rects[i].y1 * factor;
            int w = (rects[i].x2 - rects[i].x1) * factor;
            int h = (rects[i].y2 - rects[i].y1) * factor;
            pixman_region32_union_rect(
                        &tmp,
                        &tmp,
                        x,
                        y,
                        w,
                        h);
        }
    }

    pixman_region32_fini((pixman_region32_t*)m_region);
    *(pixman_region32_t*)m_region = tmp;
    m_changed = true;
}

bool LRegion::containsPoint(const LPoint &point) const
{
    return pixman_region32_contains_point((pixman_region32_t*)m_region, point.x(), point.y(), NULL);
}

void LRegion::offset(const LPoint &offset)
{
    pixman_region32_translate((pixman_region32_t*)m_region, offset.x(), offset.y());
    m_changed = true;
}

void LRegion::copy(const LRegion &regionToCopy)
{
    m_changed = true;
    m_rects.clear();

    if(!m_region)
    {
        m_region = malloc(sizeof(pixman_region32_t));
        pixman_region32_init((pixman_region32_t*)m_region);
    }

    pixman_region32_copy((pixman_region32_t*)m_region, (pixman_region32_t*)regionToCopy.m_region);

}

void LRegion::inverse(const LRect &rect)
{
    pixman_box32_t r;
    r.x1 = rect.x();
    r.x2 = r.x1 + rect.w();
    r.y1 = rect.y();
    r.y2 = r.y1 + rect.h();

    pixman_region32_inverse((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, &r);
}

bool LRegion::empty() const
{
    return !pixman_region32_not_empty((pixman_region32_t*)m_region);
}

void LRegion::clip(const LRect &rect)
{
    pixman_region32_intersect_rect((pixman_region32_t*)m_region, (pixman_region32_t*)m_region, rect.x(), rect.y(), rect.w(), rect.h());
    m_changed = true;
}

const vector<LRect> &LRegion::rects() const
{
    if(m_changed)
    {
        m_changed = false;
        int n;
        pixman_box32_t *rects = pixman_region32_rectangles((pixman_region32_t*)m_region, &n);
        m_rects.resize(n);

        for(int i = 0; i < n; i++)
        {
            m_rects[i] = (LRect(rects[i].x1,
                                   rects[i].y1,
                                   rects[i].x2 - rects[i].x1,
                                   rects[i].y2 - rects[i].y1));
        }
    }
    return m_rects;
}
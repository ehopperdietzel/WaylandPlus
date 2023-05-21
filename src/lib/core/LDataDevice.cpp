#include <private/LDataDevicePrivate.h>
#include <private/LCompositorPrivate.h>
#include <private/LDataOfferPrivate.h>
#include <private/LDataSourcePrivate.h>
#include <private/LSeatPrivate.h>
#include <private/LDNDManagerPrivate.h>

#include <protocols/Wayland/DataOfferResource.h>
#include <protocols/Wayland/SeatGlobal.h>
#include <protocols/Wayland/private/DataDeviceResourcePrivate.h>


#include <LWayland.h>
#include <LTime.h>

using namespace Louvre;


LDataDevice::LDataDevice()
{
    m_imp = new LDataDevicePrivate();
}

LDataDevice::~LDataDevice()
{
    delete m_imp;
}

LClient *LDataDevice::client() const
{
    return m_imp->client;
}

LSeat *LDataDevice::seat() const
{
    return client()->seat();
}

void LDataDevice::sendSelectionEvent()
{
    // Send data device selection first
    if(seat()->dataSelection())
    {
        for(Protocols::Wayland::SeatGlobal *d : client()->seatGlobals())
        {
            if(d->dataDeviceResource())
            {
                Protocols::Wayland::DataOfferResource *lDataOfferResource = new Protocols::Wayland::DataOfferResource(d->dataDeviceResource(), 0);

                lDataOfferResource->dataOffer()->imp()->usedFor = LDataOffer::Selection;
                d->dataDeviceResource()->sendDataOffer(lDataOfferResource);

                for(const LDataSource::LSource &s : seat()->dataSelection()->sources())
                    lDataOfferResource->sendOffer(s.mimeType);

                d->dataDeviceResource()->sendSelection(lDataOfferResource);

            }
        }
    }
}

void LDataDevice::LDataDevicePrivate::sendDNDEnterEvent(LSurface *surface, Float64 x, Float64 y)
{
    LSeat *seat = client->seat();

    if(seat->dndManager()->dragging() && seat->dndManager()->focus() != surface)
    {
        sendDNDLeaveEvent();

        if(seat->dndManager()->source())
        {
            for(Protocols::Wayland::SeatGlobal *d : client->seatGlobals())
            {
                if(d->dataDeviceResource())
                {
                    Protocols::Wayland::DataOfferResource *lDataOfferResource = new Protocols::Wayland::DataOfferResource(d->dataDeviceResource(), 0);

                    lDataOfferResource->dataOffer()->imp()->usedFor = LDataOffer::DND;
                    d->dataDeviceResource()->imp()->dataOffered = lDataOfferResource->dataOffer();
                    d->dataDeviceResource()->sendDataOffer(lDataOfferResource);

                    for(const LDataSource::LSource &s : seat->dndManager()->source()->sources())
                        lDataOfferResource->sendOffer(s.mimeType);

                    d->dataDeviceResource()->sendEnter(surface,
                                                       x/seat->compositor()->globalScale(),
                                                       y/seat->compositor()->globalScale(),
                                                       lDataOfferResource);

                    lDataOfferResource->sendSourceActions(seat->dndManager()->source()->dndActions());


                }
            }

            seat->dndManager()->imp()->focus = surface;
        }
        else
        {
            if(surface == seat->dndManager()->origin())
            {

                for(Protocols::Wayland::SeatGlobal *d : client->seatGlobals())
                {
                    if(d->dataDeviceResource())
                    {
                        d->dataDeviceResource()->sendEnter(surface,
                                                           x/seat->compositor()->globalScale(),
                                                           y/seat->compositor()->globalScale(),
                                                           NULL);
                    }
                }

                seat->dndManager()->imp()->focus = surface;
            }
        }
    }
}

void LDataDevice::LDataDevicePrivate::sendDNDMotionEvent(Float64 x, Float64 y)
{
    LSeat *seat = client->seat();

    if(seat->dndManager()->dragging() && seat->dndManager()->focus())
    {
        if(seat->dndManager()->source() || (!seat->dndManager()->source() && seat->dndManager()->focus() == seat->dndManager()->origin()))
        {
            for(Protocols::Wayland::SeatGlobal *d : client->seatGlobals())
                if(d->dataDeviceResource())
                    d->dataDeviceResource()->sendMotion(x/seat->compositor()->globalScale(), y/seat->compositor()->globalScale());
        }
    }
}

void LDataDevice::LDataDevicePrivate::sendDNDLeaveEvent()
{
    LSeat *seat = client->seat();

    if(seat->dndManager()->dragging() && seat->dndManager()->focus())
        for(Protocols::Wayland::SeatGlobal *d : client->seatGlobals())
            if(d->dataDeviceResource())
                d->dataDeviceResource()->sendLeave();

    seat->dndManager()->imp()->matchedMimeType = false;
    seat->dndManager()->imp()->focus = nullptr;
}

LDataDevice::LDataDevicePrivate *LDataDevice::imp() const
{
    return m_imp;
}

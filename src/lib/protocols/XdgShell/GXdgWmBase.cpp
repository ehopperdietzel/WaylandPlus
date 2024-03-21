#include <protocols/XdgShell/RXdgPositioner.h>
#include <protocols/XdgShell/GXdgWmBase.h>
#include <protocols/XdgShell/RXdgSurface.h>
#include <private/LClientPrivate.h>
#include <private/LSurfacePrivate.h>

using namespace Louvre::Protocols::XdgShell;

static const struct xdg_wm_base_interface imp
{
    .destroy = &GXdgWmBase::destroy,
    .create_positioner = &GXdgWmBase::create_positioner,
    .get_xdg_surface = &GXdgWmBase::get_xdg_surface,
    .pong = &GXdgWmBase::pong
};

XdgShell::GXdgWmBase::GXdgWmBase
(
    wl_client *client,
    Int32 version,
    UInt32 id
) noexcept
    :LResource
    (
        client,
        &xdg_wm_base_interface,
        version,
        id,
        &imp
    )
{
    this->client()->imp()->xdgWmBaseGlobals.push_back(this);
}

GXdgWmBase::~GXdgWmBase() noexcept
{
    LVectorRemoveOneUnordered(client()->imp()->xdgWmBaseGlobals, this);
}

/******************** REQUESTS ********************/

void GXdgWmBase::bind(wl_client *client, void */*data*/, UInt32 version, UInt32 id) noexcept
{
    new GXdgWmBase(client, version, id);
}

void GXdgWmBase::destroy(wl_client */*client*/, wl_resource *resource) noexcept
{
    auto &res { *static_cast<GXdgWmBase*>(wl_resource_get_user_data(resource)) };

    if (res.m_xdgSurfacesCount != 0)
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_DEFUNCT_SURFACES, "xdg_wm_base was destroyed before children.");
        return;
    }

    wl_resource_destroy(resource);
}

void GXdgWmBase::create_positioner(wl_client */*client*/, wl_resource *resource, UInt32 id) noexcept
{
    new RXdgPositioner(static_cast<GXdgWmBase*>(wl_resource_get_user_data(resource)), id);
}

void GXdgWmBase::get_xdg_surface(wl_client */*client*/, wl_resource *resource, UInt32 id, wl_resource *surface) noexcept
{
    auto &surfaceRes { *static_cast<Wayland::RSurface*>(wl_resource_get_user_data(surface)) };

    if (surfaceRes.surface()->imp()->hasRoleOrPendingRole())
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_ROLE, "Given wl_surface has another role.");
        return;
    }

    new RXdgSurface(static_cast<GXdgWmBase*>(wl_resource_get_user_data(resource)), surfaceRes.surface(), id);
}

void GXdgWmBase::pong(wl_client */*client*/, wl_resource *resource, UInt32 serial)
{
    static_cast<GXdgWmBase*>(wl_resource_get_user_data(resource))->client()->pong(serial);
}

/******************** EVENTS ********************/

void XdgShell::GXdgWmBase::ping(UInt32 serial) noexcept
{
    xdg_wm_base_send_ping(resource(), serial);
}

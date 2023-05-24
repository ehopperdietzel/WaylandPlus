#include <protocols/Wayland/RKeyboard.h>
#include <LSeat.h>
#include <private/LClientPrivate.h>
#include <protocols/Wayland/RPointer.h>
#include <protocols/Wayland/private/GSeatPrivate.h>

static struct wl_seat_interface seat_implementation =
{
    .get_pointer = &GSeat::GSeatPrivate::get_pointer,
    .get_keyboard = &GSeat::GSeatPrivate::get_keyboard,
    .get_touch = &GSeat::GSeatPrivate::get_touch,
#if LOUVRE_SEAT_VERSION >= WL_SEAT_RELEASE_SINCE_VERSION
    .release = &GSeat::GSeatPrivate::release
#endif
};

void GSeat::GSeatPrivate::bind(wl_client *client, void *compositor, UInt32 version, UInt32 id)
{
    new GSeat((LCompositor*)compositor,
                            client,
                            &wl_seat_interface,
                            version,
                            id,
                            &seat_implementation,
                            &GSeat::GSeatPrivate::resource_destroy);
}

void GSeat::GSeatPrivate::resource_destroy(wl_resource *resource)
{
    GSeat *seatResource = (GSeat*)wl_resource_get_user_data(resource);
    delete seatResource;
}

void GSeat::GSeatPrivate::get_pointer(wl_client *client, wl_resource *resource, UInt32 id)
{
    L_UNUSED(client);

    GSeat *seatGlobal = (GSeat*)wl_resource_get_user_data(resource);

    if (!(seatGlobal->client()->seat()->capabilities() & LSeat::Pointer))
    {
        wl_resource_post_error(resource,WL_SEAT_ERROR_MISSING_CAPABILITY,"get_pointer called on seat without the matching capability.");
        return;
    }

    new RPointer(seatGlobal, id);
}

void GSeat::GSeatPrivate::get_keyboard(wl_client *client, wl_resource *resource, UInt32 id)
{
    L_UNUSED(client);

    GSeat *seatResource = (GSeat*)wl_resource_get_user_data(resource);

    if (!(seatResource->client()->seat()->capabilities() & LSeat::Keyboard))
    {
        wl_resource_post_error(resource, WL_SEAT_ERROR_MISSING_CAPABILITY, "get_keyboard called on seat without the matching capability.");
        return;
    }

    new RKeyboard(seatResource, id);
}

void GSeat::GSeatPrivate::get_touch(wl_client *client, wl_resource *resource, UInt32 id)
{
    L_UNUSED(client);
    L_UNUSED(id);

    LClient *lClient = (LClient*)wl_resource_get_user_data(resource);

    if (!(lClient->seat()->capabilities() & LSeat::Touch))
    {
        wl_resource_post_error(resource, WL_SEAT_ERROR_MISSING_CAPABILITY, "get_touch called on seat without the matching capability.");
        return;
    }
}

#if LOUVRE_SEAT_VERSION >= WL_SEAT_RELEASE_SINCE_VERSION
void GSeat::GSeatPrivate::release(wl_client *client, wl_resource *resource)
{
    L_UNUSED(client);
    wl_resource_destroy(resource);
}
#endif
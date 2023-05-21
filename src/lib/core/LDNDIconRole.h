#ifndef LDNDICONROLE_H
#define LDNDICONROLE_H

#include <LBaseSurfaceRole.h>

/*!
 * @brief Drag & drop icon role for surfaces
 *
 * The LDNDIconRole class is a role for surfaces that allows the compositor to use them as icons for drag & drop sessions.\n
 * Clients create the role through the **start_drag** request from the Wayland **wl_data_device** protocol interface.\n
 * The LDNDIconRole role used in a drag & drop session can be accessed from LDNDManager::icon().\n
 * See the default implementation and documentation of LPointer::pointerPosChangeEvent() to see how to assign its position and LOutput::paintGL()
 * to see how it is rendered on screen.
 *
 * <center><IMG SRC="https://lh3.googleusercontent.com/evKJ2MbTJ42-qFYSP02NPxUULSFpTz3oBSqn6RvR20u_r5wvgJpHF6o-3Zg7aWgNBhrkIsM8iNWiQQHxPjvGml9zDB2wwNwWK0scqTsHpLIbxMqYv60afSruzbWNBCDZaGI_y77eRA=w2400"></center>
 */

class Louvre::LDNDIconRole : public LBaseSurfaceRole
{
public:

    struct Params;

    /*!
     * @brief Constructor of the LDNDIconRole class.
     *
     * @param params Internal library parameters passed in the LCompositor::createDNDIconRoleRequest() virtual constructor.
     */
    LDNDIconRole(Params *params);

    /*!
     * @brief Destructor of the LDNDIconRole class.
     *
     * Invoked internally by the library after LCompositor::destroyDNDIconRoleRequest() is called.
     */
    virtual ~LDNDIconRole();

    LDNDIconRole(const LDNDIconRole&) = delete;
    LDNDIconRole& operator= (const LDNDIconRole&) = delete;

    /*!
     * @brief Notify a hotspot change.
     *
     * Reimplement this virtual method if you want to be notified when the icon hotspot changes.
     * #### Default implementation
     * @snippet LDNDIconRoleDefault.cpp hotspotChanged
     */
    virtual void hotspotChanged() const;

    /*!
     * @brief Position of the surface given the role.
     *
     * The position of the icon given the role is calculated by subtracting the hotspot from the surface position.\n
     *
     * This method can be reimplemented to change the positioning logic of the surface given the role.
     * #### Default implementation
     * @snippet LDNDIconRoleDefault.cpp rolePosC
     */
    virtual const LPoint &rolePosC() const override;

    /*!
     * @brief Hotspot of the drag & drop icon in surface coordinates.
     */
    const LPoint &hotspotS() const;

    /*!
     * @brief Hotspot of the drag & drop icon in compositor coordinates.
     */
    const LPoint &hotspotC() const;

    /*!
     * @brief Hotspot of the drag & drop icon in buffer coordinates.
     */
    const LPoint &hotspotB() const;


    class LDNDIconRolePrivate;

    /*!
     * @brief Access to the private API of LDNDIconRole.
     *
     * Returns an instance of the LDNDIconRolePrivate class (following the ***PImpl Idiom*** pattern) which contains all the private members of LDNDIconRole.\n
     * Used internally by the library.
     */
    LDNDIconRolePrivate *imp() const;

private:
    LDNDIconRolePrivate *m_imp = nullptr;
    virtual void handleSurfaceOffset(Int32 x, Int32 y) override;
    virtual void handleSurfaceCommit() override;
    void globalScaleChanged(Int32 oldScale, Int32 newScale) override;
};

#endif // LDNDICONROLE_H
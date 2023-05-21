#ifndef LCURSORROLE_H
#define LCURSORROLE_H

#include <LBaseSurfaceRole.h>

/*!
 * @brief Cursor role for surfaces
 *
 * The LCursorRole class is a role for surfaces that allows the compositor to use them as cursors.\n
 * Clients create the role by requesting **set_cursor** from the **wl_pointer** interface of the Wayland protocol.\n
 * The library automatically invokes the LPointer::setCursorRequest() method when a client with pointer focus wants to assign an LCursorRole as a cursor.\n
 * The LPointer::setCursorRequest() method is also invoked when the role hotspot changes, so it is not necessary to reimplement this class to handle these changes.\n
 * For more information see the default implementation and documentation of LPointer::setCursorRequest().
 */
class Louvre::LCursorRole : public LBaseSurfaceRole
{
public:
    struct Params;

    /*!
     * @brief Constructor of the LCursorRole class.
     */
    LCursorRole(Params *params);

    /*!
     * @brief Destructor of the LCursorRole class.
     */
    virtual ~LCursorRole();

    LCursorRole(const LCursorRole&) = delete;
    LCursorRole& operator= (const LCursorRole&) = delete;

    /*!
     * @brief Position of the surface given the role.
     *
     * The cursor position given the role is calculated by subtracting the hotspot from the surface position.\n
     * This position is generally not used since the cursor is usually rendered using the LCursor class.\n
     * However, it could be useful in cases where you do not want to use the LCursor class.
     *
     * This method can be reimplemented to change the positioning logic of the surface given the role.
     * #### Default implementation
     * @snippet LCursorRoleDefault.cpp rolePosC
     */
    virtual const LPoint &rolePosC() const override;

    /*!
     * @brief Notifies a hotspot change.
     *
     * It is recommended to use the LPointer::setCursorRequest() method to listen for hotspot changes instead.
     *
     * #### Default implementation
     * @snippet LCursorRoleDefault.cpp hotspotChanged
     */
    virtual void hotspotChanged();

    /*!
     * @brief Cursor hotspot in surface coordinates.
     */
    const LPoint &hotspotS() const;

    /*!
     * @brief Cursor hotspot in compositor coordinates.
     */
    const LPoint &hotspotC() const;

    /*!
     * @brief Cursor hotspot in buffer coordinates.
     */
    const LPoint &hotspotB() const;

    class LCursorRolePrivate;

    /*!
     * @brief Access to the private API of LCursorRole.
     *
     * Returns an instance of the LCursorRolePrivate class (following the ***PImpl Idiom*** pattern) which contains all the private members of LCursorRole.\n
     * Used internally by the library.
     */
    LCursorRolePrivate *imp() const;
private:
    LCursorRolePrivate *m_imp = nullptr;
    virtual void handleSurfaceCommit() override;
    virtual void handleSurfaceOffset(Int32 x, Int32 y) override;
    void globalScaleChanged(Int32 oldScale, Int32 newScale) override;

};

#endif // LCURSORROLE_H
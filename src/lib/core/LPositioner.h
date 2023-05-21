#ifndef LPOSITIONER_H
#define LPOSITIONER_H

#include <LNamespaces.h>
#include <LRect.h>

/*!
 * @brief Positioning rules for a Popup
 *
 * The LPositioner class defines the rules by which a Popup should be positioned relative to the anchor point of its parent.\n
 * Each LPopupRole has its own LPositioner accessible through LPopupRole::positioner().\n
 * The library implements the default LPositioner rules in LPopupRole::rolePosC(), therefore this section
 * is for educational purposes only.
 *
 * @section Anchor-Rect
 *
 * The anchor rectangle (accessible through anchorRectS() or anchorRectC()) is a sub-rectangle of the window geometry of the Popup's parent surface.\n
 * Within this rectangle, an anchor point (accessible through anchor()) is defined, which the Popup uses as a reference to position itself.\n
 * The anchor point can be located at the center, corners, or midpoint of the edges of the anchor rectangle (gray and blue points).\n
 * For example, in the following image, a Popup is positioned using the anchor point Anchor::AnchorRight and Anchor::AnchorBottomLeft.\n
 *
 * <IMG SRC="https://lh3.googleusercontent.com/dCpt0Kl2MBnwpxf7VUiphJ28Tdrxh-3jmNIG-9GyK9nt_-3vCMuj1vgmYajPnYd9CH51fIBYocCUlBKsdXAGqQnufFxYZ1whQ0T6pIfCO1E6NHJj-ii2phQY-kRdUe2lZAnqF0mvyA=w2400">
 *
 * @section Gravity
 *
 * The gravity of the Popup, accessible through gravity(), indicates the direction that the Popup tries to move to.\n
 * You can consider the anchor point as a "nail" and the Popup as a frame composed only of edges. If the gravity is down,
 * the top edge of the Popup will collide with the nail, preventing it from moving further.\n
 * In the following image, a Popup with gravity Gravity::GravityBottomRight and Gravity::GravityTopLeft is shown.\n
 * 
 * <IMG SRC="https://lh3.googleusercontent.com/92DINcYGHOAothPGrLctUtN7mCKRpqESPh4vRA8XN--IehoprgWKYn74myk1CsjXMR_IcaLM7kJZWny7rvytDRr-nXzljMA-W0LWtQ4neu-HGxpT8V2P0blWg5zYymbGQ8vja5Rx6w=w2400">
 *
 * @section Constraint-Adjustments
 *
 * You might be wondering why use such complex rules to position Popups? The answer is that they make it easier to reposition them in cases when their position causes them to be restricted (for example, when they are shown outside the visible area of the screen).\n
 * Specific rules to adjust the position of Popups when they are restricted are defined by the client in constraintAdjustment().\n
 * Typically, it first tries to invert the gravity and anchor point on the axes where the restriction occurs (for example, in nested context menus), if the Popup is still restricted, it tries to slide it along the relevant axes,
 * if this still doesn't solve the problem, the Popup is configured to adjust its size and if finally none of these options work, the original position is used.\n
 * For more information on LPositioner rules, you can analyze the implementation of LPopupRole::rolePosC() or consult the documentation of the [xdg_shell::positioner](https://wayland.app/protocols/xdg-shell#xdg_positioner) interface.
 *
 */
class Louvre::LPositioner
{
public:
    LPositioner();
    ~LPositioner();

    /// Constraint Adjustments
    typedef UInt32 ConstraintAdjustments;

    /*!
     * @brief Anchor point
     *
     * Possible locations of the anchor point within the anchor rectangle.
     */
    enum Anchor : UInt32
    {
        /// Center of the anchor rectangle
        NoAnchor = 0,

        /// Center of the top edge
        AnchorTop         = 1,

        /// Center of the bottom edge
        AnchorBottom      = 2,

        /// Center of the left edge
        AnchorLeft        = 3,

        /// Center of the right edge
        AnchorRight       = 4,

        /// Top-left corner
        AnchorTopLeft     = 5,

        /// Bottom-left corner
        AnchorBottomLeft  = 6,

        /// Top-right corner
        AnchorTopRight    = 7,

        /// Bottom-right corner
        AnchorBottomRight = 8
    };

    /*!
     * @brief Popup gravity
     *
     * The direction in which the Popup tries to move.
     */
    enum Gravity : UInt32
    {
        /// Center of the Popup
        NoGravity = 0,

        /// Upwards
        GravityTop         = 1,

        /// Downwards
        GravityBottom      = 2,

        /// Leftwards
        GravityLeft        = 3,

        /// Rightwards
        GravityRight       = 4,

        /// Upwards and Leftwards
        GravityTopLeft     = 5,

        /// Downwards and Leftwards
        GravityBottomLeft  = 6,

        /// Upwards and Rightwards
        GravityTopRight    = 7,

        /// Downwards and Rightwards
        GravityBottomRight = 8
    };

    /*!
     * @brief Constraint adjustment
     *
     * Rules for removing restriction from a Popup.
     */
    enum ConstraintAdjustment : ConstraintAdjustments
    {
        /// Original position is not modified
        NoAdjustment = 0,

        /// Horizontally slide
        SlideX          = 1,

        /// Vertically slide
        SlideY          = 2,

        /// Invert horizontal component of gravity and anchor point
        FlipX           = 4,

        /// Invert vertical component of gravity and anchor point
        FlipY           = 8,

        /// Scale Popup horizontally
        ResizeX         = 16,

        /// Scale Popup vertically
        ResizeY         = 32
    };

    /*!
     * @brief Wayland client instance
     *
     * Pointer to the client that created the Popup.
     */
    LClient *client() const;

    /*!
     * @brief Wayland resource
     *
     * xdg_positioner resource from the XDG Shell protocol.
     */
    wl_resource *resource() const;

    /*!
     * @brief Size in surface coordinates
     *
     * Size of the Popup to be positioned (window geometry) in surface coordinates.
     */
    const LSize &sizeS() const;

    /*!
     * @brief Size in compositor coordinates
     *
     * Size of the Popup to be positioned (window geometry) in compositor coordinates.
     */
    const LSize &sizeC() const;

    /*!
     * @brief Anchor rectangle in surface coordinates
     *
     * Anchor rectangle relative to the parent's geometry origin in surface coordinates.
     */
    const LRect &anchorRectS() const;

    /*!
     * @brief Anchor rectangle in compositor coordinates
     *
     * Anchor rectangle relative to the parent's geometry origin in compositor coordinates.
     */
    const LRect &anchorRectC() const;

    /*!
     * @brief Additional offset in surface coordinates
     *
     * Additional offset in surface coordinates added to the Popup's position calculated by the rules.
     */
    const LPoint &offsetS() const;

    /*!
     * @brief Additional offset in compositor coordinates
     *
     * Additional offset in compositor coordinates added to the Popup's position calculated by the rules.
     */
    const LPoint &offsetC() const;

    /*!
     * @brief Anchor point
     *
     * Point on the anchor rectangle defined by Anchor enum.
     */
    UInt32 anchor() const;

    /*!
     * @brief Popup gravity
     *
     * Direction in which the Popup is trying to move, defined in Gravity.
     */
    UInt32 gravity() const;

#if LOUVRE_XDG_WM_BASE_VERSION >=3

    /*!
     * @brief Reactive repositioning
     *
     * If true, the Popup's position should be recalculated every time the initial conditions change, for example when the position of the parent changes.
     */
    bool isReactive() const;

    /*!
     * @brief Parent surface size in surface coordinates
     *
     * Size of the window geometry of the parent surface in surface coordinates.
     */
    const LSize &parentSizeS() const;

    /*!
     * @brief Parent surface size in compositor coordinates
     *
     * Size of the window geometry of the parent surface in compositor coordinates.
     */
    const LSize &parentSizeC() const;

    /*!
     * @brief Configuration serial of the parent
     *
     * Configuration serial of the parent to consider in positioning.
     */
    UInt32 parentConfigureSerial() const;
#endif

    /*!
     * @brief Constraint adjustment rules
     *
     * Flags with the rules to use in case the Popup is restricted, defined in ConstraintAdjustment.
     */
    ConstraintAdjustments constraintAdjustment() const;

    class LPositionerPrivate;

    /*!
     * @brief Access to the private API of LPositioner.
     *
     * Returns an instance of the LPositionerPrivate class (following the ***PImpl Idiom*** pattern) which contains all the private members of LPositioner.\n
     * Used internally by the library.
     */
    LPositionerPrivate *imp() const;
private:

    LPositionerPrivate *m_imp = nullptr;



};

#endif // LPOSITIONER_H
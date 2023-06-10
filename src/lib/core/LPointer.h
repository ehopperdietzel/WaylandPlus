#ifndef LPOINTER_H
#define LPOINTER_H

#include <LObject.h>
#include <LPoint.h>
#include <LToplevelRole.h>
#include <linux/input-event-codes.h>

/*!
 * @brief Class for handling pointer events.
 *
 * The LPointer class has virtual methods to access pointer events (mouse, trackpad, etc) generated by the input backend.\n
 * It allows to move and scale Toplevel surfaces, send pointer events to client surfaces, and configure parameters such as
 * the scroll wheel step value.\n
 * There is a single instance of LPointer, which can be accessed from LSeat::pointer().
 */
class Louvre::LPointer : public LObject
{
public:

    /*!
     * @brief Constructor parameters.
     *
     * Configuration parameters provided in the virtual LCompositor::createPointerRequest() virtual constructor.
     */
    struct Params;

    /*!
     * @brief Pointer buttons.
     *
     * Enumeration of common pointer buttons.
     */
    enum Button : UInt32
    {
        /// Left button
        Left = BTN_LEFT,

        /// Right button
        Right = BTN_RIGHT,

        /// Middle button
        Middle = BTN_MIDDLE,

        /// Side button
        Side = BTN_SIDE,

        /// Extra button
        Extra = BTN_EXTRA,

        /// Forward button
        Forward = BTN_FORWARD,

        /// Back button
        Back = BTN_BACK,

        /// Task button
        Task = BTN_TASK
    };

    /*!
     * @brief Button states.
     *
     * Possible states of a pointer button.
     */
    enum ButtonState : UInt32
    {
        /// Button not pressed
        Released = 0,

        /// Button pressed
        Pressed = 1
    };

    /*!
     * @brief Source of a scroll event
     *
     * Possible sources of a scroll event.
     */
    enum AxisSource : UInt32
    {
        /// Rotation of the mouse wheel (discrete)
        Wheel = 0,

        /// Finger swipe on a trackpad (continuous)
        Finger = 1,

        /// Continuous movement (with unspecified source)
        Continuous = 2,

        /// Side movement of a mouse wheel (since 6)
        WheelTilt = 3
    };

    /*!
     * @brief Edge constraint when resizing a Toplevel
     */
    enum ResizeEdgeSize
    {
        /// Disabes the constraint on the specified edge.
        EdgeDisabled = -2147483647
    };

    /*!
     * @brief LPointer class constructor.
     *
     * There is a single instance of LPointer, which can be accessed from LSeat::pointer().
     *
     * @param params Internal library parameters provided in the virtual LCompositor::createPointerRequest() constructor.
     */
    LPointer(Params *params);

    /*!
     * @brief LPointer class destructor.
     *
     * Invoked internally by the library after notification of its destruction with LCompositor::destroyPointerRequest().
     */
    virtual ~LPointer();

    LPointer(const LPointer&) = delete;
    LPointer& operator= (const LPointer&) = delete;

    /*!
     * @brief Surface with pointer focus.
     *
     * Surface with pointer focus assigned with setFocus(). Only surfaces with pointer focus can receive events.\n
     * @returns nullptr if no surface has pointer focus.
     */
    LSurface *focusSurface() const;

    /*!
     * @brief Look for a surface.
     *
     * Looks for the first surface that contains the point given by the *point* parameter.\n
     * To do this, it takes into account the input region of the surfaces (LSurface::inputRegionC()) and the order given by the list of surfaces of the compositor (LCompositor::surfaces()).\n
     * Some surface roles do not have an input region such as the cursor role or drag & drop icon, so these surfaces are ignored.
     *
     * @param point Point in compositor coordinates.
     * @returns Returns the first surface that contains the point or nullptr if no surface is found.
     */
    LSurface *surfaceAtC(const LPoint &point);

    /*!
     * @name Client events
     */
///@{

    /*!
     * @brief Assigns the pointer focus.
     *
     * Assigns the pointer focus to the surface passed in the argument using the cursor position.\n
     * Once assigned, the surface that previously had it loses pointer focus.\n
     * Passing nullptr as an argument removes pointer focus from all surfaces.
     */
    void setFocusC(LSurface *surface);

    /*!
     * @brief Assigns the pointer focus.
     *
     * Assigns the pointer focus to the specified surface at the given local compositor position within the surface.\n
     * If the surface is nullptr, pointer focus will be removed from all surfaces.
     *
     * @param surface Surface that will acquire the pointer focus or nullptr to remove focus
     * @param localPosC Local compositor position within the surface that the pointer enters.
     */
    void setFocusC(LSurface *surface, const LPoint &localPosC);

    /*!
     * @brief Assigns the pointer focus.
     *
     * Assigns the pointer focus to the specified surface at the given local surface position within the surface.\n
     * If the surface is nullptr, pointer focus will be removed from all surfaces.
     *
     * @param surface Surface that will acquire the pointer focus or nullptr to remove focus
     * @param localPosS Local surface position within the surface that the pointer enters.
     */
    void setFocusS(LSurface *surface, const LPoint &localPosS);

    /*!
     * @brief Sends the pointer position.
     *
     * Sends the current cursor position to the surface with focus.\n
     * The library internally transforms the cursor position to the surface's local coordinates.
     */
    void sendMoveEventC();

    /*!
     * @brief Sends the pointer position.
     *
     * Sends the current pointer position to the surface with focus.\n
     * If you want to use the cursor position as the pointer, you can use the variant of this method without arguments.
     *
     * @param localPosC Pointer position relative to the top-left corner of the surface in compositor coordinates.
     */
    void sendMoveEventC(const LPoint &localPosC);

    /*!
     * @brief Sends the pointer position.
     *
     * Sends the current pointer position to the surface with focus.\n
     *
     * @param localPosS Pointer position relative to the top-left corner of the surface in surface coordinates.
     */
    void sendMoveEventS(const LPoint &localPosS);

    /*!
     * @brief Sends a pointer button state.
     *
     * Sends the state of a pointer button to the surface with focus.
     *
     * @param button Button code.
     * @param state State of the button.
     */
    void sendButtonEvent(Button button, ButtonState state);

    /*!
     * @brief Sends a scroll event.
     *
     * Sends a scroll event to the surface with focus.\n
     *
     * @param x Direction and magnitude of the scroll on the horizontal axis. 0 stops scrolling.
     * @param y Direction and magnitude of the scroll on the vertical axis. 0 stops scrolling.
     * @param source One of the values from AxisSource.
     */
    void sendAxisEvent(Float64 axisX, Float64 axisY, Int32 discreteX, Int32 discreteY, UInt32 source);

    /*!
     * @brief Closes all popups.
     *
     * Closes all active Popup surfaces in reverse order of creation.
     */
    void dismissPopups();

///@}

    /*!
     * @name Interactive Resizing
     *
     * Methods for changing the size of a Toplevel surface in an interactive manner.
     *
     * <center><img src="https://lh3.googleusercontent.com/5Ntj7N5_Sz7jLFToVmF6dETk6mlP6w0BrqAz1-pC3Ruu-_9zDstycXFReovd3_T9MVWcTAthVdLQ6PekJXXhm2NxHZvb_xiC6hjKw3D8yRd3rQi2Ej2RgqVY-BdqeB4q4WAU7iQPWQ=w2400"></center>
     */
///@{
    /*!
     * @brief Begins an interactive resizing session.
     *
     * Starts an interactive resizing session on a Toplevel surface from one of its edges or corners.\n
     * You can restrict the space in which the surface expands by defining a rectangle given by the L, T, R, and B values.\n
     * If you do not want to restrict any of the edges, assign its value to EdgeDisabled.
     *
     * To update the position and size of the Toplevel, call the methods updateResizingToplevelSize() and updateResizingToplevelPos().\n
     * Once the size change is made, call the method stopResizingToplevel() to end the session.
     *
     * You can see an example of its use in LToplevelRole::startResizeRequest().
     *
     * @param toplevel Toplevel that will change size.
     * @param edge Edge or corner from which the resizing will be performed.
     * @param L Restriction of the left edge.
     * @param T Restriction of the top edge.
     * @param R Restriction of the right edge.
     * @param B Restriction of the bottom edge.
     */
    void startResizingToplevelC(LToplevelRole *toplevel, LToplevelRole::ResizeEdge edge, Int32 L = EdgeDisabled, Int32 T = EdgeDisabled, Int32 R = EdgeDisabled, Int32 B = EdgeDisabled);

    /*!
     * @brief Updates the size of a Toplevel during an interactive resizing session.
     *
     * You can see an example of its use in the default implementation of pointerPosChangeEvent().
     */
    void updateResizingToplevelSize();

    /*!
     * @brief Updates the position of a Toplevel during an interactive resizing session.
     *
     * You can see an example of its use in the default implementation of LToplevelRole::geometryChanged().
     */
    void updateResizingToplevelPos();

    /*!
     * @brief Ends an interactive resizing session.
     *
     * You can see an example of its use in the default implementation of pointerButtonEvent().
     */
    void stopResizingToplevel();

    /*!
     * @brief Toplevel surface of an interactive resizing session.
     *
     * @returns nullptr if there is no interactive resizing session in progress.
     */
    LToplevelRole *resizingToplevel() const;

    /*!
     * @brief Initial position of a Toplevel in an interactive resizing session.
     *
     * Initial position of a Toplevel surface in an interactive resizing session.
     */
    const LPoint &resizingToplevelInitPos() const;

    /*!
     * @brief Initial size of a Toplevel in an interactive resizing session.
     *
     * Initial size of a Toplevel surface in an interactive resizing session.
     */
    const LSize &resizingToplevelInitSize() const;

    /*!
     * @brief Initial cursor position in an interactive resizing session.
     *
     * Initial cursor position in an interactive resizing session.
     */
    const LPoint &resizingToplevelInitCursorPos() const;

    /*!
     * @brief Edge or corner of a Toplevel in an interactive resizing session.
     *
     * Edge or corner from which the resizing of the Toplevel surface is performed.
     */
    LToplevelRole::ResizeEdge resizingToplevelEdge() const;

///@}

    /*!
     * @name Interactive Movement
     */
///@{
    /*!
     * @brief Begins an interactive moving session.
     *
     * Begins an interactive moving session on a Toplevel surface.\n
     * You can restrict the space in which the surface is positioned by defining a rectangle given by the values L, T, R, and B.\n
     * If you do not want to restrict any of the edges, you should assign its value to EdgeDisabled.
     *
     * To update the position and size of the Toplevel, you should call the updateMovingToplevelPos() method.\n
     * Once the position change is made, you should call the stopMovingToplevel() method to end the session.
     *
     * You can see an example of its use in LToplevelRole::startMoveRequest().
     *
     * @param toplevel Toplevel that will change size.
     * @param L Restriction of the left edge.
     * @param T Restriction of the top edge.
     * @param R Restriction of the right edge.
     * @param B Restriction of the bottom edge.
     */
    void startMovingToplevelC(LToplevelRole *toplevel, Int32 L = EdgeDisabled, Int32 T = EdgeDisabled, Int32 R = EdgeDisabled, Int32 B = EdgeDisabled);

    /*!
     * @brief Updates the position of a Toplevel during an interactive moving session.
     *
     * You can see an example of its use in the default implementation of pointerPosChangeEvent().
     */
    void updateMovingToplevelPos();

    /*!
     * @brief Stops an interactive moving session.
     *
     * You can see an example of its use in the default implementation of pointerButtonEvent().
     */
    void stopMovingToplevel();

    /*!
     * @brief Toplevel surface of an interactive moving session.
     *
     * @returns nullptr if there is no interactive moving session in progress.
     */
    LToplevelRole *movingToplevel() const;

    /*!
     * @brief Initial position of a Toplevel in an interactive moving session.
     *
     * Initial position of a Toplevel surface in an interactive moving session.
     */
    const LPoint &movingToplevelInitPos() const;

    /*!
     * @brief Initial cursor position during an interactive move session.
     *
     * Initial cursor position during an interactive move session of a Toplevel surface.
     */
    const LPoint &movingToplevelInitCursorPos() const;

///@}

    /*!
     * @brief Keeps pointer focus.
     *
     * Utility function to keep the pointer focus on the surface being pressed (by the left mouse button for example)
     * even when the pointer is outside of it.\n
     * @warning Do not confuse with the icon role of a drag & drop session.
     *
     * @param surface Surface being pressed or nullptr.
     */
    void setDragginSurface(LSurface *surface);

    /*!
     * @brief Surface that maintains pointer focus.
     *
     * Surface being pressed.
     *
     * @returns The surface being pressed or nullptr.
     */
    LSurface *draggingSurface() const;

    /*!
     * @name Virtual methods
     */
///@{

    /*!
     * @brief Notifies a pointer movement.
     *
     * Notification of pointer movement generated by the input backend.\n
     * Reimplement this virtual method if you want to be notified when the pointer changes its position by a delta (dx,dy).
     *
     * @warning Some input backends like X11 do not work with relative values and invoke pointerPosChangeEvent() instead.
     *
     * @param dx relative position of the x component of the pointer from its previous position.
     * @param dy relative position of the y component of the pointer from its previous position.
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp pointerMoveEvent
     */
    virtual void pointerMoveEvent(Float32 dx, Float32 dy);

    /*!
     * @brief Notifies a change in pointer position.
     *
     * Notification of a change in pointer position generated by the input backend.\n
     * Reimplement this virtual method if you want to be notified when the pointer changes its position to (x,y).
     *
     * @warning Some input backends like Libinput do not work with absolute values and invoke pointerMoveEvent() instead.
     *
     * @param x Absolute position of the x component of the pointer.
     * @param y Absolute position of the y component of the pointer.
     *
     * #### Default implementation
     * @snippet LPointerDefault.cpp pointerPosChangeEvent
     */
    virtual void pointerPosChangeEvent(Float32 x, Float32 y);

    /*!
     * @brief Notifies a change of state of a pointer button.
     *
     * Notification of change of state of a pointer button generated by the input backend.\n
     * Reimplement this virtual method if you want to be notified when a pointer button changes state.
     *
     * @param button Code of the pressed button.
     * @param state State of the button (Pressed or Released).
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp pointerButtonEvent
     */
    virtual void pointerButtonEvent(Button button, ButtonState state);

    /*!
     * @brief Notifies a scroll event.
     *
     * Notification of a pointer scroll event generated by the input backend.\n
     * Reimplement this virtual method if you want to be notified when the pointer emits a scroll event.
     *
     * @warning The origin of the event could affect how the parameters (x,y) are interpreted. For example, if the origin is AxisSource::Wheel the parameters (x,y) are ignored and scrollWheelStep() is used instead.
     * @param x Direction and magnitude of the scroll on the x component.
     * @param y Direction and magnitude of the scroll on the y component.
     * @param source Origin of the event.
     *
     * #### Default implementation
     * @snippet LPointerDefault.cpp pointerAxisEvent
     */
    virtual void pointerAxisEvent(Float64 axisX, Float64 axisY, Int32 discreteX, Int32 discreteY, UInt32 source);

    /*!
     * @brief Request to assign the cursor.
     *
     * Invoked when a client requests to assign the cursor texture or hide it.\n
     * Only clients that have a surface with pointer focus can request to assign the cursor texture.\n
     * Reimplement this virtual method if you want to assign the cursor texture requested by the client.
     *
     * @param cursorRole Surface role to use as cursor. If it is nullptr, it means that the client wants to hide the cursor.
     *
     * #### Default Implementation
     * @snippet LPointerDefault.cpp setCursorRequest
     */
    virtual void setCursorRequest(LCursorRole *cursorRole);
///@}

    LPRIVATE_IMP(LPointer)
};

#endif // LPOINTER_H

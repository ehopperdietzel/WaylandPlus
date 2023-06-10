#ifndef LNAMESPACES_H
#define LNAMESPACES_H

#include <protocols/Wayland/wayland.h>
#include <libinput.h>
#include <list>

#define LOUVRE_GLOBAL_ITERS_BEFORE_DESTROY 5

#define LOUVRE_MAX_SURFACE_SIZE 10000000
#define LOUVRE_DEBUG 1

// Globals
#define LOUVRE_WL_COMPOSITOR_VERSION 6
#define LOUVRE_WL_CALLBACK_VERSION 1
#define LOUVRE_WL_SEAT_VERSION 9
#define LOUVRE_OUTPUT_VERSION 4
#define LOUVRE_SUBCOMPOSITOR_VERSION 1
#define LOUVRE_DATA_DEVICE_MANAGER_VERSION 3

#define LOUVRE_XDG_WM_BASE_VERSION 2

#define LOUVRE_XDG_DECORATION_MANAGER_VERSION 1

#define LOUVRE_WP_PRESENTATION_VERSION 1

#define LOUVRE_LINUX_DMA_BUF_VERSION 3

#define L_UNUSED(object){(void)object;}

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y

#define LPRIVATE_CLASS(class_name) \
class class_name::CAT(class_name,Private){ \
public: \
    CAT(class_name,Private)() = default; \
    ~CAT(class_name,Private)() = default; \
    CAT(class_name,Private)(const CAT(class_name,Private)&) = delete; \
    CAT(class_name,Private) &operator=(const CAT(class_name,Private)&) = delete;

#define LPRIVATE_IMP(class_name) \
    class CAT(class_name,Private); \
    inline CAT(class_name,Private) *imp() const {return m_imp;}; \
    private: \
    CAT(class_name,Private) *m_imp = nullptr; \
/*!
 * @brief Namespaces
 */
namespace Louvre
{
    // API Classes
    class LGraphicBackend;
    class LInputBackend;
    class LClient;
    class LCompositor;
    class LObject;
    class LOutput;
    class LOutputManager;
    class LOutputMode;
    class LOpenGL;
    class LPainter;
    class LPositioner;
    class LRegion;
    class LResource;
    class LSurface;
    class LSession;
    class LTexture;
    class LWayland;

    // Data
    class LDataDevice;
    class LDataSource;
    class LDataOffer;
    class LDNDManager;

    // Surface roles
    class LBaseSurfaceRole;
    class LCursorRole;
    class LDNDIconRole;
    class LToplevelRole;
    class LPopupRole;
    class LSubsurfaceRole;

    // Input related
    class LSeat;
    class LPointer;
    class LKeyboard;
    class LCursor;
    class LXCursor;

    // Other
    class LDMABuffer;

    // Utils
    class LLog;
    class LTime;
    template <class TA, class TB> class LPointTemplate;
    template <class TA, class TB> class LRectTemplate;

    // Types

    /// @brief 64 bits unsigned integer
    typedef uint64_t        UInt64;

    /// @brief 64 bits signed integer
    typedef int64_t         Int64;

    /// @brief 32 bits unsigned integer
    typedef uint32_t        UInt32;

    /// @brief 32 bits signed integer
    typedef int32_t         Int32;

    /// @brief 16 bits unsigned integer
    typedef uint16_t        UInt16;

    /// @brief 16 bits signed integer
    typedef int16_t         Int16;

    /// @brief 8 bits unsigned integer
    typedef unsigned char   UChar8;

    /// @brief 8 bits signed integer
    typedef char            Char8;

    /// @brief 64 bits float
    typedef double          Float64;

    /// @brief 32 bits float
    typedef float           Float32;

    /// @brief 24 bits Wayland float
    typedef wl_fixed_t      Float24;

    /// 2D vector of 32 bits integers
    typedef LPointTemplate<Int32,Float32> LPoint;

    /// 2D vector of 32 bits integers
    typedef LPoint LSize;

    /// 2D vector of 32 bits floats
    typedef LPointTemplate<Float32,Int32> LPointF;

    /// 2D vector of 32 bits floats
    typedef LPointF LSizeF;

    /// 4D vector of 32 bits integers
    typedef LRectTemplate<Int32,Float32> LRect;

    /// 4D vector of 32 bits floats
    typedef LRectTemplate<Float32,Int32> LRectF;

    typedef UInt32 LKey;

    typedef void* EGLContext;
    typedef void* EGLDisplay;
    typedef unsigned int GLuint;
    typedef unsigned int GLenum;

    struct LDMAFormat
    {
        UInt32 format;
        UInt64 modifier;
    };

    #define LOUVRE_MAX_DMA_PLANES 4
    struct LDMAPlanes
    {
        UInt32 width;
        UInt32 height;
        UInt32 format;
        UInt32 num_fds;
        Int32 fds[LOUVRE_MAX_DMA_PLANES];
        UInt32 strides[LOUVRE_MAX_DMA_PLANES];
        UInt32 offsets[LOUVRE_MAX_DMA_PLANES];
        UInt64 modifiers[LOUVRE_MAX_DMA_PLANES];
    };

    struct LGraphicBackendInterface
    {
        bool (*initialize)(LCompositor *compositor);
        void (*pause)(LCompositor *compositor);
        void (*resume)(LCompositor *compositor);
        bool (*scheduleOutputRepaint)(LOutput *output);
        void (*uninitialize)(LCompositor *compositor);
        const std::list<LOutput*>*(*getConnectedOutputs)(LCompositor *compositor);
        bool (*initializeOutput)(LOutput *output);
        void (*uninitializeOutput)(LOutput *output);
        EGLDisplay (*getOutputEGLDisplay)(LOutput *output);
        const LSize *(*getOutputPhysicalSize)(LOutput *output);
        Int32 (*getOutputCurrentBufferIndex)(LOutput *output);
        const char *(*getOutputName)(LOutput *output);
        const char *(*getOutputManufacturerName)(LOutput *output);
        const char *(*getOutputModelName)(LOutput *output);
        const char *(*getOutputDescription)(LOutput *output);
        const LOutputMode *(*getOutputPreferredMode)(LOutput *output);
        const LOutputMode *(*getOutputCurrentMode)(LOutput *output);
        const std::list<LOutputMode*> *(*getOutputModes)(LOutput *output);
        bool (*setOutputMode)(LOutput *output, LOutputMode *mode);
        const LSize *(*getOutputModeSize)(LOutputMode *mode);
        Int32 (*getOutputModeRefreshRate)(LOutputMode *mode);
        bool (*getOutputModeIsPreferred)(LOutputMode *mode);
        bool (*hasHardwareCursorSupport)(LOutput *output);
        void (*setCursorTexture)(LOutput *output, UChar8 *buffer);
        void (*setCursorPosition)(LOutput *output, const LPoint &position);

        // Buffers
        const std::list<LDMAFormat*>*(*getDMAFormats)(LCompositor *compositor);
        EGLDisplay (*getAllocatorEGLDisplay)(LCompositor *compositor);
        EGLContext (*getAllocatorEGLContext)(LCompositor *compositor);

        bool (*createTextureFromCPUBuffer)(LTexture *texture, const LSize &size, UInt32 stride, UInt32 format, const void *pixels);
        bool (*createTextureFromWaylandDRM)(LTexture *texture, void *wlBuffer);
        bool (*createTextureFromDMA)(LTexture *texture, const LDMAPlanes *planes);
        bool (*updateTextureRect)(LTexture *texture, UInt32 stride, const LRect &dst, const void *pixels);
        UInt32 (*getTextureID)(LOutput *output, LTexture *texture);
        void (*destroyTexture)(LTexture *texture);
    };

    struct LInputBackendInterface
    {
        bool (*initialize)(const LSeat *seat);
        UInt32 (*getCapabilities)(const LSeat *seat);
        void *(*getContextHandle)(const LSeat *seat);
        void (*uninitialize)(const LSeat *seat);
        void (*suspend)(const LSeat *seat);
        void (*forceUpdate)(const LSeat *seat);
        void (*resume)(const LSeat *seat);
    };

    namespace Protocols
    {
        namespace Wayland
        {
            class GCompositor;
            class GSubcompositor;
            class GSeat;
            class GDataDeviceManager;
            class GOutput;

            class RSurface;
            class RRegion;
            class RPointer;
            class RKeyboard;
            class RDataDevice;
            class RDataSource;
            class RDataOffer;
            class RSubsurface;
            class RCallback;
        }

        namespace XdgShell
        {
            class GXdgWmBase;

            class RXdgPositioner;
            class RXdgSurface;
            class RXdgToplevel;
            class RXdgPopup;
        };

        namespace XdgDecoration
        {
            class GXdgDecorationManager;

            class RXdgToplevelDecoration;
        };

        namespace WpPresentationTime
        {
            class GWpPresentation;

            class RWpPresentationFeedback;
        };

        namespace LinuxDMABuf
        {
            class GLinuxDMABuf;

            class RLinuxBufferParams;
            class RLinuxDMABufFeedback;
        };
    }
};

#endif // LNAMESPACES_H

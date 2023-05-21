#ifndef LDATAOFFER_H
#define LDATAOFFER_H

#include <LNamespaces.h>

/*!
 * @brief Data offered to a client
 *
 * The LDataOffer class represents the **wl_data_offer** interface of the Wayland protocol. Allows you to share the characteristics of an LDataSource
 * (clipboard or drag & drop session) with a client when one of its surfaces acquire pointer, keyboard or touch focus.\n
 * It is mainly used by the library and therefore not necessary to interact with it directly.
 */
class Louvre::LDataOffer
{
public:

    /*!
     * @brief Usage modes of a data offer.
     *
     * Indicates the context in which a data offer is used.
     */
    enum Usage : UInt32
    {
        /// @brief Indefinite use.
        /// The data offering has not yet been assigned an use.
        Undefined = 0,

        /// @brief Used by the clipboard.
        Selection = 1,

        /// @brief Used in a drag & drop session.
        DND = 2
    };


    LDataOffer(const LDataOffer&) = delete;
    LDataOffer& operator= (const LDataOffer&) = delete;

    /*!
     * @brief Usage of a data offer.
     *
     * Indicates the context in which the data offer is used.n
     * It can be used by the clipboard, in drag & drop sessions or have an indefinite use.
     */
    Usage usedFor() const;

    /*!
     * @brief Global seat instance.
     */
    LSeat *seat() const;

    /*!
     * @brief Wayland resource of the data offer.
     *
     * Returns the resource generated by the **wl_data_offer** interface of the Wayland protocol.
     */
    Protocols::Wayland::DataOfferResource *dataOfferResource() const;


    class LDataOfferPrivate;

    /*!
     * @brief Access to the private API of LDataOffer.
     *
     * Returns an instance of the LDataOfferPrivate class (following the ***PImpl Idiom*** pattern) which contains all the private members of LDataOffer.\n
     * Used internally by the library.
     */
    LDataOfferPrivate *imp() const;
private:
    friend class Protocols::Wayland::DataOfferResource;
    LDataOffer(Protocols::Wayland::DataOfferResource *dataOfferResource);
    ~LDataOffer();
    LDataOfferPrivate *m_imp = nullptr;
};

#endif // LDATAOFFER_H
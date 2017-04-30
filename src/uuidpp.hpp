/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * uuidpp.hpp
 *
 * Copyright (C) 2017 Emilien Kia <emilien.kia@gmail.com>
 *
 * uuidpp is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * uuidpp is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef _UUIDPP_HPP_
#define _UUIDPP_HPP_

#include <array>
#include <string>
#include <vector>

/**
 * UUID - Universally Unique Identifier.
 * @see https://tools.ietf.org/html/rfc4122
 * @note Only RFC4122 variant is supported.
 */
class uuid : public std::array<uint8_t, 16>
{
public:
    typedef std::array<uint8_t, 16> parent_t;

    /** Version of the UUID */
    enum class version_t : uint8_t
    {
        version_unknown         = 0x00,
        version_time_based      = 0x01,
        version_dce_security    = 0x02,
        version_name_based_md5  = 0x03,
        version_random          = 0x04,
        version_name_based_sha1 = 0x05,
    };

    /** Variant of the UUID */
    enum class variant_t : uint8_t
    {
        variant_unknown     = 0,
        variant_ncs         = 1,
        variant_rfc4122     = 2,
        variant_microsoft   = 3,
        variant_future      = 4
    };

    /**
     * Default constructor.
     * Construct a nil UUID.
     */
    constexpr uuid():
    parent_t({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0})
    {}

    /** Copy constructor. */
    constexpr uuid(const uuid&) = default;
    /** Move constructor. */
    constexpr uuid(uuid&&) = default;
    /** Copy assignment operator. */
    uuid& operator=(const uuid& other) = default;
    /** Move assignement operator. */
    uuid& operator=(uuid&& other) = default;
    /** Destructor. */
    ~uuid() = default;

    /**
     * Construct a UUID by copying an array of bytes.
     * @param arr Array of bytes from which construct the UUID.
     */
    constexpr uuid(const std::array<uint8_t, 16>& arr):
    parent_t(arr)
    {}

    /**
     * Construct a UUID by moving an array of bytes.
     * @param arr Array of bytes from which construct the UUID.
     */
    constexpr uuid(std::array<uint8_t, 16>&& arr):
    parent_t(std::move(arr))
    {}

    /**
     * Construct a UUID from it most and least significant bytes.
     * @param msb Most significant bytes.
     * @param lsb Least significant bytes.
     */
    uuid(uint64_t msb, uint64_t lsb);

    /**
     * Construct a UUID from its layout decomposition.
     * @see https://tools.ietf.org/html/rfc4122#section-4.1.2
     * @param time_low The low field of the timestamp.
     * @param time_mid The middle field of the timestamp.
     * @param time_hi_and_version The high field of the timestamp multiplexed with the version number.
     * @param clock_seq The field of the clock sequence multiplexed with the variant.
     * @param node The spatially unique node identifier.
     * Only least significant 48 bits are used.
     */
    uuid(uint32_t time_low, uint16_t time_mid, uint16_t time_hi_and_version,
         uint16_t clock_seq, uint64_t node);
    // TODO review types to adjust names to real considered sizes:

    /**
     * Construct a UUID from its fields.
     * @param time The timestamp. Only 60 least significant bits are used.
     * @param version Version of the UUID.
     * @param clock_seq Clock sequence.
     * @param node Node content.
     */
    uuid(uint64_t time, version_t version, uint16_t clock_seq, uint64_t node);

    /**
     * Construct a UUID from a binary content through a couple of iterators.
     * @tparam It Iterator type.
     * @param begin Begin iterator to the content to use.
     * @param end End iterator to the content to use.
     */
    template<class It>
    uuid(const It& begin, const It& end)
    {
        It it = begin;
        for(size_t n=0; n<16 && it!=end;)
        {
            at(n++) = *it++;
        }
    }

    /**
     * Construct a UUID from a binary content through a container.
     * @tparam Cont Container type.
     * @param cont container to use.
     */
    template<class Cont>
    uuid(const Cont& cont)
    {
        typename Cont::const_iterator it = cont.begin();
        for(size_t n=0; n<16 && it!=cont.end();)
        {
            at(n++) = *it++;
        }
    }

    /**
     * Build a UUID version 1 based on a timestamp a clock sequence and a mac address.
     * @param timestamp Timestamp to use. Number of 100ns since 00:00:00.00, 15 October 1582
     * in UTC time.
     * @param clock_seq Clock sequence.
     * @param mac_address Mac address. (only the 6 least significant bytes are used)
     * @return The built UUID.
     */
    static uuid version1(uint64_t timestamp, uint16_t clock_seq, uint64_t mac_address);

    /**
     * Build a UUID version 1 based on a timestamp a clock sequence and a mac address.
     * @param timestamp Timestamp to use. Number of 100ns since 00:00:00.00, 15 October 1582
     * in UTC time.
     * @param clock_seq Clock sequence.
     * @param mac_address Mac address.
     * @return The built UUID.
     */
    static uuid version1(uint64_t timestamp, uint16_t clock_seq, const std::array<uint8_t,6>& mac_address);

    // TODO Add version1 generators from chrono::clock time

    // TODO add version2

    /**
     * Build a random-based UUID version 4.
     * @return The built UUID.
     */
    static uuid version4();

    /**
     * Build a MD5 hash based UUID from a namespace and a name.
     * @param ns Namespace to use.
     * @param name Name to use (as byte array).
     * @param name_len Size of byte array in bytes.
     * @return The built UUID.
     */
    static uuid version3(uuid ns, const void* name, size_t name_len);
    
    /**
     * Build a MD5 hash based UUID from a namespace and a name.
     * @param ns Namespace to use.
     * @param name Name to use (0-terminated string as byte array).
     * @return The built UUID.
     */
    static uuid version3(uuid ns, const char* name)
    {
        return version3(std::move(ns), std::string(name));
    }

    /**
     * Build a MD5 hash based UUID from a namespace and a name.
     * @param ns Namespace to use.
     * @param name Name to use.
     * @return The built UUID.
     */
    static inline uuid version3(uuid ns, const std::string& name)
    {
        return version3(std::move(ns), (const void*)name.data(), name.size());
    }

    /**
     * Build a MD5 hash based UUID from a namespace and a name.
     * @tparam Cont Type of container.
     * @param ns Namespace to use.
     * @param name Name to use (as byte container).
     * @return The built UUID.
     */
    template<class Cont>
    static inline uuid version3(uuid ns, const Cont& name)
    {
        return version3(std::move(ns), (const void*)name.data(), name.size());
    }

    /**
     * Build a MD5 hash based UUID from a namespace and a name.
     * @tparam It Type of iterator
     * @param ns Namespace to use.
     * @param begin Start iterator to name.
     * @param end After-last iterator to name.
     * @return The built UUID.
     */
    template<class It>
    static inline uuid version3(uuid ns, const It& begin, const It& end)
    {
        std::vector<uint8_t> buff(begin, end);
        return version3(std::move(ns), buff.data(), buff.size());
    }

    /**
     * Build a SHA1 hash based UUID from a namespace and a name.
     * @param ns Namespace to use.
     * @param name Name to use (as byte array).
     * @param name_len Size of byte array in bytes.
     * @return The built UUID.
     */
    static uuid version5(uuid ns, const void* name, size_t name_len);

    /**
     * Build a SHA1 hash based UUID from a namespace and a name.
     * @param ns Namespace to use.
     * @param name Name to use (0-terminated string as byte array).
     * @return The built UUID.
     */
    static uuid version5(uuid ns, const char* name)
    {
        return version5(std::move(ns), std::string(name));
    }

    /**
     * Build a SHA1 hash based UUID from a namespace and a name.
     * @tparam Cont Type of container.
     * @param ns Namespace to use.
     * @param name Name to use (as byte container).
     * @return The built UUID.
     */
    template<class Cont>
    static inline uuid version5(uuid ns, const Cont& name)
    {
        return version5(std::move(ns), name.data(), name.size());
    }

    /**
     * Test if the UUID is nil (or null) aka equals to 0.
     * @return True if the UUID is completly equals to 0.
     */
    constexpr bool nil() const noexcept
    {
    // TODO simplify that to remain constexpr!
    return at(0) == 0 && at(1) == 0 && at(2) == 0 && at(3) == 0
        && at(4) == 0 && at(5) == 0 && at(6) == 0 && at(7) == 0
        && at(8) == 0 && at(9) == 0 && at(10) == 0 && at(11) == 0
        && at(12) == 0 && at(13) == 0 && at(14) == 0 && at(15) == 0
        ;
    }

    /**
     * Test if the UUID is not nil (or not null).
     * @return True if the UUID is not completly equals to 0.
     */
    constexpr operator bool() const noexcept {return !nil();}

    /**
     * Returns the variant of the UUID
     * @return Variant of the UUID.
     */
    constexpr variant_t variant() const noexcept
    {
        return    ((at(8) >> 5) == 0x07 ? variant_t::variant_future
                : ((at(8) >> 5) == 0x06 ? variant_t::variant_microsoft
                : ((at(8) >> 6) == 0x02 ? variant_t::variant_rfc4122
                : ((at(8) >> 7) == 0x00 ? variant_t::variant_ncs
                : variant_t::variant_unknown))));
    }
    
    /**
     * Return the version of the UUID.
     * @return Version number of the UUID.
     */
    constexpr version_t version() const noexcept
    {
        return version_t((at(6) & 0xF0) >> 4);
    }

    /**
     * Compare this UUID to another.
     * @param other Other UUID to compare.
     * @return Negative if this if less than other, 0 if equal, positive if greater.
     */
    int compare(uuid const & other) const;

    friend bool operator==(uuid const& l, uuid const& r)
    {
        return l.compare(r) == 0;
    }

    friend bool operator!=(uuid const& l, uuid const& r)
    {
        return l.compare(r) != 0;
    }

    friend bool operator<(uuid const& l, uuid const& r)
    {
        return l.compare(r) < 0;
    }

    friend bool operator>(uuid const& l, uuid const& r)
    {
        return l.compare(r) > 0;
    }

    friend bool operator<=(uuid const& l, uuid const& r)
    {
        return l.compare(r) <= 0;
    }

    friend bool operator>=(uuid const& l, uuid const& r)
    {
        return l.compare(r) >= 0;
    }

    void swap(uuid& other) noexcept
    {
        parent_t::swap(other);
    }

    /**
     * Format a UUID as a string on the form "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
     * @return The formatted UUID.
     */
    std::string to_string() const;

    /**
     * Format a UUID as a string on the form "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx".
     * @return The formatted UUID.
     */
    std::string to_hex() const;

    /**
     * Format a UUID as a string on the form "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}".
     * @return The formatted UUID.
     */
    std::string to_msguid() const;

    /**
     * Format a UUID as a string on the form "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx".
     * @return The formatted UUID.
     */
    std::string to_urn() const;
};

namespace uuid_ns
{
    /** DNS UUID namespace. */
    extern const uuid dns;
    /** URL UUID namespace. */
    extern const uuid url;
    /** OID UUID namespace. */
    extern const uuid oid;
    /** X500 UUID namespace. */
    extern const uuid x500;
}

namespace std
{
    inline void swap(uuid& l, uuid& r) noexcept
    {
        l.swap(r);
    }

}

#endif // _UUIDPP_HPP_
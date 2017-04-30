/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * uuidpp.cpp
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
#include "uuidpp.hpp"

#include <random>

#include "portable-endian.h"
#include "md5.h"
#include "sha1.h"

static constexpr char const* const hex = "0123456789abcdef";

uuid::uuid(uint64_t msb, uint64_t lsb)
{
    *((uint64_t*)&at(0)) = htobe64(msb);
    *((uint64_t*)&at(8)) = htobe64(lsb);
}

uuid::uuid(uint32_t time_low, uint16_t time_mid, uint16_t time_hi_and_version,
         uint16_t clock_seq, uint64_t node)
{
    *((uint32_t*)&at(0)) = htobe32(time_low);
    *((uint16_t*)&at(4)) = htobe16(time_mid);
    *((uint16_t*)&at(6)) = htobe16(time_hi_and_version);
    *((uint64_t*)&at(8)) = htobe64(node);
    *((uint16_t*)&at(8)) = htobe16(clock_seq & 0x3FFF | 0x8000);
}

uuid::uuid(uint64_t time, version_t version, uint16_t clock_seq, uint64_t node):
uuid(    time & 0xFFFFFFFF, // time_low
        (time >> 32) & 0xFFFF, // time_mid
        (time >> 48) & 0x0FFF | ((uint16_t)version << 12), // time_hi_and_version
        clock_seq, // clock_seq
        node
    )
{
}

int uuid::compare(uuid const & other) const
{
    for(size_t n=0; n<16; ++n)
    {
        if(at(n)!=other.at(n))
        {
            return at(n)<other.at(n) ? -n : n;
        }
    }
    return 0;
}

uuid uuid::version1(uint64_t timestamp, uint16_t clock_seq, uint64_t mac_address)
{
    return uuid(timestamp, version_t::version_time_based, clock_seq, mac_address);
}

uuid uuid::version1(uint64_t timestamp, uint16_t clock_seq, const std::array<uint8_t,6>& mac_address)
{
    uint64_t node = *(uint64_t*)mac_address.data() << 16;
    return uuid(timestamp, version_t::version_time_based, clock_seq, be64toh(node));
}

uuid uuid::version4()
{
    static std::random_device rd;  //Will be used to obtain a seed for the random number engine
    static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    static std::uniform_int_distribution<short> dis(0, 255);

    parent_t src;
    for(size_t n=0; n<16; ++n)
    {
        src.at(n) = (uint8_t)dis(gen);
    }

    src.at(8) = src.at(8) & 0x3F | 0x80; // variant
    src.at(6) = src.at(6) & 0x0F | 0x40; // version

    return uuid(src);
}

uuid uuid::version3(uuid ns, const void* name, size_t name_len)
{
    uuid res;
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, ns.data(), ns.size());
    MD5_Update(&md5, (const uint8_t*)name, name_len);
    MD5_Final(res.data(), &md5);
    res.at(8) = res.at(8) & 0x3F | 0x80; // variant
    res.at(6) = res.at(6) & 0x0F | 0x30; // version
    return res;
}

uuid uuid::version5(uuid ns, const void* name, size_t name_len)
{
    uuid res;
    uint8_t buffer[20];
    SHA1_CTX sha1;
    SHA1_Init(&sha1);
    SHA1_Update(&sha1, ns.data(), ns.size());
    SHA1_Update(&sha1, (const uint8_t*)name, name_len);
    SHA1_Final(buffer, &sha1);
    std::copy(buffer, buffer+16, res.data());
    res.at(8) = res.at(8) & 0x3F | 0x80; // variant
    res.at(6) = res.at(6) & 0x0F | 0x50; // version
    return res;
}


std::string uuid::to_hex() const
{
    std::string res(16*2, 0);
    for(size_t n=0, m=0; n<16*2;)
    {
        res[n]   = hex[(at(m)&0xF0u)>>4];
        res[++n] = hex[(at(m)&0x0Fu)];
        ++n;
        ++m;
    }
    return res;
}

std::string uuid::to_string() const
{
    std::string res(16*2+4, 0);
    res[8] = res[13] = res[18] = res[23] = '-';

    for(size_t n=0, m=0; n<16*2+4;)
    {
        if (res[n] != '-')
        {
            res[n]   = hex[(at(m)&0xF0u)>>4];
            res[++n] = hex[(at(m)&0x0Fu)];
            ++m;
        }
        ++n;
    }
    return res;
}

std::string uuid::to_msguid() const
{
    std::string res(16*2+4+2, 0);
    res[8+1] = res[13+1] = res[18+1] = res[23+1] = '-';
    res[0]  = '{';
    res[37] = '}';

    for(size_t n=1, m=0; n<16*2+4+2-1;)
    {
        if (res[n] != '-')
        {
            res[n]   = hex[(at(m)&0xF0u)>>4];
            res[++n] = hex[(at(m)&0x0Fu)];
            ++m;
        }
        ++n;
    }
    return res;
}

std::string uuid::to_urn() const
{
    return "urn:uuid:" + to_string();
}


namespace uuid_ns
{
const uuid dns = uuid{{
    0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
    0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
}};

const uuid url = uuid{{
    0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
    0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
}};

const uuid oid = uuid{{
    0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1,
    0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
}};

const uuid x500 = uuid{{
    0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1,
    0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
}};

}

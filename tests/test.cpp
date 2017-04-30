/* -*- Mode: C++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * test.cpp
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

#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "uuidpp.hpp"


TEST_CASE("UUID nil constructor", "[UUID]")
{
    uuid id;
    REQUIRE(id.nil());
    REQUIRE(!id);
}

TEST_CASE("UUID content constructor", "[UUID]")
{
     std::array<uint8_t, 16> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uuid id{values};

    REQUIRE(!id.nil());
    REQUIRE(id);
    for(size_t n = 0; n<values.size(); ++n)
    {
        REQUIRE(id[n]==values[n]);
    }
}

TEST_CASE("UUID container constructor", "[UUID]")
{
    std::vector<uint8_t> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uuid id{values};

    REQUIRE(id);
    for(size_t n = 0; n<values.size(); ++n)
    {
        REQUIRE(id[n]==values[n]);
    }
}

TEST_CASE("UUID iterator constructor", "[UUID]")
{
    std::vector<uint8_t> values{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uuid id{values.begin(), values.end()};

    REQUIRE(id);
    for(size_t n = 0; n<values.size(); ++n)
    {
        REQUIRE(id[n]==values[n]);
    }
}

TEST_CASE("UUID other content constructor", "[UUID]")
{
    uuid id{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};

    REQUIRE(!id.nil());
    REQUIRE(id);

    uuid id2{id};
    REQUIRE(!id2.nil());
    REQUIRE(id2);
    for(size_t n = 0; n<id.size(); ++n)
    {
        REQUIRE(id2[n]==id[n]);
    }
}

TEST_CASE("UUID equality operator", "[UUID]")
{
    uuid id1{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    uuid id2{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    uuid id3{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0}};

    REQUIRE(id1==id2);
    REQUIRE(!(id1==id3));
}

TEST_CASE("UUID less-than operator", "[UUID]")
{
    uuid id1{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    uuid id2{{0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 10, 11, 12, 13, 14, 15}};
    uuid id3{{0, 1, 2, 3, 4, 5, 6, 7, 7, 9, 10, 11, 12, 13, 14, 15}};

    REQUIRE(id1<id2);
    REQUIRE(!(id1<id3));
}

TEST_CASE("UUID assignation", "[UUID]")
{
    uuid id{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    uuid id2;

    REQUIRE(!id2);
    id2 = id;
    REQUIRE(id2);
    REQUIRE(id2==id);
}

TEST_CASE("UUID msb-lsb construction", "[UUID]")
{
    uuid id1((uint64_t)0x0001020304050607ull, (uint64_t)0x08090A0B0C0D0E0Full);
    uuid id2{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id1==id2);
}

TEST_CASE("UUID decomposed construction", "[UUID]")
{
    uuid id1(0x00010203ul, 0x0405u, 0x0607u, 0x0809u, 0x0A0B0C0D0E0Full);
    uuid id2{{0, 1, 2, 3, 4, 5, 6, 7, 0x88, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id1==id2);
}

TEST_CASE("UUID generic composed construction", "[UUID]")
{
    uuid id1(0x0607040500010203ull, uuid::version_t::version_unknown, 0x0809u, 0x0A0B0C0D0E0Full);
    uuid id2{{0, 1, 2, 3, 4, 5, 6, 7, 0x88, 9, 10, 11, 12, 13, 14, 15}}; // 0x88 Because of RFC4122 variant only
    REQUIRE(id1==id2);
}

TEST_CASE("UUID compact hex string format", "[UUID]")
{
    uuid id{{0xF0, 1, 0x82, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id.to_hex() == "f00182030405060708090a0b0c0d0e0f");
}

TEST_CASE("UUID string format", "[UUID]")
{
    uuid id{{0xF0, 1, 0x82, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id.to_string() == "f0018203-0405-0607-0809-0a0b0c0d0e0f");
}

TEST_CASE("UUID MS GUID string format", "[UUID]")
{
    uuid id{{0xF0, 1, 0x82, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id.to_msguid() == "{f0018203-0405-0607-0809-0a0b0c0d0e0f}");
}

TEST_CASE("UUID URN string format", "[UUID]")
{
    uuid id{{0xF0, 1, 0x82, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
    REQUIRE(id.to_urn() == "urn:uuid:f0018203-0405-0607-0809-0a0b0c0d0e0f");
}

TEST_CASE("UUID version 1", "[UUID]")
{
    uuid id1 = uuid::version1(0, 0, 0x0123456789ABull);
    uuid id2 = uuid::version1(0, 0, {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB});
    REQUIRE(id1.version()==uuid::version_t::version_time_based);
    REQUIRE(id2.version()==uuid::version_t::version_time_based);
    REQUIRE(id1.variant()==uuid::variant_t::variant_rfc4122);
    REQUIRE(id2.variant()==uuid::variant_t::variant_rfc4122);
    REQUIRE(id1==id2);
}

TEST_CASE("UUID version 4", "[UUID]")
{
    uuid id = uuid::version4();
    REQUIRE(id.version()==uuid::version_t::version_random);
    REQUIRE(id.variant()==uuid::variant_t::variant_rfc4122);
}

TEST_CASE("UUID version 3 dns", "[UUID]")
{
    uuid id = uuid::version3(uuid_ns::dns, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="b21f1f88-e52f-3026-8f6a-219d5b420288");

    uuid id2 = uuid::version3(uuid_ns::dns, std::string("0123456789ABCDEF"));
    REQUIRE(id2.to_string()=="b21f1f88-e52f-3026-8f6a-219d5b420288");

    uuid id3 = uuid::version3(uuid_ns::dns, "0123456789ABCDEF");
    REQUIRE(id3.to_string()=="b21f1f88-e52f-3026-8f6a-219d5b420288");

    const char* text = "0123456789ABCDEF";
    uuid id4 = uuid::version3(uuid_ns::dns, text, text+16);
    REQUIRE(id4.to_string()=="b21f1f88-e52f-3026-8f6a-219d5b420288");
}

TEST_CASE("UUID version 3 url", "[UUID]")
{
    uuid id = uuid::version3(uuid_ns::url, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="a1f5b2a5-8a32-3919-8cf2-2c10e49ea9d1");
}

TEST_CASE("UUID version 3 oid", "[UUID]")
{
    uuid id = uuid::version3(uuid_ns::oid, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="c0d80567-feac-32c0-87f1-b22c420bea66");
}

TEST_CASE("UUID version 3 x500", "[UUID]")
{
    uuid id = uuid::version3(uuid_ns::x500, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="e7d423b4-af73-3ee7-90f8-9ba99ea52630");
}

TEST_CASE("UUID version 5 dns", "[UUID]")
{
    uuid id = uuid::version5(uuid_ns::dns, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="a1075986-c842-5f1a-80b2-142154f0fad6");
}

TEST_CASE("UUID version 5 url", "[UUID]")
{
    uuid id = uuid::version5(uuid_ns::url, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="0810486f-6367-537e-bb4e-4371721bc7d1");
}

TEST_CASE("UUID version 5 oid", "[UUID]")
{
    uuid id = uuid::version5(uuid_ns::oid, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="6d085a01-5c20-5caa-8e42-12537cceb286");
}

TEST_CASE("UUID version 5 x500", "[UUID]")
{
    uuid id = uuid::version5(uuid_ns::x500, "0123456789ABCDEF", 16);
    REQUIRE(id.to_string()=="e5654925-c85c-5039-83b2-1ed5420939e5");
}

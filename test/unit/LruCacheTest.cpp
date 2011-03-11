/*
 * Copyright (c) 2010, Tim Day <timday@timday.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// This code is from http://www.bottlenose.demon.co.uk/article/lru.htm.  It is
// under a Internet Systems Consortium (ISC) license (an OSI-approved BSD-alike license).

#include <boost/test/unit_test.hpp>

#include "libpc/PointData.hpp"
#include "libpc/LruCache.hpp"

using namespace libpc;

static size_t count_evaluations=0; 
 
// Dummy function we want to cache 
std::string fn(const std::string& s) 
{ 
  ++count_evaluations; 
  std::string r; 
  std::copy(s.rbegin(),s.rend(),std::back_inserter(r)); 
  return r; 
} 

BOOST_AUTO_TEST_SUITE(LruCacheTest)

BOOST_AUTO_TEST_CASE(test1)
{
    Schema schema;
    Dimension d1(Dimension::Field_X, Dimension::Uint32);
    schema.addDimension(d1);
    SchemaLayout layout(schema);

    PointData* item0 = new PointData(layout, 10);
    PointData* item00 = new PointData(layout, 10);
    PointData* item1 = new PointData(layout, 10);
    PointData* item11 = new PointData(layout, 10);
    PointData* item2 = new PointData(layout, 10);
    PointData* item22 = new PointData(layout, 10);

    // write the data into the buffer
    for (int i=0; i<10; i++)
    {
      item0->setField(i, 0, i);
      item00->setField(i, 0, i);
      item1->setField(i, 0, i+10);
      item11->setField(i, 0, i+10);
      item2->setField(i, 0, i+20);
      item22->setField(i, 0, i+20);
    }

    LruCache lru(2);

    // bunch of insertions/lookups
    lru.insert(0,item0);
    lru.insert(0,item0);
    lru.insert(10,item1);
    lru.insert(10,item1);
    lru.insert(10,item0);
    lru.insert(0,item0);
    lru.insert(20,item2);
    lru.insert(20,item2);
    lru.insert(0,item0);

    BOOST_CHECK(lru.lookup(0) == item0);
    BOOST_CHECK(lru.lookup(10) == NULL);
    BOOST_CHECK(lru.lookup(20) == item2);
     
    { 
        std::vector<boost::uint32_t> actual; 
        lru.get_keys(std::back_inserter(actual)); 
        BOOST_CHECK(actual.size() == 2); 
        BOOST_CHECK(actual[0] == 0);
        BOOST_CHECK(actual[1] == 20);
    }

    lru.insert(0,item00);
    lru.insert(20,item22);
     
    { 
        std::vector<boost::uint32_t> actual; 
        lru.get_keys(std::back_inserter(actual)); 
        BOOST_CHECK(actual.size() == 2); 
        BOOST_CHECK(actual[0] == 20);
        BOOST_CHECK(actual[1] == 0);
    }

    BOOST_CHECK(lru.lookup(0) == item0);
    BOOST_CHECK(lru.lookup(10) == NULL);
    BOOST_CHECK(lru.lookup(20) == item2);

    //delete item0;
    delete item00;
    delete item1;
    delete item11;
    //delete item2;
    delete item22;

    return;
}


BOOST_AUTO_TEST_SUITE_END()

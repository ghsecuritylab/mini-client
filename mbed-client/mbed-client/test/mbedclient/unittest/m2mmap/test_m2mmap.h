/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TEST_M2M_MAP_H
#define TEST_M2M_MAP_H

#include "m2mmap.h"
#include "m2mstring.h"

using namespace m2m;

class Test_M2MMap
{
public:
    Test_M2MMap();
    virtual ~Test_M2MMap();

    void test_operator_array();
    void test_size();
    void test_empty();
    void test_capacity();
    void test_erase();
    void test_find();
};

#endif // TEST_M2M_MAP_H
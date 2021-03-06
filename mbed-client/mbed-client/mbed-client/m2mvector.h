/*
 * Copyright (c) 2015-2016 ARM Limited. All rights reserved.
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

#ifndef M2M_VECTOR_H
#define M2M_VECTOR_H

#include <assert.h>
#include "mbed-client/m2mcorememory.h"

/*! \file m2mvector.h
* \brief A simple C++ Vector class, used as replacement for std::vector.
*/


namespace m2m
{

template <class ObjectTemplate>

class Vector /*: public M2MCoreMemory*/
{
  public:
    explicit Vector( int init_size = MIN_CAPACITY)
            : _size(0),
              _capacity((init_size >= MIN_CAPACITY) ? init_size : MIN_CAPACITY) {
#ifdef MBED_CLIENT_DYNMEM_LIB
        _object_template = (ObjectTemplate*)M2MCoreMemory::memory_alloc(_capacity * sizeof(ObjectTemplate));
#else
        _object_template = new ObjectTemplate[ _capacity ];
#endif

    }

    Vector(const Vector & rhs ): _object_template(0) {
        operator=(rhs);
    }

    ~Vector() {
#ifdef MBED_CLIENT_DYNMEM_LIB
        if(_object_template) {
            M2MCoreMemory::memory_free(_object_template);
        }
        _object_template = 0;
#else
        delete [] _object_template;
#endif
    }

    const Vector & operator=(const Vector & rhs) {
        if(this != &rhs) {
#ifdef MBED_CLIENT_DYNMEM_LIB
            if(_object_template) {
                M2MCoreMemory::memory_free(_object_template);
                _object_template = 0;
            }
#else
            delete[] _object_template;
#endif
            _size = rhs.size();
            _capacity = rhs._capacity;

#ifdef MBED_CLIENT_DYNMEM_LIB
            _object_template = (ObjectTemplate*)M2MCoreMemory::memory_alloc(capacity() * sizeof(ObjectTemplate));
#else
            _object_template = new ObjectTemplate[capacity()];
#endif
            for(int k = 0; k < size(); k++) {
                _object_template[k] = rhs._object_template[k];
            }
        }
        return *this;
    }

    void resize(int new_size) {
        if(new_size > _capacity) {
            reserve(new_size * 2 + 1);
        }
        _size = new_size;
    }

    void reserve(int new_capacity) {
        if(new_capacity < _size) {
            return;
        }
        ObjectTemplate *old_array = _object_template;

#ifdef MBED_CLIENT_DYNMEM_LIB
        _object_template = (ObjectTemplate*)M2MCoreMemory::memory_alloc(new_capacity * sizeof(ObjectTemplate));
#else
        _object_template = new ObjectTemplate[new_capacity];
#endif
        for(int k = 0; k < _size; k++) {
            _object_template[k] = old_array[k];
        }
        _capacity = new_capacity;
#ifdef MBED_CLIENT_DYNMEM_LIB
        M2MCoreMemory::memory_free(old_array);
        old_array = 0;
#else
        delete [] old_array;
#endif
    }

    ObjectTemplate & operator[](int idx) {
        return _object_template[idx];
    }

    const ObjectTemplate& operator[](int idx) const {
        return _object_template[idx];
    }

    bool empty() const{
        return size() == 0;
    }

    int size() const {
        return _size;
    }

    int capacity() const {
        return _capacity;
    }

    void push_back(const ObjectTemplate& x) {
#ifdef MBED_CLIENT_DYNMEM_LIB
        if(!_object_template && _size == 0) {
            _object_template = (ObjectTemplate*)M2MCoreMemory::memory_alloc(_capacity * sizeof(ObjectTemplate));            
        }
#endif
        if(_size == _capacity) {
            reserve(2 * _capacity + 1);
        }
        _object_template[_size] = x;
        _size++;
    }

    void pop_back() {
        _size--;
    }

    void clear() {
        _size = 0;
    }

    const ObjectTemplate& back() const {
        return _object_template[_size - 1];
    }

    typedef ObjectTemplate* iterator;
    typedef const ObjectTemplate* const_iterator;

    iterator begin() {
        return &_object_template[0];
    }

    const_iterator begin() const {
        return &_object_template[0];
    }

    iterator end() {
        return &_object_template[_size];
    }

    const_iterator end() const {
        return &_object_template[_size];
    }

    void erase(int position) {
        if(position < _size) {
            for(int k = position; k + 1 < _size; k++) {
                _object_template[k] = _object_template[k + 1];
            }
            _size--;
        }
    }

    enum {
        MIN_CAPACITY = 1
    };

  private:
    int                 _size;
    int                 _capacity;
    ObjectTemplate*     _object_template;
};

} // namespace

#endif // M2M_VECTOR_H

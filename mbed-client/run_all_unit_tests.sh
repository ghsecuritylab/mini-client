#!/bin/bash
# Copyright (c) 2015 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# * http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

echo
echo "Running mbed-client unit tests"
echo
cd mbed-client
make -f Makefile.test clean
make -f Makefile.test clone
make -f Makefile.test test

echo
echo "Running mbed-client-c unit tests"
echo
cd mbed-client-c
make -f Makefile.test clean
make -f Makefile.test clone
make -f Makefile.test test
cd ..

echo
echo "Running mbed-client-classic unit tests"
echo
cd mbed-client-classic
make -f Makefile.test clean
make -f Makefile.test clone
make -f Makefile.test test
cd ..

echo
echo "Running mbed-client-mbed-tls unit tests"
echo
cd mbed-client-mbed-tls
make -f Makefile.test clean
make -f Makefile.test clone
make -f Makefile.test test
cd ..

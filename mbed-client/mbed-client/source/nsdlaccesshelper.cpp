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
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlinterface.h"
#include "mbed-client/m2mcorememory.h"

#include <stdlib.h>

// callback function for NSDL library to call into
uint8_t __nsdl_c_callback(struct nsdl_s *nsdl_handle,
                          sn_coap_hdr_s *received_coap_ptr,
                          sn_nsdl_addr_s *address,
                          sn_nsdl_capab_e nsdl_capab)
{
    uint8_t status = 0;
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        status = interface->resource_callback(nsdl_handle,received_coap_ptr,
                                                     address, nsdl_capab);
        // Payload freeing must be done in app level if blockwise message
        if (received_coap_ptr->coap_status == COAP_STATUS_PARSER_BLOCKWISE_MSG_RECEIVED) {
            M2MCoreMemory::memory_free(received_coap_ptr->payload_ptr);
            received_coap_ptr->payload_ptr = NULL;
        }
    }
    return status;
}

void* __nsdl_c_memory_alloc(uint16_t size)
{
    if(size)
        return M2MCoreMemory::memory_alloc(size);
    else
        return 0;
}

void __nsdl_c_memory_free(void *ptr)
{
    if(ptr)
        M2MCoreMemory::memory_free(ptr);
}

uint8_t __nsdl_c_send_to_server(struct nsdl_s * nsdl_handle,
                                sn_nsdl_capab_e protocol,
                                uint8_t *data_ptr,
                                uint16_t data_len,
                                sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        status = interface->send_to_server_callback(nsdl_handle,
                                                           protocol, data_ptr,
                                                           data_len, address_ptr);
    }
    return status;
}

uint8_t __nsdl_c_received_from_server(struct nsdl_s * nsdl_handle,
                                      sn_coap_hdr_s *coap_header,
                                      sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        status = interface->received_from_server_callback(nsdl_handle,
                                                                 coap_header,
                                                                 address_ptr);
        // Payload freeing must be done in app level if blockwise message
        if (coap_header &&
                coap_header->options_list_ptr &&
                coap_header->options_list_ptr->block1 != -1) {
            M2MCoreMemory::memory_free(coap_header->payload_ptr);
            coap_header->payload_ptr = NULL;
        }
    }
    return status;
}

sn_nsdl_dynamic_resource_parameters_s *__get_first(struct nsdl_s * nsdl_handle,
                                                   struct iterator_state_s** state,
                                                   char **path)
{
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        return interface->get_first(state, path);
    }
    return NULL;
}

sn_nsdl_dynamic_resource_parameters_s *__get_next(struct nsdl_s * nsdl_handle,
                                                  struct iterator_state_s** state,
                                                  char **path)
{
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        return interface->get_next(state, path);
    }
    return NULL;
}

sn_nsdl_dynamic_resource_parameters_s* __find_resource(struct nsdl_s * nsdl_handle, const char *path)
{
    M2MNsdlInterface *interface = (M2MNsdlInterface*)sn_nsdl_get_context(nsdl_handle);
    if(interface) {
        return interface->find_resource(path);
    }
    return NULL;
}

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
#include "CppUTest/TestHarness.h"
#include "test_m2mnsdlinterface.h"
#include "common_stub.h"
#include "m2mdevice_stub.h"
#include "m2msecurity_stub.h"
#include "m2mnsdlobserver.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mresourcebase_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mresource.h"
#include "m2mbase_stub.h"
#include "m2mserver.h"
#include "m2msecurity.h"
#include "m2mresourceinstance.h"
#include "m2mconnectionsecurity.h"
#include "m2mconstants.h"
#include "m2mtlvdeserializer_stub.h"
#include "uriqueryparser_stub.h"
#include "m2mconnectionhandler_stub.h"
#include "m2mconnectionsecurity_stub.h"
#include "m2mblockmessage_stub.h"

class TestObserver : public M2MNsdlObserver,
                     public M2MConnectionObserver {

public:
    TestObserver(){}
    virtual ~TestObserver(){}
    void coap_message_ready(uint8_t *,
                            uint16_t,
                            sn_nsdl_addr_s *){
        message_ready = true;
    }

    void client_registered(M2MServer *){
        registered = true;
    }

    void registration_updated(const M2MServer &){
        register_updated = true;
    }

    void registration_error(uint8_t, bool retry = false){
        register_error = true;
    }

    void client_unregistered(){
        unregistered = true;
    }

    void bootstrap_done(M2MSecurity *sec){
        if(sec) {
            boot_done = true;
            delete sec;
            sec = NULL;
        }
    }

    void bootstrap_wait(M2MSecurity *sec){
        if(sec) {
            boot_wait = true;
            delete sec;
            sec = NULL;
        }
    }

    void bootstrap_error_wait(const char* /*reason*/){
        boot_error = true;
    }

    void bootstrap_error(const char */*reason*/){
        boot_error = true;
    }

    void coap_data_processed(){
        data_processed = true;
    }

    void value_updated(M2MBase *){
        value_update = true;
    }

    void data_available(uint8_t* data,
                        uint16_t data_size,
                        const M2MConnectionObserver::SocketAddress &address){

    }

    void socket_error(uint8_t error_code, bool retry = true){

    }

    void address_ready(const M2MConnectionObserver::SocketAddress &address,
                                   M2MConnectionObserver::ServerType server_type,
                                   const uint16_t server_port){

    }

    void data_sent(){

    }

    bool register_error;
    bool boot_error;
    bool boot_wait;
    bool boot_done;
    bool registered;
    bool register_updated;
    bool data_processed;
    bool unregistered;
    bool message_ready;
    bool value_update;
};

struct nsdl_s {
    uint16_t update_register_msg_id;
    uint16_t register_msg_len;
    uint16_t update_register_msg_len;

    uint16_t register_msg_id;
    uint16_t unregister_msg_id;

    uint16_t bootstrap_msg_id;
    uint16_t oma_bs_port;                                                       /* Bootstrap port */
    uint8_t oma_bs_address_len;                                                 /* Bootstrap address length */
    unsigned int sn_nsdl_endpoint_registered:1;
    bool handle_bootstrap_msg:1;

    struct grs_s *grs;
    uint8_t *oma_bs_address_ptr;                                                /* Bootstrap address pointer. If null, no bootstrap in use */
    sn_nsdl_ep_parameters_s *ep_information_ptr;                                // Endpoint parameters, Name, Domain etc..
    sn_nsdl_oma_server_info_t *nsp_address_ptr;                                 // NSP server address information

    void (*sn_nsdl_oma_bs_done_cb)(sn_nsdl_oma_server_info_t *server_info_ptr); /* Callback to inform application when bootstrap is done */
    void *(*sn_nsdl_alloc)(uint16_t);
    void (*sn_nsdl_free)(void *);
    uint8_t (*sn_nsdl_tx_callback)(struct nsdl_s *, sn_nsdl_capab_e , uint8_t *, uint16_t, sn_nsdl_addr_s *);
    uint8_t (*sn_nsdl_rx_callback)(struct nsdl_s *, sn_coap_hdr_s *, sn_nsdl_addr_s *);
    void (*sn_nsdl_oma_bs_done_cb_handle)(sn_nsdl_oma_server_info_t *server_info_ptr,
                                          struct nsdl_s *handle); /* Callback to inform application when bootstrap is done with nsdl handle */
};

Test_M2MNsdlInterface::Test_M2MNsdlInterface()
{
    observer = new TestObserver();
    connection_handler = new M2MConnectionHandler(*observer, NULL, M2MInterface::NOT_SET, M2MInterface::Uninitialized);
    nsdl = new M2MNsdlInterface(*observer, *connection_handler);
}

Test_M2MNsdlInterface:: ~Test_M2MNsdlInterface()
{
    delete nsdl;
    nsdl = NULL;
    delete connection_handler;
    connection_handler = NULL;
    delete observer;
    observer = NULL;
}

void Test_M2MNsdlInterface::test_create_endpoint()
{
    u_int8_t value[]  = {"120"};
    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }

    nsdl->create_endpoint("name", "type",120,"domain",100,"context");
    CHECK(nsdl->_endpoint->lifetime_len == 3);
    CHECK(*nsdl->_endpoint->lifetime_ptr == *value);
}

void Test_M2MNsdlInterface::test_delete_endpoint()
{
    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(uint8_t));

    nsdl->delete_endpoint();
    CHECK(nsdl->_endpoint == NULL);
}

void Test_M2MNsdlInterface::test_create_nsdl_list_structure()
{
    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::int_value = 0;
    M2MObject *object = new M2MObject(0, "0");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 1, "", "0/1");

    M2MResource* create_resource = new M2MResource(*instance,
                                                   2,
                                                    M2MBase::Dynamic,
                                                   "res_type",
                                                   M2MBase::INTEGER,
                                                   false,
                                                   (char*)"0/1/2");

    M2MResourceInstance* res_instance = new M2MResourceInstance(*create_resource, 3, M2MBase::Dynamic, "res_inst_type",
                                       M2MBase::INTEGER, "0/1/2/3", false, false);

    m2mobject_stub::instance_list.clear();
    m2mobject_stub::instance_list.push_back(instance);
    instance->add_resource(create_resource);

    M2MObjectList list;
    list.push_back(object);

    m2mbase_stub::string_value = name->c_str();
    m2mbase_stub::mode_value = M2MBase::Static;

    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    m2mresource_stub::bool_value = true;
    m2mbase_stub::mode_value = M2MBase::Dynamic;

    create_resource->add_resource_instance(res_instance);
    m2mbase_stub::base_type = M2MBase::Resource;

    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    m2mbase_stub::mode_value = M2MBase::Directory;
    CHECK(nsdl->create_nsdl_list_structure(list)== true);

    list.clear();
    delete object;
    delete name;
    m2mobject_stub::instance_list.clear();
    delete instance;
    delete create_resource;

    delete res_instance;
}

void Test_M2MNsdlInterface::test_delete_nsdl_resource()
{
    common_stub::int_value = 0;
    M2MObject *object = new M2MObject(2, "name");

    CHECK(nsdl->remove_nsdl_resource(object) == false);

    common_stub::int_value = 1;

    CHECK(nsdl->remove_nsdl_resource(object) == true);
    delete object;
}

void Test_M2MNsdlInterface::test_create_bootstrap_resource()
{
    common_stub::uint_value = 11;
    CHECK(nsdl->create_bootstrap_resource(NULL) == true);

    const char address[] = "coap://127.0.0.1:5683?param=1&param2=2&param3=3";
    uriqueryparser_stub::int_value = 3;
    nsdl->_bootstrap_id = 0;
    uriqueryparser_stub::bool_value = true;
    nsdl->set_server_address(address);
    CHECK(nsdl->create_bootstrap_resource(NULL) == true);

    common_stub::uint_value = 0;
    CHECK(nsdl->create_bootstrap_resource(NULL) == false);

    // Query param count set to 0
    nsdl->_bootstrap_id = 0;
    uriqueryparser_stub::int_value = 0;
    CHECK(nsdl->create_bootstrap_resource(NULL) == false);
}

void Test_M2MNsdlInterface::test_set_server_address()
{
    // to cover the test coverage
    nsdl->set_server_address(NULL,4,100,SN_NSDL_ADDRESS_TYPE_IPV6);
}

void Test_M2MNsdlInterface::test_send_register_message()
{
    common_stub::uint_value = 12;
    common_stub::int_value = 0;
    nsdl->set_server_address(NULL,4,100,SN_NSDL_ADDRESS_TYPE_IPV6);
    CHECK(nsdl->send_register_message() == true);

    common_stub::uint_value = 0;
    nsdl->set_server_address(NULL,4,100,SN_NSDL_ADDRESS_TYPE_IPV6);

    const char address[] = "coap://127.0.0.1:5683?param=1&param2=2&param3=3";
    uriqueryparser_stub::int_value = 3;
    nsdl->set_server_address(address);

    CHECK(nsdl->send_register_message() == false);

    common_stub::uint_value = 10;
    CHECK(nsdl->send_register_message() == true);

    // Query param count set to 0
    uriqueryparser_stub::int_value = 0;
    CHECK(nsdl->send_register_message() == true);
}

void Test_M2MNsdlInterface::test_send_update_registration()
{
    common_stub::uint_value = 23;
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(1));
    CHECK(nsdl->send_update_registration(120, true) == true);

    // Update lifetime value
    common_stub::uint_value = 1;
    CHECK(nsdl->send_update_registration(100, true) == true);

    // Lifetime value is 0, don't change the existing lifetime value
    common_stub::uint_value = 1;
    CHECK(nsdl->send_update_registration(0, true) == true);

    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_send_unregister_message()
{
    common_stub::uint_value = 22;
    CHECK(nsdl->send_unregister_message() == true);

    // Unreg already in progress
    common_stub::uint_value = 0;
    CHECK(nsdl->send_unregister_message() == true);
}

void Test_M2MNsdlInterface::test_memory_alloc()
{
    CHECK(nsdl->memory_alloc(0) == 0);
    uint8_t *ptr = 0;
    ptr = (uint8_t*)nsdl->memory_alloc(sizeof(uint8_t));
    CHECK(ptr != NULL);
    nsdl->memory_free(ptr);
}

void Test_M2MNsdlInterface::test_memory_free()
{
    uint8_t *ptr = (uint8_t*)nsdl->memory_alloc(sizeof(uint8_t));
    nsdl->memory_free((void*)ptr);
    //memory leak test will fail, if there is a leak, so no need for CHECK
}

void Test_M2MNsdlInterface::test_send_to_server_callback()
{
  uint8_t *data_ptr = (uint8_t*)malloc(sizeof(uint8_t));
  uint16_t data_len = sizeof(uint8_t);
  sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));

  nsdl->send_to_server_callback(NULL, SN_NSDL_PROTOCOL_COAP, data_ptr,data_len,address);
  CHECK(observer->message_ready == true);

  free(data_ptr);
  free(address);
}

void Test_M2MNsdlInterface::test_received_from_server_callback()
{
    nsdl_s* handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(handle,0,sizeof(nsdl_s));

    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;


    coap_header->options_list_ptr = (sn_coap_options_list_s *)malloc(sizeof(sn_coap_options_list_s));
    memset(coap_header->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

    coap_header->options_list_ptr->max_age = 61;

    coap_header->options_list_ptr->location_path_len = 2;
    coap_header->options_list_ptr->location_path_ptr = (uint8_t *)malloc(sizeof(coap_header->options_list_ptr->location_path_len));
    memset(coap_header->options_list_ptr->location_path_ptr, 0, sizeof(coap_header->options_list_ptr->location_path_len));

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    free(nsdl->_endpoint->location_ptr);
    nsdl->_endpoint->location_ptr = NULL;

    uint8_t life1[] = {"120"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(life1));
    memcpy(nsdl->_endpoint->lifetime_ptr,life1,sizeof(life1));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(life1);

    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(coap_header->options_list_ptr->location_path_ptr);
    coap_header->options_list_ptr->location_path_ptr = NULL;

    free(coap_header->options_list_ptr);
    coap_header->options_list_ptr = NULL;

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    uint8_t life[] = {"120"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(life));
    memcpy(nsdl->_endpoint->lifetime_ptr,life,sizeof(life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(life);

    observer->data_processed = false;
    observer->registered = false;

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);
    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    uint8_t big_life[] = {"4000"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(big_life));
    memcpy(nsdl->_endpoint->lifetime_ptr,big_life,sizeof(big_life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(big_life);
    observer->data_processed = false;
    observer->registered = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    free(nsdl->_endpoint->lifetime_ptr);
    nsdl->_endpoint->lifetime_ptr = NULL;

    observer->data_processed = false;
    observer->registered = false;

    uint8_t less_life[] = {"30"};
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(less_life));
    memcpy(nsdl->_endpoint->lifetime_ptr,less_life,sizeof(less_life));
    nsdl->_endpoint->lifetime_len = (uint8_t)sizeof(less_life);

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->registered == true);

    observer->data_processed = false;
    observer->registered = false;
    observer->unregistered = false;
    observer->register_error = false;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_OPTION;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_INCOMPLETE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_PRECONDITION_FAILED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_UNAUTHORIZED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_GATEWAY;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->data_processed == true);
    CHECK(observer->register_error == true);


    coap_header->msg_id = 8;
    handle->unregister_msg_id = 8;

    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_DELETED;
    observer->register_error = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->unregistered == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;

    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    observer->register_error = false;
    handle->unregister_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_GATEWAY_TIMEOUT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    // Receive initial bs message with error
    handle->unregister_msg_id = 0;
    observer->boot_error = false;
    nsdl->_bootstrap_id = 8;
    handle->bootstrap_msg_id = 8;
    coap_header->coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;
    uint8_t data[] = {'e', 'r', 'r'};
    coap_header->payload_ptr = data;
    coap_header->payload_len = sizeof(data);
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    CHECK(observer->boot_error == true);

    // Receive initial bs message with error
    observer->boot_error = false;
    coap_header->coap_status = COAP_STATUS_OK;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_OPTION;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->boot_error == true);

    // Receive initial bs message with error
    observer->boot_error = false;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->boot_error == true);

    coap_header->payload_len = 0;
    coap_header->payload_ptr = 0;

    //_update_id == msg_id
    handle->update_register_msg_id = 10;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_updated == true);

    const char server_address[] = "coap://127.0.0.1:5683?param=1&param2=2&param3=3";
    nsdl->set_server_address(server_address);
    uriqueryparser_stub::bool_value = true;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    coap_header->coap_status = COAP_STATUS_OK;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    uriqueryparser_stub::bool_value = true;
    coap_header->msg_id = 10;
    nsdl->_binding_mode = M2MInterface::UDP;
    coap_header->coap_status = COAP_STATUS_OK;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    uriqueryparser_stub::bool_value = false;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    CHECK(observer->register_error == true);

    coap_header->msg_id = 11;
    CHECK( 0== nsdl->received_from_server_callback(handle,coap_header,NULL) );

    handle->update_register_msg_id = 0;
    handle->register_msg_id = 0;
    handle->unregister_msg_id = 0;
    coap_header->msg_id = 10;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    uint8_t object[] = {"name"};

    coap_header->uri_path_ptr = object;
    coap_header->uri_path_len = sizeof(object);

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    //Test methods that are not allowed
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    nsdl->received_from_server_callback(handle,coap_header,NULL);

    //Continue testing with post method
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    uint8_t object_instance[] = {"name/0"};

    coap_header->uri_path_ptr = object_instance;
    coap_header->uri_path_len = sizeof(object_instance);

    coap_header->payload_ptr = (uint8_t*)malloc(1);
    m2mobjectinstance_stub::bool_value = true;
    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    M2MObject *obj = new M2MObject(0, "name");

    m2mbase_stub::string_value = "name";
    sn_nsdl_dynamic_resource_parameters_s *nsdl_resource;

    nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    nsdl_resource->path = (char*)malloc(5);
    nsdl_resource->path[0] = 'n';
    nsdl_resource->path[1] = 'a';
    nsdl_resource->path[2] = 'm';
    nsdl_resource->path[3] = 'e';
    nsdl_resource->path[4] = '\0';

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name", "", "");

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));
    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    common_stub::coap_header = NULL;

    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobjectinstance_stub::header = NULL;

    m2mobjectinstance_stub::header =  (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobjectinstance_stub::header, 0, sizeof(sn_coap_hdr_s));

    m2mobjectinstance_stub::header->msg_code = COAP_MSG_CODE_RESPONSE_CREATED;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mobjectinstance_stub::header = NULL;

    free(coap_header->payload_ptr);
    coap_header->payload_ptr = NULL;

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    free(coap_header->payload_ptr);
    free(m2mobject_stub::header);
    m2mobject_stub::header = NULL;

    uint8_t object_instance1[] = {"name/65536"};

    coap_header->uri_path_ptr = object_instance1;
    coap_header->uri_path_len = sizeof(object_instance1);

    obj = new M2MObject(2, "name");

    nsdl->_object_list.push_back(obj);

    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name", "", "");

    m2mobject_stub::header = (sn_coap_hdr_s*) malloc(sizeof(sn_coap_hdr_s));
    memset(m2mobject_stub::header,0,sizeof(sn_coap_hdr_s));

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    free(m2mobject_stub::header);

    uint8_t resource[] = {"name/0/resource"};

    coap_header->uri_path_ptr = resource;
    coap_header->uri_path_len = sizeof(resource);

    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,NULL));

    // Test EMPTY ACK
    coap_header->msg_code = COAP_MSG_CODE_EMPTY;
    coap_header->msg_id = 100;
    obj = new M2MObject(0, "0");
    m2mbase_stub::string_value = "0";
    m2mbase_stub::int_value = 100;
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name", "", "");
    M2MResource res2(*m2mobject_stub::inst, 2, M2MBase::Dynamic,"test",M2MBase::STRING,false, "test");

    m2mobject_stub::inst->add_resource(&res2);
    m2mbase_stub::object_msg_id = 100;
    m2mbase_stub::ret_counter = 0;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));

    // Test RESET message
    coap_header->msg_type = COAP_MSG_TYPE_RESET;
    m2mbase_stub::object_msg_id = 100;
    m2mbase_stub::ret_counter = 0;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mbase_stub::base_type = M2MBase::Object;
    m2mbase_stub::ret_counter = 0;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    m2mbase_stub::ret_counter = 0;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));
    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    m2mbase_stub::ret_counter = 0;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,NULL));

    delete m2mobject_stub::inst;
    delete obj;
    nsdl->_object_list.clear();


    // Bootstrap cases start from here
    // handle_bootstrap_put_message() invalid object name
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address, 0, sizeof(sn_nsdl_addr_s));
    address->addr_len = 1;
    address->addr_ptr = (uint8_t *)malloc(1);
    address->addr_ptr[0] = 1;
    address->port = 5683;
    handle->oma_bs_address_len = 1;
    handle->oma_bs_port = 5683;
    handle->oma_bs_address_ptr = (uint8_t *)malloc(1);
    handle->oma_bs_address_ptr[0] = 1;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    observer->boot_error = false;
    CHECK(0== nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    // handle_bootstrap_put_message() invalid content type
    obj = new M2MObject(0, "0");
    m2mbase_stub::string_value = "0";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name", "", "");
    uint8_t security[] = {"0"};
    coap_header->uri_path_ptr = security;
    coap_header->uri_path_len = sizeof(security);
    M2MResource res(*m2mobject_stub::inst, 0, M2MBase::Dynamic,"test",M2MBase::STRING,false,"test");
    m2mobject_stub::inst->add_resource(&res);
    observer->boot_error = false;
    m2msecurity_stub::resource = new M2MResource(*m2mobject_stub::inst, 1, M2MBase::Dynamic,"type",M2MBase::STRING,false,"1");
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    // handle_bootstrap_put_message() success
    coap_header->token_ptr = String::convert_integer_to_array(1,coap_header->token_len);
    observer->boot_error = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    coap_header->content_format = sn_coap_content_format_e(COAP_CONTENT_OMA_TLV_TYPE);
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);

    // handle_bootstrap_put_message() TLV parsing fails
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = true;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::NotAllowed;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;

    // handle_bootstrap_put_message() TLV object instance
    obj = new M2MObject(1, "1");
    m2mbase_stub::string_value = "1";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name","", "");
    uint8_t server[] = {"1"};
    coap_header->uri_path_ptr = server;
    coap_header->uri_path_len = 1;

    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = true;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);


    // handle_bootstrap_put_message() TLV server object
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    m2mtlvdeserializer_stub::bool_value = false;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);
    delete obj;
    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();

    // handle_bootstrap_put_message() TLV device object
    obj = new M2MObject(3, "3");
    m2mbase_stub::string_value = "3";
    nsdl->_object_list.push_back(obj);
    m2mobject_stub::inst = new M2MObjectInstance(*obj, 0, "name","", "");
    uint8_t device[] = {"3"};
    coap_header->uri_path_ptr = device;
    coap_header->uri_path_len = 1;

    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = true;
    m2mtlvdeserializer_stub::bool_value = false;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);

    // handle_bootstrap_put_message() TLV not resource
    observer->boot_error = false;
    observer->boot_done = false;
    m2mtlvdeserializer_stub::is_object_bool_value = false;
    m2mtlvdeserializer_stub::bool_value = false;
    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::None;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    nsdl->_bootstrap_id = 8;
    handle->bootstrap_msg_id = 8;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() _identity_accepted false
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    uint8_t object_name[] = {"0/0"};
    coap_header->uri_path_ptr = object_name;
    coap_header->uri_path_len = 3;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    free(common_stub::coap_header);
    common_stub::coap_header = NULL;

    // handle_bootstrap_delete() object name not match
    observer->boot_error = false;
    coap_header->msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    coap_header->msg_id = 8;
    nsdl->received_from_server_callback(handle,coap_header,NULL);
    coap_header->msg_id = 18;
    uint8_t invalid[] = {"0/0/1"};
    coap_header->uri_path_ptr = invalid;
    coap_header->uri_path_len = 5;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() path does not match
    coap_header->uri_path_ptr = server;
    coap_header->uri_path_len = 1;
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() send coap response
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    m2msecurity_stub::string_value = new String("coaps://");
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(nsdl->_security == NULL);

    //handle_bootstrap_finished() success no security
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::NoSecurity;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_wait = false;
    observer->boot_done = false;
    coap_header->uri_path_ptr = (uint8_t*)malloc(2);
    coap_header->uri_path_len = 2;
    coap_header->uri_path_ptr[0] = 'b';
    coap_header->uri_path_ptr[1] = 's';
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);
    CHECK(observer->boot_wait == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() success certificate
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2mresourceinstance_stub::int_value = 10;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_wait = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    m2mresourcebase_stub::int_value = 1;

    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == false);
    CHECK(observer->boot_wait == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, Psk not supported
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Psk;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = false;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, Bootstrap server
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = true;
    m2msecurity_stub::bool_value = true;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, key size 0
    nsdl->_security = new M2MSecurity(M2MSecurity::M2MServer);
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = false;
    m2msecurity_stub::bool_value = false;
    m2mresourceinstance_stub::int_value = 0;
    observer->boot_error = false;
    observer->boot_done = false;
    m2mresourcebase_stub::int_value = 0;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    //handle_bootstrap_finished() fail, _security null
    m2msecurity_stub::sec_mode = M2MSecurity::Certificate;
    m2msecurity_stub::int_value = false;
    m2msecurity_stub::bool_value = false;
    m2mresourceinstance_stub::int_value = 0;
    observer->boot_error = false;
    observer->boot_done = false;
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;
    CHECK(0 == nsdl->received_from_server_callback(handle,coap_header,address));
    CHECK(observer->boot_error == true);
    CHECK(observer->boot_done == false);

    delete m2mobject_stub::inst;
    nsdl->_object_list.clear();
    delete obj;
    delete m2msecurity_stub::string_value;
    delete m2msecurity_stub::resource;
    M2MDevice::delete_instance();

    free(nsdl_resource->path);
    free(nsdl_resource);

    free(common_stub::coap_header);
    free(address->addr_ptr);
    free(address);
    free(coap_header->token_ptr);
    free(coap_header->uri_path_ptr);
    free(coap_header);
    free(handle->oma_bs_address_ptr);
    free(handle);
}

void Test_M2MNsdlInterface::test_resource_callback()
{
    m2mbase_stub::use_real_uri_path = true;
    uint8_t value[] = {"name"};

    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)calloc(sizeof(sn_coap_hdr_s), 1);
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)calloc(sizeof(sn_nsdl_addr_s), 1);
    common_stub::coap_header = (sn_coap_hdr_ *)calloc(sizeof(sn_coap_hdr_), 1);

    common_stub::coap_header->payload_ptr =(uint8_t*)malloc(1);
    common_stub::coap_header->payload_len = 1;


    common_stub::int_value = 0;

    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    M2MObject *obj = new M2MObject(0, "0");
    M2MObjectInstance* obj_instance = new M2MObjectInstance(*obj, 1, "", "0/1");

    M2MResource* res = new M2MResource(*obj_instance,
                                                   2,
                                                    M2MBase::Dynamic,
                                                   "res_type",
                                                   M2MBase::INTEGER,
                                                   false,
                                                   (char*)"0/1/2");

    M2MResourceInstance* res_instance = new M2MResourceInstance(*res, 3, M2MBase::Dynamic, "res_inst_type",
                                       M2MBase::INTEGER, "0/1/2/3", false, false);

    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;

    coap_header->uri_path_ptr = (uint8_t*)"0";
    coap_header->uri_path_len = sizeof("0");

    nsdl->_object_list.push_back(obj);

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    m2mobject_stub::bool_value = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);
    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    // Test rest of value updated
    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;
    m2mobject_stub::bool_value = true;
    coap_header->uri_path_ptr = (uint8_t*)"0/1";
    coap_header->uri_path_len = sizeof("0/1");

    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    coap_header->uri_path_ptr = (uint8_t*)"0/1/2";
    coap_header->uri_path_len = sizeof("0/1/2");
    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);
    free(m2mresourcebase_stub::value);

    m2mbase_stub::nsdl_resource->static_resource_parameters->path = (char*)malloc(9);
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[0] = 'n';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[1] = 'a';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[2] = 'm';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[3] = 'e';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[4] = '/';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[5] = '0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[6] = '/';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[7] = '0';
    m2mbase_stub::nsdl_resource->static_resource_parameters->path[8] = '\0';

    m2mbase_stub::base_type = M2MBase::Resource;
    m2mblockmessage_stub::is_block_message = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);

    m2mblockmessage_stub::is_block_message = false;
    free(m2mbase_stub::nsdl_resource->static_resource_parameters->path);

    coap_header->uri_path_ptr = (uint8_t*)"0/1/2/3";
    coap_header->uri_path_len = sizeof("0/1/2/3");
    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_COAP) ==0);


    delete obj;
    delete obj_instance;
    delete res;
    delete res_instance;

    free(common_stub::coap_header);
    free(address);
    free(coap_header);
}


void Test_M2MNsdlInterface::test_resource_callback_get()
{
}

void Test_M2MNsdlInterface::test_resource_callback_put()
{

    uint8_t value[] = {"name/0/resource"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    M2MObject *object = new M2MObject(0, "0");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 1, "name","0/1", "");

    M2MResource* create_resource = new M2MResource(*instance,
                                                   2,
                                                   M2MBase::Dynamic,
                                                   "res",
                                                   M2MBase::INTEGER,
                                                   false,
                                                   "0/1/2");
    m2mobject_stub::int_value = 2;

    m2mobject_stub::instance_list.push_back(instance);

    instance->add_resource(create_resource);

    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));

    uint8_t query[] = {"pmax=200&pmin=120"};
    coap_header->options_list_ptr->uri_query_ptr = (uint8_t*)malloc(sizeof(query));
    coap_header->options_list_ptr->uri_query_len = sizeof(query);

    m2mbase_stub::bool_value = true;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Resource;
    m2mobject_stub::bool_value = true;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Object;
    m2mbase_stub::bool_value = false;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mobject_stub::instance_list.clear();
    delete object;
    delete instance;
    delete create_resource;

    free(coap_header->options_list_ptr->uri_query_ptr);
    free(coap_header->options_list_ptr);
    if(common_stub::coap_header){
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);

    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_resource_callback_post()
{
    uint8_t value[] = {"name/0/name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";
    m2mbase_stub::bool_value = false;
    M2MObject *object = new M2MObject(0, "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 0, "name","","");
    M2MResource* create_resource = new M2MResource(*instance,
                                                   1,
                                                   M2MBase::Dynamic,
                                                   "name",
                                                   M2MBase::INTEGER,
                                                   false,
                                                   "name");
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    instance->add_resource(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::POST_ALLOWED;

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));

    uint8_t query[] = {"pmax=200&pmin=120"};
    coap_header->options_list_ptr->uri_query_ptr = (uint8_t*)malloc(sizeof(query));
    coap_header->options_list_ptr->uri_query_len = sizeof(query);

    m2mbase_stub::bool_value = true;

    sn_nsdl_dynamic_resource_parameters_s *nsdl_resource;

    nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));

    nsdl_resource->path = (char*)malloc(5);
    nsdl_resource->path[0] = 'n';
    nsdl_resource->path[1] = 'a';
    nsdl_resource->path[2] = 'm';
    nsdl_resource->path[3] = 'e';
    nsdl_resource->path[4] = '\0';
//    nsdl_resource->pathlen = 5;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Object;
    m2mbase_stub::bool_value = false;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete object;
    m2mobject_stub::instance_list.clear();
    delete instance;
    delete create_resource;

    free(coap_header->options_list_ptr->uri_query_ptr);
    free(coap_header->options_list_ptr);
    if(common_stub::coap_header){
        if( common_stub::coap_header->options_list_ptr){
            free(common_stub::coap_header->options_list_ptr);
            common_stub::coap_header->options_list_ptr = NULL;
        }
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);
    free(address);
    free(nsdl_resource->path);
    free(nsdl_resource);

    m2mbase_stub::clear();
    common_stub::clear();
    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();
}

void Test_M2MNsdlInterface::test_resource_callback_delete()
{
    uint8_t value[] = {"name/0"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_DELETE;
    common_stub::coap_header->msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    common_stub::int_value = 0;

    sn_nsdl_dynamic_resource_parameters_s *nsdl_resource;

    nsdl_resource =
            (sn_nsdl_dynamic_resource_parameters_s*) malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    nsdl_resource->path = (char*)malloc(7);
    nsdl_resource->path[0] = 'n';
    nsdl_resource->path[1] = 'a';
    nsdl_resource->path[2] = 'm';
    nsdl_resource->path[3] = 'e';
    nsdl_resource->path[4] = '/';
    nsdl_resource->path[5] = '0';
    nsdl_resource->path[6] = '\0';
//    nsdl_resource->pathlen = 7;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name/0";
    M2MObject *object = new M2MObject(1, "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 0, "name", "", "name/0");
    m2mbase_stub::int_value = 0;
    m2mobject_stub::int_value = 1;
    m2mobject_stub::bool_value = true;
    m2mobject_stub::instance_list.push_back(instance);
    nsdl->_object_list.push_back(object);

    m2mbase_stub::operation = M2MBase::DELETE_ALLOWED;

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    m2mbase_stub::base_type = M2MBase::Resource;
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) ==0);

    delete instance;
    delete object;
    free(nsdl_resource->path);
    free(nsdl_resource);
    free(common_stub::coap_header);
    free(address);
    free(coap_header);
    m2mobject_stub::instance_list.clear();
}

/*
void Test_M2MNsdlInterface::test_resource_callback_reset()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header,0,sizeof(sn_coap_hdr_s));
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_type = COAP_MSG_TYPE_RESET;
    m2mobjectinstance_stub::base_type = M2MBase::Object;
    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = name;
    M2MObject *object = new M2MObject(*name);
    M2MObjectInstance* instance = new M2MObjectInstance(*name,*object);
    M2MResource* create_resource = new M2MResource(*instance,
                                                   *name,
                                                   *name,
                                                   M2MResourceInstance::INTEGER,
                                                   M2MResource::Dynamic,false);
    m2mobject_stub::int_value = 2;
    m2mobject_stub::instance_list.push_back(instance);

    m2mobjectinstance_stub::resource_list.push_back(create_resource);
    m2mobjectinstance_stub::int_value = 1;

    nsdl->_object_list.push_back(object);
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);


    m2mobject_stub::base_type = M2MBase::ObjectInstance;
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    m2mobject_stub::base_type = M2MBase::Resource;
    // No response for RESET message
    CHECK(nsdl->resource_callback(NULL,coap_header,address,SN_NSDL_PROTOCOL_HTTP) == 1);

    delete instance;
    delete object;
    delete name;
    delete create_resource;
    free(address);
    free(coap_header);
    m2mobject_stub::instance_list.clear();
    //nsdl->_object_list.clear();
}
*/
void Test_M2MNsdlInterface::test_process_received_data()
{
    uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t));
    uint16_t data_size = sizeof(uint16_t);
    sn_nsdl_addr_s *address = (sn_nsdl_addr_s *)malloc(sizeof(sn_nsdl_addr_s));

    common_stub::int_value = 0;

    CHECK(nsdl->process_received_data(data,data_size,address) == true);

    common_stub::int_value = -1;

    CHECK(nsdl->process_received_data(data,data_size,address) == false);

    free(address);
    free(data);
    common_stub::clear();
}

void Test_M2MNsdlInterface::test_stop_timers()
{
    // Check if there is no memory leak or crash
    nsdl->stop_timers();
}

void Test_M2MNsdlInterface::test_timer_expired()
{
    nsdl->timer_expired(M2MTimerObserver::NsdlExecution);
    CHECK(nsdl->_counter_for_nsdl == 1);

    if( nsdl->_endpoint == NULL){
        nsdl->_endpoint = (sn_nsdl_ep_parameters_s*)nsdl->memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    }
    nsdl->_endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(uint8_t));
    memset(nsdl->_endpoint->lifetime_ptr, 0, sizeof(uint8_t));


    // For checking the registration update
    nsdl->timer_expired(M2MTimerObserver::Registration);

    nsdl->delete_endpoint();
    CHECK(nsdl->_endpoint == NULL);
}

void Test_M2MNsdlInterface::test_observation_to_be_sent()
{
    Vector<uint16_t> instance_list_ids;
    m2mbase_stub::clear();
    M2MObject *object = new M2MObject(0, "name");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 0, "name", "", "");
    M2MResource *res = new M2MResource(*instance,0, M2MBase::Dynamic,"res", M2MBase::INTEGER,false, "res");
    M2MResource *res2 = new M2MResource(*instance,2, M2MBase::Dynamic,"res2", M2MBase::INTEGER,false, "res2");


    M2MResourceInstance* res_instance = new M2MResourceInstance(*res,
                                                                11, M2MBase::Dynamic, "res",
                                                                M2MBase::INTEGER,"",false, false);
    M2MResourceInstance* res_instance_1 = new M2MResourceInstance(*res2,
                                                                  22, M2MBase::Dynamic, "res2",
                                                                  M2MBase::INTEGER,"",false, false);
    //m2mresource_stub::list.clear();
    //m2mresource_stub::list.push_back(res_instance);
    //m2mresource_stub::list.push_back(res_instance_1);
    m2mresource_stub::int_value = 2;
    instance_list_ids.push_back(0);

    uint8_t value[] = {"value"};
    m2mresourcebase_stub::value = (uint8_t *)malloc(sizeof(value));
    memset( m2mresourcebase_stub::value, 0, sizeof(value));
    memcpy(m2mresourcebase_stub::value,value,sizeof(value));
    m2mresourceinstance_stub::int_value = sizeof(value);

    m2mbase_stub::uint16_value = 321;
    m2mbase_stub::string_value = "token";


    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));
    sn_nsdl_oma_server_info_t * nsp_address = (sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    memset(nsp_address,0,sizeof(sn_nsdl_oma_server_info_t));
    sn_nsdl_addr_s* address = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    nsdl->_nsdl_handle->nsp_address_ptr = nsp_address;
    memset(nsdl->_nsdl_handle->nsp_address_ptr,0,sizeof(sn_nsdl_oma_server_info_t));
    nsdl->_nsdl_handle->nsp_address_ptr->omalw_address_ptr = address;

    // To cover Queue Mode as well
    nsdl->_endpoint->binding_and_mode = (sn_nsdl_oma_binding_and_mode_t)0x02;

    //CHECK if nothing crashes
    m2mbase_stub::base_type = M2MBase::Resource;
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    m2mresourcebase_stub::resource_type = M2MResource::OPAQUE;

    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    nsdl->observation_to_be_sent(res2, 1, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    //m2mresource_stub::list.clear();
    m2mresource_stub::int_value = 0;

    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    nsdl->observation_to_be_sent(res, 500, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    M2MObjectInstance *object_instance = new M2MObjectInstance(*object, 0, "name", "","");
    m2mobject_stub::int_value = 1;
    m2mobject_stub::inst = object_instance;
    object_instance->add_resource(res);
    nsdl->_object_list.push_back(object);
    instance_list_ids.push_back(1);
    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    m2mbase_stub::base_type = M2MBase::Object;
    nsdl->observation_to_be_sent(object, 1, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    nsdl->observation_to_be_sent(object, 500, instance_list_ids, true);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    //CHECK if nothing crashes
    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    nsdl->observation_to_be_sent(object_instance, 1, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    common_stub::coap_header = (sn_coap_hdr_s *) malloc(sizeof(sn_coap_hdr_s));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)calloc(sizeof(sn_coap_options_list_s), 1);
    nsdl->observation_to_be_sent(object_instance, 500, instance_list_ids);
    free(common_stub::coap_header->options_list_ptr);
    free(common_stub::coap_header);

    free(m2mresourcebase_stub::value);
    m2mresourcebase_stub::value = NULL;

    m2mbase_stub::clear();
    m2mresourceinstance_stub::clear();
    m2mobjectinstance_stub::clear();
    m2mobject_stub::clear();
    //m2mresource_stub::list.clear();
    delete res;
    res = NULL;

    delete res2;
    res2 = NULL;

    delete res_instance;
    res_instance = NULL;

    delete res_instance_1;
    res_instance_1 = NULL;

    delete object_instance;
    object_instance = NULL;

    delete object;
    object = NULL;

    delete instance;
    instance = NULL;

    free(nsp_address);
    free(address);
    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_resource_to_be_deleted()
{
    //Checking coverage for the code
    M2MObject *object = new M2MObject(1, "name");
    nsdl->resource_to_be_deleted(object);
    delete object;
}

void Test_M2MNsdlInterface::test_value_updated()
{
#if 0
    M2MObject *object = new M2MObject("name", "name");
    M2MObjectInstance *object_instance = new M2MObjectInstance(*object, "name", "", "");
    M2MResource *resource = new M2MResource(*object_instance,
                                            "resource_name",
                                            M2MBase::Dynamic,
                                            "resource_type",
                                            M2MBase::INTEGER,
                                            false,
                                            "resource_name");

    M2MResourceInstance *resource_instance = new M2MResourceInstance(*resource,
                                                                     "resource_name",
                                                                     M2MBase::Dynamic,
                                                                     "resource_type",
                                                                     M2MBase::INTEGER,
                                                                     "",false, false);

    m2mobject_stub::base_type = M2MBase::Object;
    m2mbase_stub::string_value = "name";
    m2mbase_stub::operation = M2MBase::GET_ALLOWED;
    nsdl->value_updated(object/*,"name"*/);
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mobjectinstance_stub::base_type = M2MBase::ObjectInstance;

    nsdl->value_updated(object_instance/*,"name/0"*/);
    CHECK(observer->value_update == true);
    observer->value_update = false;

    common_stub::resource = (sn_nsdl_dynamic_resource_parameters_s*)malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_dynamic_resource_parameters_s));

//    common_stub::resource->resource = (char*)malloc(2);
//    memset(common_stub::resource->resource,0, 2);

    common_stub::resource->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    common_stub::resource->observable = false;
    m2mbase_stub::bool_value = true;

    m2mresourceinstance_stub::int_value = 2;
    uint8_t value[] = "1";
    m2mresourcebase_stub::value = value;

    m2mresourceinstance_stub::base_type = M2MBase::Resource;

    nsdl->value_updated(resource/*,"name/0/name"*/);
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mresourceinstance_stub::clear();
 //   free(common_stub::resource->resource);
    free(common_stub::resource);
    common_stub::resource = NULL;
    common_stub::clear();

    m2mresourceinstance_stub::base_type = M2MBase::ResourceInstance;

    common_stub::resource = (sn_nsdl_dynamic_resource_parameters_s*)malloc(sizeof(sn_nsdl_dynamic_resource_parameters_s));
    memset(common_stub::resource,0, sizeof(sn_nsdl_dynamic_resource_parameters_s));

  //  common_stub::resource->resource = (uint8_t*)malloc(2);
    //memset(common_stub::resource->resource,0, 2);

    common_stub::resource->mode = SN_GRS_STATIC;
    m2mbase_stub::mode_value = M2MBase::Static;

    m2mresourceinstance_stub::int_value = 2;
    m2mresourcebase_stub::value = value;

    nsdl->value_updated(resource_instance/*,"name/0/name/0"*/);
    CHECK(observer->value_update == true);
    observer->value_update = false;

    m2mbase_stub::is_value_updated_function_set = true;
    nsdl->value_updated(resource_instance/*,"name/0/name/0"*/);
    CHECK(observer->value_update == false);
    observer->value_update = false;

    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();

    m2mresourceinstance_stub::clear();

//    free(common_stub::resource->resource);
    free(common_stub::resource);

    common_stub::resource = NULL;
    common_stub::clear();

    delete resource_instance;
    delete resource;
    delete object_instance;
    delete object;
#endif
}


void Test_M2MNsdlInterface::test_find_resource()
{
    // make the stub to use real uri_path()
    m2mbase_stub::use_real_uri_path = true;

    M2MObject *object = new M2MObject(0, "0");
    M2MObjectInstance *object_instance = new M2MObjectInstance(*object, 1, "","0/1");
    M2MResource *resource = new M2MResource(*object_instance,
                                            2,
                                            M2MBase::Dynamic,
                                            "resource_type",
                                            M2MBase::INTEGER,
                                            false,
                                            "0/1/2",
                                            false,
                                            false);

    M2MResourceInstance *resource_instance = new M2MResourceInstance(*resource,
                                                                     3,
                                                                     M2MBase::Dynamic,
                                                                     "resource_type",
                                                                     M2MBase::INTEGER,
                                                                     "0/1/2/3",false, false);


    m2mobject_stub::instance_list.push_back(object_instance);

    object_instance->add_resource(resource);
    resource->add_resource_instance(resource_instance);
    nsdl->_object_list.push_back(object);
    m2mresource_stub::bool_value = true;

    m2mbase_stub::object_instance_msg_id = 10;
    m2mbase_stub::resource_msg_id = 100;

    m2mbase_stub::base_type = M2MBase::Object;
    CHECK(nsdl->find_resource("0", 0) == object);
    CHECK(nsdl->find_resource("1", 0) == NULL);

    object->set_notification_msgid(1);
    CHECK(nsdl->find_resource("0", 1) == object);
    CHECK(nsdl->find_resource("0", 11) == NULL);


    m2mbase_stub::base_type = M2MBase::ObjectInstance;
    CHECK(nsdl->find_resource("0/1", 0) == object_instance);
    object_instance->set_notification_msgid(10);
    CHECK(nsdl->find_resource("0/1", 10) == object_instance);

    m2mbase_stub::base_type = M2MBase::Resource;
    m2mbase_stub::ret_counter = 0;
    CHECK(nsdl->find_resource("0/1/2", 0) == resource);
    CHECK(nsdl->find_resource("0/1/2", 125) == NULL);
    resource->set_notification_msgid(100);
    CHECK(nsdl->find_resource("0/1/2", 100) == resource);

    m2mbase_stub::base_type = M2MBase::ResourceInstance;
    CHECK(nsdl->find_resource("0/1/2/3", 0) == resource_instance);
    resource_instance->set_notification_msgid(1000);
// XXX: does not actually work yet, or this test is bogus:    CHECK(nsdl->find_resource("0/1/2/3", 1000) == resource_instance);

    CHECK(nsdl->find_resource("0/1/2/3", 9999) == NULL);

    m2mobject_stub::clear();
    m2mobjectinstance_stub::clear();

    m2mresourceinstance_stub::clear();
    m2mresource_stub::clear();
    m2mobjectinstance_stub::clear();
    m2mobject_stub::clear();

    delete resource_instance;
    delete resource;
    delete object_instance;
    delete object;
}

void Test_M2MNsdlInterface::test_remove_object()
{
    M2MObject *obj = new M2MObject(2, "2");
    nsdl->_object_list.push_back(obj);

    nsdl->remove_object((M2MBase*)obj);

    CHECK(nsdl->_object_list.empty() == true);

    nsdl->_object_list.clear();
    delete obj;
}

void Test_M2MNsdlInterface::test_add_object_to_list()
{
    M2MObject *obj = new M2MObject(0, "0");
    nsdl->_object_list.push_back(obj);

    nsdl->add_object_to_list(obj);
    CHECK(nsdl->_object_list.size() == 1);

    nsdl->_object_list.clear();
    delete obj;
}

void Test_M2MNsdlInterface::test_send_delayed_response()
{
    common_stub::int_value = 0;
    m2mbase_stub::int_value = 0;

    M2MObject *object = new M2MObject(2, "2");
    M2MObjectInstance* instance = new M2MObjectInstance(*object, 0, "name", "", "");
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));

    sn_nsdl_oma_server_info_t * nsp_address = (sn_nsdl_oma_server_info_t *)malloc(sizeof(sn_nsdl_oma_server_info_t));
    memset(nsp_address,0,sizeof(sn_nsdl_oma_server_info_t));
    sn_nsdl_addr_s* address = (sn_nsdl_addr_s*)malloc(sizeof(sn_nsdl_addr_s));
    memset(address,0,sizeof(sn_nsdl_addr_s));

    M2MResource* resource = new M2MResource(*instance,
                                            1,
                                            M2MBase::Dynamic,
                                            "name",
                                            M2MBase::INTEGER,
                                            false,
                                            "name");

    uint8_t val[] = {"name"};
    m2mresource_stub::delayed_token = (uint8_t*)malloc(sizeof(val));
    memcpy(m2mresource_stub::delayed_token,val,sizeof(val));
    m2mresource_stub::delayed_token_len = sizeof(val);


    nsdl->_nsdl_handle->nsp_address_ptr = nsp_address;
    memset(nsdl->_nsdl_handle->nsp_address_ptr,0,sizeof(sn_nsdl_oma_server_info_t));
    nsdl->_nsdl_handle->nsp_address_ptr->omalw_address_ptr = address;

    nsdl->send_delayed_response(resource);

    free(nsp_address);
    free(address);
    free(nsdl->_nsdl_handle);

    delete object;
    delete instance;
    delete resource;
    free(m2mresource_stub::delayed_token);
    m2mresource_stub::delayed_token = NULL;
    m2mresource_stub::delayed_token_len = 0;
}

void Test_M2MNsdlInterface::test_get_nsdl_handle()
{
    CHECK(nsdl->get_nsdl_handle() == nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_claim_release_mutex()
{
    CHECK(m2mconnectionhandler_stub::mutex_count == 0);
    nsdl->claim_mutex();
    CHECK(m2mconnectionhandler_stub::mutex_count == 1);
    nsdl->release_mutex();
    CHECK(m2mconnectionhandler_stub::mutex_count == 0);
}


void Test_M2MNsdlInterface::test_endpoint_name()
{
    String endpoint = "test";
    nsdl->_endpoint_name = endpoint;
    STRCMP_EQUAL(nsdl->endpoint_name(), endpoint.c_str());
}

void Test_M2MNsdlInterface::test_update_endpoint()
{
    const char *name = "endpoint_name";
    nsdl->update_endpoint(name);
    STRCMP_EQUAL(name, nsdl->endpoint_name());

}

void Test_M2MNsdlInterface::test_update_domain()
{
    nsdl->_endpoint->domain_name_ptr = (uint8_t *)malloc(2);
    memset(nsdl->_endpoint->domain_name_ptr,0,2);
    memcpy(nsdl->_endpoint->domain_name_ptr, "",1);
    const char *name = "domain";
    nsdl->update_domain(name);
    STRCMP_EQUAL(name, (const char*)nsdl->_endpoint->domain_name_ptr);

}

void Test_M2MNsdlInterface::test_internal_endpoint_name()
{
    nsdl->_nsdl_handle = (nsdl_s*)malloc(sizeof(nsdl_s));
    memset(nsdl->_nsdl_handle,0,sizeof(nsdl_s));
    nsdl->_nsdl_handle->ep_information_ptr = (sn_nsdl_ep_parameters_s *)malloc(sizeof(sn_nsdl_ep_parameters_s));
    memset(nsdl->_nsdl_handle->ep_information_ptr,0,sizeof(sn_nsdl_ep_parameters_s));
    STRCMP_EQUAL(nsdl->internal_endpoint_name().c_str(), "");

    nsdl->_nsdl_handle->ep_information_ptr->location_ptr = (uint8_t *)malloc(13);
    nsdl->_nsdl_handle->ep_information_ptr->location_len = 13;
    memset(nsdl->_nsdl_handle->ep_information_ptr->location_ptr, 0, 13);

    memcpy(nsdl->_nsdl_handle->ep_information_ptr->location_ptr,(uint8_t*)"rd/1000/1234\0", 13);
    STRCMP_EQUAL(nsdl->internal_endpoint_name().c_str(), "1234");

    free(nsdl->_nsdl_handle->ep_information_ptr->location_ptr);
    free(nsdl->_nsdl_handle->ep_information_ptr);
    free(nsdl->_nsdl_handle);
}

void Test_M2MNsdlInterface::test_get_nsdl_execution_timer()
{
    M2MTimer &timer = nsdl->get_nsdl_execution_timer();
    (void)timer;
}

void Test_M2MNsdlInterface::test_start_nsdl_execution_timer()
{
    nsdl->start_nsdl_execution_timer();
    CHECK(nsdl->_nsdl_exceution_timer_running);
}

void Test_M2MNsdlInterface::test_coap_error()
{
    sn_coap_hdr_s coap_header;
    coap_header.coap_status = COAP_STATUS_BUILDER_MESSAGE_SENDING_FAILED;

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_1);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_BAD_OPTION;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_2);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_INCOMPLETE;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_3);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_PRECONDITION_FAILED;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_4);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_5);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_UNSUPPORTED_CONTENT_FORMAT;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_6);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_UNAUTHORIZED;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_7);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_FORBIDDEN;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_8);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_NOT_ACCEPTABLE;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_9);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_10);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_11);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_SERVICE_UNAVAILABLE;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_13);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_INTERNAL_SERVER_ERROR;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_14);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_BAD_GATEWAY;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_15);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_GATEWAY_TIMEOUT;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_16);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_PROXYING_NOT_SUPPORTED;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_17);

    coap_header.msg_code = COAP_MSG_CODE_RESPONSE_CHANGED;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_ERROR_REASON_12);

    coap_header.coap_status = COAP_STATUS_OK;
    STRCMP_EQUAL(nsdl->coap_error(coap_header),COAP_NO_ERROR);

}

void Test_M2MNsdlInterface::test_get_unregister_ongoing()
{
    nsdl->_unregister_ongoing = true;
    CHECK(nsdl->get_unregister_ongoing() == true);
}

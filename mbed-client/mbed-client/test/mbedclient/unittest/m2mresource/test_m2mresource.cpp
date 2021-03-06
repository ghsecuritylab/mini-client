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
#include "test_m2mresource.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"
#include "m2mbase_stub.h"
#include "m2mresourcebase_stub.h"
#include "m2mresourceinstance_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mtlvdeserializer_stub.h"
#include "m2mreporthandler_stub.h"
#include "m2mobject_stub.h"
#include "common_stub.h"
#include "m2mreporthandler.h"
#include "m2mconstants.h"

class TestReportObserver :  public M2MReportObserver{
public :
    TestReportObserver() {}
    ~TestReportObserver() {}
    virtual void observation_to_be_sent(const m2m::Vector<uint16_t>&, uint16_t, bool){ }
};

class Handler : public M2MObservationHandler {

public:

    Handler(){}
    ~Handler(){}
    virtual void observation_to_be_sent(M2MBase *, uint16_t, const m2m::Vector<uint16_t>&,bool){
        visited = true;
    }
    void send_delayed_response(M2MBase *){}
    void resource_to_be_deleted(M2MBase *){visited=true;}
    void remove_object(M2MBase *){visited = true;}
    void value_updated(M2MBase *){visited = true;}

    void clear() {visited = false;}
    bool visited;
};

Test_M2MResource::Test_M2MResource()
{
    handler = new Handler();
    object = new M2MObject(2, "name");
    object_instance = new M2MObjectInstance(*object, 0, "name", "type", "");
    resource = new M2MResource(*object_instance,
                               1,
                               M2MBase::Dynamic,
                               "resource_type",
                               M2MBase::INTEGER,false, "name", true);
}

Test_M2MResource::~Test_M2MResource()
{
    delete object;
    delete object_instance;
    delete resource;
    delete handler;
    m2mtlvdeserializer_stub::clear();
}

void Test_M2MResource::test_static_resource()
{
    u_int8_t value[] = {"value"};
    M2MResource *res = new M2MResource(*m2mobject_stub::inst,
                                       1,
                                      M2MBase::Dynamic,
                                      "resource_type",
                                      M2MBase::INTEGER,
                                      value,
                                      (uint8_t)sizeof(value),"name");
    CHECK(res != NULL);
    delete res;

}

void Test_M2MResource::test_base_type()
{
    u_int8_t value[] = {"value"};
    M2MResource resource(*m2mobject_stub::inst,
                                       3,
                                      M2MBase::Dynamic,
                                      "resource_type",
                                      M2MBase::INTEGER,
                                      value,
                                      (uint8_t)sizeof(value),
                                      "name",
                                      true);

    CHECK(M2MBase::Resource == resource.base_type());
}

void Test_M2MResource::test_muliptle_instances()
{
    resource->_sn_resource.multiple_instance = false;
    CHECK(false == resource->supports_multiple_instances());

    resource->_sn_resource.multiple_instance = true;
    CHECK(true == resource->supports_multiple_instances());
}

void Test_M2MResource::test_handle_observation_attribute()
{
    char *d = "s";

    m2mresourceinstance_stub::resource_type = M2MResourceInstance::INTEGER;
    CHECK(false == resource->handle_observation_attribute(d));

    m2mresourceinstance_stub::resource_type = M2MResourceInstance::FLOAT;
    CHECK(false == resource->handle_observation_attribute(d));

    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              1,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::INTEGER,
                                                                              "name",
                                                                              false,
                                                                              false);

    resource->add_resource_instance(res);

    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);

    m2mbase_stub::bool_value = true;
    CHECK(false == resource->handle_observation_attribute(d));

    m2mreporthandler_stub::bool_return = true;
    CHECK(true == resource->handle_observation_attribute(d));

    m2mbase_stub::bool_value = false;
    CHECK(true == resource->handle_observation_attribute(d));

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
}

void Test_M2MResource::test_add_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              3,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::INTEGER,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);
    CHECK(resource->resource_instance_count() == 1);
    CHECK(resource->resource_instances() == res);
}

void Test_M2MResource::test_remove_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              0,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);
    CHECK(resource->remove_resource_instance(0) == true);
}

void Test_M2MResource::test_resource_instance()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              4,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);
    CHECK(resource->resource_instance(4) != NULL);
}

void Test_M2MResource::test_resource_instances()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              3,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);

    const M2MResourceInstance* first_const = resource->resource_instances();
    CHECK(first_const == res);

    M2MResourceInstance* first_nonconst = resource->resource_instances();
    CHECK(first_nonconst == res);
}

void Test_M2MResource::test_resource_instance_count()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              4,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);
    CHECK(resource->resource_instance_count() == 1);
}

void Test_M2MResource::test_add_observation_level()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              5,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);

    M2MBase::Observation obs_level = M2MBase::R_Attribute;

    resource->add_observation_level(obs_level);
}

void Test_M2MResource::test_remove_observation_level()
{
    M2MResourceInstance *res = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              6,
                                                                              M2MBase::Dynamic,
                                                                              "type",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    resource->add_resource_instance(res);

    M2MBase::Observation obs_level = M2MBase::R_Attribute;

    resource->remove_observation_level(obs_level);
}

void Test_M2MResource::test_handle_get_request()
{
    uint8_t value[] = {"name"};
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_GET;

    common_stub::int_value = 0;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));
    common_stub::coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));

    coap_header->token_ptr = (uint8_t*)malloc(sizeof(value));
    memcpy(coap_header->token_ptr, value, sizeof(value));

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->observe = 0;

    coap_header->options_list_ptr->accept = sn_coap_content_format_e(110);

    m2mresourceinstance_stub::header = NULL;

    M2MResourceInstance *res_instance = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              0,
                                                                              M2MBase::Dynamic,
                                                                              "res2",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);

    M2MResourceInstance *res_instance_1 = new M2MResourceInstance(*m2mobjectinstance_stub::resource,
                                                                              1,
                                                                              M2MBase::Dynamic,
                                                                              "res2",
                                                                              M2MBase::STRING,
                                                                              "name",
                                                                              false,
                                                                              false);
    CHECK(resource->handle_get_request(NULL,coap_header,handler) == NULL);

    // Modify the object directly, as the multiple_instance -flag needed in test is normally
    // a object creation time decision.
    resource->_sn_resource.multiple_instance = true;

    resource->add_resource_instance(res_instance);
    resource->add_resource_instance(res_instance_1);

    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);


    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    // Not OMA TLV or JSON
    common_stub::coap_header->options_list_ptr->accept = sn_coap_content_format_e(-1);
    m2mbase_stub::uint16_value = 110;
    coap_header->options_list_ptr->accept = sn_coap_content_format_e(-1);

    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    // OMA TLV
    m2mbase_stub::uint16_value = COAP_CONTENT_OMA_TLV_TYPE;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    // OMA JSON
    m2mbase_stub::uint16_value = 100;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    coap_header->options_list_ptr->observe = 0;
    m2mbase_stub::uint16_value = 0x1c1c;
    m2mbase_stub::bool_value = true;

    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::uint16_value = 10;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    coap_header->options_list_ptr->observe = 0;

    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    coap_header->options_list_ptr->observe = 1;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    // Not observable
    m2mbase_stub::bool_value = false;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;
    CHECK(resource->handle_get_request(NULL,coap_header,handler) != NULL);

    CHECK(resource->handle_get_request(NULL,NULL,handler) != NULL);

    free(coap_header->token_ptr);
    free(coap_header->options_list_ptr);

    if(common_stub::coap_header){
        free(common_stub::coap_header->options_list_ptr);
        free(common_stub::coap_header);
        common_stub::coap_header = NULL;
    }
    free(coap_header);

    m2mbase_stub::clear();
    common_stub::clear();
}

void Test_M2MResource::test_handle_put_request()
{
    uint8_t value[] = {"name"};
    bool execute_value_updated = false;
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);


    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_PUT;

    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    m2mbase_stub::operation = M2MBase::PUT_ALLOWED;
    m2mbase_stub::uint16_value = 200;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->payload_ptr = (uint8_t*)malloc(1);

    sn_coap_hdr_s *coap_response = NULL;

    m2mresourceinstance_stub::header = NULL;

    CHECK(resource->handle_put_request(NULL,coap_header,handler,execute_value_updated) == NULL);

    // Modify the object directly, as the multiple_instance -flag needed in test is normally
    // a object creation time decision.
    resource->_sn_resource.multiple_instance = true;

//    m2mbase_stub::uint8_value = 99; // XXX: is this still needed?
    coap_header->content_format = sn_coap_content_format_e(-1);
    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK( coap_response != NULL);

    m2mbase_stub::uint16_value = 0;

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->uri_query_ptr = value;
    coap_header->options_list_ptr->uri_query_len = sizeof(value);

    coap_header->content_format = sn_coap_content_format_e(COAP_CONTENT_OMA_TLV_TYPE);
    m2mtlvdeserializer_stub::bool_value = true;

    m2mbase_stub::bool_value = false;


    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK( coap_response != NULL);

    free(coap_header->options_list_ptr);
    coap_header->options_list_ptr = NULL;

    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);
    CHECK( coap_response != NULL);

    m2mtlvdeserializer_stub::bool_value = false;

    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::NotFound;
    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);
    CHECK(coap_response->msg_code == COAP_MSG_CODE_RESPONSE_NOT_FOUND);

    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::NotValid;
    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);
    CHECK(coap_response->msg_code == COAP_MSG_CODE_RESPONSE_BAD_REQUEST);

    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::NotAllowed;
    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);
    CHECK(coap_response->msg_code == COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED);

    m2mtlvdeserializer_stub::error = M2MTLVDeserializer::OutOfMemory;
    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);
    CHECK(coap_response->msg_code == COAP_MSG_CODE_RESPONSE_REQUEST_ENTITY_TOO_LARGE);

    coap_header->content_format = sn_coap_content_format_e(100);

    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    m2mbase_stub::bool_value = true;

    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;

    coap_response = resource->handle_put_request(NULL,coap_header,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    coap_response = resource->handle_put_request(NULL,NULL,handler,execute_value_updated);

    CHECK( coap_response != NULL);

    //free(coap_header->options_list_ptr);
    free(coap_header->payload_ptr);
    free(common_stub::coap_header);
    free(coap_header);
    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
    m2mtlvdeserializer_stub::clear();
    common_stub::clear();
    m2mbase_stub::clear();
}

void Test_M2MResource::test_handle_post_request()
{
    uint8_t value[] = {"name"};
    bool execute_value_updated = false;
    sn_coap_hdr_s *coap_header = (sn_coap_hdr_s *)malloc(sizeof(sn_coap_hdr_s));
    memset(coap_header, 0, sizeof(sn_coap_hdr_s));

    coap_header->uri_path_ptr = value;
    coap_header->uri_path_len = sizeof(value);

    coap_header->msg_code = COAP_MSG_CODE_REQUEST_POST;

    String *name = new String("name");
    common_stub::int_value = 0;
    m2mbase_stub::string_value = "name";

    m2mbase_stub::operation = M2MBase::POST_ALLOWED;
    m2mbase_stub::uint16_value = 200;

    resource->_sn_resource.data_type = M2MBase::STRING;

    common_stub::coap_header = (sn_coap_hdr_ *)malloc(sizeof(sn_coap_hdr_));
    memset(common_stub::coap_header,0,sizeof(sn_coap_hdr_));

    coap_header->payload_ptr = (uint8_t*)malloc(1);

    coap_header->options_list_ptr = (sn_coap_options_list_s*)malloc(sizeof(sn_coap_options_list_s));
    coap_header->options_list_ptr->uri_query_ptr = value;
    coap_header->options_list_ptr->uri_query_len = sizeof(value);

    coap_header->content_format = sn_coap_content_format_e(COAP_CONTENT_OMA_TLV_TYPE);


    resource->_delayed_response = true;
    resource->_delayed_token = (uint8_t*)malloc(1);
    *resource->_delayed_token  = 2;
    coap_header->token_ptr = (uint8_t*)malloc(1);
    *coap_header->token_ptr = 1;
    coap_header->token_len = 1;

    m2mbase_stub::bool_value = false;
    m2mresourceinstance_stub::string_value = name;

    CHECK(resource->handle_post_request(NULL,coap_header,handler,execute_value_updated) != NULL);

    coap_header->content_format = sn_coap_content_format_e(0);

    CHECK(resource->handle_post_request(NULL,coap_header,handler,execute_value_updated) != NULL);

    m2mresourceinstance_stub::int_value = sizeof(value);
    m2mresourcebase_stub::value = value;

    resource->_delayed_response = false;
    sn_coap_hdr_s *coap_response = resource->handle_post_request(NULL,coap_header,handler,execute_value_updated);

    CHECK(coap_response != NULL);
    free(coap_response->payload_ptr);

    m2mbase_stub::operation = M2MBase::NOT_ALLOWED;
    CHECK(resource->handle_post_request(NULL,coap_header,handler,execute_value_updated) != NULL);

    CHECK(resource->handle_post_request(NULL,NULL,handler,execute_value_updated) != NULL);

    free(coap_header->token_ptr);
    free(coap_header->options_list_ptr);
    free(coap_header->payload_ptr);

    delete name;
    free(coap_header);
    free(common_stub::coap_header);

    m2mbase_stub::clear();
    common_stub::clear();
}

void Test_M2MResource::test_notification_update()
{
    TestReportObserver obs;
    m2mbase_stub::report = new M2MReportHandler(obs);
    m2mbase_stub::bool_value = true;

   // resource->notification_update();

    delete m2mbase_stub::report;
    m2mbase_stub::report = NULL;
}

void Test_M2MResource::test_set_delayed_response()
{
    resource->set_delayed_response(true);
    CHECK(resource->_delayed_response == true);
}

void Test_M2MResource::test_send_delayed_post_response()
{
    resource->_delayed_response = true;
    m2mobjectinstance_stub::observe = handler;
    CHECK(resource->send_delayed_post_response() == true);
}

void Test_M2MResource::test_get_delayed_token()
{
    uint8_t value[] = {"1"};
    uint8_t *token = NULL;
    uint8_t token_len = 0;
    resource->_delayed_token_len = 1;
    resource->_delayed_token = (uint8_t*)malloc(sizeof(1));
    memcpy(resource->_delayed_token,value,1);
    resource->get_delayed_token(token,token_len);
    CHECK(token != NULL);
    free(token);

    token = NULL;
    token = (uint8_t*)malloc(1);
    token_len = 1;
    resource->get_delayed_token(token,token_len);
    CHECK(token != NULL);
    free(token);

}

void Test_M2MResource::test_delayed_response()
{
    resource->_delayed_response = false;
    CHECK(resource->delayed_response() == false);
}

void Test_M2MResource::test_execute_params()
{
    M2MResource::M2MExecuteParameter *params = new M2MResource::M2MExecuteParameter(1, 2, 3);

    CHECK(params->get_argument_value() == NULL);
    CHECK(params->get_argument_value_length() == 0);
    CHECK(params->get_argument_object_id() == 1);
    CHECK(params->get_argument_object_instance_id() == 2);
    CHECK(params->get_argument_resource_id() == 3);

    const uint8_t value[] = {"testvalue"};
    int length = sizeof(value);
    uint8_t* temp_value = (uint8_t*)malloc(length);
    memcpy(temp_value, value, length);

    delete params;

    // then test with something more than just empty values
    params = new M2MResource::M2MExecuteParameter(4, 1, 2);
    params->_value = temp_value;
    params->_value_length = length;
    params->_object_instance_id = 7;

    CHECK(params->_value == params->get_argument_value());
    CHECK(params->_value_length == params->get_argument_value_length());
    CHECK(params->_resource_id == params->get_argument_resource_id());
    CHECK(params->_object_id == params->get_argument_object_id());
    CHECK(params->_object_instance_id == params->get_argument_object_instance_id());

    delete params;

    free(temp_value);
}

void Test_M2MResource::test_ctor()
{
    uint8_t value[] = {"1234"};
    m2mbase_stub::string_value = "2/0/12";

    M2MResource* res = new M2MResource(*object_instance, 12, M2MBase::Dynamic, "res_type", M2MBase::INTEGER, value, sizeof(value), "2/0/12");
    CHECK(res != NULL);
    STRCMP_EQUAL("2/0/12", res->uri_path());

    delete res;
}

void Test_M2MResource::test_get_parent_object_instance()
{

    uint8_t value[] = {"1234"};
    m2mbase_stub::string_value = "2/0/12";

    M2MResource* res = new M2MResource(*object_instance, 12, M2MBase::Dynamic, "res_type", M2MBase::INTEGER, value, sizeof(value), "2/0/12");

    CHECK(object_instance != NULL);

    // Only for the code coverage
    CHECK(&res->get_parent_object_instance() == object_instance);
    delete res;
}

void Test_M2MResource::test_set_observation_handler()
{
    resource->set_observation_handler(handler);
    CHECK(resource->observation_handler() == handler);
}

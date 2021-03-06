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
#ifndef M2M_INTERFACE_IMPL_H
#define M2M_INTERFACE_IMPL_H

#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mserver.h"
#include "mbed-client/m2mconnectionobserver.h"
#include "mbed-client/m2mconnectionsecurity.h"
#include "include/m2mnsdlobserver.h"
#include "include/m2mnsdlinterface.h"
#include "mbed-client/m2mtimerobserver.h"
#include "mbed-client/m2mtimer.h"
#include "mbed-client/m2mconnectionhandler.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mcorememory.h"

//FORWARD DECLARATION
class M2MConnectionSecurity;
class EventData;
class M2MUpdateRegisterData;
class M2MRegisterData;
/**
 *  @brief M2MInterfaceImpl.
 *  This class implements handling of all mbed Client Interface operations
 *  defined in OMA LWM2M specifications.
 *  This includes Bootstrapping, Client Registration, Device Management &
 *  Service Enablement and Information Reporting.
 */

class  M2MInterfaceImpl : public M2MInterface,
                          public M2MNsdlObserver,
                          public M2MConnectionObserver,
                          public M2MTimerObserver,
                          public M2MCoreMemory
{
private:
    // Prevents the use of assignment operator by accident.
    M2MInterfaceImpl& operator=( const M2MInterfaceImpl& /*other*/ );

    // Prevents the use of copy constructor by accident
    M2MInterfaceImpl( const M2MInterfaceImpl& /*other*/ );

friend class M2MInterfaceFactory;

private:

    /**
     * @brief Constructor
     * @param observer, Observer to pass the event callbacks for various
     * interface operations.
     * @param listen_port Listening port for the endpoint, default is 8000.
     * @param mode Binding mode of the client, default is UDP
     * @param stack Network stack to be used for connection, default is LwIP_IPv4.
     */
    M2MInterfaceImpl(M2MInterfaceObserver& observer,
                     const uint16_t listen_port,
                     M2MInterface::BindingMode mode,
                     M2MInterface::NetworkStack stack);

    /**
     * @brief Setup, handles all memory allocation
     * @param endpoint_name Endpoint name of the client.
     * @param endpoint_type Endpoint type of the client.
     * @param life_time Life time of the client in seconds
     * @param domain Domain of the client.
     * @param context_address Context address, default is empty.
     * @return true if setup is successful else false.
     */
    bool setup(const char *endpoint_name,
               const char *endpoint_type,
               const int32_t life_time,
               const char *domain,
               const char *context_address);

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MInterfaceImpl();

    /**
     * @brief Initiates bootstrapping of the client with the provided Bootstrap
     * server information.
     * @param security_object Security object which contains information
     * required for successful bootstrapping of the client.
     */
    virtual void bootstrap(M2MSecurity *security);

    /**
     * @brief Cancels on going bootstrapping operation of the client. If the client has
     * already successfully bootstrapped then this function deletes existing
     * bootstrap information from the client.
     */
    virtual void cancel_bootstrap();

    /**
     * @brief Initiates registration of the provided Security object to the
     * corresponding LWM2M server.
     * @param security_object Security object which contains information
     * required for registering to the LWM2M server.
     * If client wants to register to multiple LWM2M servers then it has call
     * this function once for each of LWM2M server object separately.
     * @param object_list Objects which contains information
     * which the client want to register to the LWM2M server.
     */
    virtual void register_object(M2MSecurity *security_object, const M2MObjectList &object_list);

    /**
     * @brief Updates or refreshes the client's registration on the LWM2M
     * server.
     * @param security_object Security object from which the device object
     * needs to update registration, if there is only one LWM2M server registered
     * then this parameter can be NULL.
     * @param lifetime Lifetime for the endpoint client in seconds.
     */
    virtual void update_registration(M2MSecurity *security_object, const uint32_t lifetime = 0);

    /**
     * @brief Updates or refreshes the client's registration on the LWM2M
     * server. Use this function to publish new objects to LWM2M server.
     * @param security_object The security object from which the device object
     * needs to update the registration. If there is only one LWM2M server registered,
     * this parameter can be NULL.
     * @param object_list Objects that contain information about the
     * client attempting to register to the LWM2M server.
     * @param lifetime The lifetime of the endpoint client in seconds. If the same value
     * has to be passed, set the default value to 0.
     */
    virtual void update_registration(M2MSecurity *security_object, const M2MObjectList &object_list,
                                     const uint32_t lifetime = 0);

    /**
     * @brief Unregisters the registered object from the LWM2M server
     * @param security_object Security object from which the device object
     * needs to be unregistered. If there is only one LWM2M server registered
     * this parameter can be NULL.
     */
    virtual void unregister_object(M2MSecurity* security = NULL);

    /**
     * @brief Sets the function which will be called indicating client
     * is going to sleep when the Binding mode is selected with Queue mode.
     * @param callback A function pointer that will be called when client
     * goes to sleep.
     */
    virtual void set_queue_sleep_handler(callback_handler handler);

    /**
     * @brief Sets the network interface handler that is used by client to connect
     * to a network over IP.
     * @param handler A network interface handler that is used by client to connect.
     *  This API is optional but provides a mechanism for different platforms to
     * manage usage of underlying network interface by client.
     */
    virtual void set_platform_network_handler(void *handler = NULL);

    /**
     * \brief Sets the function callback that will be called by mbed-client for
     * fetching random number from application for ensuring strong entropy.
     * \param random_callback A function pointer that will be called by mbed-client
     * while performing secure handshake.
     * Function signature should be uint32_t (*random_number_callback)(void);
     */
    virtual void set_random_number_callback(random_number_cb callback);

    /**
     * \brief Sets the function callback that will be called by mbed-client for
     * providing entropy source from application for ensuring strong entropy.
     * \param entropy_callback A function pointer that will be called by mbed-client
     * while performing secure handshake.
     * Function signature , if using mbed-client-mbedtls should be
     * int (*mbedtls_entropy_f_source_ptr)(void *data, unsigned char *output,
     *                                     size_t len, size_t *olen);
     */
    virtual void set_entropy_callback(entropy_cb callback);

    /**
     * @brief Updates the endpoint name.
     * @param name New endpoint name
     */
    virtual void update_endpoint(const char *name);

    /**
     * @brief Updates the domain name.
     * @param domain New domain name
     */
    virtual void update_domain(const char *domain);

    /**
     * @brief Return internal endpoint name
     * Note: this is still returning a copy in String as the returned value is manufactured on the fly.
     * @return internal endpoint name
     */
    virtual const String internal_endpoint_name() const;

    /**
     * @brief Return internal endpoint name
     * @return internal endpoint name
     */
    virtual const char *internal_endpoint_name_str() const;

    /**
     * @brief Return error description for the latest error code
     * @return Error description string
     */
    virtual const char *error_description() const;

protected: // From M2MNsdlObserver

    virtual void coap_message_ready(uint8_t *data_ptr,
                                    uint16_t data_len,
                                    sn_nsdl_addr_s *address_ptr);

    virtual void client_registered(M2MServer *server_object);

    virtual void registration_updated(const M2MServer &server_object);

    virtual void registration_error(uint8_t error_code, bool retry = false);

    virtual void client_unregistered();

    virtual void bootstrap_done(M2MSecurity *security_object);

    virtual void bootstrap_wait(M2MSecurity *security_object);

    virtual void bootstrap_error_wait(const char *reason);

    virtual void bootstrap_error(const char *reason);

    virtual void coap_data_processed();

    virtual void value_updated(M2MBase *base);

protected: // From M2MConnectionObserver

    virtual void data_available(uint8_t* data,
                                uint16_t data_size,
                                const M2MConnectionObserver::SocketAddress &address);

    virtual void socket_error(uint8_t error_code, bool retry = true);

    virtual void address_ready(const M2MConnectionObserver::SocketAddress &address,
                               M2MConnectionObserver::ServerType server_type,
                               const uint16_t server_port);

    virtual void data_sent();

protected: // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type);


private: // state machine state functions

    /**
    * When the state is Idle.
    */
    void state_idle(EventData* data);

    /**
    * When the client starts bootstrap.
    */
    void state_bootstrap( EventData *data);

    /**
    * When the bootstrap server address is resolved.
    */
    void state_bootstrap_address_resolved( EventData *data);

    /**
    * When the bootstrap resource is created.
    */
    void state_bootstrap_resource_created( EventData *data);

    /**
    * When the server has sent response and bootstrapping is done.
    */
    void state_bootstrapped( EventData *data);

    /**
    * When the client starts register.
    */
    void state_register( EventData *data);

    /**
    * When the server address for register is resolved.
    */
    void state_register_address_resolved( EventData *data);

    /**
    * When the client is registered.
    */
    void state_registered( EventData *data);

    /**
    * When the client is updating registration.
    */
    void state_update_registration( EventData *data);

    /**
    * When the client starts unregister.
    */
    void state_unregister( EventData *data);

    /**
    * When the client has been unregistered.
    */
    void state_unregistered( EventData *data);

    /**
    * When the coap data is been sent through socket.
    */
    void state_sending_coap_data( EventData *data);

    /**
    * When the coap data is sent successfully.
    */
    void state_coap_data_sent( EventData *data);

    /**
    * When the socket is receiving coap data.
    */
    void state_receiving_coap_data( EventData *data);

    /**
    * When the socket has received coap data.
    */
    void state_coap_data_received( EventData *data);

    /**
    * When the coap message is being processed.
    */
    void state_processing_coap_data( EventData *data);

    /**
    * When the coap message has been processed.
    */
    void state_coap_data_processed( EventData *data);

    /**
    * When the client is waiting to receive or send data.
    */
    void state_waiting( EventData *data);

    /**
     * Start registration update.
     */
    void start_register_update(M2MUpdateRegisterData *data);

    /**
    * State enumeration order must match the order of state
    * method entries in the state map
    */
    enum E_States {
        STATE_IDLE = 0,
        STATE_BOOTSTRAP,
        STATE_BOOTSTRAP_ADDRESS_RESOLVED,
        STATE_BOOTSTRAP_RESOURCE_CREATED,
        STATE_BOOTSTRAP_WAIT,
        STATE_BOOTSTRAP_ERROR_WAIT, // 5
        STATE_BOOTSTRAPPED,
        STATE_REGISTER,
        STATE_REGISTER_ADDRESS_RESOLVED,
        STATE_REGISTERED,
        STATE_UPDATE_REGISTRATION, // 10
        STATE_UNREGISTER,
        STATE_UNREGISTERED,
        STATE_SENDING_COAP_DATA,
        STATE_COAP_DATA_SENT,
        STATE_COAP_DATA_RECEIVED, // 15
        STATE_PROCESSING_COAP_DATA,
        STATE_COAP_DATA_PROCESSED,
        STATE_WAITING,
        STATE_MAX_STATES
    };

    /**
     * @brief Redirects the state machine to right function.
     * @param current_state Current state to be set.
     * @param data Data to be passed to the state function.
     */
    void state_function( uint8_t current_state, EventData* data  );

    /**
     * @brief State Engine maintaining state machine logic.
     */
    void state_engine(void);

    /**
    * External event which can trigger the state machine.
    * @param New The state to which the state machine should go.
    * @param data The data to be passed to the state machine.
    */
    void external_event(uint8_t, EventData* = NULL);

    /**
    * Internal event generated by state machine.
    * @param New State which the state machine should go to.
    * @param data The data to be passed to the state machine.
    */
    void internal_event(uint8_t, EventData* = NULL);

    /**
    * Function which returns if client is in queue mode or not.
    * @return true if in queue mode else false.
    */
    bool is_queue_mode() const;

    enum
    {
        EVENT_IGNORED = 0xFE,
        CANNOT_HAPPEN
    };

    /**
     * Helper method for extracting the IP address part and port from the
     * given server address.
     * @param server_address Source URL (without "coap" or "coaps" prefix).
     * @param ip_address The extracted IP.
     * @param port The extracted port.
     */
    void process_address(const char* server_address, uint16_t& port);

    /**
     * Helper method for storing the error description to _error_description if the feature
     * has not been turned off.
     * @param error description
     */
    void set_error_description(const char *description);

    void add_object(M2MRegisterData *data, M2MObject *object);

    enum ReconnectionState{
        None,
        WithUpdate,
        FullRegistration,
        Unregistration
    };

private:

    M2MNsdlInterface            _nsdl_interface;
    M2MTimer                    _retry_timer;
    callback_handler            _callback_handler;
    M2MConnectionSecurity       _security_connection; // Doesn't own
    M2MConnectionHandler        _connection_handler;
    M2MInterfaceObserver        &_observer;
    M2MSecurity                 *_register_server; //TODO: to be the list not owned
    M2MTimer                    *_queue_sleep_timer;
    M2MSecurity                 *_security;
    EventData                   *_event_data;
    M2MTimer                    *_registration_flow_timer;
    uint32_t                    _reconnection_time;
    char                        _server_ip_address[MAX_ALLOWED_IP_STRING_LENGTH];
#ifndef DISABLE_ERROR_DESCRIPTION
    // The DISABLE_ERROR_DESCRIPTION macro will reduce the flash usage by ~1800 bytes.
    char                         _error_description[MAX_ALLOWED_STRING_LENGTH];
#endif
    uint16_t                    _server_port;
    uint16_t                    _listen_port;
    const uint8_t               _max_states;
    uint8_t                     _current_state;
    uint8_t                     _initial_reconnection_time:4;
    BindingMode                 _binding_mode:4;
    ReconnectionState           _reconnection_state:2;
    bool                        _event_ignored:1;
    bool                        _event_generated:1;
    bool                        _reconnecting:1;
    bool                        _retry_timer_expired:1;
    bool                        _bootstrapped:1;
    bool                        _queue_mode_timer_ongoing:1;

    friend class Test_M2MInterfaceImpl;

};

#define BEGIN_TRANSITION_MAP \
    static const uint8_t TRANSITIONS[] = {\

#define TRANSITION_MAP_ENTRY(entry)\
    entry,

#define END_TRANSITION_MAP(data) \
    0 };\
    external_event(TRANSITIONS[_current_state], data);

#endif //M2M_INTERFACE_IMPL_H



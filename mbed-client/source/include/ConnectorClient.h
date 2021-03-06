//----------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//
// (C) COPYRIGHT 2016 ARM Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//----------------------------------------------------------------------------
#ifndef __CONNECTOR_CLIENT_H__
#define __CONNECTOR_CLIENT_H__

#include "mbed-client/functionpointer.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mtimerobserver.h"
#include "mbed-client/m2mtimer.h"
#include "mbed-client/m2mcorememory.h"
#include "include/CloudClientStorage.h"

class ConnectorClientCallback;

using namespace std;


/**
 * \brief ConnectorClientEndpointInfo
 * A structure that contains the needed endpoint information to register with the Cloud service.
 * Note: this should be changed to a class instead of struct and/or members changed to "const char*".
 */
struct ConnectorClientEndpointInfo {

public:
    ConnectorClientEndpointInfo(M2MSecurity::SecurityModeType m) : mode(m) {
        memset(endpoint_name, 0, MAX_ALLOWED_STRING_LENGTH);
        memset(account_id, 0, MAX_ALLOWED_STRING_LENGTH);
        memset(internal_endpoint_name, 0, MAX_ALLOWED_STRING_LENGTH);
    };
    ~ConnectorClientEndpointInfo() {};

public:

    char                            endpoint_name[MAX_ALLOWED_STRING_LENGTH];
    char                            account_id[MAX_ALLOWED_STRING_LENGTH];
    char                            internal_endpoint_name[MAX_ALLOWED_STRING_LENGTH];
    M2MSecurity::SecurityModeType   mode;
};

/**
 * \brief ConnectorClient
 * This class is an interface towards the M2MInterface client to handle all
 * data flow towards Connector through this client.
 * This class is intended to be used via ServiceClient, not directly.
 * This class contains also the bootstrap functionality.
 */
class ConnectorClient : public M2MInterfaceObserver,
                        public M2MTimerObserver,
                        public M2MCoreMemory {

public:
    /**
     * \brief An enum defining the different states of
     * ConnectorClient during the client flow.
     */
    enum StartupSubStateRegistration {
        State_Bootstrap_Start,
        State_Bootstrap_Started,
        State_Bootstrap_Success,
        State_Bootstrap_Failure,
        State_Registration_Start,
        State_Registration_Started,
        State_Registration_Success,
        State_Registration_Failure,
        State_Unregistered
    };

public:

    /**
    *  \brief Constructor.    
    *  \param callback, A callback for the status from ConnectorClient.
    */
    ConnectorClient(ConnectorClientCallback* callback);

    /**
    *  \brief Destructor.
    */
    ~ConnectorClient();

    /**
    *  \brief Starts the bootstrap sequence from the Service Client.
    */
    void start_bootstrap();

    /**
    *  \brief Starts the registration sequence from the Service Client.
    *  \param client_objs, A list of objects to be registered with Cloud.
    */
    void start_registration(M2MObjectList* client_objs);

    /**
    *  \brief Sends an update registration message to the LWM2M server.
    */
    void update_registration();

    /**
     * \brief Returns the M2MInterface handler.
     * \return M2MInterface, Handled for M2MInterface.
    */
    M2MInterface * m2m_interface();

    /**
     * \brief Checks whether to use Bootstrap or direct Connector mode.
     * \return True if bootstrap mode, False if direct Connector flow
    */
    bool use_bootstrap();

    /**
     * \brief Checks whether to go connector registration flow
     * \return True if connector credentials available otherwise false.
    */
    bool connector_credentials_available();

    /**
    * \brief Returns pointer to the ConnectorClientEndpointInfo object.
    * \return ConnectorClientEndpointInfo pointer.
    */
   const ConnectorClientEndpointInfo *endpoint_info() const;

public:
    // implementation of M2MInterfaceObserver:

    /**
     * \brief A callback indicating that the bootstap has been performed successfully.
     * \param server_object, The server object that contains the information fetched
     * about the LWM2M server from the bootstrap server. This object can be used
     * to register with the LWM2M server. The object ownership is passed.
     */
    virtual void bootstrap_done(M2MSecurity *server_object);

    /**
     * \brief A callback indicating that the device object has been registered
     * successfully with the LWM2M server.
     * \param security_object, The server object on which the device object is
     * registered. The object ownership is passed.
     * \param server_object, An object containing information about the LWM2M server.
     * The client maintains the object.
     */
    virtual void object_registered(M2MSecurity *security_object, const M2MServer &server_object);

    /**
     * \brief A callback indicating that the device object has been successfully unregistered
     * from the LWM2M server.
     * \param server_object, The server object from which the device object is
     * unregistered. The object ownership is passed.
     */
    virtual void object_unregistered(M2MSecurity *server_object);

    /**
     * \brief A callback indicating that the device object registration has been successfully
     * updated on the LWM2M server.
     * \param security_object, The server object on which the device object registration is
     * updated. The object ownership is passed.
     * \param server_object, An object containing information about the LWM2M server.
     * The client maintains the object.
     */
    virtual void registration_updated(M2MSecurity *security_object, const M2MServer & server_object);

    /**
     * \brief A callback indicating that there was an error during the operation.
     * \param error, An error code for the occurred error.
     */
    virtual void error(M2MInterface::Error error);

    /**
     * \brief A callback indicating that the value of the resource object is updated by the server.
     * \param base, The object whose value is updated.
     * \param type, The type of the object.
     */
    virtual void value_updated(M2MBase *base, M2MBase::BaseType type);

protected: // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type);

private:
    /**
     * \brief Redirects the state machine to right function.
     * \param current_state, The current state to be set.
     * \param data, The data to be passed to the state function.
     */
    void state_function(StartupSubStateRegistration current_state);

    /**
     * \brief The state engine maintaining the state machine logic.
     */
    void state_engine(void);

    /**
    * \brief An internal event generated by the state machine.
    * \param new_state, The new state to which the state machine should go.
    * \param data, The data to be passed to the state machine.
    */
    void internal_event(StartupSubStateRegistration new_state);

    /**
    * When the bootstrap starts.
    */
    void state_bootstrap_start();

    /**
    * When the bootstrap is started.
    */
    void state_bootstrap_started();

    /**
    * When the bootstrap is successful.
    */
    void state_bootstrap_success();

    /**
    * When the bootstrap failed.
    */
    void state_bootstrap_failure();

    /**
    * When the registration starts.
    */
    void state_registration_start();

    /**
    * When the registration started.
    */
    void state_registration_started();

    /**
    * When the registration is successful.
    */
    void state_registration_success();

    /**
     * When the registration failed.
    */
    void state_registration_failure();

    /**
    * When the client is unregistered.
    */
    void state_unregistered();

    /**
     * \brief A utility function to create an M2MSecurity object
     * for registration.
     */
    void create_register_object();

    /**
     * \brief A utility function to create an M2MSecurity object
     * for bootstrap.
     */
    bool create_bootstrap_object();

    /**
     * \brief A utility function to set the connector credentials
     * in storage. This includes endpoint, domain, connector URI
     *  and certificates.
     * \param server, The Connector certificates.
     */
    ccs_status_e set_connector_credentials(M2MSecurity *server);

    /**
     * \brief A utility function to check whether bootstrap credentials are stored in KCM.
     */
    bool bootstrap_credentials_stored_in_kcm();

    /**
    * \brief Returns the binding mode selected by the client
    * through the configuration.
    * \return Binding mode of the client.
    */
    static M2MInterface::BindingMode transport_mode();

private:
    // A callback to be called after the sequence is complete.
    ConnectorClientCallback*            _callback;
    StartupSubStateRegistration         _current_state;
    bool                                _event_generated;
    bool                                _state_engine_running;
    M2MInterface                        *_interface;
    M2MSecurity                         *_register_security;
    M2MSecurity                         *_bootstrap_security;
    ConnectorClientEndpointInfo         _endpoint_info;
    M2MObjectList                       *_client_objs;    
    M2MTimer                            _rebootstrap_timer;
    bool                                _bootstrap_done;
};

/**
 * \brief ConnectorClientCallback
 * A callback class for passing the client progress and error condition to the
 * ServiceClient class object.
 */
class ConnectorClientCallback {
public:

    /**
    * \brief Indicates that the registration or unregistration operation is complete
    * with success or failure.
    * \param status, Indicates success or failure in terms of status code.
    */
    virtual void registration_process_result(ConnectorClient::StartupSubStateRegistration status) = 0;

    /**
    * \brief Indicates the Connector error condition of an underlying M2MInterface client.
    * \param error, Indicates an error code translated from M2MInterface::Error.
    * \param reason, Indicates human readable text for error description.
    */
    virtual void connector_error(M2MInterface::Error error, const char *reason) = 0;

    /**
    * \brief A callback indicating that the value of the resource object is updated
    *  by the LWM2M Cloud server.
    * \param base, The object whose value is updated.
    * \param type, The type of the object.
    */
    virtual void value_updated(M2MBase *base, M2MBase::BaseType type) = 0;
};

#endif // !__CONNECTOR_CLIENT_H__

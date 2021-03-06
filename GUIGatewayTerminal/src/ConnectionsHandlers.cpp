/* 
 * File:   ConnectionsHandlers.cpp
 * Author: cleo
 * 
 * Created on July 15, 2014, 11:09 AM
 */

#include <ConnectionsHandlers.h>
#include <GlobalDefinitions.h>
#include <BusinessLogic.h>
#include <GUIManager.h>
#include <Parameters.h>

#include <support/CapabilitiesManager.h>
#include <support/SubscriptionsManager.h>
#include <support/ServicesManager.h>
#include <support/DataStorage.h>

#include <synapse/PredefinedServicesAndCaps.h>
#include <synapse/Messages.h>

using namespace Synapse;

ConnectionsHandlers::ConnectionsHandlers() { 
    connections_ = new ConnectionsManager();
    connections_->setConnectionCallbacks(this);
    connections_->setRequestCallbacks(this);
    connections_->setResponseCallbacks(this);
    connections_->setUpdateCallbacks(this);
}

ConnectionsHandlers::~ConnectionsHandlers() throw() {
    delete connections_;
}

bool ConnectionsHandlers::initialize(){
    
    std::string key,cert;
    long timeoutms;
    PARAMETERS->getValue("certificate", cert); 
    PARAMETERS->getValue("privateKey", key);       
    PARAMETERS->getValue("pollTimeout", timeoutms);       
    bool result = connections_->initialize(cert, key, timeoutms);
    
    if (result) {
        std::string gatewayHost;
        long gatewayPort;
        PARAMETERS->getValue("gatewayHost", gatewayHost);
        PARAMETERS->getValue("gatewayPort", gatewayPort);    
        
        int intentos = 5;
        do {
            result = connections_->connectToClient( gatewayHost, gatewayPort, 5 );
            intentos--;
        } while ((!result) && (intentos > 0));
    }

    return result;
}

void ConnectionsHandlers::sendMessageToClient( std::string clientID, Packet& msg ) {

    connections_->sendMessageToClient(clientID, msg );
    
}

void ConnectionsHandlers::sendMessageToClient( Ident ident, Packet& msg ) {

    connections_->sendMessageToClient( ident, msg );
    
}

/* ************** Connection Callbacks ************** */
void ConnectionsHandlers::OnClient_Connect(SynapseConnector* peer) {
    LOG->LogMessage(LOG_INFO, "NEW CONNECTION FROM %s (fd %d)", peer->getID().c_str(), peer->getFD());
}

void ConnectionsHandlers::OnClient_Disconnect(SynapseConnector* peer){
    LOG->LogMessage(LOG_INFO, "DISCONNECTION FROM %s (fd %d)", peer->getID().c_str(), peer->getFD());
}    

/* **************** Request Callbacks **************** */
void ConnectionsHandlers::OnIdent_Request(Request* request, SynapseConnector* peer){
    
    std::string moduleDescription;
    PARAMETERS->getValue("moduleDescription", moduleDescription);
    Ident ident(peer->getLocalIP().c_str(), peer->getLocalPort(), ModuleType_IOTerminal, moduleDescription.c_str());
    IdentResp resp(ident);
    peer->write( (Packet&) resp);    
}

void ConnectionsHandlers::OnServices_Request(Request* request, SynapseConnector* peer){
    ServicesResp servResp;
    servResp.setServices( SERVICES->getLocalServices() );
    peer->write( (Packet&) servResp);    
}

void ConnectionsHandlers::OnCapabilities_Request(Request* request, SynapseConnector* peer){
    CapabilitiesResp capResp;
    capResp.setCapabilities( CAPABILITIES->getLocalCapabilities() );
    peer->write( (Packet&) capResp);
}

void ConnectionsHandlers::OnSubscription_Request(Subscribe* request, SynapseConnector* peer){
    SubscribeResult result = SubscribeResult_UnknownCapability;
    SubscribeResp resp(request->getCapability(), request->getUserHash(), result);
    peer->write( (Packet&) resp);    
}

void ConnectionsHandlers::OnUnsubscription_Request(Unsubscribe* request, SynapseConnector* peer){
    UnsubscribeResult result = UnsubscribeResult_UnknownCapability;
    UnsubscribeResp resp(request->getCapability(), request->getUserHash(), result);
    peer->write( (Packet&) resp);    
}

void ConnectionsHandlers::OnSetAnalogValue_Request(SetAnalogValue* request, SynapseConnector* peer){
    SetValueResp resp(request->getSubCommandID(), request->getCapability(), request->getUserHash(), SetValueResult_UnknownCapability);
    peer->write( (Packet&) resp);
}

void ConnectionsHandlers::OnSetDigitalValue_Request(SetDigitalValue* request, SynapseConnector* peer){
    SetValueResp resp(request->getSubCommandID(), request->getCapability(), request->getUserHash(), SetValueResult_UnknownCapability);
    peer->write( (Packet&) resp);
}

void ConnectionsHandlers::OnGetNetwork_Request(GetServiceData* request, SynapseConnector* peer){
    GetServiceDataResp resp(request->getSubCommandID(), request->getService(), GetDataResult_UnknownService);
    peer->write( (Packet&) resp);    
}

void ConnectionsHandlers::OnGetUserHash_Request(UserValidation* request, SynapseConnector* peer){
    GetServiceDataResp resp(request->getSubCommandID(), request->getService(), GetDataResult_UnknownService);
    peer->write( (Packet&) resp);    
}

void ConnectionsHandlers::OnGetUserSecurityLevel_Request(UserSecurityLevelQuery* request, SynapseConnector* peer){
    GetServiceDataResp resp(request->getSubCommandID(), request->getService(), GetDataResult_UnknownService);
    peer->write( (Packet&) resp);    
}                 

void ConnectionsHandlers::OnGetAnalogValue_Request(GetAnalogValue* request, SynapseConnector* peer){
    GetValueResp resp(request->getSubCommandID(), request->getCapability(), request->getUserHash(), GetDataResult_UnknownCapability);
    peer->write( (Packet&) resp);
}

void ConnectionsHandlers::OnGetDigitalValue_Request(GetDigitalValue* request, SynapseConnector* peer){
    GetValueResp resp(request->getSubCommandID(), request->getCapability(), request->getUserHash(), GetDataResult_UnknownCapability);
    peer->write( (Packet&) resp);
}

void ConnectionsHandlers::OnRevokeSubscription_Request(RevokeSubscription* request, SynapseConnector* peer){
}

/* **************** Response Callbacks **************** */
void ConnectionsHandlers::OnIdent_Response(IdentResp* response, SynapseConnector* peer){
    
    if (identifiedHosts_.find( peer->getID() ) != identifiedHosts_.end()) {
        identifiedHosts_.at( peer->getID() ) = response->getIdent();
    } else {
        identifiedHosts_.insert( std::pair<std::string, Ident>( peer->getID(), response->getIdent() ) );
    }

    std::cout << "Connection from " << response->getIdent().getModuleDescription() << " received! IP:" << response->getIdent().getHost() << " Port:" 
              << response->getIdent().getPort() << " PeerID:" << peer->getID() << std::endl;
    
    Request reqServ(CommandID_Services);
    peer->write( (Packet&) reqServ);            
    
    Request reqCap(CommandID_Capabilities);
    peer->write( (Packet&) reqCap);          

}

void ConnectionsHandlers::OnServices_Response(ServicesResp* response, SynapseConnector* peer){
    
    ServicesList list = response->getServices();
    
    SERVICES->addForeignServices( identifiedHosts_.at( peer->getID() ), list);    
    
    std::cout << list.size() << " services obtained from " << peer->getID() << std::endl;            
    
    for (ServicesList::iterator it = list.begin(); it != list.end(); it++){
        ServiceDefinition def = *it;
                
        std::cout << "ID " << def.getId() << ") Service " << def.getDescription() << ", type " << def.getType() << std::endl;            
        
        if ((def.getType() == ServiceType_UserValidation) && STORAGE->getUserHash().empty()) {
            std::string usr, pwd;
            PARAMETERS->getValue("user", usr);
            PARAMETERS->getValue("password", pwd);                
            UserValidation request( def, usr, pwd );
            CONN_HANDLERS->sendMessageToClient( peer->getID(), (Packet&) request );        
        }
    }

}

void ConnectionsHandlers::OnCapabilities_Response(CapabilitiesResp* response, SynapseConnector* peer){
    
    CapabilitiesList list = response->getCapabilities();
    
    CAPABILITIES->addForeignCapabilities(identifiedHosts_.at( peer->getID() ), list);
    
    std::cout << list.size() << " capabilities obtained from " << peer->getID() << std::endl;   
    
    for (CapabilitiesList::iterator it = list.begin(); it != list.end(); it++){
        CapabilityDefinition def = *it;
        
        std::cout << "ID " << def.getId() << ") Capability " << def.getDescription() << ", type " << def.getType() << ", access " << def.getAccess() << std::endl;   
        
        if (!SUBSCRIPTIONS->existsSubscription(def.getId())) {
            if (def.getId() == ID_RESOURCESUPDATE_CAPABILITY){
                Subscribe request(def, STORAGE->getUserHash());
                sendMessageToClient(peer->getID(), (Packet&) request);                 
            }
        }
        
        if ( def.getId() > MAX_PREDEFINED_CAPABILITY) {
            GUI->updateGUIRepresentation( ID_CAPABILITIES_LIST_SERVICE, def.getId() );
        }
    }  
}

void ConnectionsHandlers::OnSubscription_Response(SubscribeResp* response, SynapseConnector* peer){
    
    if ((response->getResult() == SubscribeResult_Ok) || (response->getResult() == SubscribeResult_AlreadySubscribed)) {
        
        SUBSCRIPTIONS->addSubscription(response->getCapability(), peer->getID() );
        if (GUI->addGUIRepresentation(response->getCapability())){
            
            if (response->getCapability().getId() == ID_RESOURCESUPDATE_CAPABILITY){ 
                
                CapabilitiesList capabilities;
                CAPABILITIES->getCapabilitiesByType( Capability_Undefined, SourceData_Foreign, capabilities );
                
                for (CapabilitiesList::iterator it = capabilities.begin(); it != capabilities.end(); it++) {
                    if ( it->getId() > MAX_PREDEFINED_CAPABILITY) {
                        GUI->updateGUIRepresentation( ID_CAPABILITIES_LIST_SERVICE, it->getId() );
                    }
                }
                
            } else {
            
                GetValue* request = NULL;
                CapabilitiesTypes type = response->getCapability().getType();

                if ((type == Capability_AnalogInput) || (type == Capability_AnalogOutput)) {
                    request = new GetAnalogValue(response->getCapability(), STORAGE->getUserHash());

                } else if ((type == Capability_DigitalInput) || (type == Capability_DigitalOutput)) { 
                    request = new GetDigitalValue(response->getCapability(), STORAGE->getUserHash());
                }

                if (request) {
                    std::cout << "Get Value Requested to " << peer->getID() << " Capability: " << response->getCapability().getDescription() << std::endl;
                    //sendMessageToClient( peer->getID(), (Packet&) *request ); 
                    peer->write( (Packet&) *request ); 
                    delete request;
                }  
            }
        }   
    }
}

void ConnectionsHandlers::OnUnsubscription_Response(UnsubscribeResp* response, SynapseConnector* peer){
    if (SUBSCRIPTIONS->existsSubscription(response->getCapability().getId())) {
        SUBSCRIPTIONS->removeSubscription(response->getCapability() );
    }
}

void ConnectionsHandlers::OnSetAnalogValue_Response(SetValueResp* response, SynapseConnector* peer){
    std::cout << "Set Analog Value Response from " << peer->getID() << " Capability: " << response->getCapability().getDescription() << " Result: " << response->getResult() << std::endl;   
}

void ConnectionsHandlers::OnSetDigitalValue_Response(SetValueResp* response, SynapseConnector* peer){
    std::cout << "Set Digital Value Response from " << peer->getID() << " Capability: " << response->getCapability().getDescription() << " Result: " << response->getResult() << std::endl;   
}

void ConnectionsHandlers::OnGetNetwork_Response(NetworkResp* response, SynapseConnector* peer){
    long basePort, maxAcceptors;
    PARAMETERS->getValue("acceptorBasePort", basePort);
    PARAMETERS->getValue("maxAcceptorsPerHost", maxAcceptors);
    
    NetworkList neighbors = response->getNetworkList();
    
    std::cout << neighbors.size() << " neighbors obtained from " << peer->getID() << std::endl;   
    
    for (NetworkList::iterator it = neighbors.begin(); it != neighbors.end(); it++){
        for (long port = basePort; port < basePort + maxAcceptors; port++) {
            
            std::cout << "Neighbor " << it->getIp() << ", type " << it->getType() << std::endl;
            
            if ( (peer->getLocalIP().compare( it->getIp() ) == 0) && (port == (long) connections_->getAcceptorPort()) ) //Soy yo mismo
                continue;
            
            bool result = connections_->connectToClient( it->getIp(), port, 2);
            std::cout << "Neighbor " << it->getIp() << ":" << port << " connection Result: " << result << std::endl;
        }
    }
}

void ConnectionsHandlers::OnGetUserHash_Response(UserValidationResp* response, SynapseConnector* peer){
    
    std::cout << "UserHash Response from " << peer->getID() << " Hash: " << response->getHash() << " Result: " << response->getResult() << std::endl;   
    
    if (response->getResult() == ValidationResult_Ok){
        STORAGE->setUserHash( response->getHash() );
    } else {
        STORAGE->setUserHash( "" );
    }   
}

void ConnectionsHandlers::OnGetUserSecurityLevel_Response(UserSecurityLevelQueryResp* response, SynapseConnector* peer){
    //Nada, como no tenemos servicios ni capabilities, no verificamos la identidad de nadie
}                 

void ConnectionsHandlers::OnGetAnalogValue_Response(GetAnalogValueResp* response, SynapseConnector* peer){
    std::cout << "Get Analog Value Response from " << peer->getID() << " Capability: " << response->getCapability().getDescription() 
              << " Result: " << response->getResult() << " Value: " << response->getValue() << std::endl;   
    STORAGE->setValue(response->getCapability().getId(), response->getValue());
    GUI->updateGUIRepresentation(response->getCapability().getId(), response->getValue());
}

void ConnectionsHandlers::OnGetDigitalValue_Response(GetDigitalValueResp* response, SynapseConnector* peer){
    std::cout << "Get Digital Value Response from " << peer->getID() << " Capability: " << response->getCapability().getDescription()
              << " Result: " << response->getResult() << " Value: " << response->getValue() << std::endl;       
    STORAGE->setValue(response->getCapability().getId(), response->getValue());
    GUI->updateGUIRepresentation(response->getCapability().getId(), response->getValue());
}    

void ConnectionsHandlers::OnRevokeSubscription_Response(RevokeSubscriptionResp* request, SynapseConnector* peer){
}

void ConnectionsHandlers::OnNetwork_Update(NetworkUpdate* update, SynapseConnector* peer){
    long basePort, maxAcceptors;
    PARAMETERS->getValue("acceptorBasePort", basePort);
    PARAMETERS->getValue("maxAcceptorsPerHost", maxAcceptors);
    
    NetworkList neighbors = update->getNetworkList();
    
    std::cout << neighbors.size() << " neighbors updated from " << peer->getID() << std::endl;   
    
    for (NetworkList::iterator it = neighbors.begin(); it != neighbors.end(); it++){
        if (it->getType() == NetworkItemStatus_Added) {
            for (long port = basePort; port < basePort + maxAcceptors; port++) {
            
                std::cout << "Neighbor " << it->getIp() << ", type " << it->getType() << std::endl;
                
                if ( (peer->getLocalIP().compare( it->getIp() ) == 0) && (port == (long) connections_->getAcceptorPort()) ) //Soy yo mismo
                    continue;
                            
                bool result = connections_->connectToClient( it->getIp(), port, 2);
                std::cout << "Neighbor " << it->getIp() << ":" << port << " connection Result: " << result << std::endl;
            }
        }
    }    
}

void ConnectionsHandlers::OnResources_Update(ResourcesUpdate* update, SynapseConnector* peer){
    
    std::cout << "On Resources Update from " << peer->getID() << " Type: " << update->getUpdateType() << std::endl;   
    
    ResourceUpdateType resourcesUpdated = update->getUpdateType();
    if ((resourcesUpdated == ResourceUpdateType_Services) || (resourcesUpdated == ResourceUpdateType_Both)) {    
        Request reqServ(CommandID_Services);
        peer->write( (Packet&) reqServ);            
    }
    
    if ((resourcesUpdated == ResourceUpdateType_Capabilities) || (resourcesUpdated == ResourceUpdateType_Both)) {    
        Request reqCap(CommandID_Capabilities);
        peer->write( (Packet&) reqCap);    
    }    
}

void ConnectionsHandlers::OnAnalogValue_Update(AnalogValueUpdate* update, SynapseConnector* peer){
    std::cout << "Analog Value Update from " << peer->getID() << " Capability: " << update->getCapability().getDescription() 
              << " Value: " << update->getValue() << std::endl;     
    STORAGE->setValue(update->getCapability().getId(), update->getValue());
    GUI->updateGUIRepresentation(update->getCapability().getId(), update->getValue());
}

void ConnectionsHandlers::OnDigitalValue_Update(DigitalValueUpdate* update, SynapseConnector* peer){
    std::cout << "Digital Value Update from " << peer->getID() << " Capability: " << update->getCapability().getDescription() 
              << " Value: " << update->getValue() << std::endl;         
    STORAGE->setValue(update->getCapability().getId(), update->getValue());
    GUI->updateGUIRepresentation(update->getCapability().getId(), update->getValue());
} 

void ConnectionsHandlers::OnRevokedUser_Update(RevokedUserUpdate* update, SynapseConnector* peer){
}
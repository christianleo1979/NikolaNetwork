
#include <synapse/SetValueResp.h>

SetValueResp::SetValueResp(SubCommandID subCommandId, CapabilityDefinition capability, std::string userHash, SetValueResult result) 
: Response(CommandID_SetValue, subCommandId), capability_(capability), userHash_(userHash), result_(result) {

}

SetValueResp::~SetValueResp() {
}

std::string SetValueResp::encode( ) {
    std::ostringstream stream;
    stream << capability_.encode() << FIELD_SEPARATOR << userHash_.c_str() << FIELD_SEPARATOR << result_ << FIELD_SEPARATOR; 
        
    return Response::encode( stream.str() );
}

bool SetValueResp::decode(const void* buffer, size_t len) {

    std::string aux = std::string((const char*) buffer, len);

    unsigned int pos = 0;
    if (!Response::decode( buffer, len, pos))
        return false;
    
    std::string streamCap;
    if (!getField( aux, pos, streamCap ) )
        return false;        
    
    if (!capability_.decode(streamCap) )
        return false;             
    
    if (!getField( aux, pos, userHash_ ) ) {     //Si el hash está vacío, solo es válido si el nivel de seguridad es 0 y es de lectura
        if ((capability_.getAccess() != AccessType_Read) && (capability_.getMinReadSecuritylevel() != 0)) {
            return false;    
        } else {
            pos++;
        }
    }
    
    long auxResult;
    if (!getField( aux, pos, auxResult ) )
        return false;
    result_ = static_cast<SetValueResult>(auxResult);
            
    LOG->LogMessage(LOG_DEBUG,"SetValueResp::decode(): Capability type=%d id=%d result=%d", (int)capability_.getType(), capability_.getId(), (int) result_);
    return true;
}



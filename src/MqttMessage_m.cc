//
// Generated file, do not edit! Created by nedtool 4.6 from MqttMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "MqttMessage_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("MqttMessageType");
    if (!e) enums.getInstance()->add(e = new cEnum("MqttMessageType"));
    e->insert(MQTT_SEARCHGW, "MQTT_SEARCHGW");
    e->insert(MQTT_GWINFO, "MQTT_GWINFO");
    e->insert(MQTT_CONNECT, "MQTT_CONNECT");
    e->insert(MQTT_CONNACK, "MQTT_CONNACK");
    e->insert(MQTT_PUBLISH, "MQTT_PUBLISH");
    e->insert(MQTT_PUBACK, "MQTT_PUBACK");
    e->insert(MQTT_ADVERTISE, "MQTT_ADVERTISE");
    e->insert(MQTT_DISCONNECT, "MQTT_DISCONNECT");
    e->insert(SELF_MESSAGE_F, "SELF_MESSAGE_F");
    e->insert(SELF_MESSAGE_E, "SELF_MESSAGE_E");
    e->insert(SELF_TIMEOUT, "SELF_TIMEOUT");
    e->insert(SELF_RECEACK, "SELF_RECEACK");
    e->insert(SELF_TIMEOUT_S, "SELF_TIMEOUT_S");
    e->insert(SELF_TIMEOUT_C, "SELF_TIMEOUT_C");
    e->insert(SELF_TIMEOUT_A, "SELF_TIMEOUT_A");
);

Register_Class(MqttMessage);

MqttMessage::MqttMessage(const char *name, int kind) : ::cPacket(name,kind)
{
    this->srcAddress_var = 0;
    this->destAddress_var = 0;
    this->managerAddr_var = 0;
    this->gatewayAddr_var = 0;
    this->gatewayProcId_var = 0;
    this->brokerProcId_var = 0;
    this->departureTime_var = 0;
    this->serialNumber_var = 0;
    this->sensorRetry_var = 0;
    this->gatewayRetry_var = 0;
    this->rtoInit_var = 0;
    this->rtoCalc_var = 0;
    this->clientId_var = 0;
    this->rtt_var = 0;
    this->rttS_var = 0;
    this->rttW_var = 0;
    this->rto_var = 0;
}

MqttMessage::MqttMessage(const MqttMessage& other) : ::cPacket(other)
{
    copy(other);
}

MqttMessage::~MqttMessage()
{
}

MqttMessage& MqttMessage::operator=(const MqttMessage& other)
{
    if (this==&other) return *this;
    ::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MqttMessage::copy(const MqttMessage& other)
{
    this->srcAddress_var = other.srcAddress_var;
    this->destAddress_var = other.destAddress_var;
    this->managerAddr_var = other.managerAddr_var;
    this->gatewayAddr_var = other.gatewayAddr_var;
    this->gatewayProcId_var = other.gatewayProcId_var;
    this->brokerProcId_var = other.brokerProcId_var;
    this->departureTime_var = other.departureTime_var;
    this->serialNumber_var = other.serialNumber_var;
    this->sensorRetry_var = other.sensorRetry_var;
    this->gatewayRetry_var = other.gatewayRetry_var;
    this->rtoInit_var = other.rtoInit_var;
    this->rtoCalc_var = other.rtoCalc_var;
    this->clientId_var = other.clientId_var;
    this->rtt_var = other.rtt_var;
    this->rttS_var = other.rttS_var;
    this->rttW_var = other.rttW_var;
    this->rto_var = other.rto_var;
}

void MqttMessage::parsimPack(cCommBuffer *b)
{
    ::cPacket::parsimPack(b);
    doPacking(b,this->srcAddress_var);
    doPacking(b,this->destAddress_var);
    doPacking(b,this->managerAddr_var);
    doPacking(b,this->gatewayAddr_var);
    doPacking(b,this->gatewayProcId_var);
    doPacking(b,this->brokerProcId_var);
    doPacking(b,this->departureTime_var);
    doPacking(b,this->serialNumber_var);
    doPacking(b,this->sensorRetry_var);
    doPacking(b,this->gatewayRetry_var);
    doPacking(b,this->rtoInit_var);
    doPacking(b,this->rtoCalc_var);
    doPacking(b,this->clientId_var);
    doPacking(b,this->rtt_var);
    doPacking(b,this->rttS_var);
    doPacking(b,this->rttW_var);
    doPacking(b,this->rto_var);
}

void MqttMessage::parsimUnpack(cCommBuffer *b)
{
    ::cPacket::parsimUnpack(b);
    doUnpacking(b,this->srcAddress_var);
    doUnpacking(b,this->destAddress_var);
    doUnpacking(b,this->managerAddr_var);
    doUnpacking(b,this->gatewayAddr_var);
    doUnpacking(b,this->gatewayProcId_var);
    doUnpacking(b,this->brokerProcId_var);
    doUnpacking(b,this->departureTime_var);
    doUnpacking(b,this->serialNumber_var);
    doUnpacking(b,this->sensorRetry_var);
    doUnpacking(b,this->gatewayRetry_var);
    doUnpacking(b,this->rtoInit_var);
    doUnpacking(b,this->rtoCalc_var);
    doUnpacking(b,this->clientId_var);
    doUnpacking(b,this->rtt_var);
    doUnpacking(b,this->rttS_var);
    doUnpacking(b,this->rttW_var);
    doUnpacking(b,this->rto_var);
}

int MqttMessage::getSrcAddress() const
{
    return srcAddress_var;
}

void MqttMessage::setSrcAddress(int srcAddress)
{
    this->srcAddress_var = srcAddress;
}

int MqttMessage::getDestAddress() const
{
    return destAddress_var;
}

void MqttMessage::setDestAddress(int destAddress)
{
    this->destAddress_var = destAddress;
}

int MqttMessage::getManagerAddr() const
{
    return managerAddr_var;
}

void MqttMessage::setManagerAddr(int managerAddr)
{
    this->managerAddr_var = managerAddr;
}

int MqttMessage::getGatewayAddr() const
{
    return gatewayAddr_var;
}

void MqttMessage::setGatewayAddr(int gatewayAddr)
{
    this->gatewayAddr_var = gatewayAddr;
}

int MqttMessage::getGatewayProcId() const
{
    return gatewayProcId_var;
}

void MqttMessage::setGatewayProcId(int gatewayProcId)
{
    this->gatewayProcId_var = gatewayProcId;
}

int MqttMessage::getBrokerProcId() const
{
    return brokerProcId_var;
}

void MqttMessage::setBrokerProcId(int brokerProcId)
{
    this->brokerProcId_var = brokerProcId;
}

double MqttMessage::getDepartureTime() const
{
    return departureTime_var;
}

void MqttMessage::setDepartureTime(double departureTime)
{
    this->departureTime_var = departureTime;
}

int MqttMessage::getSerialNumber() const
{
    return serialNumber_var;
}

void MqttMessage::setSerialNumber(int serialNumber)
{
    this->serialNumber_var = serialNumber;
}

int MqttMessage::getSensorRetry() const
{
    return sensorRetry_var;
}

void MqttMessage::setSensorRetry(int sensorRetry)
{
    this->sensorRetry_var = sensorRetry;
}

int MqttMessage::getGatewayRetry() const
{
    return gatewayRetry_var;
}

void MqttMessage::setGatewayRetry(int gatewayRetry)
{
    this->gatewayRetry_var = gatewayRetry;
}

double MqttMessage::getRtoInit() const
{
    return rtoInit_var;
}

void MqttMessage::setRtoInit(double rtoInit)
{
    this->rtoInit_var = rtoInit;
}

double MqttMessage::getRtoCalc() const
{
    return rtoCalc_var;
}

void MqttMessage::setRtoCalc(double rtoCalc)
{
    this->rtoCalc_var = rtoCalc;
}

int MqttMessage::getClientId() const
{
    return clientId_var;
}

void MqttMessage::setClientId(int clientId)
{
    this->clientId_var = clientId;
}

double MqttMessage::getRtt() const
{
    return rtt_var;
}

void MqttMessage::setRtt(double rtt)
{
    this->rtt_var = rtt;
}

int MqttMessage::getRttS() const
{
    return rttS_var;
}

void MqttMessage::setRttS(int rttS)
{
    this->rttS_var = rttS;
}

int MqttMessage::getRttW() const
{
    return rttW_var;
}

void MqttMessage::setRttW(int rttW)
{
    this->rttW_var = rttW;
}

int MqttMessage::getRto() const
{
    return rto_var;
}

void MqttMessage::setRto(int rto)
{
    this->rto_var = rto;
}

class MqttMessageDescriptor : public cClassDescriptor
{
  public:
    MqttMessageDescriptor();
    virtual ~MqttMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(MqttMessageDescriptor);

MqttMessageDescriptor::MqttMessageDescriptor() : cClassDescriptor("MqttMessage", "cPacket")
{
}

MqttMessageDescriptor::~MqttMessageDescriptor()
{
}

bool MqttMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<MqttMessage *>(obj)!=NULL;
}

const char *MqttMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int MqttMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 17+basedesc->getFieldCount(object) : 17;
}

unsigned int MqttMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<17) ? fieldTypeFlags[field] : 0;
}

const char *MqttMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "srcAddress",
        "destAddress",
        "managerAddr",
        "gatewayAddr",
        "gatewayProcId",
        "brokerProcId",
        "departureTime",
        "serialNumber",
        "sensorRetry",
        "gatewayRetry",
        "rtoInit",
        "rtoCalc",
        "clientId",
        "rtt",
        "rttS",
        "rttW",
        "rto",
    };
    return (field>=0 && field<17) ? fieldNames[field] : NULL;
}

int MqttMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddress")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddress")==0) return base+1;
    if (fieldName[0]=='m' && strcmp(fieldName, "managerAddr")==0) return base+2;
    if (fieldName[0]=='g' && strcmp(fieldName, "gatewayAddr")==0) return base+3;
    if (fieldName[0]=='g' && strcmp(fieldName, "gatewayProcId")==0) return base+4;
    if (fieldName[0]=='b' && strcmp(fieldName, "brokerProcId")==0) return base+5;
    if (fieldName[0]=='d' && strcmp(fieldName, "departureTime")==0) return base+6;
    if (fieldName[0]=='s' && strcmp(fieldName, "serialNumber")==0) return base+7;
    if (fieldName[0]=='s' && strcmp(fieldName, "sensorRetry")==0) return base+8;
    if (fieldName[0]=='g' && strcmp(fieldName, "gatewayRetry")==0) return base+9;
    if (fieldName[0]=='r' && strcmp(fieldName, "rtoInit")==0) return base+10;
    if (fieldName[0]=='r' && strcmp(fieldName, "rtoCalc")==0) return base+11;
    if (fieldName[0]=='c' && strcmp(fieldName, "clientId")==0) return base+12;
    if (fieldName[0]=='r' && strcmp(fieldName, "rtt")==0) return base+13;
    if (fieldName[0]=='r' && strcmp(fieldName, "rttS")==0) return base+14;
    if (fieldName[0]=='r' && strcmp(fieldName, "rttW")==0) return base+15;
    if (fieldName[0]=='r' && strcmp(fieldName, "rto")==0) return base+16;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *MqttMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int",
        "int",
        "double",
        "int",
        "int",
        "int",
        "double",
        "double",
        "int",
        "double",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<17) ? fieldTypeStrings[field] : NULL;
}

const char *MqttMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int MqttMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    MqttMessage *pp = (MqttMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string MqttMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    MqttMessage *pp = (MqttMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSrcAddress());
        case 1: return long2string(pp->getDestAddress());
        case 2: return long2string(pp->getManagerAddr());
        case 3: return long2string(pp->getGatewayAddr());
        case 4: return long2string(pp->getGatewayProcId());
        case 5: return long2string(pp->getBrokerProcId());
        case 6: return double2string(pp->getDepartureTime());
        case 7: return long2string(pp->getSerialNumber());
        case 8: return long2string(pp->getSensorRetry());
        case 9: return long2string(pp->getGatewayRetry());
        case 10: return double2string(pp->getRtoInit());
        case 11: return double2string(pp->getRtoCalc());
        case 12: return long2string(pp->getClientId());
        case 13: return double2string(pp->getRtt());
        case 14: return long2string(pp->getRttS());
        case 15: return long2string(pp->getRttW());
        case 16: return long2string(pp->getRto());
        default: return "";
    }
}

bool MqttMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    MqttMessage *pp = (MqttMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcAddress(string2long(value)); return true;
        case 1: pp->setDestAddress(string2long(value)); return true;
        case 2: pp->setManagerAddr(string2long(value)); return true;
        case 3: pp->setGatewayAddr(string2long(value)); return true;
        case 4: pp->setGatewayProcId(string2long(value)); return true;
        case 5: pp->setBrokerProcId(string2long(value)); return true;
        case 6: pp->setDepartureTime(string2double(value)); return true;
        case 7: pp->setSerialNumber(string2long(value)); return true;
        case 8: pp->setSensorRetry(string2long(value)); return true;
        case 9: pp->setGatewayRetry(string2long(value)); return true;
        case 10: pp->setRtoInit(string2double(value)); return true;
        case 11: pp->setRtoCalc(string2double(value)); return true;
        case 12: pp->setClientId(string2long(value)); return true;
        case 13: pp->setRtt(string2double(value)); return true;
        case 14: pp->setRttS(string2long(value)); return true;
        case 15: pp->setRttW(string2long(value)); return true;
        case 16: pp->setRto(string2long(value)); return true;
        default: return false;
    }
}

const char *MqttMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *MqttMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    MqttMessage *pp = (MqttMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}



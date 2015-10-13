//
// Generated file, do not edit! Created by nedtool 4.6 from MqttMessage.msg.
//

#ifndef _MQTTMESSAGE_M_H_
#define _MQTTMESSAGE_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Enum generated from <tt>MqttMessage.msg:17</tt> by nedtool.
 * <pre>
 * enum MqttMessageType
 * {
 * 
 *     MQTT_SEARCHGW = 0;
 *     MQTT_GWINFO = 1;
 *     MQTT_CONNECT = 2;
 *     MQTT_CONNACK = 3;
 *     MQTT_PUBLISH = 4;
 *     MQTT_PUBACK = 5;
 *     MQTT_ADVERTISE = 6;
 *     MQTT_DISCONNECT = 7;
 * 
 *     SELF_MESSAGE_F = 20;
 *     SELF_MESSAGE_E = 21;
 *     SELF_TIMEOUT = 22;
 *     SELF_RECEACK = 23;
 *     SELF_TIMEOUT_S = 24;	//searchgw
 *     SELF_TIMEOUT_C = 25;	//connect
 *     SELF_TIMEOUT_A = 26;	//aging
 * }
 * </pre>
 */
enum MqttMessageType {
    MQTT_SEARCHGW = 0,
    MQTT_GWINFO = 1,
    MQTT_CONNECT = 2,
    MQTT_CONNACK = 3,
    MQTT_PUBLISH = 4,
    MQTT_PUBACK = 5,
    MQTT_ADVERTISE = 6,
    MQTT_DISCONNECT = 7,
    SELF_MESSAGE_F = 20,
    SELF_MESSAGE_E = 21,
    SELF_TIMEOUT = 22,
    SELF_RECEACK = 23,
    SELF_TIMEOUT_S = 24,
    SELF_TIMEOUT_C = 25,
    SELF_TIMEOUT_A = 26
};

/**
 * Class generated from <tt>MqttMessage.msg:36</tt> by nedtool.
 * <pre>
 * packet MqttMessage
 * {
 *     //    int someField;
 *     //    string anotherField;
 *     //    double arrayField1[];
 *     //    double arrayField2[10];
 *     int srcAddress;
 *     int destAddress;
 *     int gatewayProcId;
 *     int brokerProcId;
 * 
 *     double departureTime;
 *     int serialNumber;
 *     int sensorRetry;
 *     int gatewayRetry;
 *     double rtoInit;
 *     double rtoCalc;
 * 
 *     int clientId;
 *     double rtt;
 *     int rttS;
 *     int rttW;
 *     int rto;
 * }
 * 
 * //message SelfMessage
 * //{
 * //    unsigned int serialNumber;
 * //	unsigned int numberOfRetry;
 * //	double rtoInit;
 * //	double rtoCalc;
 * //	
 * //}
 * </pre>
 */
class MqttMessage : public ::cPacket
{
  protected:
    int srcAddress_var;
    int destAddress_var;
    int gatewayProcId_var;
    int brokerProcId_var;
    double departureTime_var;
    int serialNumber_var;
    int sensorRetry_var;
    int gatewayRetry_var;
    double rtoInit_var;
    double rtoCalc_var;
    int clientId_var;
    double rtt_var;
    int rttS_var;
    int rttW_var;
    int rto_var;

  private:
    void copy(const MqttMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const MqttMessage&);

  public:
    MqttMessage(const char *name=NULL, int kind=0);
    MqttMessage(const MqttMessage& other);
    virtual ~MqttMessage();
    MqttMessage& operator=(const MqttMessage& other);
    virtual MqttMessage *dup() const {return new MqttMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSrcAddress() const;
    virtual void setSrcAddress(int srcAddress);
    virtual int getDestAddress() const;
    virtual void setDestAddress(int destAddress);
    virtual int getGatewayProcId() const;
    virtual void setGatewayProcId(int gatewayProcId);
    virtual int getBrokerProcId() const;
    virtual void setBrokerProcId(int brokerProcId);
    virtual double getDepartureTime() const;
    virtual void setDepartureTime(double departureTime);
    virtual int getSerialNumber() const;
    virtual void setSerialNumber(int serialNumber);
    virtual int getSensorRetry() const;
    virtual void setSensorRetry(int sensorRetry);
    virtual int getGatewayRetry() const;
    virtual void setGatewayRetry(int gatewayRetry);
    virtual double getRtoInit() const;
    virtual void setRtoInit(double rtoInit);
    virtual double getRtoCalc() const;
    virtual void setRtoCalc(double rtoCalc);
    virtual int getClientId() const;
    virtual void setClientId(int clientId);
    virtual double getRtt() const;
    virtual void setRtt(double rtt);
    virtual int getRttS() const;
    virtual void setRttS(int rttS);
    virtual int getRttW() const;
    virtual void setRttW(int rttW);
    virtual int getRto() const;
    virtual void setRto(int rto);
};

inline void doPacking(cCommBuffer *b, MqttMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, MqttMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef _MQTTMESSAGE_M_H_

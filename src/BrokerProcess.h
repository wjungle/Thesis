//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __MQTT_BROKERPROCESS_H_
#define __MQTT_BROKERPROCESS_H_

#include <omnetpp.h>
#include "MqttMessage_m.h"

#define STACKSIZE 16384

/**
 * TODO - Generated class
 */
class BrokerProcess : public cSimpleModule
{
  private:
    /* how many the broker receive PUBLISH */
    unsigned int numPublish, numDuplicated;

    /* record the 3 number of Sn-SensorRe-GatewayRe */
    int fromMessageSn, fromSensorRe, fromGatewayRe;
    int currMessageSn;

    int src, dest;
    MqttMessage *puback, *connack;
    double tcpPLR;
    cGate *brokerOutGate;
    int gatewayProcId;
    int nonincreaseSn;

  public:
    BrokerProcess();
    virtual ~BrokerProcess();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
//    virtual bool needCancelEvent(unsigned int, unsigned int, unsigned int, unsigned int);
    virtual void finish();
};

#endif

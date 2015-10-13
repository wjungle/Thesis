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

#include "Manager.h"

Define_Module(Manager);

void Manager::initialize()
{
//    gatewayAddr = getVectorSize();
//    ev << "gatewayAddr= " << gatewayAddr << endl;

    // thesis
    RtoVector.setName("manager RTO");
    range = par("range");
    granularity =  range / (SIZE - 1);
    numPuback = 0;

    broadcast = par("broadcast");

    initQuantization();
}

void Manager::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    if (mqmsg->getKind() == MQTT_SEARCHGW)
    {
        gatewayAddr = mqmsg->getGatewayAddr();
        mqmsg->setManagerAddr(getIndex());
        send(mqmsg, "line$o", gatewayAddr);
    }
    else if (mqmsg->getKind() == MQTT_GWINFO)
    {
        int dest = mqmsg->getDestAddress();
        send(mqmsg, "line$o", dest);
    }
    else if (mqmsg->getKind() == MQTT_CONNECT)
    {
        mqmsg->setManagerAddr(getIndex());
        send(mqmsg, "line$o", gatewayAddr);
    }
    else if (mqmsg->getKind() == MQTT_CONNACK)
    {
        int dest = mqmsg->getDestAddress();
        send(mqmsg, "line$o", dest);
    }
    else if (mqmsg->getKind() == MQTT_PUBLISH)
    {
        mqmsg->setManagerAddr(getIndex());
        send(mqmsg, "line$o", gatewayAddr);
    }
    else if (mqmsg->getKind() == MQTT_PUBACK)
    {
        numPuback++;
        int dest;
        int rtoIdx = mqmsg->getRto();
        if (numPuback > 1)
        {
            EV << "manager-numPuback= " << numPuback  << " rtoIdx=" << rtoIdx << endl;
            EV << " rto= " << value[rtoIdx] << endl;
            RtoVector.record(value[rtoIdx]);
        }
        if (broadcast == 2)
        {
            dest = mqmsg->getDestAddress();
            send(mqmsg, "line$o", dest);
        }
        else if (broadcast == 1)
        {
            for(int i = 0; i < (gate("line$o", 0)->getVectorSize()-1); i++)
            {
                cMessage *copy = mqmsg->dup();
                send(copy, "line$o", i);
            }
            delete mqmsg;
        }
    }
}

void Manager::initQuantization()
{
    double temp = 0;
    for(int i = 0; i < SIZE; i++)
    {
        value[i] = temp;
        temp += granularity;
    }
}

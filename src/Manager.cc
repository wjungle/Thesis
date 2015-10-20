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

Manager::Manager()
{

}

Manager::~Manager()
{

}


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
    rtoIdxAll=-1;
    rtoAll=0;

    initQuantization();
}

void Manager::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    if (mqmsg->getKind() == MQTT_SEARCHGW)
    {
        send(mqmsg, "lineGw$o");
    }
    else if (mqmsg->getKind() == MQTT_GWINFO)
    {
        int dest = mqmsg->getDestAddress();
//        if (rtoIdxAll >= 0)
//            mqmsg->setRto(rtoIdxAll);
        send(mqmsg, "line$o", dest);
    }
    else if (mqmsg->getKind() == MQTT_CONNECT)
    {
        send(mqmsg, "lineGw$o");
    }
    else if (mqmsg->getKind() == MQTT_CONNACK)
    {
        int dest = mqmsg->getDestAddress();
        send(mqmsg, "line$o", dest);
    }
    else if (mqmsg->getKind() == MQTT_PUBLISH)
    {
        send(mqmsg, "lineGw$o");
    }
    else if (mqmsg->getKind() == MQTT_PUBACK)
    {
        numPuback++;
        int dest;
        int rtoIdx = mqmsg->getRto();
        if (numPuback > 1)
        {
            EV << "manager-numPuback= " << numPuback  << " rtoIdx=" << rtoIdx << " rto= " << value[rtoIdx] << endl;

            if (numPuback == 2)
            {
                rtoIdxAll = rtoIdx;
                rtoAll = value[rtoIdx];
            }
            else if (numPuback > 2)
            {
                rtoIdxAll = round(rtoIdxAll*0.5) + round(rtoIdx*0.5);
                rtoAll = 0.5*rtoAll + 0.5*value[rtoIdx];
            }
//            ev << " rtoAll= " << rtoAll << endl;
//            ev << "rtoIdxAll = " << rtoIdxAll << " value[rtoIdxAll]=" <<  value[rtoIdxAll] << endl;

            mqmsg->setRto(rtoIdxAll);
            RtoVector.record(value[rtoIdxAll]);
        }

        if (broadcast == 1)
        {
            for(int i = 0; i < (gate("line$o", 0)->getVectorSize()-1); i++)
            {
                cMessage *copy = mqmsg->dup();
                send(copy, "line$o", i);
            }
            delete mqmsg;
        }
        else
        {
            dest = mqmsg->getDestAddress();
            send(mqmsg, "line$o", dest);
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

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

#include "BrokerProcess.h"

Define_Module(BrokerProcess);

BrokerProcess::BrokerProcess()
{
    connack = puback = NULL;
}

BrokerProcess::~BrokerProcess()
{
    delete connack; delete puback;
}

void BrokerProcess::initialize()
{
    numPublish = 0; numDuplicated=0;
    brokerOutGate = getParentModule()->gate("out");
    fromMessageSn = 0; fromSensorRe = 0; fromGatewayRe = 0;
    currMessageSn = 0;
    nonincreaseSn = 0;
}

void BrokerProcess::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    if (mqmsg->getKind() == MQTT_CONNECT)
    {
        char msgname[20];
        sprintf(msgname, "CONNACK(%d)", mqmsg->getSrcAddress());
        EV << "send " <<msgname << endl;
        connack = new MqttMessage(msgname, MQTT_CONNACK);
        connack -> setSrcAddress(mqmsg -> getDestAddress());
        connack -> setDestAddress(mqmsg -> getSrcAddress());
        connack -> setBrokerProcId(getId());
        connack -> setGatewayProcId(mqmsg -> getGatewayProcId());
        sendDirect(connack, brokerOutGate);
        connack = NULL;
        delete mqmsg;
    }
    else if (mqmsg->getKind() == MQTT_PUBLISH)
    {
        if (nonincreaseSn < mqmsg->getSerialNumber())
        {
            numPublish++;
            nonincreaseSn = mqmsg->getSerialNumber();
        }
        else if (nonincreaseSn == mqmsg->getSerialNumber())
        {
            numDuplicated++;
            nonincreaseSn = mqmsg->getSerialNumber();
        }
        else    //don't care nonincreaseSn > mqmsg->getSerialNumber()
        {}


        EV << mqmsg->getName() <<" received. \n";
        //EV << "client Id: " << mqmsg -> getClientId() << endl;

        src = mqmsg -> getSrcAddress();
        dest = mqmsg -> getDestAddress();
        fromMessageSn = mqmsg->getSerialNumber();
        fromSensorRe = mqmsg->getSensorRetry();
        fromGatewayRe = mqmsg->getGatewayRetry();
        gatewayProcId = mqmsg->getGatewayProcId();

        //EV << mqmsg->getSendingTime()<< " is the PUBLISH sending time. \n";
        char msgname[20];
        sprintf(msgname, "PUBACK(%d) #%d-%d-%d ", mqmsg->getClientId(), fromMessageSn, fromSensorRe, fromGatewayRe);
        puback = new MqttMessage(msgname, MQTT_PUBACK);
        puback->setDestAddress(src);
        puback->setSrcAddress(dest);
        puback->setSerialNumber(fromMessageSn);
        puback->setSensorRetry(fromSensorRe);
        puback->setGatewayRetry(fromGatewayRe);
        puback->setGatewayProcId(gatewayProcId);
        sendDirect(puback, brokerOutGate);
        EV << "sending Time: " << puback->getSendingTime();
        EV << ";arrival Time: " << puback->getArrivalTime();
        puback = NULL;
        delete mqmsg;
    }
}

void BrokerProcess::finish()
{
    EV << "src " << src << endl;
    EV << "number of receive PUBLISH: " << numPublish << endl;
    EV << "number of Duplicated PUBLISH: " << numDuplicated << endl;
    EV << "Duplicated Publications Ratio " << (double)numDuplicated/numPublish << endl;
    recordScalar("src ", src);
    recordScalar("number of receive PUBLISH ", numPublish);
    recordScalar("number of Duplicated PUBLISH ", numDuplicated);
    recordScalar("Duplicated Publications Ratio ", (double)numDuplicated/numPublish);
}

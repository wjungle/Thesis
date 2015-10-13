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

#include "Broker.h"

Define_Module(Broker);

void Broker::initialize()
{
    brkProcType = cModuleType::find("mqtt.BrokerProcess");
    if (brkProcType == NULL)
        EV << "module not found!\n";
}

void Broker::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    if (mqmsg->getKind() == MQTT_CONNECT)
    {
        cModule *mod = brkProcType->createScheduleInit("brokerproc", this);
        EV << "MQTT_CONNECT: Created broker process ID=" << mod->getId() << endl;
        sendDirect(mqmsg, mod, "in");
    }
    else
    {
        int brokerProcId = mqmsg->getBrokerProcId();
        //EV << "Redirecting msg to process ID=" << brokerProcId << endl;
        cModule *mod = simulation.getModule(brokerProcId);
        if (!mod) {
            EV << " That process already exited, deleting msg\n";
            delete mqmsg;
        } else {
            sendDirect(mqmsg, mod, "in");
        }
    }
}

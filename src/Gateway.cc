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

#include "Gateway.h"

Define_Module(Gateway);

Gateway::Gateway()
{

}

Gateway::~Gateway()
{
//    delete gwinfo;
}

void Gateway::initialize()
{
    gwProcType = cModuleType::find("mqtt.GatewayProcess");
    if (gwProcType == NULL)
        EV << "module not found!\n";
}

void Gateway::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

//    for (cSubModIterator iter(*getParentModule()); !iter.end(); iter++)
//    {
//        ev << iter()->getFullName() << endl;
//        if (iter()->isName(getName())) // if iter() is in the same
//        // vector as this module
//        {
//        int itsIndex = iter()->getIndex();
//        // do something to it
//        ev << itsIndex << " ";
//        }
//    }

    if (mqmsg->getKind() == MQTT_SEARCHGW)
    {
        EV << mqmsg->getName() << " received, sending back an GWINFO message. \n";
        src = mqmsg->getSrcAddress();
        int mAddr = mqmsg->getManagerAddr();
        //ev << "mProcId= " << mProcId << endl;
        gwinfo = new MqttMessage("GWINFO", MQTT_GWINFO);
        gwinfo->setDestAddress(src);
        send(gwinfo, "line$o", mAddr);
        delete mqmsg;
    }

    else if (mqmsg->getKind() == MQTT_CONNECT)
    {
        cModule *mod = gwProcType->createScheduleInit("gatewayproc", this);
        EV << "MQTT_CONNECT: Created gateway process ID=" << mod->getId() << endl;

        sendDirect(mqmsg, mod, "in");
    }
    else
    {
        //int mAddr = mqmsg->getManagerAddr();
        int gatewayProcId = mqmsg->getGatewayProcId();
        //EV << "Redirecting msg to process ID=" << gatewayProcId  << endl;
        cModule *mod = simulation.getModule(gatewayProcId);
//        cModule *gwmod = simulation.getModule(getId());
//        EV << "gwmod size= " << gwmod->size() << endl;
        if (!mod) {
            EV << " That process already exited, deleting msg\n";
            delete mqmsg;
        } else {
            sendDirect(mqmsg, mod, "in");
        }
    }
}


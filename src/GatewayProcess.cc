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

#include "GatewayProcess.h"

Define_Module(GatewayProcess);

GatewayProcess::GatewayProcess()
{
    gwinfo = advertise = NULL;
}

GatewayProcess::~GatewayProcess()
{
    cancelAndDelete(timeoutEvent);
    delete publish; //delete puback;
    //delete gwinfo; delete advertise;
}

void GatewayProcess::initialize()
{
    rtoInit = 0; rtoCalc = 0;
    rtoFixed = getParentModule()->par("timeout");
    maxRetry = getParentModule()->par("limit");
    range = getParentModule()->par("range");
    size = getParentModule()->par("quanSize");
    messageTimestamp = 0;
    receackTimestamp = 0;

    timeoutEvent = new MqttMessage("timeout", SELF_TIMEOUT);
    msg = new MqttMessage("ppp", MQTT_PUBLISH);

    sn = 0; src = 0; dest = 0; clientId = 0;

    currMessageSn = -1; currMessageSs = -1;
    currTimeoutSn = -1; currTimeoutSs = -1; currTimeoutGw = -1;
    currReceackSn = -1; currReceackSs = -1; currReceackGw = -1;

    publishMessageSn=0; publishMessageRe=0;
    pubackMessageSn=0; pubackMessageSs=0, pubackMessageGw=0;

    totalRetry = 0; totalRetry2 = 0;
    totalRtt=0; strongRtt=0; weakRtt=0;
    failed=0; retransmission=0;
    successfulSn=0; successfulSs=0;
    srtt=0; rttvar=0; rtoGw=-1; rtoAll = -1;
    Rtt_s=0; Rtt_w=0;

    // thesis
    granularity =  range / (SIZE - 1);
    rttSensorS=-1; rttSensorW=-1;
    rtoSensorS=1; rtoSensorW=1; rtoSensor=1;
    srttSensorS=0; rttvarSensorS=0; srttSensorW=0; rttvarSensorW=0;
    numStrong=0; numWeak=0;
    idxRto = 0; idxS=0; idxW=0;
    numAging = 0;

    numRecPub=0; numPublish=0; numPuback=0; numFailed=0;
    trafficLight = GREEN;

    RttVector.setName("gateway RTT");
    RttwVector.setName("gateway weak RTT");
    RttsVector.setName("gateway strong RTT");
    RtoVector.setName("gateway RTO");
    RtoSensorVector.setName("sensor RTO");

    initQuantization();
}

void GatewayProcess::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    if ((mqmsg->getKind() == MQTT_CONNECT) /*|| (mqmsg->getKind() == MQTT_CONNACK)*/)
    {
        // forward
        //dest = mqmsg->getDestAddress();
        gatewayDestGate = getParentModule()->gate("line$o", 1);
        EV << "Relaying msg to addr=" << 1 << "\n";

        mqmsg -> setGatewayProcId(getId());
        sendDirect(mqmsg, gatewayDestGate);
        gatewayDestGate = NULL;
    }

    else if (mqmsg->getKind() == MQTT_CONNACK)
    {
        //int manager = mqmsg->getManagerAddr();
        cGate *managerDestGate = getParentModule()->gate("line$o", 0);
        //EV << "Relaying back msg to addr=" << manager << "\n";
        //EV << "GatewayProcId=" << mqmsg->getGatewayProcId() << "\n";
//        mqmsg -> setGatewayProcId(getId());
        sendDirect(mqmsg, managerDestGate);
        managerDestGate = NULL;
    }

    /***************************************/
    /************  from Sensor  ************/
    /***************************************/
    else if (mqmsg->getKind() == MQTT_PUBLISH)
    {
        // thesis
#if 1
        if (publishMessageSn < mqmsg -> getSerialNumber())
        {
            if ((idxS != mqmsg->getRttS()) || (idxW != mqmsg->getRttW()))
            {
                idxS = mqmsg->getRttS();
                idxW = mqmsg->getRttW();
                rttSensorS = value[idxS];
                rttSensorW = value[idxW];
                ev << "rttSensorS= " << rttSensorS << " rttSensorW= " << rttSensorW << endl;
#if 1
                if (rttSensorS > 0)
                {
                    numStrong++;
                    rtoSensorS = rfc6298(&srttSensorS, &rttvarSensorS, rttSensorS, numStrong, 4);
                    ev << "rtoSensorS= " << rtoSensorS ;
                    rtoSensor = 0.5 * rtoSensor + 0.5 * rtoSensorS;
                }
                else if (rttSensorW > 0)
                {
                    numWeak++;
                    rtoSensorW = rfc6298(&srttSensorW, &rttvarSensorW, rttSensorW, numWeak, 1);
                    ev << "rtoSensorW= " << rtoSensorW ;
                    rtoSensor = 0.75 * rtoSensor + 0.25 * rtoSensorW;
                }
#else           // only strong
                if (rttSensorS > 0)
                {
                    numStrong++;
                    rtoSensorS = rfc6298(&srttSensorS, &rttvarSensorS, rttSensorS, numStrong, 4);
                    ev << "rtoSensorS= " << rtoSensorS ;
                    rtoSensor = rtoSensorS;
                }
#endif
                ev << " ;rtoSensor = " << rtoSensor << endl;
//                RtoSensorVector.record(rtoSensor);
            }
            else
                numAging++;
        }
#endif

        publishMessageSn = mqmsg -> getSerialNumber();
        publishMessageRe = mqmsg -> getSensorRetry();
        src = mqmsg -> getSrcAddress();
        dest = mqmsg -> getDestAddress();
        clientId = mqmsg -> getClientId();

        numRecPub++;

        // Record gateway current processing PUBLISH number, include the SerialNumber and SensorRetry
        if (currMessageSn < mqmsg->getSerialNumber())
        {
            currMessageSn = mqmsg->getSerialNumber();
            currMessageSs = mqmsg->getSensorRetry();
            EV << "================================NEW SENSOR PUBLISH=======================================" << endl;
            trafficLight = GREEN;
        }
        else if (currMessageSn == mqmsg->getSerialNumber())
        {
            if (currMessageSs <= mqmsg->getSensorRetry())
            {
                currMessageSn = mqmsg->getSerialNumber();
                currMessageSs = mqmsg->getSensorRetry();
                EV << "================================NEW SENSOR PUBLISH=======================================" << endl;
                trafficLight = GREEN;
            }
            else
                trafficLight = RED;
        }
        else
        {
            trafficLight = RED;
        }

        //EV << "trafficLight= " << trafficLight << endl;
        bool sendToBroker = 0;

        if (trafficLight == GREEN)
        {
            // all new PUBLISH from sensor
            if (currMessageSs == 0)//publishMessageRe
            {
                sendToBroker = 1;
                trafficLight = GREEN;
                if (timeoutEvent)               //if (publishMessageSn > 1)
                {
                    cancelEvent(timeoutEvent);
                }
            }
            else    // look up the corresponding PUBACK if receive.
            {
                if (trafficLight == GREEN)  // corresponding PUBACK has received.
                {
                    cancelEvent(timeoutEvent);
                    sendToBroker = 1;
                }
            }
        }
        else if (trafficLight == RED)
            sendToBroker = 0;

        if (sendToBroker)
        {
            // Publish message.
            /* brokerProcId for receiving PUBACK, gatewayProcId for sending PUBLISH*/
            publish = generateNewMessage(publishMessageSn, publishMessageRe, src, dest, clientId,
                                         mqmsg -> getBrokerProcId(), mqmsg -> getGatewayProcId());
            sendCopyOf(publish);

            //TIMEOUT event, parameters is same as PUBLISH
            char msgname[20];
            sprintf(msgname, "timeout(%d) #%d-%d-%d ", mqmsg->getClientId(), publishMessageSn, publishMessageRe, 0);
            timeoutEvent->setName(msgname);
            timeoutEvent->setSerialNumber(publishMessageSn);
            timeoutEvent->setSensorRetry(publishMessageRe);
            timeoutEvent->setGatewayRetry(0);

            double rto6298 = rtoInit6298(rtoGw);
            if((getParentModule()->par("method")).longValue() == 0)
                rtoInit = rtoFixed;
            else if((getParentModule()->par("method")).longValue() == 1)
            {
                rtoInit = rto6298;
                timeoutEvent->setRtoInit(rtoInit);
                timeoutEvent->setRtoCalc(rtoInit);
            }
            scheduleAt(simTime()+rtoInit, timeoutEvent);
            EV << "; timeout: " << simTime()+rtoInit  << " rto init: " << rtoInit <<endl;
        }
        delete mqmsg;
    }

    /***************************************/
    /************     Timer     ************/
    /***************************************/
    else if (mqmsg->getKind() == SELF_TIMEOUT)
    {
        bubble("Timeout!");

        if (currTimeoutSn < mqmsg->getSerialNumber())
        {
            currTimeoutSn = mqmsg->getSerialNumber();
            currTimeoutSs = mqmsg->getSensorRetry();
            currTimeoutGw = mqmsg->getGatewayRetry();
        }
        else if (currTimeoutSn == mqmsg->getSerialNumber())
        {
            if (currTimeoutSs < mqmsg->getSensorRetry())
            {
                currTimeoutSs = mqmsg->getSensorRetry();
                currTimeoutGw = mqmsg->getGatewayRetry();
            }
            else if (currTimeoutSs == mqmsg->getSensorRetry())
            {
                if (currTimeoutGw < mqmsg->getGatewayRetry())
                {
                    currTimeoutGw = mqmsg->getGatewayRetry();
                }
            }
        }

        if (mqmsg->getGatewayRetry() < (int)maxRetry)
        {
            currTimeoutGw = mqmsg->getGatewayRetry();
            EV << "          Timeout of PUBLISH expired, re-sending message and restarting timer ";

            // Publish message.
            totalRetry2++;
            char msgname[20];
            sprintf(msgname, "PUBLISH(%d) #%d-%d-%d ", mqmsg->getClientId(), mqmsg->getSerialNumber(), mqmsg->getSensorRetry(), mqmsg->getGatewayRetry()+1);
            publish->setSerialNumber(mqmsg->getSerialNumber());
            publish->setSensorRetry(mqmsg->getSensorRetry());
            publish->setGatewayRetry(mqmsg->getGatewayRetry()+1);
            publish->setName(msgname);
            sendCopyOf(publish);


            // TIMEOUT event
            cancelEvent(timeoutEvent);
            char msgname2[20];
            sprintf(msgname2, "timeout(%d) #%d-%d-%d ", mqmsg->getClientId(), mqmsg->getSerialNumber(), mqmsg->getSensorRetry(), mqmsg->getGatewayRetry()+1);
            timeoutEvent->setName(msgname2);
            timeoutEvent->setSerialNumber(mqmsg->getSerialNumber());
            timeoutEvent->setSensorRetry(mqmsg->getSensorRetry());
            timeoutEvent->setGatewayRetry(mqmsg->getGatewayRetry()+1);

            if((getParentModule()->par("method")).longValue() == 0)
                rtoCalc = rtoFixed;
            else if((getParentModule()->par("method")).longValue() == 1)
            {
                rtoCalc = mqmsg->getRtoCalc() * 2;
                rtoCalc = adjustRange(rtoCalc);
                timeoutEvent->setRtoCalc(rtoCalc);
            }
            scheduleAt(simTime()+rtoCalc, timeoutEvent);
            EV << "; timeout: " << simTime()+rtoCalc << " rto calc: " << rtoCalc <<endl;
        }
        else
        {
            EV << currTimeoutSn << " failed\n";
            totalRetry += maxRetry;
            //currMessageSn = 0;
            failed = mqmsg->getSensorRetry();  //failed = mqmsg->getSerialNumber();
            numFailed++;
            //delete publish;
        }
    }

    /***************************************/
    /************  from Broker  ************/
    /***************************************/
    // RECEACK
    else if (mqmsg->getKind() == MQTT_PUBACK)
    {
        bool sendBackSensor = 0;

        if (currReceackSn < mqmsg->getSerialNumber())
        {
            currReceackSn = mqmsg->getSerialNumber();
            currReceackSs = mqmsg->getSensorRetry();
            currReceackGw = mqmsg->getGatewayRetry();
            sendBackSensor = 1;
        }
        else if (currReceackSn == mqmsg->getSerialNumber())
        {
            if (currReceackSs < mqmsg->getSensorRetry())
            {
                currReceackSs = mqmsg->getSensorRetry();
                currReceackGw = mqmsg->getGatewayRetry();
                sendBackSensor = 1;
            }
            else if (currReceackSs == mqmsg->getSensorRetry())
            {
                if (currReceackGw < mqmsg->getGatewayRetry())
                {
                    currReceackGw = mqmsg->getGatewayRetry();
                    sendBackSensor = 1;
                }
            }
        }
        pubackMessageSn = mqmsg->getSerialNumber();
        pubackMessageSs = mqmsg->getSensorRetry();
        pubackMessageGw = mqmsg->getGatewayRetry();
        EV << "          pubackMessageSn= " << pubackMessageSn << "; pubackMessageSs= " << pubackMessageSs << " ;pubackMessageGw= " << pubackMessageGw << endl;
        EV << "-----------------------------------------   PUBACK   -----------------------------------------" << endl;
        EV << "          currMessageSn= " << currMessageSn << "; currMessageSs= " << currMessageSs << endl;
        EV << "          currTimeoutSn= " << currTimeoutSn << "; currTimeoutSs= " << currTimeoutSs << " ;currTimeoutGw= " << currTimeoutGw << endl;
        EV << "          currReceackSn= " << currReceackSn << "; currReceackSs= " << currReceackSs << " ;currReceackGw= " << currReceackGw <<endl;

        if (sendBackSensor == 1)
        if (isPubackValid(currMessageSn, currMessageSs, currTimeoutSn, currTimeoutSs, currTimeoutGw, currReceackSn, currReceackSs, currReceackGw))
        {
//            if (!isSuccessful(currReceackSn, currReceackSs))
//            {
                // PUBACK message arrived.
                numPuback++;
                successfulSn = currReceackSn;
                successfulSs = currReceackSs;
                //currMessageSn = 0;
                trafficLight = RED;
                EV << "          " << currReceackSn << " " << mqmsg->getName() << " arrived time: " << mqmsg->getArrivalTime() << endl;

                // EV << "Timer cancelled.\n";
                cancelEvent(timeoutEvent);
                //delete publish;     //don't know why

                totalRetry += mqmsg->getGatewayRetry();
                receackTimestamp = mqmsg->getArrivalTime().dbl();
                double Rtt = receackTimestamp - messageTimestamp;

//                double Rtt_s = 0, Rtt_w = 0;
                // Karn's algorithm, strong estimator
                if(mqmsg->getGatewayRetry() == 0)
                {
                    Rtt_s = receackTimestamp - messageTimestamp;
                    rtoGw = rfc6298(&srtt, &rttvar, Rtt_s, numPuback, 4);
                    if (rtoGw > 60)
                        rtoGw = 60;
                }
                else
                {
                    Rtt_w = receackTimestamp - messageTimestamp;
                }

                // thesis
                if(numRecPub > 1)
                {
                    ev << "numRecPub=" << numRecPub <<" numPuback=" << numPuback<< endl;
                    rtoAll = rtoSensor/* + rtoGw*/;
                    idxRto = quantization(rtoAll);
                    ev << "    >>>>>> " << " rtoSensor= " << rtoSensor <<  endl;
                    ev << "    idxRto= " << idxRto << endl;
                }

                totalRtt += Rtt;
                strongRtt += Rtt_s;
                weakRtt += Rtt_w;

//                RttVector.record(Rtt);
//                RtoVector.record(rtoGw);
//                RttsVector.record(Rtt_s);
//                RttwVector.record(Rtt_w);

                char msgname0[20];
                sprintf(msgname0, "(%d) #%d ", clientId, mqmsg->getSerialNumber());//getIndex()
                EV << "          " << msgname0 << "calcRTT= " << Rtt << endl;
                EV << "          " << "RTT_s= " << Rtt_s << " Rtt_w= " << Rtt_w << endl;
                EV << "          " << "rtoGw= " << rtoGw << endl;

                // puback
                char msgname[20];
                sprintf(msgname, "PUBACK(%d) #%d-%d ", clientId, publishMessageSn, publishMessageRe);
                puback = new MqttMessage(msgname, MQTT_PUBACK);
                puback->setSerialNumber(publishMessageSn);
                puback->setSensorRetry(publishMessageRe);
                puback->setSrcAddress(mqmsg->getSrcAddress());
                puback->setDestAddress(mqmsg->getDestAddress());
                puback->setRto(idxRto);         // thesis
                //puback->setGatewayRetry(puback->getGatewayRetry());
                gatewaySrcGate = getParentModule()->gate("line$o", 0);

                sendDirect(puback, gatewaySrcGate);
                trafficLight = GREEN;
                puback = NULL;
//            }
        }
        delete mqmsg;
    }
}


MqttMessage *GatewayProcess::generateNewMessage(unsigned int sn, unsigned int sensor,int src, int dest, int cId, int bkProcId, int gwProcId)
{
    char msgname[20];
    sprintf(msgname, "PUBLISH(%d) #%d-%d-%d ", cId, sn, sensor, 0);//publishMessageSn, publishMessageRe
    EV << msgname;
//    MqttMessage *msg = new MqttMessage(msgname, MQTT_PUBLISH);
    msg -> setName(msgname);
    msg -> setSerialNumber(sn);
    msg -> setSensorRetry(sensor);
    msg -> setGatewayRetry(0);
    msg -> setSrcAddress(src);
    msg -> setDestAddress(dest);
    msg -> setBrokerProcId(bkProcId);
    msg -> setGatewayProcId(gwProcId);
    msg -> setClientId(cId);
    return msg;
}

void GatewayProcess::sendCopyOf(MqttMessage *msg)
{
    MqttMessage *copy = (MqttMessage *) msg->dup();
    gatewayDestGate = getParentModule()->gate("line$o", 1);
    sendDirect(copy, gatewayDestGate);
    if(msg->getGatewayRetry() == 0)
    {
        numPublish++;
        messageTimestamp = copy->getSendingTime().dbl();
    }
    EV << "           sending Time: " << copy->getSendingTime();
    EV << ";arrival Time: " << copy->getArrivalTime();
}

double GatewayProcess::rtoInit6298(double rto)
{
    //If the RTT is not sampled yet
    if (rto < 0)
        return 1;
    else
        return rto;
}

double GatewayProcess::adjustRange(double rto)
{
    if (rto < 3)
        return 3;
    if (rto > 60)
        return 60;
    return rto;
}

bool GatewayProcess::isSensorPhase(unsigned int lishSn, unsigned int lishRe, unsigned int ackSn, unsigned int ackRe)
{
    if (lishSn == ackSn)
    {
        if(lishRe == ackRe)
        {
            return true;
        }
    }
    return false;
}

bool GatewayProcess::isPubackValid(int cMesSn, int cMesSs, int cTimSn, int cTimSs, int cTimGw, int cRecSn, int cRecSs, int cRecGw)
{
    // currentMessageSn is bigger than currReceackSn, denotes this PUBACK is expired.

    if (cRecSn > cMesSn)                    // 1
    {
        return true;
    }
    else if (cRecSn == cMesSn)
    {
        if (cRecSs > cMesSs)                // 2
        {
            return true;
        }
        else if (cRecSs == cMesSs)
        {
            if (cRecSn > cTimSn)            // 3
            {
                return true;
            }
            else if (cRecSn == cTimSn)
            {
                if (cRecSs > cTimSs)        // 4
                {
                    return true;
                }
                else if (cRecSs == cTimSs)
                {
                    if (cRecGw > cTimGw)    // 5
                        return true;
                }
            }
        }
    }

    return false;
}

bool GatewayProcess::isSuccessful(int cRecSn, int cRecSs)
{
    if (cRecSn == successfulSn)
    {
//        if (cRecSs == successfulSs)
//        {
            char msgname[20];
            sprintf(msgname, "%d-%d is successful\n", successfulSn, successfulSs);
            ev << msgname;
            return true;
//        }
    }
    return false;
}

double GatewayProcess::rfc6298(double *srtt, double *rttvar, double rtt, unsigned n, unsigned factorK)
{
    double alpha = 0.125, beta = 0.25;

    if (n == 1)
    {
        *srtt = rtt;
        *rttvar = rtt/2;
    }
    else
    {
        *rttvar = ((1 - beta)* *rttvar) + (beta * fabs(*srtt - rtt));
        *srtt = ((1 - alpha) * *srtt) + (alpha *rtt);
    }

    double tmp = *srtt + factorK * *rttvar;
//    ev << "tmp= " << tmp << " n=" << n << endl;
//    if (tmp < 1)
//        return 1;
//    else
        return tmp;
}

void GatewayProcess::initQuantization()
{
    double temp = 0;
    for(int i = 0; i < SIZE; i++)
    {
        value[i] = temp;
        temp += granularity;
    }
}

int GatewayProcess::quantization(double rtt)
{
    if( rtt > range)
    {
        return (SIZE-1);
    }
    else
    {
        for(int j=0; j<SIZE; j++)
        {
            if( (fabs(rtt - value[j])) > granularity)
                continue;
            else
            {
                double a = value[j];
                double b = value[j+1];
                //ev << "a= " << a << " b= " << b << endl;
                //ev << "diff a= " << fabs(a-rtt) << " diff b= " << fabs(b-rtt) << endl;
                return ((fabs(a-rtt) > fabs(b-rtt)) ? j+1 : j);
            }
        }
        return 0;
    }
}

void GatewayProcess::updateDisplay()
{
    char buf[60];
    sprintf(buf, "rtoGw: %lf", rtoGw);
    getDisplayString().setTagArg("t",0,buf);
}

void GatewayProcess::finish()
{
    EV << "numRecPub:   " << numRecPub << endl;
    EV << "numPublish:   " << numPublish << endl;
    EV << "?numFailed:   " << numFailed << endl;
    EV << "?numPuback:   " << numPuback << endl;
    EV << "totalRetry:   " << totalRetry2 << endl;
    EV << "mean RTT:   " << totalRtt/numPuback << endl;
    EV << "mean Retry: " << (double)totalRetry2/numPublish << endl;

    recordScalar("#RTO gw", rtoGw);
    //recordScalar("#RTO", rto);
    //recordScalar("#RTO", rto);
}

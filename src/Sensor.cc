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

#include "Sensor.h"

Define_Module(Sensor);

Sensor::Sensor()
{
    timeoutEvent = NULL;
    searchgw = connect = publish = NULL;
    advertise = NULL;
}

Sensor::~Sensor()
{
    cancelAndDelete(timeoutConnect);
    cancelAndDelete(timeoutAging);
    cancelAndDelete(timeoutSearch);
    cancelAndDelete(timeoutEvent);
    cancelAndDelete(publishEventFixed);
    cancelAndDelete(publishEventExpon);
    //delete searchgw;delete connect;
    delete publish;
    // delete gwinfo; delete advertise;
}

void Sensor::initialize()
{
    rtoFixed = par("timeout");
    maxRetry = par("limit");
    rtoInitMethod = par("rtoInitMethod");
    rtoMultMethod = par("rtoMultMethod");
    rtoCalcMethod = par("rtoCalcMethod");
    size = par("quanSize");
    range = par("range");
    overhearing = par("overhearing");
    messageTimestamp = 0; receackTimestamp = 0;

    msg = new MqttMessage("p", MQTT_PUBLISH);
    timeoutEvent = new MqttMessage("timeout", SELF_TIMEOUT);
    publishEventFixed = new MqttMessage("publishEventFixed", SELF_MESSAGE_F);
    publishEventExpon = new MqttMessage("publishEventExpon", SELF_MESSAGE_E);
    timeoutSearch = new MqttMessage("SearchGw timeout", SELF_TIMEOUT_S);
    timeoutConnect = new MqttMessage("Connect timeout", SELF_TIMEOUT_C);
    timeoutAging = new MqttMessage("Aging timeout", SELF_TIMEOUT_A);

    seq = 0; busy = 0; failed = 0; successful = 0; retransmission = 0;
    ownAddr = gate("out")->getNextGate()->getIndex();
    serverAddr =gate("out")->getNextGate()->size()-1;
    gatewayAddr = gate("out")->getNextGate()->size()-1;
    brokerprocId = 0; gatewayprocId = 0;

    currMessageSn = -1; currMessageSs = -1;
    currTimeoutSn = -1; currTimeoutRe = -1;
    currReceackSn = -1; currReceackRe = -1;
    currNumberOfRetry = 0; currPubackSn = 0;

    totalRetry = 0; totalRetry2=0;
    Rtt = 0; Rtt_s = 0; Rtt_w = 0;
    Rto = 1; RtoS= 1; RtoW= 1; RtoA=0;
    srtt=0; rttvar=0; srttS=0; rttvarS=0; srttW=0; rttvarW=0;
    totalRtt=0; strongRtt=0; weakRtt=0;

    // thesis
    rtoGateway = -1; //RTO calculated from Gateway
    idxS=0; idxW=0;

    numMessage = 0; numPublish = 0; numPuback = 0;
    numFailed = 0; numThrow = 0; numPreemptived=0;
    numStrong = 0; numWeak = 0;

    // Generate and send SEARCHGW message.
    EV << "Sending SEARCHGW message 2\n";
    searchgw = new MqttMessage("MQTT_SEARCHGW", MQTT_SEARCHGW);
    searchgw->setSrcAddress(ownAddr);
    searchgw->setGatewayAddr(gatewayAddr);

    MqttMessage *copy = (MqttMessage *) searchgw->dup();
    send(copy, "out");
    //send( searchgw, "out");
    //searchgw = NULL;

    // TIMEOUT event
    scheduleAt(simTime()+ rtoFixed, timeoutSearch);    //TIMEOUT event
    //EV << "; timeout: " << simTime()+ rtoFixed << " rto Fixed: " << rtoFixed <<endl;

    initQuantization();

    //    WATCH(totalRetry);
    WATCH(numMessage); WATCH(numPublish); WATCH(numPuback);
    WATCH(numFailed); WATCH(numThrow);

    RttVector.setName("sensor RTT");
    RttwVector.setName("sensor weak RTT");
    RttsVector.setName("sensor strong RTT");
    RtoVector.setName("sensor RTO");

    numYes=0; numNo=0;


}

void Sensor::handleMessage(cMessage *msg)
{
    MqttMessage *mqmsg = check_and_cast<MqttMessage *>(msg);

    // GWINFO -> CONNECT
    if (mqmsg->getKind() == MQTT_GWINFO)
    {
        if (timeoutSearch->isScheduled())
            cancelEvent(timeoutSearch);
        // GWINFO message arrived.
        EV << mqmsg->getName() << " arrived; send ";

        // Generate the CONNECT message.
        char msgname[20];
        sprintf(msgname, "CONNECT(%d)", getIndex());
        EV << msgname << endl;
        connect = new MqttMessage (msgname, MQTT_CONNECT);
        connect -> setSrcAddress(ownAddr);
        connect -> setDestAddress(serverAddr);
        //
        MqttMessage *copy = (MqttMessage *) connect->dup();
        send(copy, "out");
        //send(connect, "out");
        // TIMEOUT event
        scheduleAt(simTime()+ rtoFixed, timeoutConnect);    //TIMEOUT event
        //EV << "; timeout: " << simTime()+ rtoFixed << " rto Fixed: " << rtoFixed <<endl;
        //
        delete mqmsg;
        delete searchgw;
    }

    else if (mqmsg->getKind() == SELF_TIMEOUT_S)
    {
        std::cerr << "miss gwinfo" << endl;
        send(searchgw->dup(), "out");
        scheduleAt(simTime()+ rtoFixed, timeoutSearch);    //TIMEOUT event
    }

    else if (mqmsg->getKind() == SELF_TIMEOUT_C)
    {
        std::cerr << "miss connack" << endl;
        send(connect->dup(), "out");
        scheduleAt(simTime()+ rtoFixed, timeoutConnect);    //TIMEOUT event
    }

    // aging
    else if (mqmsg->getKind() == SELF_TIMEOUT_A)
    {
        if (Rto == RtoA)
            Rto = (2 + Rto) / 2;
    }

    // CONNACK -> publishEvent
    else if (mqmsg->getKind() == MQTT_CONNACK)
    {
        if (timeoutConnect->isScheduled())
            cancelEvent(timeoutConnect);
        // CONNACK message arrived.
        EV << mqmsg->getName() << " arrived ";
        brokerprocId = mqmsg->getBrokerProcId();
        gatewayprocId = mqmsg->getGatewayProcId();

        // Generate the first publishEvent to start PUBLISH.
        EV << "Sending the first publishEvent message\n";
        scheduleAt(simTime()+0.0, publishEventFixed);
        scheduleAt(simTime()+0.0, publishEventExpon);
        delete mqmsg;
        delete connect;
    }

    // PUBLISH (MESSAGE)
    else if(mqmsg->getKind() == SELF_MESSAGE_E)  // generate interval of message generation
    {
        // Generate publish event.
        numMessage++;
        simtime_t publishIaTimeExpon = par("pubIaTimeExpon");
        scheduleAt(simTime()+publishIaTimeExpon, publishEventExpon);

        handlePublish(busy, retransmission, par("discipline").longValue());
    }

#if 1
    else if(mqmsg->getKind() == SELF_MESSAGE_F)  // generate interval of message generation
    {
        // Generate publish event.
        numMessage++;
        simtime_t publishIaTimeFixed = par("pubIaTimeFixed");
        scheduleAt(simTime()+publishIaTimeFixed, publishEventFixed);

        handlePublish(busy,  retransmission, par("discipline").longValue());

//        if(!busy)
//        {
//            handlePublishEvent();
//        }
//        else
//        {
//            // throw away, not publish message
//            //EV << "Throw away message" << "(" << simTime() << ")" << endl;
//            //cancelEvent(timeoutEvent);
//            numThrow++;
//        }
    }
#endif

    // TIMEOUT
    else if (mqmsg->getKind() == SELF_TIMEOUT)
    {
        bubble("timeout!");

        if(currTimeoutSn < mqmsg->getSerialNumber())
        {
            currTimeoutSn = mqmsg->getSerialNumber();
            currTimeoutRe = mqmsg->getSensorRetry();
            retransmission = mqmsg->getSerialNumber();
        }
        else if (currTimeoutSn == mqmsg->getSerialNumber())
        {
            if (currTimeoutRe < mqmsg->getSensorRetry())
            {
                currTimeoutRe = mqmsg->getSensorRetry();
                retransmission = mqmsg->getSerialNumber();
            }
        }

        if(mqmsg->getSerialNumber() == currMessageSn)
        {
            if(mqmsg->getSensorRetry() < (int)maxRetry)
            {
                handleTimeout(mqmsg);
            }
            else
            {
                EV << currTimeoutSn << " failed";
                totalRetry += maxRetry;
                if (par("discipline").longValue() == 1)
                {
                    retransmission = 0;                 //coap
                }
                busy = 0;
                failed = mqmsg->getSerialNumber();
//                std::cerr << "failed= " <<  mqmsg->getSerialNumber() << endl;
                numFailed++;
                EV << " numFailed= " << numFailed;
                EV <<" total Retry= " << totalRetry <<endl;
                //delete publish;
            }
        }
    }

    // RECEACK
    else if (mqmsg->getKind() == MQTT_PUBACK)
    {
//        ev << "PUBACK-src= " << mqmsg->getSrcAddress() << " dest= " << mqmsg->getDestAddress() << endl;
//        ev << "sensor address= " << getIndex() << " rto from gateway= " << value[mqmsg->getRto()]<< endl;

        if (mqmsg->getDestAddress() != getIndex())
        {
            if(mqmsg->getRto()>0)
            {
                ev << "(others)rto from gateway= " << value[mqmsg->getRto()] << " current RTO= " << Rto << endl;
                if (overhearing == 1)
                {
                    if (Rto < value[mqmsg->getRto()])
                    {
                        ev << "change RTO with overhearing!\n";
                        Rto = value[mqmsg->getRto()];
                    }
                }
            }
            delete mqmsg;
        }
        else
        {
            if(mqmsg->getRto()>0)
                ev << "(self)rto from gateway= " << value[mqmsg->getRto()] << " current RTO= " << Rto<< endl;

            if(currReceackSn < mqmsg->getSerialNumber())    // let currReceackSn be non-decreasing
                currReceackSn = mqmsg->getSerialNumber();
            currReceackRe = mqmsg->getSensorRetry();

            EV << "currMessageSn= " << currMessageSn << "; currMessageSs= " << currMessageSs <<"     fail= " << failed << endl;
            EV << "currTimeoutSn= " << currTimeoutSn << "; currTimeoutRe= " << currTimeoutRe << endl;
            EV << "currReceackSn= " << currReceackSn << "; currReceackRe= " << currReceackRe << endl;

            if(currReceackSn >= currMessageSn)
            {
                if (failed == currReceackSn)
                {
                    EV << "PUBACK was timeout~ numPuback:" << numPuback << " f: " << numFailed << endl;
                    cancelEvent(timeoutEvent);
                }
                else if (successful == currReceackSn)
                {
                    EV << "PUBACK has received, but re-sending is before it. " << endl;
                }
                else
                {
                    // PUBACK message arrived.
                    if (currPubackSn < mqmsg->getSerialNumber())
                        numPuback++;
                    if (par("discipline").longValue() == 1)
                    {
                        retransmission = 0;             //coap discipline
                        currNumberOfRetry = 0;          //coap discipline
                    }
                    currPubackSn = mqmsg->getSerialNumber();
                    successful = currReceackSn; //currMessageSn;
                    busy = 0;
                    EV << "numPuback= " << numPuback << endl;
                    EV << currReceackSn << " " << mqmsg->getName() << " arrived time: " << mqmsg->getArrivalTime() << endl;

                    // EV << "Timer cancelled.\n";
                    cancelEvent(timeoutEvent);

                    totalRetry += mqmsg->getSensorRetry();
                    receackTimestamp = mqmsg->getArrivalTime().dbl();
                    Rtt = receackTimestamp - messageTimestamp;

                    // thesis; the second PUBACK just receive calculated RTO from gateway.
                    if (numPuback > 1)
                    {
                        rtoGateway = value[mqmsg->getRto()];
                        ev << "[" << mqmsg->getRto() << "]"<<" rtoGateway= " << rtoGateway << endl;
                    }

                    if (rtoCalcMethod == 0)
                    {

                    }
                    // RFC 6298
                    else if (rtoCalcMethod == 1)
                    {
                        ev << "[[[RFC 6298 method]]]" << endl;
                        // Karn's algorithm
                        if(mqmsg->getSensorRetry() == 0)
                        {
                            Rtt_s = receackTimestamp - messageTimestamp;
                            RtoS = rfc6298(&srtt, &rttvar, Rtt_s, numPuback, 4);
                            RtoS = adjustRange6298(RtoS);
                        }
                        Rto = RtoS;
                    }
                    // CoCoA+
                    else if (rtoCalcMethod == 2)
                    {
                        ev << "[[[[CoCoa+ method]]]" << endl;
                        // Karn's algorithm, strong estimator
                        if(mqmsg->getSensorRetry() == 0)
                        {
                            numStrong++;
                            Rtt_s = receackTimestamp - messageTimestamp;
                            RtoS = rfc6298(&srttS, &rttvarS, Rtt_s, numStrong, 4);
                            Rto = 0.5 * Rto + 0.5 * RtoS;
                            ev << "Rtt_s= " << Rtt_s << " RtoS= " << RtoS << endl;

                            // verify
        //                    if (Rtt_s > 10)
        //                    {
        //                        if (  (currReceackSn == currTimeoutSn) && (currReceackRe <= currTimeoutRe))
        //                            numYes++;
        //                        else
        //                        {
        //                            std::cerr << "currReceackSn = " << currReceackSn << endl;
        //                            numNo++;
        //                        }
        //                    }
                        }
                        else
                        {
                            numWeak++;
                            Rtt_w = receackTimestamp - messageTimestamp;
                            RtoW = rfc6298(&srttW, &rttvarW, Rtt_w, numWeak, 1);
                            Rto = 0.75 * Rto + 0.25 * RtoW;
                            ev << "Rtt_w= " << Rtt_w << " RtoW= " << RtoW << endl ;
                        }
                        Rto = adjustRangeCocoa(Rto);
                        // aging for cocoa+
                        RtoA = Rto;
                        cancelEvent(timeoutAging);
                        scheduleAt(simTime() + 30, timeoutAging);

                        strongRtt += Rtt_s; //calculate total
                        weakRtt += Rtt_w;
                    }
                    // thesis
                    else if (rtoCalcMethod == 3)
                    {
                        ev << "[[[thesis method]]]" << endl;
                        // Karn's algorithm, strong estimator
                        if(mqmsg->getSensorRetry() == 0)
                        {
                            numStrong++;
                            Rtt_s = receackTimestamp - messageTimestamp;
                            idxS = quantization(Rtt_s);
                            idxW = 0;
                        }
                        else
                        {
                            numWeak++;
                            Rtt_w = receackTimestamp - messageTimestamp;
                            idxW = quantization(Rtt_w);
                            idxS = 0;
                        }
                        if (rtoGateway > 0)
                            Rto = rtoGateway;
                    }

                    totalRtt += Rtt;
    //                RttVector.record(Rtt);
                    RtoVector.record(Rto);
    //                RttsVector.record(Rtt_s);
    //                RttwVector.record(Rtt_w);

    //                std::cerr << "rtt= " <<  mqmsg->getSerialNumber() << endl;

                    char msgname[20];
                    sprintf(msgname, "(%d) #%d ", getIndex(), mqmsg->getSerialNumber());
                    EV << msgname << "======================>>calcRTT= " << Rtt << " totalRetry2= " << totalRetry2 <<endl;
                    EV << "==========================>>RTT_s= " << Rtt_s << " Rtt_w= " << Rtt_w << " Rto= " << Rto << endl;

    #if 0
                    if (numPublish > 1000000)
                    {
                        endSimulation();
                    }
    #endif
                }
            }
    //        else
    //        {
    //            EV << "expired PUBACK message: " << mqmsg->getSerialNumber() << " process is terminated." << endl;
    //        }
            delete mqmsg;
        }
        if (ev.isGUI())
            updateDisplay();
    }
}

void Sensor::publishMessage()
{
    // RECEACK event, Publish message. */
    publish = generateNewMessage();
    sendCopyOf(publish);

    if (currMessageSn < publish->getSerialNumber())
    {
        currMessageSn = publish->getSerialNumber();
        currMessageSs = publish->getSensorRetry();
    }
    else if (currMessageSn == publish->getSerialNumber())
    {
        if (currMessageSs < publish->getSensorRetry())
            currMessageSs = publish->getSensorRetry();
    }
    busy = 1;

}

void Sensor::handlePublish(int busy, int retransmission, long discipline)
{
    if (discipline == 0)    // persistent discipline
    {
        if (!busy)
        {
            handlePublishPersistent();
        }
        else
        {
            // throw away, not publish message
            //EV << "Throw away message" << "(" << simTime() << ")" << endl;
            //cancelEvent(timeoutEvent);
            numThrow++;
        }
    }
    else if (discipline == 1)   //CoAP discipline
    {
        if( !busy || retransmission != 0 )
        {
            handlePublishCoap();
        }
        else
            numThrow++;
    }
}

void Sensor::handlePublishPersistent()
{
    /* Publish message. */
    publishMessage();

    // TIMEOUT event
    char msgname[20];
    sprintf(msgname, "timeout(%d) %d-%d", getIndex(), publish->getSerialNumber(), 0);
    timeoutEvent->setName(msgname);
    timeoutEvent->setSerialNumber(seq);
    timeoutEvent->setSensorRetry(0);

    rtoInit = settingRtoInit(Rto, rtoInitMethod);//timeoutEvent,
    timeoutEvent->setRtoInit(rtoInit);
    timeoutEvent->setRtoCalc(rtoInit);

    //cancelEvent(timeoutEvent);
    scheduleAt(simTime()+rtoInit, timeoutEvent);    //TIMEOUT event
    EV << "; timeout: " << simTime()+rtoInit << " rto init: " << rtoInit <<endl;
}

void Sensor::handlePublishCoap()
{
    int retry;
    EV << "busy= " << busy << " retransmission= " << retransmission << endl;

    // Preemptive
    if(retransmission != 0)
    {
//        std::cerr << "Preemptive= " << publish->getSerialNumber() << endl;
        cancelEvent(timeoutEvent);      //verify
        retransmission = 0;
        retry = currNumberOfRetry;
        totalRetry += currNumberOfRetry;
        currNumberOfRetry = 0;
        numPreemptived++;
        ev << "numPreemptived= " << numPreemptived << endl;
        //delete publish;
    }
    else if (!busy)
        retry = 0;

    /* Publish message. */
    publishMessage();

    // TIMEOUT event
    char msgname[20];
    sprintf(msgname, "timeout(%d) %d-%d", getIndex(), publish->getSerialNumber(), retry);
    timeoutEvent->setName(msgname);
    timeoutEvent->setSerialNumber(seq);
    timeoutEvent->setSensorRetry(retry);

    rtoInit = settingRtoInit(Rto, rtoInitMethod);//timeoutEvent,
    timeoutEvent->setRtoInit(rtoInit);
    timeoutEvent->setRtoCalc(rtoInit);

    scheduleAt(simTime()+ rtoInit, timeoutEvent);    //TIMEOUT event
    EV << "; timeout: " << simTime()+ rtoInit << " rto init: " << rtoInit <<endl;
}

void Sensor::handleTimeout(MqttMessage *mqmsg)
{
    currNumberOfRetry++;
    //currTimeoutRe = mqmsg->getSensorRetry();
//            EV << ">>currTimeoutSn= " << currTimeoutSn << "; currTimeoutRe= " << currTimeoutRe << endl;
    EV << "Timeout of PUBLISH expired, re-sending message and restarting timer  ";

    // Publish message.
    totalRetry2++;
    char msgname[20];
    publish->setSensorRetry(mqmsg->getSensorRetry()+1);
    sprintf(msgname, "PUBLISH(%d) #%d-%d ", mqmsg->getClientId(), mqmsg->getSerialNumber(), publish->getSensorRetry());
    publish->setName(msgname);
    sendCopyOf(publish);

    // TIMEOUT event
    cancelEvent(timeoutEvent);

    char msgname2[20];
    sprintf(msgname2, "timeout(%d) %d-%d", getIndex(), publish->getSerialNumber(), mqmsg->getSensorRetry()+1);
    timeoutEvent->setName(msgname2);
    timeoutEvent->setSensorRetry(mqmsg->getSensorRetry()+1);

    rtoCalc = settingRtoTimeout(mqmsg->getRtoInit(), mqmsg->getRtoCalc(), rtoMultMethod);

//    if(par("rtoMultiMethod").longValue() == 0)
//        rtoCalc = rtoFixed;
//    else if(par("rtoMultiMethod").longValue() == 1)
//    {
//        rtoCalc = mqmsg->getRtoCalc() * 2;
//        rtoCalc = adjustRange(rtoCalc);
//        timeoutEvent->setRtoCalc(rtoCalc);
//    }
    scheduleAt(simTime()+rtoCalc, timeoutEvent);
    EV << "; timeout: " << simTime()+rtoCalc << " rto calc: " << rtoCalc <<endl;
}

MqttMessage *Sensor::generateNewMessage()
{
    char msgname[20];
    sprintf(msgname, "PUBLISH(%d) #%d-%d ", getIndex(), ++seq, 0);
    EV << msgname;
//    MqttMessage *msg = new MqttMessage(msgname, MQTT_PUBLISH);
    msg -> setName(msgname);
    msg -> setSerialNumber(seq);
    msg -> setSensorRetry(0);
    msg -> setSrcAddress(ownAddr);
    msg -> setDestAddress(serverAddr);
    msg -> setBrokerProcId(brokerprocId);
    msg -> setGatewayProcId(gatewayprocId);
    msg -> setClientId(getIndex());
    idxS ? msg->setRttS(idxS) : msg->setRttW(idxW);
    ev << " ;idxS= " << idxS ;
    ev << " ;idxW= " << idxW << endl;

    if (seq == 1)
    {
        msg -> setRttS(0);
        msg -> setRttW(0);
    }
//    if(par("rtoInitMethod").longValue() == 1)
//    {
//        // CoAP : random 2~3
//        rtoInit = uniform(2,3);
//        msg -> setRtoInit(rtoInit);
//        msg -> setRtoCalc(rtoInit);
//    }
    if (seq > 1)
    {
        msg -> setRtt(Rtt);
    }
    return msg;
}

double Sensor::settingRtoInit(double rto, int InitMethod)//MqttMessage * timeoutEvent,
{
    if (InitMethod == 0)
    {
        return rtoFixed;
    }
    else if (InitMethod == 1)
    {
        return uniform(2,3);
    }
    else if (InitMethod == 2)
    {
        return rtoInitCalc(rto, 1);
    }
    else if (InitMethod == 3)
    {
        return rtoInitCalc(rto, 2);
    }
    return rtoFixed;
}

double Sensor::settingRtoTimeout(double rtoInit, double rtoCalc, int MultMethod)
{
    double calc;
    if (MultMethod == 0)
    {
        return rtoFixed;
    }
    else if (MultMethod == 2)
    {
        if(rtoInit <= 0)
        {
            ev <<"initRTO <= 0!!"<<endl;
            calc = 0;
        }
        else if (rtoInit > 3)
            calc =  rtoCalc *1.5;
        else if (rtoInit >= 1 && rtoInit <= 3)
            calc =  rtoCalc * 2;
        else
            calc = rtoCalc * 3;
        return adjustRangeCocoa(calc);
    }
    else
    {
        calc = rtoCalc * 2;
        return adjustRange6298(calc);
    }
    return rtoCalc * 2;
}

void Sensor::sendCopyOf(MqttMessage *msg)
{
    MqttMessage *copy = (MqttMessage *) msg->dup();
    send(copy, "out");
    if(msg->getSensorRetry() == 0)
    {
        numPublish++;
        /* for calculating RTT */
        messageTimestamp = copy->getSendingTime().dbl();
    }
    EV << " sending Time: " << copy->getSendingTime();
    EV << " ;arrival Time: " <<copy->getArrivalTime();
}

double Sensor::rtoInitCalc(double rto, double init)
{
    //If the RTT is not sampled yet
    if (rto < 0)
        return init;
    else
        return rto;
}

double Sensor::adjustRange6298(double rto)
{
//    if (rto < 3)
//        return 3;
    if (rto > 60)
        return 60;
    return rto;
}

double Sensor::adjustRangeCocoa(double rto)
{
    if (rto > 32)
        return 32;
    return rto;
}

double Sensor::rfc6298(double *srtt, double *rttvar, double rtt, unsigned n, unsigned factorK)
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
//    if (tmp < 1)
//        return 1;
//    else
        return tmp;
}

bool Sensor::IsPubackValid(unsigned cMesSn, unsigned cTimSn, unsigned cTimRe, unsigned cRecSn, unsigned cRecRe)
{
    // currentMessageSn is bigger than currReceackSn, denotes this PUBACK is expired.
    if(cRecSn < cMesSn)
    {
        return false;
    }
    else
    {
        if (cRecSn == cTimSn)
        {
            if(cRecRe > cTimRe)
            {
                return true;
            }
        }
        else if (cRecSn > cTimSn)
            return true;
    }
    return false;
}

void Sensor::initQuantization()
{
    double temp;
    double granularity =  range / (SIZE - 1);
    for(int i = 0; i < SIZE; i++)
    {
        value[i] = temp;
        //EV << value[i] << "  ";
        temp += granularity;
        //if (i == 20) ev << "\n";
    }
}

int Sensor::quantization(double rtt)
{
    double granularity =  range / (SIZE - 1);
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

void Sensor::updateDisplay()
{
    char buf[60];
    //sprintf(buf, "mes: %ld pub: %ld ack: %ld\n thr: %ld fai: %ld", numMessage, numPublish, numPuback, numThrow, numFailed);
    sprintf(buf, "mes: %ld pub: %ld ack: %ld\n thr: %ld fai: %ld pre: %ld", numMessage, numPublish, numPuback, numThrow, numFailed, numPreemptived);
    getDisplayString().setTagArg("t",0,buf);
}


void Sensor::finish()
{
    // This function is called by OMNeT++ at the end of the simulation.
    EV << "Message:    " << numMessage << endl;
    EV << "PUBLISH:    " << numPublish << endl;
    EV << "PUBACK :    " << numPuback << endl;
    EV << "Throw away: " << numThrow << endl;
    EV << "Failed:     " << numFailed << endl;
    EV << "Preemptived:     " << numPreemptived << endl;

    //EV << "Total Retry " << totalRetry << endl;
    EV << "Total Retry " << totalRetry2 << endl;
    EV << "PDR2:   " << (double)numPuback/numPublish << endl;
    EV << "DPR:   " << (double)numThrow/numMessage << endl;
    EV << "mean RTT:   " << totalRtt/numPuback << endl;
    EV << "mean Retry: " << (double)totalRetry2/numPublish << endl;

    //EV << "numYes= " << numYes << " numNo= " << numNo << endl;

    recordScalar("#message", numMessage);
    recordScalar("#publish", numPublish);
    recordScalar("#puback", numPuback);
    recordScalar("#throw", numThrow);
    recordScalar("#failed", numFailed);
    recordScalar("#Preemptived",numPreemptived);

    recordScalar("#Total Retry", totalRetry2);
    //recordScalar("#total RTT", totalRtt);
    recordScalar("#PDR2", (double)numPuback/numPublish);
    recordScalar("#DPR", (double)numThrow/numMessage);
    recordScalar("#mean RTT", totalRtt/numPuback);
    recordScalar("#mean Retry", (double)totalRetry2/numPublish);
}
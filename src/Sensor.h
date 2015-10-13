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

#ifndef __MQTT_SENSOR_H_
#define __MQTT_SENSOR_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "MqttMessage_m.h"

#define SIZE 512
/**
 * TODO - Generated class
 */
class Sensor : public cSimpleModule
{
   private:
    double timeout, rtoInit, rtoCalc, rtoFixed;
    unsigned int maxRetry;
    int rtoInitMethod, rtoMultMethod, rtoCalcMethod;

    double messageTimestamp, receackTimestamp;
    MqttMessage *msg;
    MqttMessage *timeoutEvent, *publishEventFixed, *publishEventExpon, *timeoutSearch, *timeoutConnect, *timeoutAging;
    MqttMessage *publish, *puback, *searchgw, *gwinfo, *advertise, *connect;

    int seq, busy;
    int failed, successful, retransmission;
    int ownAddr, serverAddr, gatewayAddr;
    int brokerprocId, gatewayprocId;

    int currMessageSn, currMessageSs, currTimeoutSn, currTimeoutRe, currPubackSn;
    int currReceackSn, currReceackRe;
    int currNumberOfRetry;

    unsigned int totalRetry, totalRetry2;
    double Rtt, Rtt_s, Rtt_w;
    double srtt, rttvar, Rto, RtoS, RtoW, RtoA;
    double srttS, rttvarS, srttW, rttvarW;
    double totalRtt, strongRtt, weakRtt;
    int idxS, idxW;

    // thesis
    double range;
    int numQuantization;
    double value[SIZE];
    double rtoGateway;
    int size;

    long numMessage, numPublish, numPuback, numFailed, numThrow, numPreemptived;
    long numYes, numNo, numStrong, numWeak;
    cOutVector RttVector, RtoVector, RttwVector, RttsVector;

  public:
    Sensor();
    virtual ~Sensor();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void publishMessage();
    virtual void handlePublish(int, int, long);
    virtual void handlePublishPersistent();
    virtual void handlePublishCoap();
    virtual void handleTimeout(MqttMessage *);
    virtual MqttMessage *generateNewMessage();
    virtual double settingRtoInit(double, int);//MqttMessage *,
    virtual double settingRtoTimeout(double, double, int);
    virtual void sendCopyOf(MqttMessage *msg);
    virtual double rtoInitCalc(double, double);
    virtual double adjustRange6298(double);
    virtual double adjustRangeCocoa(double);
    virtual double rfc6298(double*, double*, double, unsigned, unsigned);
//    virtual bool isSensorPhase(unsigned int, unsigned int, unsigned int, unsigned int);
    virtual bool IsPubackValid(unsigned, unsigned, unsigned, unsigned, unsigned);
    virtual void initQuantization();
    virtual int quantization(double);
    virtual void updateDisplay();
    virtual void finish();
};

#endif

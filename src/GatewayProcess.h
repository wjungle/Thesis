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

#ifndef __MQTT_GATEWAYPROCESS_H_
#define __MQTT_GATEWAYPROCESS_H_

#include <omnetpp.h>
#include "MqttMessage_m.h"

#define SIZE 512
/**
 * TODO - Generated class
 */
class GatewayProcess : public cSimpleModule
{
  private:
    double rtoInit, rtoCalc, rtoFixed;
    unsigned int maxRetry;
    double messageTimestamp, receackTimestamp;

    MqttMessage *msg;
    MqttMessage *timeoutEvent;
    MqttMessage *puback, *publish;
    MqttMessage *gwinfo, *advertise;

    unsigned int sn;
    int src, dest, clientId;

    int currMessageSn, currMessageSs;
    int currTimeoutSn, currTimeoutSs, currTimeoutGw;
    int currReceackSn, currReceackSs, currReceackGw;

    int publishMessageSn, publishMessageRe;
    int pubackMessageSn, pubackMessageSs, pubackMessageGw;

    unsigned int totalRetry, totalRetry2;
    double totalRtt, strongRtt, weakRtt;
    unsigned int failed, retransmission;
    int successfulSn, successfulSs;
    double srtt, rttvar, rtoGw, rtoAll;
    cOutVector RtoVector, RttsVector, RttwVector, RttVector, RtoSensorVector;
    double Rtt_s, Rtt_w;

    // thesis
    double granularity, range;
    double value[SIZE];
    double rttSensorS, rttSensorW;
    double rtoSensorS, rtoSensorW, rtoSensor;
    double srttSensorS, rttvarSensorS, srttSensorW, rttvarSensorW;
    unsigned numStrong, numWeak;
    int idxRto, idxS, idxW;
    int numAging;
    int size;

    unsigned numPuback, numPublish, numRecPub, numFailed;

    unsigned int trafficLight;      // determine the PUBLISH from sensor work or not

    cGate *gatewayDestGate, *gatewaySrcGate;

  public:
    GatewayProcess();
    virtual ~GatewayProcess();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual MqttMessage *generateNewMessage(unsigned int, unsigned int, int, int, int, int, int);
    virtual void sendCopyOf(MqttMessage *msg);
    virtual double rtoInit6298(double);
    virtual double adjustRange(double);
    virtual bool isSensorPhase(unsigned int, unsigned int, unsigned int, unsigned int);
    virtual bool isPubackValid(int, int, int, int, int, int, int, int);
    virtual bool isSuccessful(int, int);
    virtual double rfc6298(double*, double*, double, unsigned, unsigned);
    virtual void initQuantization();
    virtual int quantization(double);
    virtual void updateDisplay();
    virtual void finish();
};

enum LIGHT {RED, GREEN};
enum TIMER {OFF, ON};

#endif

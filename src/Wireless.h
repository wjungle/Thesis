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

#ifndef __MQTT_WIRELESS_H_
#define __MQTT_WIRELESS_H_

#include <omnetpp.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

class Wireless : public cDelayChannel
{
  private:
    double plr;
    long int counter;
    double constant, coeff;
    double alpha, beta, gamma1, gamma2;
    double mean, bVar, variance;
    simtime_t delay, olderDelay, temp;
    //std::ofstream logfile;
    cOutVector DelayVector, /*gamma1Vector, gamma2Vector,*/ tempVector;
    int cntP,cntS;
    int rttSource;

  public:
    explicit Wireless(const char *name = NULL) : cDelayChannel(name) { counter = 0; }
    virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);

  protected:
    virtual void initialize();
    void finish();
};

#endif

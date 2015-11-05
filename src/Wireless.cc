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

#include "Wireless.h"

Register_Class(Wireless);

void Wireless::initialize()
{
    //EV << "PacketLogger initialize()\n";
    cDelayChannel::initialize();
    plr = par("PLR");
//    alpha = par("gammaAlpha");
//    beta = par("gammaBeta");
    constant = par("gammaConst");
    mean = par("mean");
//    bVar = par("bVar");
    olderDelay = mean;
    rttSource = par("rttSource");

    DelayVector.setName("wireless Delay");
    gamma1Vector.setName("wireless gamma1");
    gamma2Vector.setName("wireless gamma2");
    //tempVector.setName("wireless temp");

//    temp = 0;
//    variance = bVar;
//    beta = variance/mean;
//    alpha = mean/beta;

    cntP=0;cntS=0;
}

void Wireless::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    //EV << "PacketLogger processMessage()\n";
    cDelayChannel::processMessage(msg, t, result);

    if (uniform(0,1) < plr)
    {
        EV << "\"Losing\" message" << endl;
        bubble("losing wireless message");
        result.discard = 1;
    }
    else
    {
        // if channel is disabled, signal that message should be deleted
        result.discard = 0;

        // propagation delay modeling
#if 1
//        gamma = gamma_d(alpha, beta);
//        gamma /= (alpha*beta);
//        coeff = par("coefficient");
//        delay = constant + (gamma*coeff);

        // sin()
        if (rttSource == 0)
            delay = mean * sin(1.25 * simTime().dbl()) + (mean+constant);
            //delay = 0.07 * sin(1.25 * simTime().dbl()) + 0.08;
        // saw-like
        else if (rttSource == 1)
            delay = ((2*mean) * ((0.2 * simTime().dbl()) - ceil(0.2 * simTime().dbl()))) + (2*mean+constant);
#else
//        variance = bVar;
//        beta = variance/mean;
//        alpha = mean/beta;

        if (uniform(0,1) < 0.5)
        {
            gamma1 = gamma_d(alpha, beta);
            temp = olderDelay + gamma1;
//            delay = MAX(temp, constant);
            cntP++;
        }
        else
        {
            gamma2 = gamma_d(alpha, beta);
            temp = olderDelay - gamma2;
//            delay = MAX(temp, constant);
            cntS++;
        }
        delay = MAX(temp, constant);
        olderDelay = temp;
        //olderDelay = delay;

        //delay = par("IaTime");
#endif
        DelayVector.record(delay);
        gamma1Vector.record(gamma1);
        gamma2Vector.record(gamma2);
        //tempVector.record(temp);

        EV <<" (w)delay Time: " << delay <<endl;
        result.delay = delay;
    }

}

//void Wireless::MAX(double a, )

void Wireless::finish()
{
    //EV << "PacketLogger finish()\n";
//    EV << "cntP :"<< cntP << endl;
//    EV << "cntS :"<< cntS << endl;
//    recordScalar("#cntP", cntP);
//    recordScalar("#cntS", cntS);
}


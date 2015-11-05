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

#include "Backbone.h"

Register_Class(Backbone);

void Backbone::initialize()
{
    cDelayChannel::initialize();
    plr = par("PLR");
    alpha = par("gammaAlpha");
    beta = par("gammaBeta");
    rttSource = par("rttSource");
}

void Backbone::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    //EV << "PacketLogger processMessage()\n";
    cDelayChannel::processMessage(msg, t, result);

    if (uniform(0,1) < plr)
    {
        EV << "\"Losing\" message" << endl;
        bubble("losing backbone message");
        result.discard = 1;
    }
    else
    {
        // if channel is disabled, signal that message should be deleted
        result.discard = 0;
        // propagation delay modeling
#if 1

//        constant = par("gammaConst");
//        gamma = gamma_d(par("gammaAlpha"), par("gammaBeta"));
//        gamma /= (alpha * beta);
//        delay = constant + (gamma*0.1);

        // sin()
        if (rttSource == 0)
            delay = 0.5 * sin(1.25 * simTime().dbl()) + 1;
            //delay = 0.07 * sin(1.25 * simTime().dbl()) + 0.08;
        // saw-like
        else if (rttSource == 1)
            delay = (1 * ((0.2 * simTime().dbl()) - ceil(0.2 * simTime().dbl()))) + 1.5;
#else
        delay = par("IaTime");
#endif
        EV <<" (b)delay Time: " << delay <<endl;
        result.delay = delay;
    }
}

void Backbone::finish()
{
    //EV << "PacketLogger finish()\n";
}

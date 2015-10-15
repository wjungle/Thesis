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
        //simtime_t delay = par("IaTime");
        //par("gammaConst"); //+
        constant = par("gammaConst");
        gamma = gamma_d(par("gammaAlpha"), par("gammaBeta"));
        simtime_t delay = constant + gamma;
        EV <<" (w)delay Time: " << delay <<endl;
        result.delay = delay;
    }

}

void Wireless::finish()
{
    //EV << "PacketLogger finish()\n";
}


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

package mqtt;

network Mqtt
{
    parameters:
        int numSensors @prompt("Number of sensors") = default(1);
        @display("bgb=459,255");
    submodules:
        sensor[numSensors]: Sensor {
            parameters:
                @display("p=69,200,m,10,80");
        }

        //        gateway: Gateway {
        //            parameters:
        //                pkRate = 1.5*numSensors;
        //                queueMaxLen = 20;
        //            	@display("p=213,121");
        //            gates:
        //                in[numSensors+1];
        //                out[numSensors+1];
        //        }
        gw: GW {
            parameters:
                @display("p=213,121;is=vl");
            gates:
                port[numSensors+1];	//declare for connecting with broker

        }
        broker: Broker {
            parameters:
                @display("p=359,122");
        }
    connections:
        for i=0..numSensors-1 {
            sensor[i].out --> Wireless --> gw.port$i[i];
            sensor[i].in <-- Wireless <-- gw.port$o[i];
        }
        gw.port$o[numSensors] --> Backbone --> broker.in;
        gw.port$i[numSensors] <-- Backbone <-- broker.out;
}

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
//        sensor2[numSensors]: Sensor2 {
//         	@display("p=69,213");   
//        }
        gateway: Gateway {
            parameters:
                pkRate = 1.5*numSensors;
                queueMaxLen = 20;
            	@display("p=213,121");
            gates:
                in[numSensors+1];
                out[numSensors+1];
        }
        broker: Broker {
            parameters:
            	@display("p=359,122");
        }
    connections:
        for i=0..numSensors-1 {
            sensor[i].out --> Wireless --> gateway.in[i];
            sensor[i].in <-- Wireless <-- gateway.out[i];
//            sensor2[i].out --> { delay = 0ms; } --> gateway.in[i];
//            sensor2[i].in <-- { delay = 0ms; } <-- gateway.out[i];
        }
        gateway.out[numSensors] --> Backbone --> broker.in;
        gateway.in[numSensors] <-- Backbone <-- broker.out;
}

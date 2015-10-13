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

//
// TODO auto-generated module
//
simple Sensor
{
	parameters:
        double timeout @unit(s) @prompt("Connection timeout") = default(7s);
        volatile double pubIaTimeExpon @unit(s) @prompt("Publish Exponential interarrival time"); // = default(1s)
        volatile double pubIaTimeFixed @unit(s) @prompt("Publish Fixed interarrival time"); // = default(1s)
        int limit @unit(times) = default(1times);
        int discipline;
        int rtoInitMethod;
        int rtoMultMethod;
        int rtoCalcMethod;
        int overhearing;
        int quanSize = default(512);
        double range;
        @display("i=block/wtx");
    gates:
        input in;
        output out;
}

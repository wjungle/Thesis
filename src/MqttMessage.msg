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

enum MqttMessageType
{
    MQTT_SEARCHGW = 0;
    MQTT_GWINFO = 1;
    MQTT_CONNECT = 2;
    MQTT_CONNACK = 3;
    MQTT_PUBLISH = 4;
    MQTT_PUBACK = 5;
    MQTT_ADVERTISE = 6;
    MQTT_DISCONNECT = 7;
    
    SELF_MESSAGE_F = 20;
    SELF_MESSAGE_E = 21;
    SELF_TIMEOUT = 22;
    SELF_RECEACK = 23;
    SELF_TIMEOUT_S = 24;	//searchgw
    SELF_TIMEOUT_C = 25;	//connect
    SELF_TIMEOUT_A = 26;	//aging
};

packet MqttMessage {
//    int someField;
//    string anotherField;
//    double arrayField1[];
//    double arrayField2[10];
    int srcAddress;
    int destAddress;
    int gatewayProcId;
    int brokerProcId;
    
    double departureTime;
    int serialNumber;
   	int sensorRetry;
   	int gatewayRetry;
	double rtoInit;
	double rtoCalc;
    
    int clientId;
    double rtt;
    int rttS;
    int rttW;
    int rto;
};

//message SelfMessage
//{
//    unsigned int serialNumber;
//	unsigned int numberOfRetry;
//	double rtoInit;
//	double rtoCalc;
//	
//}

//packet MqttDataMessage extends MqttMessage
//{
//    string payload;
//};
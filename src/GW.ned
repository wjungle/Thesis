
package mqtt;

module GW
{
    parameters:
        int numSensors;
    gates:
        inout port[];
        inout portBk;
    submodules:
        manager: Manager {
            @display("p=93,59");
            gates:
                line[numSensors];
//                lineGw;
        }
        gateway: Gateway {
            @display("p=298,59");
            gates:
                line[2];
        }
    connections:
        manager.lineGw <--> gateway.line[0];
        for i=0..numSensors-1 {
            manager.line[i] <--> port[i];
        }
        portBk <--> gateway.line[1];
}

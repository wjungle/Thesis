
package mqtt;

module GW
{
    parameters:
        int numSensors;
    gates:
        inout port[];
    submodules:
        manager: Manager {
            @display("p=93,59");
            gates:
                line[numSensors+1];
        }
        gateway: Gateway {
            @display("p=298,59");
            gates:
                line[2];
        }
    connections:
        manager.line[numSensors] <--> gateway.line[0];
        for i=0..numSensors-1 {
            manager.line[i] <--> port[i];
        }
        port[numSensors] <--> gateway.line[1];
}

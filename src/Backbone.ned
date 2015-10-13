
package mqtt;

//
// TODO auto-generated module
//
channel Backbone extends ned.DelayChannel
{
  parameters:
	@class(Backbone);
	double PLR @prompt("Backbone Packet Loss Ratio") = default(0.1);

	volatile double IaTime @unit(s) @prompt("half of round trip interarrival time to server"); //
}


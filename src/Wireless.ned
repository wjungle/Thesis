
package mqtt;

//
// TODO auto-generated module
//
channel Wireless extends ned.DelayChannel
{
  parameters:
	@class(Wireless);
	double PLR @prompt("Wireless Packet Loss Ratio") = default(0.0);

	volatile double IaTime @unit(s) @prompt("half of round trip interarrival time to gateway"); //
}


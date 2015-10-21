
package mqtt;

//
// TODO auto-generated module
//
channel Wireless extends ned.DelayChannel
{
  parameters:
	@class(Wireless);
	double PLR @prompt("Wireless Packet Loss Ratio") = default(0.0);

	volatile double IaTime @unit(s) @prompt("half of round trip interarrival time to gateway"); 
	volatile double gammaAlpha @prompt("alpha of Gamma distribution");
	volatile double gammaBeta @prompt("beta of Gamma distribution");
	volatile double gammaConst @prompt("constant of Gamma distribution");
	volatile double coefficient @prompt("coefficient after gereralization");
}


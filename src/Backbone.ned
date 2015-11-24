
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
	//volatile double gammaAlpha @prompt("alpha of Gamma distribution");
	//volatile double gammaBeta @prompt("beta of Gamma distribution");
	//volatile double gammaConst @prompt("constant of Gamma distribution");
	volatile double rttSource @prompt("RTT source; 0:sin(); 1:saw-like()");
}


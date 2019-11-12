#include "ns3/core-module.h"
#include "ns3/network-module.h"
 #include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/animation-interface.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NS3ScriptExample");

int main (int argc, char *argv[])
{bool verbose=true;
uint32_t ncsma=3,np2p=2;
CommandLine cmd;
cmd.AddValue("ncsma","number of extra xcsma nodes",ncsma);
cmd.AddValue("verbose","tell echo applications to log if true",verbose);
cmd.Parse(argc,argv);
if(verbose)
{
LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
}
ncsma=ncsma==0?1:ncsma;
NodeContainer p2pnodes;
p2pnodes.Create(np2p);

NodeContainer csmanodes;
csmanodes.Add(p2pnodes.Get(1));
csmanodes.Create(ncsma);

PointToPointHelper pointtopoint;
pointtopoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
pointtopoint.SetChannelAttribute("Delay",StringValue("2ms"));

NetDeviceContainer p2pdevices;
p2pdevices=pointtopoint.Install(p2pnodes);

CsmaHelper csma;
csma.SetChannelAttribute("DataRate",StringValue("100Mbps"));
csma.SetChannelAttribute("Delay",StringValue("2ms"));

NetDeviceContainer csmadevices;
csmadevices=csma.Install(csmanodes);

InternetStackHelper stack;
stack.Install(p2pnodes.Get(0));
stack.Install(csmanodes);
Ipv4AddressHelper address;
address.SetBase("10.1.1.0","255.255.255.0");
Ipv4InterfaceContainer p2pinterfaces;
p2pinterfaces=address.Assign(p2pdevices);

address.SetBase("10.1.2.0","255.255.255.0");
Ipv4InterfaceContainer csmainterfaces;

csmainterfaces=address.Assign(csmadevices);

UdpEchoServerHelper echoserver(9);

ApplicationContainer serverapps=echoserver.Install(csmanodes.Get(ncsma));
serverapps.Start(Seconds(1.0));
serverapps.Stop(Seconds(10.0));

UdpEchoClientHelper echoclient(csmainterfaces.GetAddress(ncsma),9);

echoclient.SetAttribute("MaxPackets",UintegerValue(1));
echoclient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
echoclient.SetAttribute("PacketSize",UintegerValue(1024));

ApplicationContainer clientapps=echoclient.Install(p2pnodes.Get(0));
clientapps.Start(Seconds(2.0));
clientapps.Stop(Seconds(10.0));

Ipv4GlobalRoutingHelper::PopulateRoutingTables();

pointtopoint.EnablePcapAll("second");
csma.EnablePcap("second",csmadevices.Get(1),true);

AnimationInterface anim("anim_sec2.xml");

anim.SetConstantPosition(p2pnodes.Get(0),60.5,60.5);
anim.SetConstantPosition(csmanodes.Get(0),10.0,10.0);
anim.SetConstantPosition(csmanodes.Get(1),50.5,18.25);
anim.SetConstantPosition(csmanodes.Get(2),20.0,20.5);
anim.SetConstantPosition(csmanodes.Get(3),25.5,10.75);
anim.EnablePacketMetadata(true);
Simulator::Run();
Simulator::Destroy();
return 0;
}

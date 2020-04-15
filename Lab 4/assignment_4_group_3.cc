#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include <string>
#include <fstream>

using namespace std;
using namespace ns3;

AsciiTraceHelper ascii;
Ptr<PacketSink> cbrSinks[5],ftpSink;
int total_bytes_transferred=0;
int total_packets_dropped=0;

NS_LOG_COMPONENT_DEFINE ("Network_Lab_4_Group_3");

// ============================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                1 Mbps, 10 ms
//
// ============================================

// The basic structure of this file has been taken from the seventh.cc file available in examples

// Function to store congestion window changes, keeping the old and new values for comparioson
// A study of this data can be used to demonstrate the slow start, congestion avoidance (also fast recovery stages wherever applicable)
static void CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " " << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << oldCwnd << " " << newCwnd << endl;
}

// Auxiliary function to keep track of the number of packets dropped
// It updates the total_packets_dropped counter for each packet drop
static void RxDrop (Ptr<OutputStreamWrapper> stream, Ptr<const Packet> p)
{
  total_packets_dropped++;
  NS_LOG_UNCOND ("RxDrop at " << Simulator::Now ().GetSeconds ());
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << total_packets_dropped << endl;
}

// Auxiliary function to count the number of bytes transferred
// It updates the total_bytes_transferred counter for every byte transferred
static void TotalRx(Ptr<OutputStreamWrapper> stream)
{
    total_bytes_transferred += ftpSink->GetTotalRx();

    for(int i=0; i<5; i++)
    {
        total_bytes_transferred += cbrSinks[i]->GetTotalRx();
    }

    *stream->GetStream()<<Simulator::Now ().GetSeconds ()<<" " <<total_bytes_transferred<<endl;
    Simulator::Schedule(Seconds(0.0001),&TotalRx, stream);
}

// Auxiliary function to call CwndChange function with appropriate stream to write the value in
static void Trace_Cwnd_Length (Ptr<OutputStreamWrapper> stream)
{
    //Trace changes to the congestion window
    Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeBoundCallback (&CwndChange,stream));
}

int main (int argc, char *argv[])
{
  // User can change the default tcp_protocol using command line for different simulations
  string tcp_protocol = "TcpNewReno";

  CommandLine cmd;
  cmd.AddValue ("tcp_protocol", "Transport protocol to use: TcpNewReno, TcpHybla, TcpWestwood, TcpScalable, TcpVegas", tcp_protocol);
  cmd.Parse (argc, argv);

  // Selecting TCP variant
  if (tcp_protocol.compare("TcpNewReno") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  else if (tcp_protocol.compare("TcpHybla") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpHybla::GetTypeId()));
  else if (tcp_protocol.compare("TcpWestwood") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId()));
  else if (tcp_protocol.compare("TcpScalable") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpScalable::GetTypeId()));
  else if (tcp_protocol.compare("TcpVegas") == 0)
    Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpVegas::GetTypeId()));
  else
  {
    NS_LOG_DEBUG ("Invalid TCP version");
    fprintf (stderr, "Invalid TCP version\n");
    exit (1);
  }

  string temp1,temp2,temp3;
  temp1=tcp_protocol + "_bytes_received.dat";
  temp2=tcp_protocol + "_packets_dropped.dat";
  temp3=tcp_protocol + "_cwnd_status.dat";

  // Different output streams for recording the required data
  Ptr<OutputStreamWrapper> bytes_received = ascii.CreateFileStream (temp1);
  Ptr<OutputStreamWrapper> packets_dropped = ascii.CreateFileStream (temp2);
  Ptr<OutputStreamWrapper> cwnd_status = ascii.CreateFileStream (temp3);

  // Creating Nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Creating point-to-point link with given Data rate 1Mbps and Delay 10ms
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("10ms"));
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));  // Bandwidth * delay= 10^4 bits = 1250 Bytes and we have set packet size = 1250 Bytes

  // Attaching nodes to the NetDeviceContainer
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  // Creating error model for the simulations
  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.00001));
  devices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  // Using the InternetStackHelper for carrying out the simulation and installing it on the nodes
  InternetStackHelper stack;
  stack.Install (nodes);

  // Assigning IP address
  NS_LOG_INFO ("Assigning IP Addresses.");
  uint16_t sinkPort = 12000;

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  Address sinkAddress = InetSocketAddress (interfaces.GetAddress (1), sinkPort);
  Address anyAddress = InetSocketAddress (Ipv4Address::GetAny (), sinkPort);

//------------------------- FTP traffic created by the TCP agent --------------------//

  // sender node application
  BulkSendHelper sender ("ns3::TcpSocketFactory",sinkAddress);
  sender.SetAttribute ("MaxBytes", UintegerValue(0));
  ApplicationContainer sourceApps = sender.Install (nodes.Get (0));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (1.8));

  // receiver node application (sink)
  PacketSinkHelper receiver ("ns3::TcpSocketFactory", anyAddress);
  ApplicationContainer sinkApps = receiver.Install (nodes.Get (1));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (1.8));

  ftpSink = DynamicCast<PacketSink> (sinkApps.Get (0));


//------------------------- CBR traffic generated by 5 agents -----------------------//

  // Start and End times for the 5 CBR traffic agents
  double begin[]={0.2, 0.4, 0.6, 0.8, 1.0};
  double end[]={1.8, 1.8, 1.2, 1.4, 1.6};

  // Using sockets 13000 - 13004 (can take any ephemeral ports)
  uint16_t cbr_ports[]={13000,13001,13002,13003,13004};

  // Looping for the 5 channels
  // Using OnOffHelper for generating CBR traffic during the on duration
  // Data rate 250 Kbps as mentioned
  // Taking packet size as 1250 bytes, so that setting drop tail queue size is simplified to 1 packet, can take other values also
  // Using source and sink similar to the above FTP case
  for(int i=0; i<5; i++)
  {
      ApplicationContainer cbrSourceApps;
      ApplicationContainer cbrSinkApps;

      OnOffHelper on_off ("ns3::UdpSocketFactory", InetSocketAddress (interfaces.GetAddress (1), cbr_ports[i]));
      on_off.SetAttribute ("PacketSize", UintegerValue (1250));

      on_off.SetAttribute ("DataRate", StringValue ("250Kbps"));

      on_off.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
      on_off.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

      on_off.SetAttribute ("StartTime", TimeValue (Seconds (begin[i])));
      on_off.SetAttribute ("StopTime", TimeValue (Seconds (end[i])));

      cbrSourceApps.Add (on_off.Install (nodes.Get (0)));

      PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), cbr_ports[i]));
      cbrSinkApps = sink.Install (nodes.Get (1));

      cbrSinkApps.Start (Seconds (0.0));
      cbrSinkApps.Stop (Seconds (1.8));

      cbrSinks[i] = DynamicCast<PacketSink> (cbrSinkApps.Get (0));
  }

  // Keeping track of dropped packets using the RxDrop function
  devices.Get (1)->TraceConnectWithoutContext ("PhyRxDrop", MakeBoundCallback (&RxDrop, packets_dropped));

  // Finally running the simulation
  NS_LOG_INFO ("Running Simulation");

  Simulator::Schedule(Seconds(0.00001),&TotalRx, bytes_received);
  Simulator::Schedule(Seconds(0.00001),&Trace_Cwnd_Length, cwnd_status);

  // Using Flow monitor for displaying various stats
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  Simulator::Stop (Seconds (1.8));
  Simulator::Run ();

  Ptr<Ipv4FlowClassifier> flow_classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  map<FlowId, FlowMonitor::FlowStats> info = flowMonitor->GetFlowStats ();
  FlowMonitor::FlowStats temp=info[1];

  // Displaying the various statistics
  cout << "\n\n";
  cout << "Using " << tcp_protocol <<  " TCP congestion control algorithm\n\n";
  cout << "Flow monitor statistics\n\n";
  cout << "Transmitted Packets:" << temp.txPackets << endl;
  cout << "Transmitted Bytes:" << temp.txBytes << endl;
  cout << "Offered Load:" << temp.txBytes * 8.0 / (temp.timeLastTxPacket.GetSeconds () - temp.timeFirstTxPacket.GetSeconds ()) / 1000000 << " Mbps" << endl;
  cout << "Received Packets:" << temp.rxPackets << endl;
  cout << "Received Bytes:" << temp.rxBytes<< endl;
  cout << "Throughput:" << temp.rxBytes * 8.0 / (temp.timeLastRxPacket.GetSeconds () - temp.timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << endl;
  cout<<"\n\n";
  flowMonitor->SerializeToXmlFile("stats.flowmon", true, true);
  Simulator::Destroy ();
  NS_LOG_INFO ("Finished Simulation");

  return 0;
}

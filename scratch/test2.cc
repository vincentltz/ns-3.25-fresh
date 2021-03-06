/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Oregon State University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

//
// This program configures 10 random placed nodes on an 
// 802.11b physical layer, with
// 802.11b NICs in adhoc mode, and by default, sends one packet of 1000 
// (application) bytes to node 1.
//
//
// the layout is affected by the parameters given to RandomRectanglePositionAllocator;
// by default, the rectangle is 100m by 100m.
//
// There are a number of command-line options available to control
// the default behavior.  The list of available command-line options
// can be listed with the following command:
// ./waf --run "wifi-simple-adhoc-grid --help"
//
// Note that all ns-3 attributes (not just the ones exposed in the below
// script) can be changed at command line; see the ns-3 documentation.
//
// For instance, try running:
//
// ./waf --run "wifi-simple-adhoc --maxRange=250"
// ./waf --run "wifi-simple-adhoc --maxRange=50"
//
// The source node and sink node can be changed like this:
// 
// ./waf --run "wifi-simple-adhoc --sourceNode=20 --sinkNode=10"
//
// This script can also be helpful to put the Wifi layer into verbose
// logging mode; this command will turn on all wifi logging:
// 
// ./waf --run "wifi-simple-adhoc-grid --verbose=1"
//
// By default, trace file writing is off-- to enable it, try:
// ./waf --run "wifi-simple-adhoc-grid --tracing=1"
//
// When you are done tracing, you will notice many pcap trace files 
// in your directory.  If you have tcpdump installed, you can try this:
//
// tcpdump -r wifi-simple-adhoc-grid-0-0.pcap -nn -tt
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/energy-module.h"

//#include "ns3/olsr-helper.h"
//#include "ns3/ipv4-static-routing-helper.h"
//#include "ns3/ipv4-list-routing-helper.h"
//#include "ns3/aodv-module.h"
//#include "ns3/aodv-helper.h"
//#include "ns3/aodv-neighbor.h"
//#include "ns3/energy-source.h"
//#include "ns3/li-ion-energy-source-helper.h"

#include "ns3/gossip-generator.h"
#include "ns3/gossip-generator-helper.h"
#include "ns3/gossip.h"
#include "src/network/model/node.h"

#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/ipv4-global-routing-helper.h"

#include "ns3/log.h"
#include "src/applications/model/gossip-generator.h"
#include "src/applications/model/gossip.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("GossipProtocolTest");

struct neighbors {
  uint32_t sourceNode;
  std::vector<uint32_t> neighborNodes; 
};

class simstats {
    double time;
    int hops;
    double avgMsg;
  public:
    simstats(double, int, double);
    int getHops(void);
    double getTime(void);
    double getAvgMsgs(void);
}; 

simstats::simstats (double t, int h, double a) {
  time = t;
  hops = h;
  avgMsg = a;
}

int simstats::getHops(void){
  return hops;
}

double simstats::getTime(void){
  return time;
}

double simstats::getAvgMsgs(void){
  return avgMsg;
}

// C++ program to print DFS traversal from a given vertex in a  given graph
// Graph class represents a directed graph using adjacency list representation
class Graph
{
  int V;    // No. of vertices
  std::list<int> *adj;    // Pointer to an array containing adjacency lists
  std::vector<int> visitedList;
  void DFSUtil(int v, bool visited[]);  // A function used by DFS
public:
  Graph(int V);   // Constructor
  void addEdge(int v, int w);   // function to add an edge to graph
  void DFS(int v);    // DFS traversal of the vertices reachable from v
  int IsConnected();
};
 
Graph::Graph(int V)
{
  this->V = V;
  adj = new std::list<int>[V];
}
 
void Graph::addEdge(int v, int w)
{
  adj[v].push_back(w); // Add w to v’s list.
}
 
void Graph::DFSUtil(int v, bool visited[])
{
  // Mark the current node as visited and print it
  visited[v] = true;
  std::cout << v << " ";
  visitedList.push_back(v);

  // Recur for all the vertices adjacent to this vertex
  std::list<int>::iterator i;
  for (i = adj[v].begin(); i != adj[v].end(); ++i)
      if (!visited[*i])
          DFSUtil(*i, visited);
}

int Graph::IsConnected () {
  if (visitedList.size() == (unsigned int)V) {
    std::cout << "This is a connected graph." << std::endl;
    return 1;
  }
  else {
    std::cout << "This is NOT a connected graph." << std::endl;
    return 0;
  }
}

// DFS traversal of the vertices reachable from v. It uses recursive DFSUtil()
void Graph::DFS(int v)
{
  // Mark all the vertices as not visited
  bool *visited = new bool[V];
  for (int i = 0; i < V; i++)
      visited[i] = false;

  // Call the recursive helper function to print DFS traversal
  DFSUtil(v, visited);
  std::cout << std::endl; // end the depth first traversal line
}

//void ReceivePacket (Ptr<Socket> socket)
//{
//  while (socket->Recv ())
//    {
//      NS_LOG_UNCOND ("Received one packet!");
//    }
//}
//
//static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, 
//                             uint32_t pktCount, Time pktInterval )
//{
//  if (pktCount > 0)
//    {
//      socket->Send (Create<Packet> (pktSize));
//      Simulator::Schedule (pktInterval, &GenerateTraffic, 
//                           socket, pktSize,pktCount-1, pktInterval);
//    }
//  else
//    {
//      socket->Close ();
//    }
//}

Ipv4Address
GetIpv4 (Ptr<Node> node, uint32_t index) {
  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> (); // Get Ipv4 instance of the node
  Ipv4Address addr = ipv4->GetAddress (index, 0).GetLocal (); // Get Ipv4InterfaceAddress of xth interface.
  
  return addr;
}

//void
//GeneratePackets (Ptr<Gossip> gossip, uint32_t pktNum, NodeContainer wifiNodes, NodeContainer sourceNode) 
//{ 
//  gossip->SetCurrentValue( 2 );
//  gossip->SetSequenceNumber(pktNum);
//  
//  Ipv4Address srcAddr, destAddr;
//  srcAddr = GetIpv4(sourceNode.Get(1), 1);
////  destAddr = GetIpv4(wifiNodes.Get(0), 2);
//  destAddr = GetIpv4(sourceNode.Get(0), 2);
//  
//  std::cout << "srcAddr " << srcAddr << std::endl;
//  std::cout << "destAddr " << destAddr << std::endl;
//  
//  gossip->SendPayload(srcAddr, destAddr);
//  cout << "GeneratePackets Function! pktNum " <<  pktNum << endl;
//  
//  Simulator::Schedule (Seconds(20.0), &GeneratePackets, gossip, pktNum+1, wifiNodes, sourceNode);
//}

/// Trace function for remaining energy at node.
void
RemainingEnergy (std::string context, double oldValue, double remainingEnergy)
{
//  NS_LOG_UNCOND ("Context:" << context << " --- " << Simulator::Now ().GetSeconds ()
//                 << "s Current remaining energy = " << remainingEnergy << "J");
  if (remainingEnergy <= 10.8) {
    NS_LOG_UNCOND ("Context:" << context << " --- " << Simulator::Now ().GetSeconds ()
                 << "s Current remaining energy = " << remainingEnergy << "J");
//    std::cout << "Stopped simulation at " << Simulator::Now().GetSeconds() << std::endl;
    
    Simulator::Stop();
  }
  else {
    // Do nothing...
  }
}

/// Trace function for total energy consumption at node.
void
TotalEnergy (double oldValue, double totalEnergy)
{
  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
                 << "s Total energy consumed by radio = " << totalEnergy << "J");  
}

//void 
//EnergyFraction(double oldValue, double energyFraction) {
//  //Ptr<BasicEnergySource> energySource = DynamicCast<BasicEnergySource> (sources.Get(0));
//  //std::cout << "energy fraction is " << energySource->GetEnergyFraction() << std::endl;
//  std::cout << "here" << std::endl;
//  NS_LOG_UNCOND (Simulator::Now ().GetSeconds ()
//                 << "s Energy Fraction = " << energyFraction << "J");  
//}

neighbors 
makeNeighbors (Ptr<Node> nodeA, std::vector<uint32_t> nbNodes)
{
  neighbors ret;
  ret.sourceNode = nodeA->GetId();
  ret.neighborNodes = nbNodes;
  return ret;
}

std::vector<neighbors> 
getNeighbors (NodeContainer c, double maxRange) {
  std::vector<neighbors> neighborList;

  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i) 
  { 
      Ptr<Node> nodeA = *i;
      Ptr<MobilityModel> mobA = nodeA->GetObject<MobilityModel> (); 
      if (! mobA) continue; // Strange -- node has no mobility model installed. Skip. 
      Vector posA = mobA->GetPosition (); 

      std::vector<uint32_t> nbNodes;
      for (NodeContainer::Iterator j = c.Begin(); j != c.End(); ++j) 
      {
        Ptr<Node> nodeB =*j; 
        Ptr<MobilityModel> mobB = nodeB->GetObject<MobilityModel> (); 
        if (! mobB) continue;
        Vector posB = mobB->GetPosition ();

        double distance = sqrt(pow((posB.x - posA.x), 2.0) + pow((posB.y - posA.y), 2.0));
        
        if (distance == 0.0 || distance > maxRange)
          continue;
        else if (distance <= maxRange) {
          nbNodes.push_back (nodeB->GetId());
        }
      }
      neighborList.push_back (makeNeighbors (nodeA, nbNodes));
  } 
  return neighborList; 
}

void 
printNeighborList (std::vector<neighbors> neighborList) {
  for (std::vector<neighbors>::iterator nb = neighborList.begin(); nb != neighborList.end(); ++nb) {
    std::cout << "Node " << nb->sourceNode << " 's neighbors are ";
    
    for (unsigned int i = 0; i < nb->neighborNodes.size(); i++){
      std::cout << nb->neighborNodes[i] << " ";
    }
    std::cout << std::endl;
  } 
}

// calculate fanout 
// min(fanout, num of neighbors)
// fraction of energy left 80% 0.8

// based on the fanout, randomly pick a subset from neighbors list and send the packet.
// then check when all nodes received the new packet.
// check how much energy remaining in the energy source after broadcast one packet.
// check how many packets being sent for broadcasting one packet.

//int 
//calFanout (double remainingEnergyPercentage, std::vector<neighbors> neighborList, int nodeIndex) {
//  int fanout = 0;
//
//  if (remainingEnergyPercentage >= 0.8)
//    fanout = 5;
//  else if (remainingEnergyPercentage >= 0.6)
//    fanout = 4;
//  else if (remainingEnergyPercentage >= 0.4)
//    fanout = 3;
//  else if (remainingEnergyPercentage >= 0.2)
//    fanout = 2;
//  else
//    fanout = 1;
//
//  std::cout << "Here!!!!" << std::endl;
//  std::cout << "Initial fanout was " << fanout << std::endl;
//  std::cout << "node " << nodeIndex << "'s neighborNode size is " << neighborList.at(nodeIndex).neighborNodes.size() << std::endl;
//  std::cout << "fanout is " << std::min(fanout, (int)neighborList.at(nodeIndex).neighborNodes.size()) << std::endl;
//  return std::min(fanout, (int)neighborList.at(nodeIndex).neighborNodes.size());
//}

//int
//updateFanout (EnergySourceContainer sources, std::vector<neighbors> neighborList, int nodeIndex) {
//  Ptr<BasicEnergySource> energySource = DynamicCast<BasicEnergySource> (sources.Get(nodeIndex));
//  std::cout << "energy fraction of node " << nodeIndex << " is " << energySource->GetEnergyFraction() << std::endl;
//  double frac = energySource->GetEnergyFraction();
//  int ret = calFanout (frac, neighborList, nodeIndex);
//  std::cout << "Now the fanout is " << ret << std::endl;
//  return ret;
//}
//
//void
//updateEnergyFraction (EnergySourceContainer sources, uint32_t numNodes) {
//  Simulator::Schedule (Seconds(2), &updateEnergyFraction, sources, numNodes);
////  std::cout << "energy fraction is " << sources.Get(0)->GetEnergyFraction() << std::endl;
//  
//  for(uint32_t i = 0; i < numNodes; i++) {
//    std::cout << "Energy Fraction of Node " << i << " is " << sources.Get(i)->GetEnergyFraction() << "\n";
//    
//  }
//
//}

//Ptr<GossipUdpClient> 
//GetUdpClientApp(Ptr <Node> node)
//{
//  Ptr< Application > udpClientApp = node->GetApplication (1) ;
//  return DynamicCast<GossipUdpClient>(udpClientApp);
//}
//
//Ptr<GossipUdpServer> 
//GetUdpServerApp(Ptr <Node> node)
//{
//  Ptr< Application > udpServerApp = node->GetApplication (1) ;
//  return DynamicCast<GossipUdpServer>(udpServerApp);
//}


Ptr<GossipGenerator> 
GetGossipApp(Ptr <Node> node)
{
  Ptr< Application > gossipApp = node->GetApplication (0) ;
  return DynamicCast<GossipGenerator>(gossipApp);
}

Ptr<Gossip> 
GetGossip(Ptr <Node> node)
{
  Ptr< Application > gossip = node->GetApplication (0) ;
  return DynamicCast<Gossip>(gossip);
}

void
PrintNodePositionAndAddress (NodeContainer wifiNodes) {
  // print out every nodes' position
  for (NodeContainer::Iterator i = wifiNodes.Begin (); i != wifiNodes.End (); ++i) 
  { 
    Ptr<Node> node = *i; 
//      std::string name = Names::FindName (node); // Assume that nodes are named, remove this line otherwise 
    Ptr<MobilityModel> mob = node->GetObject<MobilityModel> (); 
    if (! mob) continue; // Strange -- node has no mobility model installed. Skip. 
    Vector pos = mob->GetPosition (); 

    Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1,0);
    Ipv4Address addri = iaddr.GetLocal();

    std::cout << "Node " << node->GetId() << " is at (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n"; 
    std::cout << "Node " << node->GetId() << "'s IP address is " << addri << "\n";
  } 
}

void
PrintDoubleVector (std::vector<double> vec) {
  std::cout << "Vector size is " << vec.size() << "\n";
  for (uint32_t i = 0; i < vec.size(); i++) {
    std::cout << vec[i] << " "; 
  }
}

std::vector<double>
calBroadcastTime (std::vector<double> txVec, std::vector<double> rxVec) {
  std::vector<double> broadcastTime;
  bool sameSize;
  
  if (txVec.size() == rxVec.size()) {
    sameSize = true;
  }
  else {
    sameSize = false;
  }
  
  if (sameSize) {
    for (uint32_t i = 0; i < rxVec.size(); i++) {
      double diff = rxVec[i] - txVec[i];
      broadcastTime.push_back(diff);
    }
  }
  else {
//    std::cout << "sendPktTime and rxDataTime vectors are NOT in same size." << std::endl;
//    std::cout << "Will pick the smaller size to compute." << std::endl;
//    std::cout << "rxDataTime size: " << rxVec.size() << " sendPktTime size: " << txVec.size() << std::endl;
    
    for (uint32_t i = 0; i < rxVec.size(); i++){
      double diff = rxVec[i] - txVec[i];
      broadcastTime.push_back(diff);
    }
  }
  return broadcastTime;
}

static inline std::string
PrintID (Ptr<Node> n) {
    uint32_t id = n->GetId();
    std::ostringstream oss;
    oss << "ID: " << id;

    return oss.str ();
}


/* 
  function converts std::string types into char * types
  @param string
  @return char *
*/
char* convertStrToChar(std::string str){
  char *newChars = new char[str.size()+1];
  newChars[str.size()]='\0';
  memcpy(newChars,str.c_str(),str.size());
  return newChars;
}

int 
main (int argc, char *argv[])
{
//  NS_LOG_FUNCTION(this);
  std::string sourcePhyMode ("DsssRate11Mbps");
  std::string phyMode ("DsssRate1Mbps");
  bool verbose = false;
  bool tracing = false;
  double maxRange = 50;
  uint32_t numNodes = 10;  // !!!BUG!!!, any number less than 10 will result in a memory violation.
  double simulationTime = 100000.0; // seconds
  double initialEnergy = 108.0; // J  // 500mAh = 5400J  100mAh = 1080J
  
  Time GossipInterval = Seconds(1.0); // Must be larger than the round-trip-time! (c.f. LinkDelay)
  Time SolicitInterval = Seconds(5.0); //not planning on using this attribute
  Time udpClientStartTime = Seconds(2.0);
  
//  uint32_t packetSize = 1000; // bytes
//  uint32_t numPackets = 5;
//  uint32_t sinkNode = 0;
//  uint32_t sourceNode = 2;
//  double interval = 30.0; // seconds
//  std::string transportProt = "Tcp";
//  std::string socketType = "ns3::TcpSocketFactory";
  
  std::string outBrTimeFile = "brTime.txt";
  std::string outLifeSpanFile = "lifeSpan.txt";
  std::string outOverheadFile = "overhead.txt";
  std::string outConsumedEnergyFile = "consumedEnergy.txt";
  
  
  CommandLine cmd;
  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("numNodes", "number of nodes", numNodes);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  cmd.AddValue ("tracing", "turn on ascii and pcap tracing", tracing);
  cmd.AddValue ("maxRange", "Maximum Wifi Range", maxRange);
  cmd.AddValue("outBrTimeFile", "Filename that broadcast time are written out to", outBrTimeFile);
  cmd.AddValue("outLifeSpanFile", "Filename that simulation stop time are written out to", outLifeSpanFile);
  cmd.AddValue("outOverheadFile", "Filename that total overhead are written out to", outOverheadFile);
  cmd.AddValue("outConsumedEnergyFile", "Filename that total consumed energy are written out to", outConsumedEnergyFile);

//  cmd.AddValue("transportProt", "Transport protocol to use:Tcp, Udp", transportProt);
//  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);
//  cmd.AddValue ("numPackets", "number of packets generated", numPackets);
//  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  //cmd.AddValue ("sinkNode", "Receiver node number", sinkNode);
  //cmd.AddValue ("sourceNode", "Sender node number", sourceNode);

  cmd.Parse (argc, argv);
  
//  if (transportProt.compare("Tcp") == 0) {
//    socketType = "ns3::TcpSocketFactory";
//  }
//  else {
//    socketType = "ns3::UdpSocketFactory";
//  }
  
  // Convert to time object
//  Time interPacketInterval = Seconds (interval);

  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
                      StringValue (phyMode));

  NodeContainer wifiNodes; 
  wifiNodes.Create (numNodes);
  
  NodeContainer sourceNodes;
  sourceNodes.Add(wifiNodes);   //  sourceNodes.Add(wifiNodes.Get(0));
  sourceNodes.Create(1);
  
  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  WifiHelper sourceWifi;
  
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
      sourceWifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiPhyHelper sourceWifiPhy =  YansWifiPhyHelper::Default ();

  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (-10) ); 
  sourceWifiPhy.Set("RxGain", DoubleValue (-10));
  
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 
  sourceWifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  YansWifiChannelHelper wifiChannel, sourceWifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  //wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel",
                                  "MaxRange", DoubleValue (maxRange));
  wifiPhy.SetChannel (wifiChannel.Create ());
  
  sourceWifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  //wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  sourceWifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel",
                                  "MaxRange", DoubleValue (500.0));
  sourceWifiPhy.SetChannel (sourceWifiChannel.Create ());

  // Add an upper mac and disable rate control
  WifiMacHelper wifiMac;
  WifiMacHelper sourceWifiMac;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  sourceWifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  sourceWifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (sourcePhyMode),
                                      "ControlMode",StringValue (sourcePhyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  sourceWifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer wifiDevices = wifi.Install (wifiPhy, wifiMac, wifiNodes);
  NetDeviceContainer sourceWifiDevice = sourceWifi.Install (sourceWifiPhy, sourceWifiMac, sourceNodes);

  MobilityHelper mobility;
 
// calculate the rectangle x and y maximum range
  uint32_t max = (uint32_t)sqrt(1000*numNodes);
  std::stringstream ss;
  ss << "ns3::UniformRandomVariable[Min=0|Max=" << max << "]";
  
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                 "X", StringValue (ss.str()),
                                 "Y", StringValue (ss.str())); 
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiNodes);
  mobility.Install (sourceNodes.Get(numNodes)); // numNodes = 10, then 0-9, 10 is the single source node
  
//  MobilityHelper sourceMobility;
//  sourceMobility.SetPositionAllocator ("ns3::GridPositionAllocator",
//                                       "MinX", DoubleValue (0.0),
//                                       "MinY", DoubleValue (0.0),
//                                       "DeltaX", DoubleValue (5.0),
//                                       "DeltaY", DoubleValue (5.0),
//                                       "GridWidth", UintegerValue (5),
//                                       "LayoutType", StringValue ("RowFirst"));
//  sourceMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//  sourceMobility.Install (sourceNodes.Get(1));

  // install energy source and energy model on all wifi nodes
  BasicEnergySourceHelper basicSourceHelper;
  basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue(initialEnergy));   
  EnergySourceContainer sources = basicSourceHelper.Install (wifiNodes);
  WifiRadioEnergyModelHelper radioEnergyHelper;
  //radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
  
  radioEnergyHelper.Set("IdleCurrentA", DoubleValue(0.0));
  radioEnergyHelper.Set("SleepCurrentA", DoubleValue(0.0));
  radioEnergyHelper.Set("TxCurrentA", DoubleValue(0.38));
  radioEnergyHelper.Set("RxCurrentA", DoubleValue(0.313));

  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install (wifiDevices, sources);

  //Ptr<BasicEnergySource> energySource = DynamicCast<BasicEnergySource> (sources.Get(0));
  //std::cout << "energy fraction is " << energySource->GetEnergyFraction() << std::endl;
  
  
  // Install Internet stack on every node
  InternetStackHelper stack;
  stack.Install (wifiNodes);
  stack.Install(sourceNodes.Get(numNodes));
//  internet.Install(p2pNodes.Get(0));

  Ipv4AddressHelper address;
  NS_LOG_INFO ("Assign IP Addresses.");
  
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInter;
  wifiInter = address.Assign (wifiDevices);
  
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer sourceInter;
  sourceInter = address.Assign (sourceWifiDevice);
  
//   for (unsigned int i = 0; i < numNodes; i++) {  
//    std::cout << sources.Get(i)->GetNode()->GetId() << " && " << sources.Get(i)->GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal() << std::endl;
//  }
  
  GossipGeneratorHelper ggh ;
  ApplicationContainer nodeApps;   
  nodeApps = ggh.Install(wifiNodes);
  
  GossipHelper gh;
  ApplicationContainer app;
  app = gh.Install(sourceNodes.Get(numNodes));
  
  Address serverAddress = GetIpv4(sourceNodes.Get(numNodes), 1);
  uint16_t port = 4000;
  GossipUdpServerHelper server (port);
  ApplicationContainer udpServerApps = server.Install (sourceNodes.Get (numNodes));
  udpServerApps.Start (Seconds (0.0));
  udpServerApps.Stop (Seconds (simulationTime));
  
  Ptr<Application> AppPtr = udpServerApps.Get(0);
  GetGossip(sourceNodes.Get(numNodes))->SetUdpServer(DynamicCast<GossipUdpServer>(AppPtr));
   
  GossipUdpClientHelper client (serverAddress, port);
  ApplicationContainer udpClientApps = client.Install(wifiNodes);
  udpClientApps.Start (udpClientStartTime);
  udpClientApps.Stop (Seconds (simulationTime));
  
  for (unsigned int i = 0; i < numNodes; i++) {
    Ptr<Application> app = udpClientApps.Get(i);
    GetGossipApp(wifiNodes.Get(i))->GetUdpClient(DynamicCast<GossipUdpClient>(app));
  }
  
  
  //GetGossipApp(c.Get(0))->AddNeighbor(i.GetAddress(0), i.GetAddress(1));
//  GetGossipApp(nodes2.Get(Edge1))->AddNeighbor(InterfaceCont.GetAddress(0),InterfaceCont.GetAddress(1));

  PrintNodePositionAndAddress (wifiNodes);
//  PrintNodePositionAndAddress (sourceNodes);
  
//  Ptr<Node> node = sourceNodes.Get(0);
//  Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
//  Ipv4InterfaceAddress iaddr = ipv4->GetAddress(2,0);
//  Ipv4Address addri = iaddr.GetLocal();

//    std::cout << "Node " << node->GetId() << " is at (" << pos.x << ", " << pos.y << ", " << pos.z << ")\n"; 
//  std::cout << "Node " << node->GetId() << "'s IP address is " << addri << "\n";

  std::vector<neighbors> neighborList;
  neighborList = getNeighbors (wifiNodes, maxRange);
  printNeighborList (neighborList);
  
  Graph topology(numNodes);
  for (int i = 0; i < (int)numNodes; i++) {
    for (unsigned int j = 0; j < neighborList[i].neighborNodes.size(); j++){
      topology.addEdge(i, neighborList[i].neighborNodes[j]);
    }
  }
 
  std::cout << "Following is Depth First Traversal (starting from vertex(node) 0)" << std::endl;
  topology.DFS(0);
  
  int isConnected = topology.IsConnected();
  if (isConnected == 1)
    std::cout << "Connected" << std::endl;
  else {
    std::cout << "NOT Connected" << std::endl;
    return 0;
  }
    
  // for each node, add their neighbors
  for (unsigned int j = 0; j < neighborList.size(); j++){
//    std::cout << "Working on node " << neighborList[j].sourceNode << std::endl;
    for (unsigned int i = 0; i < neighborList[j].neighborNodes.size(); i++) {
      GetGossipApp(wifiNodes.Get(neighborList[j].sourceNode))->AddNeighbor(wifiInter.GetAddress(neighborList[j].sourceNode), wifiInter.GetAddress(neighborList[j].neighborNodes[i]));
//      std::cout << "The neighbor nodes are " << neighborList[j].neighborNodes[i] << std::endl;
    }
  }
  
//  std::cout << "Test neighbor list stored in gossip protocol!" << std::endl;
//  for (unsigned int i = 0; i < GetGossipApp(c.Get(0))->GetNeighbours().size(); i++){
//    std::cout << GetGossipApp(c.Get(0))->GetNeighbours()[i] << std::endl; 
//  }
  
  // Passing energy source pointer to each node's gossip protocol
  for (unsigned int i = 0; i < numNodes; i++) {
    GetGossipApp(wifiNodes.Get(i))->GetEnergySource(sources.Get(i));
  }
  
//  GetGossipApp(c.Get(9))->calFanout();
//  for (int i = 0; i < 10; i++){
//    GetGossipApp(c.Get(1))->ChooseNeighbors();
//  }
  
  for ( uint32_t i = 0; i < numNodes; ++i)
  { //TODO use attributes
    Ptr<GossipGenerator> ii = GetGossipApp(wifiNodes.Get(i));
    ii->SetGossipInterval(GossipInterval);
    ii->SetSolicitInterval(SolicitInterval);
  }

//  Ptr<GossipGenerator> a = GetGossipApp(wifiNodes.Get(0));
  Ptr<Gossip> a = GetGossip(sourceNodes.Get(numNodes));
  a->SetSourceNode(sourceNodes);
  a->SetNumberOfNodes(numNodes);
  
//  a->SetCurrentValue( 2 );

//  GeneratePackets(a, 1, wifiNodes, sourceNode);
  
//  a->SetCurrentValue( 2 );
//  a->SetSequenceNumber(1);

/** connect trace sources **/
  /***************************************************************************/
  // all sources are connected to node 1
  // energy source

  for (uint32_t i = 0; i < numNodes; i++) {
    Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (sources.Get (i));
//  basicSourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&RemainingEnergy));
  
    basicSourcePtr->TraceConnect ("RemainingEnergy",PrintID(sources.Get(i)->GetNode()), MakeCallback (&RemainingEnergy));
  }

  
//  Config::Connect ("/NodeList/0/$ns3::BasicEnergySource/RemainingEnergy",
//                     MakeCallback (&RemainingEnergy));
  
          
  // device energy model
//  Ptr<DeviceEnergyModel> basicRadioModelPtr =
//    basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
//  NS_ASSERT (basicRadioModelPtr != NULL);
//  basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));

/*
  Ptr<BasicEnergySource> energySourcePtr = DynamicCast<BasicEnergySource> (sources.Get(1));
  NS_ASSERT (energySourcePtr != NULL);  
  energySourcePtr->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&EnergyFraction));
 */ 
/***************************************************************************/

  if (tracing == true)
    {
      AsciiTraceHelper ascii;
      wifiPhy.EnableAsciiAll (ascii.CreateFileStream ("wifi-simple-adhoc-grid.tr"));
      wifiPhy.EnablePcap ("wifi-simple-adhoc-grid", wifiDevices);
      // Trace routing tables
/*
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("wifi-simple-adhoc-grid.routes", std::ios::out);
      olsr.PrintRoutingTableAllEvery (Seconds (2), routingStream);
      Ptr<OutputStreamWrapper> neighborStream = Create<OutputStreamWrapper> ("wifi-simple-adhoc-grid.neighbors", std::ios::out);
      olsr.PrintNeighborCacheAllEvery (Seconds (2), neighborStream);
*/
      // To do-- enable an IP-level trace that shows forwarding events only
    }

  // Give OLSR time to converge-- 30 seconds perhaps

  //Time update_delta_t = Seconds(0.5);
//  Simulator::Schedule (Seconds (1.0), &GenerateTraffic, source, packetSize, numPackets, interPacketInterval);
  //Simulator::Schedule (Seconds(5.0), &updateFanout, sources, neighborList, 1);
  //Simulator::Schedule (update_delta_t, &calFanout, 0.7, neighborList, 1);
  //Simulator::Schedule (Seconds (10.0), &printNeighbors, neighbor);

  //Ptr<BasicEnergySource> energySource = DynamicCast<BasicEnergySource> (sources.Get(0));
  //std::cout << "energy fraction is " << energySource->GetEnergyFraction() << std::endl;
  
//  std::cout << "energy fraction is " << sources.Get(0)->GetEnergyFraction() << std::endl;
  
  //Simulator::Schedule (Seconds(0.5), &(sources.Get(0)->GetEnergyFraction()));

  // Output what we are doing
  //NS_LOG_UNCOND ("Testing from node " << sourceNode << " to " << sinkNode << " with RandomRectanglePositionAllocator 100 by 100");

  //Simulator::Stop (Seconds (30.0));
//  Simulator::Schedule (Seconds(1.0), &GeneratePackets, a, 1, wifiNodes, sourceNodes);  
  
//  Simulator::Schedule (Seconds(2), &updateEnergyFraction, sources, numNodes);

  
//    std::cout << "energy fraction is " << sources.Get(0)->GetEnergyFraction() << std::endl;


  Simulator::Stop(Seconds (simulationTime + 0.1));
  Simulator::Run ();
  
  double stopTime = Simulator::Now().GetSeconds();
  
  // When one node's energy source fraction is lower than 0.02, store the time and stop simulation.
  
//  std::cout << "Simulation Stopped!!!\n"; 
//  std::cout << "Stop time is " << stopTime << "\n";
  
    //  outLifeSpanFile
  char *newLifeSpanFile = convertStrToChar(outLifeSpanFile);
  
  FILE *lifeSpanFile;
  lifeSpanFile = fopen(newLifeSpanFile, "a+");

  if (lifeSpanFile != NULL){
    fprintf(lifeSpanFile,"%f\n", stopTime);  
  }
  fclose(lifeSpanFile);
  
  
  NS_LOG_INFO(endl << " ---- Print results ---" << endl);
  int MaxHops = 0;
  Time MaxTime = Seconds(0);
  double AvgMessagesPerNode = 0;
  for ( int i=0; i< (int)numNodes;++i)
  {
    Ptr<GossipGenerator> ii = GetGossipApp(wifiNodes.Get(i));
    if (MaxHops < ii->GetPacketHops()){
      MaxHops = ii->GetPacketHops();
    }
    if (MaxTime.Compare(ii->GetReceivedDataTime()) == -1){
      MaxTime = ii->GetReceivedDataTime();
    }
    AvgMessagesPerNode += ii->GetSentMessages();
    NS_LOG_INFO("Node " << i << ": ");
    NS_LOG_INFO(" * Sent icmp messages   : " << ii->GetSentMessages());
    NS_LOG_INFO(" * Hops of data message : " << ii->GetPacketHops());
    NS_LOG_INFO(" * Time of data received: " << ii->GetReceivedDataTime().GetSeconds() << "s");
  }
  AvgMessagesPerNode /= numNodes;
  NS_LOG_INFO(endl << "Simulation terminated after " << Simulator::Now().GetSeconds() << "s");
  NS_LOG_INFO("Max hops: " << MaxHops);
  NS_LOG_INFO("Average amount of sent messages per node: " << AvgMessagesPerNode);
  NS_LOG_INFO("Time until information was spread: " << MaxTime.GetSeconds() << "s" << endl);
  simstats ret(MaxTime.GetSeconds(),MaxHops,AvgMessagesPerNode);
  
  // Source node packets sent time vector
  std::vector<double> sentPktTime;
  sentPktTime = a->GetSentPktTime();
  
//  std::cout << "For source node, the sending time is \n";
//  PrintDoubleVector(sentPktTime);
//  std::cout << "\n";
  
  for (uint32_t i = 0; i < numNodes; i++) 
  {
    std::vector<double> broadcastTime;
    Ptr<GossipGenerator> gossipApp = GetGossipApp(wifiNodes.Get(i));
    std::vector<double> rxDataTime;
    rxDataTime = gossipApp->GetRxDataTime();
    
    std::vector<uint16_t> rxPktStore;
    rxPktStore = gossipApp->GetRxPktStore();
//    std::cout << "For Wifi Node " << i << ", the receiving pkts are \n";
//    for(uint32_t j = 0; j < rxPktStore.size(); j++) {
//      std::cout << rxPktStore[j] << " ";
//    }
//    std::cout << "\n";
    
    
    broadcastTime = calBroadcastTime(sentPktTime, rxDataTime);
    
//    std::cout << "For Wifi Node " << i << ", the receiving time is \n";
//    PrintDoubleVector(broadcastTime);
//    std::cout << "\n";
//    std::cout << "For Wifi Node " << i << ", the receiving time is \n";
//    PrintDoubleVector(rxDataTime);
//    std::cout << "\n";
  }
  
//  std::vector<double> broadcastTime;
//  Ptr<GossipGenerator> gossipApp = GetGossipApp(wifiNodes.Get(0));
//  std::vector<double> rxDataTime = gossipApp->GetRxDataTime();
//  
//  broadcastTime = calBroadcastTime(sentPktTime, rxDataTime);
//  
//  std::cout << "For Node 0, the broadcast time is \n";
//  PrintDoubleVector(broadcastTime);
//  std::cout << "\n";
  
  
  std::vector<double> brTime;
  
  // find out the smallest rxDataTime vector size
  uint32_t minSize;
  Ptr<GossipGenerator> gossipApp = GetGossipApp(wifiNodes.Get(0));
  std::vector<double> rxDataTime = gossipApp->GetRxDataTime();
  minSize = rxDataTime.size();
  
  for(uint32_t i = 1; i < numNodes; i++) {
    Ptr<GossipGenerator> gossipApp = GetGossipApp(wifiNodes.Get(i));
    std::vector<double> rxDataTime = gossipApp->GetRxDataTime();
    minSize = std::min<uint32_t>(minSize, rxDataTime.size()); 
  }
  
  
  for (uint32_t i = 0; i < minSize; i++) {
    double maxTime = 0.0;
    for (uint32_t k = 0; k < numNodes; k++) {
      Ptr<GossipGenerator> gossipApp = GetGossipApp(wifiNodes.Get(k));
      std::vector<double> rxDataTime = gossipApp->GetRxDataTime();
      double diff = rxDataTime[i] - sentPktTime[i];
      
      maxTime = std::max(diff, maxTime);
    }
    brTime.push_back(maxTime);
  }
  
  char *newTimeFile = convertStrToChar(outBrTimeFile);
  
  FILE *brTimeFile;
  brTimeFile = fopen(newTimeFile, "a+");

  if (brTimeFile != NULL){
    for (uint32_t i = 0; i < brTime.size(); i++){
      fprintf(brTimeFile,"%f\n", brTime[i]);
    }  
  }
  fclose(brTimeFile);

//  ofstream myfile ("brTime.txt");
//  if(myfile.is_open() == true) {
//    for (uint32_t i = 0; i < brTime.size(); i++){
//      myfile << (i+1) << " " << brTime[i] << "\n";
//    }
////    myfile << "Writing this to a file.\n";
////    myfile << "Writing another line to this file.\n";
//    myfile.close();
//  }
//  else {
//    std::cout << "Unable to open file!";
//  }
  
  

  
  
//  std::cout << "The broadcast time for each data packet are: \n";
//  std::cout << "size of the vector is " << brTime.size() << "\n";
//  PrintDoubleVector(brTime);
//  std::cout << "\n";
  
//  std::vector<int> protocolOverhead;
  double totalOverhead = 0.0;
  
  for (uint32_t i = 0; i < numNodes; i++){
    Ptr<GossipGenerator> gossipApp2 = GetGossipApp(wifiNodes.Get(i));
    
    int sentMsg = gossipApp2->GetSentMessages();  // ack + solicit
    int sentPayload = gossipApp2->GetSentPayload();
//    int sentAck = gossipApp2->GetSentAck();
//    int sentSolicit = gossipApp2->GetSentSolicit();
    
//    std::cout << "For Node " << i << std::endl;
//    std::cout << "The sent acks are " << sentAck << std::endl;
//    std::cout << "The sent solicits are " << sentSolicit << std::endl;
//    std::cout << "The sent msgs are " << sentMsg << std::endl;
//
//    std::cout << "The sent payload are " << sentPayload << std::endl;
//    std::cout << "sentMsg " << sentMsg << std::endl;
//    std::cout << "sentPayload " << sentPayload << std::endl;
    
    std::vector<double> rxDataTime = gossipApp2->GetRxDataTime();
    uint32_t rxPktNum = rxDataTime.size();
    double overhead = (sentMsg + sentPayload) / (double)rxPktNum;
//    std::cout << "overhead " << overhead << std::endl;
    
    totalOverhead = totalOverhead + overhead;
    //double overhead = (double)sentMsg/(sentPayload + sentMsg);
//    protocolOverhead.push_back(overhead);
    
//    std::cout << "The overhead for Wifi Node " << i << " is " << overhead << std::endl;
  }
  double avgOverhead = totalOverhead / (double)numNodes;
  
  
//  std::cout << "The protocol overhead for each node is " << std::endl;
//  PrintDoubleVector(protocolOverhead);
//  std::cout << "\n";
  
//  std::cout << "The average protocol overhead is " << std::endl;
//  double temp = 0;
//  for (uint32_t i = 0; i < protocolOverhead.size(); i++) {
//    temp = temp + protocolOverhead[i];
//  }
//  double avg = temp/(double)protocolOverhead.size();
 // std::cout << avgOverhead << "\n";
  
 

//  outConsumedEnergyFile
  char *newOverheadFile = convertStrToChar(outOverheadFile);
  
  FILE *overheadFile;
  overheadFile = fopen(newOverheadFile, "a+");

  if (overheadFile != NULL){
    fprintf(overheadFile,"%f\n", avgOverhead);  
  }
  fclose(overheadFile);
  
  
  double totalEnergy = 0.0;
  for (uint32_t i = 0; i < numNodes; i++) {
    Ptr<GossipGenerator> gossipApp2 = GetGossipApp(wifiNodes.Get(i));
    std::vector<double> rxDataTime = gossipApp2->GetRxDataTime();
    uint32_t rxPktNum = rxDataTime.size();

    Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource>(sources.Get(i));
    Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
    NS_ASSERT (basicRadioModelPtr != NULL);
    
//    std::cout << "Total energy consumption of node " << i << " is " << basicRadioModelPtr->GetTotalEnergyConsumption() << " J" << std::endl;
    double energyConsumption = basicRadioModelPtr->GetTotalEnergyConsumption();
    double energyConsumptionPerPkt = energyConsumption / rxPktNum;
//    std::cout << energyConsumption << " " << energyConsumptionPerPkt << std::endl;
    totalEnergy = totalEnergy + energyConsumptionPerPkt;
  }
//  std::cout << "Total energy consumption was " << totalEnergy << std::endl;
  double avgEnergyConsumptionPerPkt = totalEnergy / numNodes;
//  std::cout << "Average energy consumption per pkt was " << avgEnergyConsumptionPerPkt << std::endl;
  
  //  outConsumedEnergyFile
  char *newConsumedEnergyFile = convertStrToChar(outConsumedEnergyFile);
  
  FILE *consumedEnergyFile;
  consumedEnergyFile = fopen(newConsumedEnergyFile, "a+");

  if (consumedEnergyFile != NULL){
    fprintf(consumedEnergyFile,"%f\n", avgEnergyConsumptionPerPkt);  
  }
  fclose(consumedEnergyFile);

  
  
//  Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource>(sources.Get(0));
//  Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels("ns3::WifiRadioEnergyModel").Get(0);
//  NS_ASSERT (basicRadioModelPtr != NULL);
//  std::cout << "Total energy consumption of node " << i << " is " << basicRadioModelPtr->GetTotalEnergyConsumption() << " J" << std::endl;

  
  Simulator::Destroy ();
  return 0;
}


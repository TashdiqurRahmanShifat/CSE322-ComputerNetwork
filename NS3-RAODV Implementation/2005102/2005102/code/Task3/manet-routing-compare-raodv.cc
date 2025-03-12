/*
 * Copyright (c) 2011 University of Kansas
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Justin Rohrer <rohrej@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  https://resilinets.org/
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

/*
 * This example program allows one to run ns-3 DSDV, AODV, or OLSR under
 * a typical random waypoint mobility model.
 *
 * By default, the simulation runs for 200 simulated seconds, of which
 * the first 50 are used for start-up time.  The number of nodes is 50.
 * Nodes move according to RandomWaypointMobilityModel with a speed of
 * 20 m/s and no pause time within a 300x1500 m region.  The WiFi is
 * in ad hoc mode with a 2 Mb/s rate (802.11b) and a Friis loss model.
 * The transmit power is set to 7.5 dBm.
 *
 * It is possible to change the mobility and density of the network by
 * directly modifying the speed and the number of nodes.  It is also
 * possible to change the characteristics of the network by changing
 * the transmit power (as power increases, the impact of mobility
 * decreases and the effective density increases).
 *
 * By default, OLSR is used, but specifying a value of 2 for the protocol
 * will cause AODV to be used, and specifying a value of 3 will cause
 * DSDV to be used.
 *
 * By default, there are 10 source/sink data pairs sending UDP data
 * at an application rate of 2.048 Kb/s each.    This is typically done
 * at a rate of 4 64-byte packets per second.  Application data is
 * started at a random time between 50 and 51 seconds and continues
 * to the end of the simulation.
 *
 * The program outputs a few items:
 * - packet receptions are notified to stdout such as:
 *   <timestamp> <node-id> received one packet from <src-address>
 * - each second, the data reception statistics are tabulated and output
 *   to a comma-separated value (csv) file
 * - some tracing and flow monitor configuration that used to work is
 *   left commented inline in the program
 */

#include "ns3/raodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/olsr-module.h"
#include "ns3/yans-wifi-helper.h"

#include <fstream>
#include <iostream>

using namespace ns3;
using namespace dsr;

NS_LOG_COMPONENT_DEFINE("manet-routing-compare");

/**
 * Routing experiment class.
 *
 * It handles the creation and run of an experiment.
 */
class RoutingExperiment
{
  public:
    RoutingExperiment();
    /**
     * Run the experiment.
     */
    void Run();

    /**
     * Handles the command-line parameters.
     * \param argc The argument count.
     * \param argv The argument vector.
     */
    void CommandSetup(int argc, char** argv);

  private:
    /**
     * Setup the receiving socket in a Sink Node.
     * \param addr The address of the node.
     * \param node The node pointer.
     * \return the socket.
     */
    Ptr<Socket> SetupPacketReceive(Ipv4Address addr, Ptr<Node> node);
    /**
     * Receive a packet.
     * \param socket The receiving socket.
     */
    void ReceivePacket(Ptr<Socket> socket);
    /**
     * Compute the throughput.
     */
    void CheckThroughput();

    uint32_t port{9};            //!< Receiving port number.
    uint32_t bytesTotal{0};      //!< Total received bytes.
    uint32_t packetsReceived{0}; //!< Total received packets.

    std::string m_CSVfileName{"manet-routing.output.csv"}; //!< CSV filename.
    int m_nSinks{10};                                      //!< Number of sink nodes.
    std::string m_protocolName{"RAODVh"};                    //!< Protocol name.
    double m_txp{7.5};                                     //!< Tx power.
    bool m_traceMobility{false};                           //!< Enable mobility tracing.
    bool m_flowMonitor{false};                             //!< Enable FlowMonitor.
    int numNode{0};
    int packetRate{0};
    int speed{0};
};

RoutingExperiment::RoutingExperiment()
{
}

static inline std::string
PrintReceivedPacket(Ptr<Socket> socket, Ptr<Packet> packet, Address senderAddress)
{
    std::ostringstream oss;

    oss << Simulator::Now().GetSeconds() << " " << socket->GetNode()->GetId();

    if (InetSocketAddress::IsMatchingType(senderAddress))
    {
        InetSocketAddress addr = InetSocketAddress::ConvertFrom(senderAddress);
        oss << " received one packet from " << addr.GetIpv4();
    }
    else
    {
        oss << " received one packet!";
    }
    return oss.str();
}

void
RoutingExperiment::ReceivePacket(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address senderAddress;
    while ((packet = socket->RecvFrom(senderAddress)))
    {
        bytesTotal += packet->GetSize();
        packetsReceived += 1;
        NS_LOG_UNCOND(PrintReceivedPacket(socket, packet, senderAddress));
    }
}

void
RoutingExperiment::CheckThroughput()
{
    double kbs = (bytesTotal * 8.0) / 1000;
    bytesTotal = 0;

    std::ofstream out(m_CSVfileName, std::ios::app);

    out << (Simulator::Now()).GetSeconds() << "," << kbs << "," << packetsReceived << ","
        << m_nSinks << "," << m_protocolName << "," << m_txp << "" << std::endl;

    out.close();
    packetsReceived = 0;
    Simulator::Schedule(Seconds(1.0), &RoutingExperiment::CheckThroughput, this);
}

Ptr<Socket>
RoutingExperiment::SetupPacketReceive(Ipv4Address addr, Ptr<Node> node)
{
    TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
    Ptr<Socket> sink = Socket::CreateSocket(node, tid);
    InetSocketAddress local = InetSocketAddress(addr, port);
    sink->Bind(local);
    sink->SetRecvCallback(MakeCallback(&RoutingExperiment::ReceivePacket, this));

    return sink;
}

void
RoutingExperiment::CommandSetup(int argc, char** argv)
{
    CommandLine cmd(__FILE__);
    cmd.AddValue("CSVfileName", "The name of the CSV output file name", m_CSVfileName);
    cmd.AddValue("traceMobility", "Enable mobility tracing", m_traceMobility);
    cmd.AddValue("protocol", "Routing protocol (OLSR, RAODV, DSDV, DSR)", m_protocolName);
    cmd.AddValue("flowMonitor", "enable FlowMonitor", m_flowMonitor);
    cmd.AddValue("node", "enable node", numNode);
    cmd.AddValue("rate", "enable rate", packetRate);
    cmd.AddValue("speed", "enable speed",speed);
    cmd.Parse(argc, argv);

    std::vector<std::string> allowedProtocols{"OLSR", "RAODV", "DSDV", "DSR"};

    if (std::find(std::begin(allowedProtocols), std::end(allowedProtocols), m_protocolName) ==
        std::end(allowedProtocols))
    {
        NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    }
}

int
main(int argc, char* argv[])
{
    RoutingExperiment experiment;
    experiment.CommandSetup(argc, argv);
    experiment.Run();

    return 0;
}
/*
void RoutingExperiment::Run()
{
    Packet::EnablePrinting();

    // Open the CSV file and write the header
    std::ofstream out(m_CSVfileName);
    out << "Nodes,Packet Transfer Rate(per s),NodeSpeed,Throughput,End to End Delay,Packet Delivery Ratio,Packet Drop Ratio\n";
    out.close();

    // Define parameter ranges
    // std::vector<int> numNodes = {20, 40, 70, 100};   // Number of nodes
    // std::vector<int> rates = {100, 200, 300, 400};   // Packet rates in packets per second
    // std::vector<int> speeds = {5, 10, 15, 20};       // Node speeds in m/s
    std::vector<int> numNodes = {20};   // Number of nodes
    std::vector<int> rates = {100};   // Packet rates in packets per second
    std::vector<int> speeds = {5};       // Node speeds in m/s

    double TotalTime = 200.0;  // Simulation time in seconds
    std::string phyMode("DsssRate11Mbps");

    // Loop through all parameter combinations
    for (int n : numNodes)
    {
        for (int r : rates)
        {
            for (int s : speeds)
            {
                NS_LOG_UNCOND("Running simulation: NumNodes=" << n << ", Rate=" << r << ", Speed=" << s);

                // Create nodes
                NodeContainer adhocNodes;
                adhocNodes.Create(n);

                // Wi-Fi configuration
                WifiHelper wifi;
                wifi.SetStandard(WIFI_STANDARD_80211b);

                YansWifiPhyHelper wifiPhy;
                YansWifiChannelHelper wifiChannel;
                wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
                wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
                wifiPhy.SetChannel(wifiChannel.Create());

                WifiMacHelper wifiMac;
                wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                             "DataMode", StringValue(phyMode),
                                             "ControlMode", StringValue(phyMode));
                wifiMac.SetType("ns3::AdhocWifiMac");
                NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

                // Mobility configuration
                MobilityHelper mobilityAdhoc;
                ObjectFactory pos;
                pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
                pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
                pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));

                Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
                mobilityAdhoc.SetPositionAllocator(taPositionAlloc);

                std::stringstream ssSpeed;
                ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << s << "]";
                mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                               "Speed", StringValue(ssSpeed.str()),
                                               "Pause", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"),
                                               "PositionAllocator", PointerValue(taPositionAlloc));
                mobilityAdhoc.Install(adhocNodes);

                // Internet stack with AODV
                AodvHelper aodv;
                InternetStackHelper internet;
                internet.SetRoutingHelper(aodv);
                internet.Install(adhocNodes);

                // Assign IP addresses
                Ipv4AddressHelper addressAdhoc;
                addressAdhoc.SetBase("10.1.1.0", "255.255.255.0");
                Ipv4InterfaceContainer adhocInterfaces = addressAdhoc.Assign(adhocDevices);

                // Traffic configuration
                OnOffHelper onoff("ns3::UdpSocketFactory", Address());
                onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
                onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
                onoff.SetAttribute("DataRate", DataRateValue(DataRate(std::to_string(r*64*8) + "bps")));
                onoff.SetAttribute("PacketSize", UintegerValue(64));

                int numFlows = n / 2; // Number of flows is half the total number of nodes

                for (int i = 0; i < numFlows; i++)
                {
                    Ptr<Socket> sink = SetupPacketReceive(adhocInterfaces.GetAddress(i), adhocNodes.Get(i));
                    AddressValue remoteAddress(InetSocketAddress(adhocInterfaces.GetAddress(i), port));
                    onoff.SetAttribute("Remote", remoteAddress);

                    Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable>();
                    ApplicationContainer app = onoff.Install(adhocNodes.Get(i + numFlows));
                    app.Start(Seconds(var->GetValue(100.0, 101.0)));
                    app.Stop(Seconds(TotalTime));
                }

                // FlowMonitor configuration
                FlowMonitorHelper flowmonHelper;
                Ptr<FlowMonitor> flowmon = flowmonHelper.InstallAll();

                // Run the simulation
                Simulator::Stop(Seconds(TotalTime));
                Simulator::Run();

                // Collect metrics
                Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
                std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats();

                double throughput = 0.0, delay = 0.0, pdr = 0.0, dropRatio = 0.0;
                uint32_t totalTxPackets = 0, totalRxPackets = 0, totalDroppedPackets = 0;

                for (const auto &flow : stats)
                {
                    totalTxPackets += flow.second.txPackets;
                    totalRxPackets += flow.second.rxPackets;
                    totalDroppedPackets += (flow.second.txPackets - flow.second.rxPackets);
                    throughput += (flow.second.rxBytes * 8.0) / (TotalTime * 1000.0); // kbps
                    delay += (flow.second.rxPackets > 0) ? (flow.second.delaySum.GetSeconds() / flow.second.rxPackets) : 0.0;
                }

                if (totalTxPackets > 0)
                {
                    pdr = (double)totalRxPackets / totalTxPackets * 100.0;
                    dropRatio = (double)totalDroppedPackets / totalTxPackets * 100.0;
                }

                // Log results
                std::ofstream out(m_CSVfileName, std::ios::app);
                out << n << "," << r << "," << s << "," << throughput << "," << delay << "," << pdr << "," << dropRatio << "\n";
                out.close();

                Simulator::Destroy();
            }
        }
    }
}




*/

void
RoutingExperiment::Run()
{
    Packet::EnablePrinting();

    // blank out the last output file and write the column headers
    // std::ofstream out(m_CSVfileName);
    // out << "SimulationSecond,"
    //     << "ReceiveRate,"
    //     << "PacketsReceived,"
    //     << "NumberOfSinks,"
    //     << "RoutingProtocol,"
    //     << "TransmissionPower" << std::endl;
    // out.close();
    //std::ofstream out(m_CSVfileName);
    std::ofstream out(m_CSVfileName, std::ios::app);
    //out << "Nodes,Packet Transfer Rate(per s),NodeSpeed,Throughput,End to End Delay,Packet Delivery Ratio,Packet Drop Ratio\n";
    if (out.tellp() == 0) {  // Check if the file is empty
        out << "Nodes,Packet Transfer Rate(per s),NodeSpeed,Throughput,End to End Delay,Packet Delivery Ratio,Packet Drop Ratio\n";
    }
    out.close();

    // int nWifis = 50;
    
    int nWifis = numNode;

    double TotalTime = 200.0;
    std::string rate("2048bps");
    std::string phyMode("DsssRate11Mbps");
    std::string tr_name("manet-routing-compare");
    // int nodeSpeed = 20; // in m/s
    int nodeSpeed = speed; // in m/s
    int nodePause = 0;  // in s
    int pktrate=packetRate;
    

    // std::vector<int> numNodes = {20, 40, 70, 100};
    // std::vector<int> rates = {100, 200, 300, 400};
    // std::vector<int> speeds = {5, 10, 15, 20};
        
    // std::vector<int> numNodes = { 70};
    // std::vector<int> rates = { 400};
    // std::vector<int> speeds = {5, 10, 15, 20};

    //Loop through all parameter combinations
    // for (int nWifis : numNodes)
    // {
    //     for (int pktrate : rates)
    //     {
    //         for (int nodeSpeed : speeds)
    //         {
                    //int nWifis = numNode;

    // double TotalTime = 200.0;
    // std::string rate("2048bps");
    // std::string phyMode("DsssRate11Mbps");
    // std::string tr_name("manet-routing-compare");
    // int nodePause = 0;


    Config::SetDefault("ns3::OnOffApplication::PacketSize", StringValue("64"));
    Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue(rate));

    // Set Non-unicastMode rate to unicast mode
    Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(phyMode));

    NodeContainer adhocNodes;
    adhocNodes.Create(nWifis);

    // setting up wifi phy and channel using helpers
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211b);

    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
    wifiPhy.SetChannel(wifiChannel.Create());

    // Add a mac and disable rate control
    WifiMacHelper wifiMac;
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(phyMode),
                                 "ControlMode",
                                 StringValue(phyMode));

    wifiPhy.Set("TxPowerStart", DoubleValue(m_txp));
    wifiPhy.Set("TxPowerEnd", DoubleValue(m_txp));

    wifiMac.SetType("ns3::AdhocWifiMac");
    NetDeviceContainer adhocDevices = wifi.Install(wifiPhy, wifiMac, adhocNodes);

    MobilityHelper mobilityAdhoc;
    int64_t streamIndex = 0; // used to get consistent mobility across scenarios

    ObjectFactory pos;
    pos.SetTypeId("ns3::RandomRectanglePositionAllocator");
    pos.Set("X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
    pos.Set("Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));

    Ptr<PositionAllocator> taPositionAlloc = pos.Create()->GetObject<PositionAllocator>();
    streamIndex += taPositionAlloc->AssignStreams(streamIndex);

    std::stringstream ssSpeed;
    // ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << nodeSpeed << "]";
    ssSpeed << "ns3::ConstantRandomVariable[Constant=" << nodeSpeed << "]";
    std::stringstream ssPause;
    ssPause << "ns3::ConstantRandomVariable[Constant=" << nodePause << "]";
    mobilityAdhoc.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                   "Speed",
                                   StringValue(ssSpeed.str()),
                                   "Pause",
                                   StringValue(ssPause.str()),
                                   "PositionAllocator",
                                   PointerValue(taPositionAlloc));
    mobilityAdhoc.SetPositionAllocator(taPositionAlloc);
    mobilityAdhoc.Install(adhocNodes);
    streamIndex += mobilityAdhoc.AssignStreams(adhocNodes, streamIndex);

    RaodvHelper raodv;
    OlsrHelper olsr;
    DsdvHelper dsdv;
    DsrHelper dsr;
    DsrMainHelper dsrMain;
    Ipv4ListRoutingHelper list;
    InternetStackHelper internet;

    if (m_protocolName == "OLSR")
    {
        list.Add(olsr, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "RAODV")
    {
        list.Add(raodv, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "DSDV")
    {
        list.Add(dsdv, 100);
        internet.SetRoutingHelper(list);
        internet.Install(adhocNodes);
    }
    else if (m_protocolName == "DSR")
    {
        internet.Install(adhocNodes);
        dsrMain.Install(dsr, adhocNodes);
        if (m_flowMonitor)
        {
            NS_FATAL_ERROR("Error: FlowMonitor does not work with DSR. Terminating.");
        }
    }
    else
    {
        NS_FATAL_ERROR("No such protocol:" << m_protocolName);
    }

    NS_LOG_INFO("assigning ip address");

    Ipv4AddressHelper addressAdhoc;
    addressAdhoc.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer adhocInterfaces;
    adhocInterfaces = addressAdhoc.Assign(adhocDevices);



    // Traffic configuration
    OnOffHelper onoff("ns3::UdpSocketFactory", Address());
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate(std::to_string(pktrate*64*8) + "bps")));
    onoff.SetAttribute("PacketSize", UintegerValue(64));







    int numFlows=nWifis/2; // Number of flows is half the total number of nodes

    for(int i=0;i<numFlows;i++)//upper half as nodes
    {
        Ptr<Socket> sink = SetupPacketReceive(adhocInterfaces.GetAddress(i), adhocNodes.Get(i));
        AddressValue remoteAddress(InetSocketAddress(adhocInterfaces.GetAddress(i), port));
        onoff.SetAttribute("Remote", remoteAddress);

        Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable>();
        ApplicationContainer app = onoff.Install(adhocNodes.Get(i + numFlows));
        app.Start(Seconds(var->GetValue(100.0, 101.0)));
        app.Stop(Seconds(TotalTime));
    }


    std::stringstream ss;
    ss << nWifis;
    std::string nodes = ss.str();

    std::stringstream ss2;
    ss2 << nodeSpeed;
    std::string sNodeSpeed = ss2.str();

    std::stringstream ss3;
    ss3 << nodePause;
    std::string sNodePause = ss3.str();

    std::stringstream ss4;
    ss4 << rate;
    std::string sRate = ss4.str();



    AsciiTraceHelper ascii;
    MobilityHelper::EnableAsciiAll(ascii.CreateFileStream(tr_name + ".mob"));

    FlowMonitorHelper flowmonHelper;
    Ptr<FlowMonitor> flowmon;
    if (m_flowMonitor)
    {
        flowmon = flowmonHelper.InstallAll();
    }

    NS_LOG_INFO("Run Simulation.");

    //CheckThroughput();

    // Run the simulation
    Simulator::Stop(Seconds(TotalTime));
    Simulator::Run();

    if (m_flowMonitor)
    {
        flowmon->SerializeToXmlFile(tr_name + ".flowmon", false, false);
        // Collect metrics
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
        std::map<FlowId, FlowMonitor::FlowStats> stats = flowmon->GetFlowStats();

        double throughput = 0.0, delay = 0.0, pdr = 0.0, dropRatio = 0.0;
        uint32_t totalTxPackets = 0, totalRxPackets = 0, totalDroppedPackets = 0,lost=0;

        for (const auto &flow : stats)
        {
            totalTxPackets += flow.second.txPackets;
            totalRxPackets += flow.second.rxPackets;
            totalDroppedPackets += (flow.second.txPackets - flow.second.rxPackets);
            throughput += (flow.second.rxBytes * 8.0);
            double avgDelayPerFlow = (flow.second.rxPackets > 0) ? (flow.second.delaySum.GetSeconds()):0.0;
            delay += avgDelayPerFlow; // Accumulate average delay per flow
            lost+=flow.second.lostPackets;
        }
        throughput=(throughput*2)/(TotalTime*1024.0);
        delay=delay/totalRxPackets;
        if (totalTxPackets > 0)
        {
            pdr = ((double)totalRxPackets / totalTxPackets) * 100.0;
            dropRatio = ((double)totalDroppedPackets / totalTxPackets) * 100.0;
        }

        // Append results to CSV file
        out.open(m_CSVfileName, std::ios::app); 
        if (out.is_open())
        {
            out << nWifis << "," << pktrate << "," << nodeSpeed << "," << throughput << "," << delay << "," << pdr << "," << dropRatio << "\n";
            out.close();
        }

        //std::cout << "Lost Packets:" << totalDroppedPackets<< std::endl;

    }

                

    Simulator::Destroy();
}



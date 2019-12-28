

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/command-line.h"
#include "ns3/simple-device-energy-model.h"
#include "ns3/li-ion-energy-source.h"
#include "ns3/energy-source-container.h"
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>
#include <ns3/core-module.h>
#include <ns3/lte-enb-rrc.h>
using namespace ns3;



void
NotifyConnectionEstablishedUe (std::string context,
                               uint64_t imsi,
                               uint16_t cellid,
                               uint16_t rnti)
{
  std::cout << context
            << " UE IMSI " << imsi
            << ": connected to CellId " << cellid
            << " with RNTI " << rnti
            << std::endl;
}

int count =0;
static void
PrintCellInfo (Ptr<LiIonEnergySource> es)
{
  if(count>=6)
    exit(1);
  count++;
  
  std::cout << "At " << Simulator::Now ().GetSeconds ()<<" seconds " << " Cell voltage: " << es->GetSupplyVoltage () << " V Remaining Capacity: " <<
  es->GetRemainingEnergy () / (3.6 * 5600) << " Ah" << std::endl;

  if (!Simulator::IsFinished ())
    {
      Simulator::Schedule (Seconds (20),
                           &PrintCellInfo,
                           es);
    }
}


int
main (int argc, char **argv)
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  LogLevel logLevel = (LogLevel)(LOG_PREFIX_ALL | LOG_LEVEL_ALL);
  // LogComponentEnable ("LteUePhy", logLevel);
  LogComponentEnable ("LteUeRrc", logLevel);

 enum State
  {
    IDLE_START = 0,
    IDLE_CELL_SEARCH,   
    IDLE_WAIT_MIB_SIB1,
    IDLE_WAIT_MIB,
    IDLE_WAIT_SIB1,
    IDLE_CAMPED_NORMALLY,
    IDLE_WAIT_SIB2,
    IDLE_RANDOM_ACCESS,
    IDLE_CONNECTING,
    CONNECTED_NORMALLY,
    CONNECTED_HANDOVER,
    
    CONNECTED_PHY_PROBLEM,
    CONNECTED_REESTABLISHING,
    NUM_STATES
  };

  

  // LogComponentEnable ("LiIonEnergySource", LOG_LEVEL_DEBUG);


   Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
   Ptr<LteUeRrc> lteUeRrc = CreateObject<LteUeRrc>();
  //  Ptr<LteUePhy> lteUePhy = CreateObject<LteUePhy>();
  //  std::cout<<lteUePhy->GetTxPower();
   
    NodeContainer enbNodes;
    enbNodes.Create(1);
    NodeContainer ueNodes;
    ueNodes.Create(1);
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(ueNodes);
    NetDeviceContainer enbDevs;
    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs;
    ueDevs = lteHelper->InstallUeDevice(ueNodes);
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    // enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    // EpsBearer bearer(q);
    // lteHelper->ActivateDataRadioBearer(ueDevs, bearer);
    std::cout<<"done attachment\n";
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    //modified function
    // lteUeRrc->setSwitchedToState();
    
  Ptr<Node> node = CreateObject<Node> ();
  Ptr<SimpleDeviceEnergyModel> sem = CreateObject<SimpleDeviceEnergyModel> ();
  Ptr<EnergySourceContainer> esCont = CreateObject<EnergySourceContainer> ();
  Ptr<LiIonEnergySource> es = CreateObject<LiIonEnergySource> ();
  esCont->Add (es);
  es->SetNode (node);
  sem->SetEnergySource (es);
  es->AppendDeviceEnergyModel (sem);
  sem->SetNode (node);
  node->AggregateObject (esCont);
  Time now = Simulator::Now ();

  // discharge at 7.33 A for 1700 seconds
  sem->SetCurrentA (7.33);
  now += Seconds (1701);


  // discharge at 7.33 A for 628 seconds
  Simulator::Schedule (now,
                       &SimpleDeviceEnergyModel::SetCurrentA,
                       sem,
                       7.33);
  now = Seconds (20);
  std::cout<<"Attack procedure initiated injecting service request to UE"<<std::endl;
  // PrintCellInfo (es);
  Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                   MakeCallback (&NotifyConnectionEstablishedUe));
    // lteHelper->EnablePhyTraces ();



  Simulator::Stop (now);
  Simulator::Run ();
  Simulator::Destroy ();

  // the cell voltage should be under 3.3v
  DoubleValue v;
  es->GetAttribute ("ThresholdVoltage", v);
  NS_ASSERT (es->GetSupplyVoltage () <= v.Get ());
}

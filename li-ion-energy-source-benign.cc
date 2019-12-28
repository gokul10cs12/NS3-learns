
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

using namespace ns3;

/**
 * In this simple example, we show how to create and drain energy from a
 * LiIonEnergySource.
 * We make a series of discharge calls to the energy source class with
 * different current drain and duration until all the energy is depleted
 * from the cell.
 *
 * Every 20 seconds it is printed out the actual cell voltage to verify
 * that it follows the discharge curve of the datasheet [1].
 *
 * At the end of the example it is verified that after the energy depletion
 * call, the cell voltage is below the threshold voltage.
 *
 * References:
 * [1] Panasonic CGR18650DA Datasheet, http://www.panasonic.com/industrial/includes/pdf/Panasonic_LiIon_CGR18650DA.pdf
 */
int count =0;
static void
PrintCellInfo (Ptr<LiIonEnergySource> es)
{

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
  

  // LogComponentEnable ("LiIonEnergySource", LOG_LEVEL_DEBUG);


   Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    NodeContainer enbNodes;
    enbNodes.Create(1);
    NodeContainer ueNodes;
    ueNodes.Create(2);
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

  // discharge at 2.33 A for 1700 seconds
  sem->SetCurrentA (1.33);
  now += Seconds (1701);

 PrintCellInfo (es);
  // discharge at 2.33 A for 628 seconds
  Simulator::Schedule (now,
                       &SimpleDeviceEnergyModel::SetCurrentA,
                       sem,
                       2.33);
  now += Seconds (10);
  std::cout<<"Normal battery drain procedure"<<std::endl;
 

  Simulator::Stop (now);
  Simulator::Run ();
  Simulator::Destroy ();

  // the cell voltage should be under 3.3v
  DoubleValue v;
  es->GetAttribute ("ThresholdVoltage", v);
  NS_ASSERT (es->GetSupplyVoltage () <= v.Get ());
}

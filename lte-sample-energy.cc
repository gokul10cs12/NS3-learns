#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>

#include <ns3/simple-device-energy-model.h>
#include <ns3/li-ion-energy-source.h>
#include <ns3/energy-source-container.h>


#include <ns3/netanim-module.h>

using namespace ns3;


static void
PrintCellInfo (Ptr<LiIonEnergySource> es)
{
  std::cout << "At " << Simulator::Now ().GetSeconds () << " Cell voltage: " << es->GetSupplyVoltage () << " V Remaining Capacity: " <<
  es->GetRemainingEnergy () / (3.6 * 3600) << " Ah" << std::endl;

  if (!Simulator::IsFinished ())
    {
      Simulator::Schedule (Seconds (20),
                           &PrintCellInfo,
                           es);
    }
}



int main(int argc, char *argv[])
{
    // the rest of the simulation program follows

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();


    Ptr<Node> node = CreateObject<Node> ();
    Ptr<SimpleDeviceEnergyModel> sem = CreateObject<SimpleDeviceEnergyModel> ();
    Ptr<EnergySourceContainer> esCont = CreateObject<EnergySourceContainer> ();
    Ptr<LiIonEnergySource> es = CreateObject<LiIonEnergySource> ();



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
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);


    esCont->Add (es);
    es->SetNode (node);
    sem->SetEnergySource (es);
    es->AppendDeviceEnergyModel (sem);
    sem->SetNode (node);
    node->AggregateObject (esCont);




  Time now = Simulator::Now ();

  // discharge at 2.33 A for 1700 seconds
  sem->SetCurrentA (2.33);
  now += Seconds (1701);



  // discharge at 4.66 A for 628 seconds
  Simulator::Schedule (now,
                       &SimpleDeviceEnergyModel::SetCurrentA,
                       sem,
                       4.66);
  now += Seconds (60);
lteHelper->Attach(ueDevs, enbDevs.Get(0));
  PrintCellInfo (es);
 

    AnimationInterface anim("lte2Nodes.xml");				
  anim.SetConstantPosition(enbNodes.Get(0), 10.0, 10.0);  
  anim.SetConstantPosition(ueNodes.Get(0), 20.0, 10.0);	
  anim.SetConstantPosition(ueNodes.Get(1), 15.0,15.0);

  
    Simulator::Stop (Seconds (10.005));
  Simulator::Run ();
  Simulator::Destroy ();

  // the cell voltage should be under 3.3v
  // DoubleValue v;
  // es->GetAttribute ("ThresholdVoltage", v);
  // NS_ASSERT (es->GetSupplyVoltage () <= v.Get ());


   
}


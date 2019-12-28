//typical lte handover scenario using NS-3 simulator
#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>

#include <ns3/netanim-module.h>
#include <ns3/mobility-building-info.h>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/building.h>

using namespace ns3;
//this is a new test added fr the test branch
int
main (int argc, char *argv[])
{

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);
  //LteHelper::SetEpcHelper();
  NodeContainer enbNodes;
  enbNodes.Create (2);
  NodeContainer ueNodes;
  ueNodes.Create (2);
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0), "MinY",
                                 DoubleValue (10.0), "DeltaX", DoubleValue (5.0), "DeltaY",
                                 DoubleValue (10.0), "GridWidth", UintegerValue (4), "LayoutType",
                                 StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (enbNodes);

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Bounds",
                             RectangleValue (Rectangle (-25, 50, -50, 50)));

  mobility.Install (ueNodes);
  NetDeviceContainer enbDevs;

  enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueDevs;

  ueDevs = lteHelper->InstallUeDevice (ueNodes);
  Ptr<ConstantPositionMobilityModel> mm0 =
      enbNodes.Get (0)->GetObject<ConstantPositionMobilityModel> ();
 
  mm0->SetPosition (Vector (5.0, 5.0, 1.5));

  lteHelper->Attach (ueDevs, enbDevs.Get (0));

  enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
  EpsBearer bearer (q);

  lteHelper->ActivateDataRadioBearer (ueDevs, bearer);

  lteHelper->HandoverRequest (Seconds (2.100),
                            ueDevs.Get (0),
                            enbDevs.Get (0),
                            enbDevs.Get (1));

  AnimationInterface anim ("LteOuput/lte_handover.xml");
  /*anim.SetConstantPosition(enbNodes.Get(0), 10.0, 10.0);  
  anim.SetConstantPosition(ueNodes.Get(0), 20.0, 10.0);	
  anim.SetConstantPosition(ueNodes.Get(1), 15.0,15.0);
*/

  Simulator::Stop (Seconds (5.005));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

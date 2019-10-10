#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>

#include <ns3/netanim-module.h>

using namespace ns3;

int main(int argc, char *argv[])
{

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    NodeContainer enbNodes;
    enbNodes.Create(1);
    NodeContainer ueNodes;
    ueNodes.Create(3);
    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
    "MinX", DoubleValue (0.0),
    "MinY", DoubleValue (10.0),
    "DeltaX", DoubleValue (5.0),
    "DeltaY", DoubleValue (10.0),
    "GridWidth", UintegerValue (4),
    "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    mobility.Install(enbNodes);

    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel","Bounds", 
        RectangleValue (Rectangle (-25, 50, -50, 50)));


    mobility.Install(ueNodes);
    NetDeviceContainer enbDevs;


    enbDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueDevs;

    ueDevs = lteHelper->InstallUeDevice(ueNodes);
    lteHelper->Attach(ueDevs, enbDevs.Get(0));

    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);

    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);


  AnimationInterface anim("LteOuput/lteMobility.xml");				
  /*anim.SetConstantPosition(enbNodes.Get(0), 10.0, 10.0);  
  anim.SetConstantPosition(ueNodes.Get(0), 20.0, 10.0);	
  anim.SetConstantPosition(ueNodes.Get(1), 15.0,15.0);
*/

    Simulator::Stop (Seconds (5.005));
    Simulator::Run ();
    Simulator::Destroy ();

    
return 0;
}


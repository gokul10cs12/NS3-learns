#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/mobility-module.h>
#include <ns3/lte-module.h>

#include <ns3/netanim-module.h>

using namespace ns3;

int main(int argc, char *argv[])
{
    // the rest of the simulation program follows

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    lteHelper->EnablePhyTraces ();

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
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    lteHelper->Attach(ueDevs, enbDevs.Get(0));
    enum EpsBearer::Qci q = EpsBearer::GBR_CONV_VOICE;
    EpsBearer bearer(q);
    lteHelper->ActivateDataRadioBearer(ueDevs, bearer);

    // lteHelper->DeActivateDedicatedEpsBearer(ueDevs, enbDevs.Get(0), q);


  AnimationInterface anim("lte2Nodes.xml");				
  anim.SetConstantPosition(enbNodes.Get(0), 10.0, 10.0);  
  anim.SetConstantPosition(ueNodes.Get(0), 20.0, 10.0);	
  anim.SetConstantPosition(ueNodes.Get(1), 15.0,15.0);


    Simulator::Stop (Seconds (10.005));
    Simulator::Run ();
    Simulator::Destroy ();
return 0;
}


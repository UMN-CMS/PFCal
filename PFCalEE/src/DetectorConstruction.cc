#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "HGCSSSimHit.hh"

#include <boost/algorithm/string.hpp>

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4PhysicalConstants.hh"

using namespace std;

//
DetectorConstruction::DetectorConstruction(G4int ver, G4int mod,
					   std::string absThickW,
					   std::string absThickPb,
					   std::string dropLayer) : 
  version_(ver), model_(mod), addPrePCB_(false)
{
  SetWThick(absThickW);
  SetPbThick(absThickPb);
  SetDropLayers(dropLayer);

  switch(version_)
    {
    case v_HGCALEE_v6: case v_HGCAL_v6: case v_HGCALEE_v624: case v_HGCALEE_v618: case v_HGCAL_v624: case v_HGCAL_v618:
      {
	G4cout << "[DetectorConstruction] starting v_HGCALEE_v6"<< G4endl;
	G4double airThick = 2*mm;
	G4double pcbThick = 2*mm;
        unsigned Nmodule=4;
	G4double wThick = 2.*mm;
	G4double wcuThick = 0.6*mm;
	
        std::vector<G4double> lThickL;
	std::vector<std::string> lEleL;
	std::vector<G4double> lThickR;
	std::vector<std::string> lEleR;
	
	lThickR.push_back(wcuThick);lEleR.push_back("WCu");
	lThickR.push_back(6*mm);lEleR.push_back("Cu");
	lThickR.push_back(wcuThick);lEleR.push_back("WCu");
	//Try adding steel, it greatly improves neutron detection
	//lThickR.push_back(6*mm);lEleR.push_back("Steel");
	lThickR.push_back(0.1*mm);lEleR.push_back("Si");
	lThickR.push_back(0.1*mm);lEleR.push_back("Si");
	lThickR.push_back(0.1*mm);lEleR.push_back("Si");
	lThickR.push_back(pcbThick);lEleR.push_back("PCB");
	lThickR.push_back(airThick);lEleR.push_back("Air");
	
	lThickL.push_back(0.5*mm);lEleL.push_back("Cu");
	lThickL.push_back(0.5*mm);lEleL.push_back("CFMix");
	lThickL.push_back(wThick);lEleL.push_back("W");
	//Try adding steel, it greatly improves neutron detection
	//lThickL.push_back(6*mm);lEleL.push_back("Steel");

	lThickL.push_back(0.5*mm);lEleL.push_back("CFMix");
	lThickL.push_back(0.5*mm);lEleL.push_back("Cu");
	lThickL.push_back(airThick);lEleL.push_back("Air");
	lThickL.push_back(pcbThick);lEleL.push_back("PCB");
	lThickL.push_back(0.1*mm);lEleL.push_back("Si");
	lThickL.push_back(0.1*mm);lEleL.push_back("Si");
	lThickL.push_back(0.1*mm);lEleL.push_back("Si");
	for(unsigned i=0; i<Nmodule; i++) {
	  m_caloStruct.push_back( SamplingSection(lThickL,lEleL) );
	  m_caloStruct.push_back( SamplingSection(lThickR,lEleR) );
	}

        Nmodule=5;
	lThickL[2] = 2.8*mm;
	lThickR[0] = 1.2*mm;
	lThickR[2] = 1.2*mm;
        if(version_ == v_HGCALEE_v624 || version_ == v_HGCAL_v624){
            Nmodule=4;
            lThickL[2] = 3.6*mm;
            lThickR[0] = 1.75*mm;
            lThickR[2] = 1.75*mm;
        }
        else if(version_ == v_HGCALEE_v618 || version_ == v_HGCAL_v618){
            Nmodule=3;
            lThickL[2] = 4.9*mm;
            lThickR[0] = 2.7*mm;
            lThickR[2] = 2.7*mm;
        }
	for(unsigned i=0; i<Nmodule; i++) {
	  m_caloStruct.push_back( SamplingSection(lThickL,lEleL) );
	  m_caloStruct.push_back( SamplingSection(lThickR,lEleR) );
	}

        Nmodule=4;
	lThickL[2] = 4.2*mm;
	lThickR[0] = 2.2*mm;
	lThickR[2] = 2.2*mm;
       for(unsigned i=0; i<Nmodule; i++) {
	  m_caloStruct.push_back( SamplingSection(lThickL,lEleL) );
	  m_caloStruct.push_back( SamplingSection(lThickR,lEleR) );
	}
	
	if(version_==v_HGCAL_v6){
	  //add HCAL
	  buildHGCALFHE(6);
	  buildHGCALBHE(6);
	}
	else if(version_==v_HGCAL_v624){
	  //add HCAL
	  buildHGCALFHE(624);
	  buildHGCALBHE(6);
	}
	else if(version_==v_HGCAL_v618){
	  //add HCAL
	  buildHGCALFHE(618);
	  buildHGCALBHE(6);
	}

	break;
      }



    }

  DefineMaterials();
  SetMagField(0);
  m_detectorMessenger = new DetectorMessenger(this);
  UpdateCalorSize();
}

void DetectorConstruction::buildHGCALFHE(const unsigned aVersion){
  G4double airThick = 2*mm;
  if(version_==v_HGCAL_v5_gap4) airThick = 4*mm;
  std::vector<G4double> lThick;
  std::vector<std::string> lEle;
  if(aVersion==6 || aVersion==624 || aVersion==618) {
    airThick = 2*mm;
    G4double pcbthick = 2*mm;
    G4double brassthick = aVersion==618? 62*mm : 35*mm;
    //putting all absorber in front of each Si layer to have correct reweighting
    lThick.push_back(0.5*mm); lEle.push_back("Cu");
    lThick.push_back(15.*mm);lEle.push_back("SSteel");
    lThick.push_back(brassthick);lEle.push_back("Brass");
    lThick.push_back(0.5*mm); lEle.push_back("Cu");
    lThick.push_back(airThick);lEle.push_back("Air");
    lThick.push_back(pcbthick);lEle.push_back("PCB");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    m_caloStruct.push_back( SamplingSection(lThick,lEle) );

    lThick.clear();
    lEle.clear();
    lThick.push_back(1.);lEle.push_back("CFMix");
    lThick.push_back(6.); lEle.push_back("Cu");
    lThick.push_back(brassthick);lEle.push_back("Brass");
    lThick.push_back(0.5*mm); lEle.push_back("Cu");
    lThick.push_back(airThick);lEle.push_back("Air");
    lThick.push_back(pcbthick);lEle.push_back("PCB");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    for(unsigned i=0; i<5; i++) {
      m_caloStruct.push_back( SamplingSection(lThick,lEle) );
    }
    unsigned nLay = aVersion==618? 3 : aVersion==624 ? 5 : 6;
    lThick[2] = aVersion==624 ? 55*mm : brassthick;
    for(unsigned i=0; i<nLay; i++) {
      m_caloStruct.push_back( SamplingSection(lThick,lEle) );
    }


  }
  else {
    G4double pcbthick = (aVersion==4)? 2*mm : 1.2*mm;
    //add last ECAL layer structure
    lThick.push_back(3*mm);lEle.push_back("Cu");
    lThick.push_back(1*mm);lEle.push_back("Pb");
    lThick.push_back(15.*mm);lEle.push_back("SSteel");
    if (aVersion==41) {lThick.push_back(52.*mm);lEle.push_back("Pb");}
    else {lThick.push_back(40.*mm);lEle.push_back("Brass");}
    lThick.push_back(0.5*mm); lEle.push_back("Cu");
    lThick.push_back(airThick);lEle.push_back("Air");
    lThick.push_back(pcbthick);lEle.push_back("PCB");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    m_caloStruct.push_back( SamplingSection(lThick,lEle) );

    //next 11 layers
    lThick.clear(); lEle.clear();
    lThick.push_back(3*mm); lEle.push_back("Cu");
    lThick.push_back(1*mm); lEle.push_back("Pb");
    if (aVersion==41) {lThick.push_back(52.*mm);lEle.push_back("Pb");}
    else {lThick.push_back(40.*mm);lEle.push_back("Brass");}
    lThick.push_back(0.5*mm); lEle.push_back("Cu");
    lThick.push_back(airThick);lEle.push_back("Air");
    lThick.push_back(pcbthick);lEle.push_back("PCB");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    lThick.push_back(0.1*mm);lEle.push_back("Si");
    
    for(unsigned i=0; i<11; i++) {
      m_caloStruct.push_back( SamplingSection(lThick,lEle) );
    }
  }
}
//
void DetectorConstruction::buildHGCALBHE(const unsigned aVersion){
  std::vector<G4double> lThick;
  std::vector<std::string> lEle;
  //first layer
  if (aVersion==6){
    lThick.push_back(1.*mm);lEle.push_back("CFMix");
    lThick.push_back(6.*mm); lEle.push_back("Cu");
  } else {
    lThick.push_back(3*mm); lEle.push_back("Cu");
    lThick.push_back(1*mm); lEle.push_back("Pb");
  }
  lThick.push_back(2.*mm);lEle.push_back("Al");
  lThick.push_back(16.*mm);lEle.push_back("Foam");
  lThick.push_back(2.*mm);lEle.push_back("Al");
  lThick.push_back(65.*mm);lEle.push_back("Air");
  lThick.push_back(78.*mm);lEle.push_back("Brass");
  if (aVersion==6) {
    lThick.push_back(2.6*mm);lEle.push_back("Air");
    lThick.push_back(3.8*mm);lEle.push_back("Scintillator");
    lThick.push_back(2.6*mm);lEle.push_back("Air");
  }
  else {
    lThick.push_back(9.*mm);lEle.push_back("Scintillator");
  }
  m_caloStruct.push_back( SamplingSection(lThick,lEle) );

  //other layers
  lThick.clear();lEle.clear();
  lThick.push_back(78.*mm);lEle.push_back("Brass");
  if (aVersion==6) {
    lThick.push_back(2.6*mm);lEle.push_back("Air");
    lThick.push_back(3.8*mm);lEle.push_back("Scintillator");
    lThick.push_back(2.6*mm);lEle.push_back("Air");
  }
  else {
    lThick.push_back(9.*mm);lEle.push_back("Scintillator");
  }

  unsigned maxi = (aVersion==4)?9:11;
  for(unsigned i=0; i<maxi; i++) {
    m_caloStruct.push_back( SamplingSection(lThick,lEle) );
  }
}
//
DetectorConstruction::~DetectorConstruction() { delete m_detectorMessenger;}

//
void DetectorConstruction::DefineMaterials()
{ 
  G4NistManager* nistManager = G4NistManager::Instance();
  m_materials["Abs"] = (version_== v_CALICE || version_==v_HGCALEE_W) ? 
    nistManager->FindOrBuildMaterial("G4_W",false) :
    nistManager->FindOrBuildMaterial("G4_Pb",false);
  m_materials["Al"] = nistManager->FindOrBuildMaterial("G4_Al",false);
  m_dEdx["Al"] = 0.4358;
  m_materials["W"] = nistManager->FindOrBuildMaterial("G4_W",false); 
  m_dEdx["W"] = 2.210;
  m_materials["Pb"] = nistManager->FindOrBuildMaterial("G4_Pb",false); 
  m_dEdx["Pb"] = 1.274;
  m_materials["Cu"] = nistManager->FindOrBuildMaterial("G4_Cu",false); 
  m_dEdx["Cu"] = 1.257;
  m_materials["Si"] = nistManager->FindOrBuildMaterial("G4_Si",false);
  m_dEdx["Si"] = 0.3876;
  m_materials["Zn"] = nistManager->FindOrBuildMaterial("G4_Zn",false);
  m_dEdx["Zn"] = 1.007;
  m_materials["Air"]=nistManager->FindOrBuildMaterial("G4_AIR",false);
  m_dEdx["Air"] = 0;
  m_materials["Fe"] = nistManager->FindOrBuildMaterial("G4_Fe",false);
  m_dEdx["Fe"] = 1.143;
  m_materials["Mn"] = nistManager->FindOrBuildMaterial("G4_Mn",false);
  m_dEdx["Mn"] = 1.062 ;
  m_materials["C"] = nistManager->FindOrBuildMaterial("G4_C",false); 
  m_dEdx["C"] = 0.3952;
  m_materials["H"] = nistManager->FindOrBuildMaterial("G4_H",false); 
  m_dEdx["H"] =  0;
  m_materials["Cl"] = nistManager->FindOrBuildMaterial("G4_Cl",false); 
  m_dEdx["Cl"] = 0;
  m_materials["Cr"] = nistManager->FindOrBuildMaterial("G4_Cr",false); 
  m_dEdx["Cr"] = 1.046;
  m_materials["Ni"] = nistManager->FindOrBuildMaterial("G4_Ni",false); 
  m_dEdx["Ni"] = 1.307;
  m_materials["O"] = nistManager->FindOrBuildMaterial("G4_O",false);
  m_materials["Br"] = nistManager->FindOrBuildMaterial("G4_Br",false);

  /*m_materials["PCB"] = new G4Material("G10",1.700*g/cm3,4);
  m_materials["PCB"]->AddElement(nistManager->FindOrBuildElement(14), 1);
  m_materials["PCB"]->AddElement(nistManager->FindOrBuildElement(8) , 2);
  m_materials["PCB"]->AddElement(nistManager->FindOrBuildElement(6) , 3);
  m_materials["PCB"]->AddElement(nistManager->FindOrBuildElement(1) , 3);
  m_dEdx["PCB"] = 0;*/
  m_materials["PCB"] = new G4Material("FR4",1.700*g/cm3,5);
  m_materials["PCB"]->AddMaterial(m_materials["Si"] , 0.18077359);
  m_materials["PCB"]->AddMaterial(m_materials["O"]  , 0.4056325);
  m_materials["PCB"]->AddMaterial(m_materials["C"]  , 0.27804208);
  m_materials["PCB"]->AddMaterial(m_materials["H"]  , 0.068442752);
  m_materials["PCB"]->AddMaterial(m_materials["Br"] , 0.067109079);
  m_dEdx["PCB"] = 0;

  m_materials["Brass"]= new G4Material("Brass",8.53*g/cm3,2);
  m_materials["Brass"]->AddMaterial(m_materials["Cu"]  , 70*perCent);
  m_materials["Brass"]->AddMaterial(m_materials["Zn"]  , 30*perCent);
  m_dEdx["Brass"] = 0.7*m_dEdx["Cu"]+0.3*m_dEdx["Zn"];
  m_materials["Steel"]= new G4Material("Steel",7.87*g/cm3,3);
  m_materials["Steel"]->AddMaterial(m_materials["Fe"]  , 0.9843);
  m_materials["Steel"]->AddMaterial(m_materials["Mn"], 0.014);
  m_materials["Steel"]->AddMaterial(m_materials["C"], 0.0017);
  m_dEdx["Steel"] = 0.9843*m_dEdx["Fe"]+0.014*m_dEdx["Mn"]+0.0017*m_dEdx["C"];
  m_materials["SSteel"]= new G4Material("SSteel",8.02*g/cm3,4);
  m_materials["SSteel"]->AddMaterial(m_materials["Fe"]  , 0.70);
  m_materials["SSteel"]->AddMaterial(m_materials["Mn"], 0.01);
  m_materials["SSteel"]->AddMaterial(m_materials["Cr"], 0.19);
  m_materials["SSteel"]->AddMaterial(m_materials["Ni"], 0.10);
  m_dEdx["SSteel"] = 0.7*m_dEdx["Fe"]+0.01*m_dEdx["Mn"]+0.19*m_dEdx["Cr"]+0.1*m_dEdx["Ni"];
  m_materials["AbsHCAL"] = (version_== v_HGCALHE_CALICE) ?
    m_materials["Steel"]:
    m_materials["Brass"];
  m_dEdx["AbsHCAL"] = (version_== v_HGCALHE_CALICE) ?
    m_dEdx["Steel"]:
    m_dEdx["Brass"];
  //m_materials["Scintillator"]= nistManager->FindOrBuildMaterial("G4_POLYSTYRENE",false); 
  m_materials["Scintillator"]= new G4Material("Scintillator",1.032*g/cm3,2);
  m_materials["Scintillator"]->AddMaterial(m_materials["C"]  , 91.512109*perCent);
  m_materials["Scintillator"]->AddMaterial(m_materials["H"]  , 8.4878906*perCent);
  m_dEdx["Scintillator"] = m_dEdx["C"];

  G4cout << m_materials["Scintillator"] << G4endl;
  m_materials["Polystyrole"]= new G4Material("Polystyrole",1.065*g/cm3,2);
  m_materials["Polystyrole"]->AddMaterial(m_materials["H"]  , 50*perCent);
  m_materials["Polystyrole"]->AddMaterial(m_materials["C"]  , 50*perCent);
  m_dEdx["Polystyrole"] = 0.5*m_dEdx["C"];

  m_materials["PVC"]= new G4Material("PVC",1.350*g/cm3,3);
  m_materials["PVC"]->AddMaterial(m_materials["H"]  , 50*perCent);
  m_materials["PVC"]->AddMaterial(m_materials["C"]  , 33.33*perCent);
  m_materials["PVC"]->AddMaterial(m_materials["Cl"]  , 16.67*perCent);
  m_dEdx["PVC"] = 0.33*m_dEdx["C"];

  /*m_materials["CFMix"]= new G4Material("CFMix",0.120*g/cm3,3);
  m_materials["CFMix"]->AddMaterial(m_materials["Air"]  , 0.009);
  m_materials["CFMix"]->AddMaterial(m_materials["PVC"]  , 0.872);
  m_materials["CFMix"]->AddMaterial(m_materials["Polystyrole"]  , 0.119);
  m_dEdx["CFMix"] = 0;*/

  m_materials["CFMix"]= new G4Material("CFMix",0.845*g/cm3,3);
  m_materials["CFMix"]->AddMaterial(m_materials["C"]  , 0.84491305);
  m_materials["CFMix"]->AddMaterial(m_materials["H"]  , 0.042542086);
  m_materials["CFMix"]->AddMaterial(m_materials["O"]  , 0.11254487);
  m_dEdx["CFMix"] = 0;

  m_materials["Foam"]= new G4Material("Foam",0.0999*g/cm3,2);
  m_materials["Foam"]->AddMaterial(m_materials["C"]  , 0.856);
  m_materials["Foam"]->AddMaterial(m_materials["H"]  , 0.144);
  m_dEdx["Foam"] = 1.749*0.856*0.0999/10.;

  m_materials["WCu"]= new G4Material("WCu",14.979*g/cm3,2);
  m_materials["WCu"]->AddMaterial(m_materials["W"]  , 75*perCent);
  m_materials["WCu"]->AddMaterial(m_materials["Cu"]  , 25*perCent);
  m_dEdx["WCu"] = 0.75*m_dEdx["W"]+0.25*m_dEdx["Cu"];

  m_materials["NeutMod"]= new G4Material("NeutMod",0.950*g/cm3,2);
  m_materials["NeutMod"]->AddMaterial(m_materials["C"]  , 0.85628);
  m_materials["NeutMod"]->AddMaterial(m_materials["H"]  , 0.14372);
  m_dEdx["NeutMod"] = 1.749*0.86*0.950/10.;

}

//
void DetectorConstruction::UpdateCalorSize(){  

  m_CalorSizeZ=0;
  
  for(size_t i=0; i<m_caloStruct.size(); i++){
    m_CalorSizeZ=m_CalorSizeZ+m_caloStruct[i].Total_thick;
  }

  m_nSectors = 1;
  m_interSectorWidth = 0;
  if (model_ == DetectorConstruction::m_SIMPLE_20){
    m_CalorSizeXY=200;
    m_sectorWidth = m_CalorSizeXY;
  }
  else if (model_ == DetectorConstruction::m_SIMPLE_50){
    m_CalorSizeXY=500;
    m_sectorWidth = m_CalorSizeXY;
  }
  else if (model_ == DetectorConstruction::m_SIMPLE_100){
    m_CalorSizeXY=1000;
    m_sectorWidth = m_CalorSizeXY;
  }
  else if (model_ == DetectorConstruction::m_BOXWITHCRACK_100 ){
    m_nSectors = 3;
    m_sectorWidth = 460;
    m_interSectorWidth = 10;
    m_CalorSizeXY=m_nSectors*m_sectorWidth;
  }
  else if (model_ == DetectorConstruction::m_2016TB ){
    m_nSectors    = 1;
    m_sectorWidth = CELL_SIZE_X * 2 *11;
    m_interSectorWidth = 0;
    m_CalorSizeXY = m_sectorWidth;
    m_minRadius   = m_CalorSizeXY/(2*sqrt(3)); // center-to-side radius of hexagon
    m_maxRadius   = m_CalorSizeXY/2.;          // center-to-corner radius of hexagon
  }
  else if (model_ == DetectorConstruction::m_FULLSECTION){
    m_CalorSizeXY=2800;//1700;
    m_minRadius = 150;
    m_maxRadius = m_CalorSizeXY;
    m_minEta = 1.4;
    m_maxEta = 3.7;
    m_sectorWidth = 2.*pi;//2.*pi/m_nSectors;
    m_interSectorWidth = 0;//0.2*pi/180.;
    m_z0pos = 2990;//3170;
    if (version_ == v_HGCALEE_v5 || version_ == v_HGCAL_v5 || version_ == v_HGCALEE_v5_gap4 || version_ == v_HGCAL_v5_gap4) m_z0pos = 2990;//3170;
    else if (version_ == v_HGCALEE_v6 || version_ == v_HGCAL_v6 || version_ == v_HGCALEE_v624 || version_ == v_HGCALEE_v618) m_z0pos = 3070;
  }
  else {
    m_CalorSizeXY=200;
    m_sectorWidth = m_CalorSizeXY;
  }

  for(size_t i=0; i<m_caloStruct.size(); i++) m_caloStruct[i].setNumberOfSectors(m_nSectors);

  m_WorldSizeZ=m_CalorSizeZ*1.1;  
  if (m_nSectors>1) m_WorldSizeXY=(m_CalorSizeXY+2*m_sectorWidth)*1.1;
  else m_WorldSizeXY=m_CalorSizeXY*1.1;

  if (model_ == DetectorConstruction::m_FULLSECTION || model_ == DetectorConstruction::m_2016TB) 
    G4cout << "[DetectorConstruction][UpdateCalorSize] Z x minR * maxR = " 
	   << m_CalorSizeZ << " x " 
	   << m_minRadius << " x " 
	   << m_maxRadius 
	   << " mm, eta range "
	   << m_minEta << " - " 
	   << m_maxEta << " nsectors = " << m_nSectors
	   << G4endl;
  else G4cout << "[DetectorConstruction][UpdateCalorSize] Z x XY = " 
	      << m_CalorSizeZ << " x " 
	      << m_CalorSizeXY << " mm " 
	      << ", nsectors = " << m_nSectors
	      <<  G4endl;

}

//
G4VPhysicalVolume* DetectorConstruction::Construct()
{

  //clean old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //world
  G4double expHall_z = model_ == DetectorConstruction::m_2016TB? 0.5*m : 6*m;
  G4double expHall_x = model_ == DetectorConstruction::m_2016TB? 20*cm : 3*m;
  G4double expHall_y = model_ == DetectorConstruction::m_2016TB? 20*cm : 3*m;

  G4Box* experimentalHall_box = new G4Box("expHall_box",expHall_x,expHall_y,expHall_z);

  G4LogicalVolume* experimentalHall_log = new G4LogicalVolume(experimentalHall_box, m_materials["Air"],"expHall_log");
  G4VPhysicalVolume* experimentalHall_phys
    = new G4PVPlacement(0,                       // no rotation
			G4ThreeVector(0.,0.,0.), // translation position
			experimentalHall_log,    // its logical volume
			"expHall",               // its name
			0,                       // its mother volume
			false,                   // no boolean operations
			0);                      // its copy number

  //detector's World
  G4double pos_x = 0.;
  G4double pos_y = 0.;
  G4double pos_z = 0.;
  if (model_ == DetectorConstruction::m_FULLSECTION){
    pos_x = 0.;
    pos_y = 0.;
    pos_z = m_z0pos+m_CalorSizeZ/2;
  }

  if (model_ == DetectorConstruction::m_FULLSECTION){
    m_solidWorld = new G4Tubs("Wbox",m_minRadius*1.1,m_maxRadius*1.1,m_WorldSizeZ/2,0,2*pi);
  }
  else {
    m_solidWorld = new G4Box("Wbox",m_WorldSizeXY/2,m_WorldSizeXY/2,m_WorldSizeZ/2);
  }
  cout << "m_WorldSizeXY = " << m_WorldSizeXY << ", m_WorldSizeZ = " << m_WorldSizeZ << endl;

  m_logicWorld = new G4LogicalVolume(m_solidWorld, m_materials["Air"], "Wlog");
  m_physWorld = new G4PVPlacement(0, G4ThreeVector(pos_x,pos_y,pos_z), m_logicWorld, "Wphys", experimentalHall_log, false, 0);

  for (unsigned iS(0); iS<m_nSectors; ++iS){
    G4double minL = m_sectorWidth*iS;
    buildSectorStack(iS,minL,m_sectorWidth-m_interSectorWidth);
    if (m_nSectors>1) fillInterSectorSpace(iS,minL+m_sectorWidth-m_interSectorWidth,m_interSectorWidth);
  }
  // Visualization attributes
  //
  m_logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

  //return m_physWorld;
  return experimentalHall_phys;
}

void DetectorConstruction::buildSectorStack(const unsigned sectorNum,
					    const G4double & minL, 
					    const G4double & width)
{


  //build the stack
  G4double zOffset(-m_CalorSizeZ/2), zOverburden(0.);
  char nameBuf[10];
  G4VSolid *solid;

  G4double totalLengthX0 = 0;
  G4double totalLengthL0 = 0;

  for(size_t i=0; i<m_caloStruct.size(); i++)
    {
      G4double crackOffset = getCrackOffset(i);
      G4double angOffset = getAngOffset(i);

      //std::cout << " sector " << sectorNum << " layer " << i << " offset " << crackOffset  << std::endl;
      if (model_ == DetectorConstruction::m_FULLSECTION) {
	crackOffset=0;
      }
      const unsigned nEle = m_caloStruct[i].n_elements;
      //index for counting Si sensitive layers
      unsigned idx = 0;

      for (unsigned ie(0); ie<nEle;++ie){
	std::string eleName = m_caloStruct[i].ele_name[ie];
	if (m_nSectors==1) sprintf(nameBuf,"%s%d",eleName.c_str(),int(i+1));
	else sprintf(nameBuf,"%s%d_%d",eleName.c_str(),int(sectorNum),int(i+1));
	if (eleName=="Si") {
	  if (m_nSectors==1) sprintf(nameBuf,"Si%d_%d",int(i+1),idx);
	  else sprintf(nameBuf,"Si%d_%d_%d",int(sectorNum),int(i+1),idx);
	  idx++;
	}
	std::string baseName(nameBuf);
	G4double thick = m_caloStruct[i].ele_thick[ie];
	//
	G4double extraWidth = 0;
	if (m_nSectors>1 && eleName=="W" && model_ != DetectorConstruction::m_FULLSECTION){
	 extraWidth = 10*mm;
	}
	if(thick>0){
#if 0
	  cout << "solid = constructSolid("<<baseName
	       <<",thick="<<thick
	       <<",zOffset+zOverburden="<<zOffset+zOverburden
	       <<",angOffset+minL="<<angOffset+minL
	       <<",width+extraWidth="<<width+extraWidth<<");"<<endl;
#endif
	  solid = constructSolid(baseName,thick,zOffset+zOverburden,angOffset+minL,width+extraWidth);
	  G4LogicalVolume *logi = new G4LogicalVolume(solid, m_materials[eleName], baseName+"log");
	  m_caloStruct[i].ele_X0[ie]   = m_materials[eleName]->GetRadlen();
	  m_caloStruct[i].ele_dEdx[ie] = m_dEdx[eleName];
	  m_caloStruct[i].ele_L0[ie]   = m_materials[eleName]->GetNuclearInterLength();
	  if (sectorNum==0 || sectorNum==m_nSectors-1) {
	    G4cout << "************ " << eleName ;
	    if (m_nSectors>1) G4cout << " sector " << sectorNum;
	    G4cout << " layer " << i << " dEdx=" << m_caloStruct[i].ele_dEdx[ie] << " X0=" << m_caloStruct[i].ele_X0[ie]
		   << " L0=" << m_caloStruct[i].ele_L0[ie] << " zpos=" << m_z0pos+zOverburden << "mm w=" << m_caloStruct[i].ele_thick[ie] << "mm";
	    //G4cout << " d=" << m_materials[eleName]->GetDensity();
	  //G4cout << G4endl;
	  //G4cout << *(m_materials[eleName]->GetMaterialTable()) << G4endl;
	    totalLengthX0 += m_caloStruct[i].ele_thick[ie]/m_caloStruct[i].ele_X0[ie]; G4cout << " TotX0=" << totalLengthX0;// << G4endl;
	    totalLengthL0 += m_caloStruct[i].ele_thick[ie]/m_caloStruct[i].ele_L0[ie]; G4cout << " TotLambda=" << totalLengthL0 << G4endl;
	  }

	  if (m_caloStruct[i].isSensitiveElement(ie)) m_logicSi.push_back(logi);
	  
	  G4double xpvpos = -m_CalorSizeXY/2.+minL+width/2+crackOffset;
	  if (model_ == DetectorConstruction::m_FULLSECTION) xpvpos=0;
#if 0
	  cout << "m_caloStruct[i].ele_vol[nEle*sectorNum+ie]=new G4PVPlacement(0, G4ThreeVector(xpvpos="<<xpvpos
	       << ",0.,zOffset+zOverburden+thick/2="<<zOffset+zOverburden+thick/2
	       << "), logi,"
	       << baseName+"phys, m_logicWorld, false, 0);" << endl;
#endif
	  m_caloStruct[i].ele_vol[nEle*sectorNum+ie]=
	    new G4PVPlacement(0, G4ThreeVector(xpvpos,0.,zOffset+zOverburden+thick/2), logi, baseName+"phys", m_logicWorld, false, 0);
	  //std::cout << " positionning layer at " << xpvpos << " 0 " << zOffset+zOverburden+thick/2 << std::endl;

	  G4VisAttributes *simpleBoxVisAtt= new G4VisAttributes(m_caloStruct[i].g4Colour(ie));
	  simpleBoxVisAtt->SetVisibility(true);
	  logi->SetVisAttributes(simpleBoxVisAtt);
	  zOverburden = zOverburden + thick;
	  //for sensitive volumes
	  //add region to be able to set specific cuts for it
	  //just for Si
	  if (eleName=="Si"){
	    unsigned nlogicsi = m_logicSi.size();
	    G4Region* aRegion = new G4Region(baseName+"Reg");
	    m_logicSi[nlogicsi-1]->SetRegion(aRegion);
	    aRegion->AddRootLogicalVolume(m_logicSi[nlogicsi-1]);
	  }
	}

      }//loop on elements
    }//loop on layers

}//buildstack

void DetectorConstruction::fillInterSectorSpace(const unsigned sectorNum,
						const G4double & minL, 
						const G4double & width)
{

  //build the stack
  G4double zOffset(-m_CalorSizeZ/2), zOverburden(0.);
  char nameBuf[10];
  G4VSolid *solid;

  for(size_t i=0; i<m_caloStruct.size(); i++)
    {
      G4double crackOffset = getCrackOffset(i);
      G4double angOffset = getAngOffset(i);

      if (model_ == DetectorConstruction::m_FULLSECTION) {
	crackOffset=0;
      }
      const unsigned nEle = m_caloStruct[i].n_elements;
      for (unsigned ie(0); ie<nEle;++ie){

	std::string eleName = m_caloStruct[i].ele_name[ie];
	G4double thick = m_caloStruct[i].ele_thick[ie];
	G4double extraWidth = 0;
	if (eleName=="W" && model_ != DetectorConstruction::m_FULLSECTION){
	 extraWidth = -10.*mm;
	 //std::cout << " -- total width: " << width+extraWidth << " offsets: " << crackOffset << " " << angOffset << std::endl;
	}
	eleName = "CFMix";
	sprintf(nameBuf,"%s%d_%d",eleName.c_str(),int(sectorNum),int(i+1));
	std::string baseName(nameBuf);
	if(thick>0){
	  solid = constructSolid(baseName,thick,zOffset+zOverburden,angOffset+minL,width+extraWidth);
	  G4LogicalVolume *logi = new G4LogicalVolume(solid, m_materials[eleName], baseName+"log");
	  G4double xpvpos = -m_CalorSizeXY/2.+minL+width/2+crackOffset;
	  if (model_ == DetectorConstruction::m_FULLSECTION) xpvpos=0;
	  G4PVPlacement *tmp = new G4PVPlacement(0, G4ThreeVector(xpvpos,0.,zOffset+zOverburden+thick/2), logi, baseName+"phys", m_logicWorld, false, 0);
	  //std::cout << "** positionning layer " << baseName << " at " << xpvpos << " 0 " << zOffset+zOverburden+thick/2 << std::endl;

	  G4VisAttributes *simpleBoxVisAtt= new G4VisAttributes(G4Colour::Magenta);
	  simpleBoxVisAtt->SetVisibility(true);
	  simpleBoxVisAtt->SetForceSolid(true);
	  logi->SetVisAttributes(simpleBoxVisAtt);
	  zOverburden = zOverburden + thick;
	}
      }//loop on elements
    }//loop on layers

}//fill intersector space

G4double DetectorConstruction::getCrackOffset(size_t layer){
  //model with 3 cracks identical by block of 10 layers
  //if (m_nSectors>1) return static_cast<unsigned>(layer/10.)*static_cast<unsigned>(m_sectorWidth/30.)*10;
  //with cracks shifted systematically layer-to-layer
  //if (m_nSectors>1) return 10*((7*layer)%31);
  //cracks shifted every two layers by 2cm
  //if (m_nSectors>1) return static_cast<unsigned>(layer/2.)*30;
  //cracks shifted every other layer by 2cm
  if (m_nSectors>1) return static_cast<unsigned>((layer%4)/2.)*30;

  return 0;
}

G4double DetectorConstruction::getAngOffset(size_t layer){
  if (model_ == DetectorConstruction::m_FULLSECTION){
    if (m_nSectors>1) return static_cast<unsigned>(layer/10.)*m_sectorWidth/3.;
    return 0;
  }
  return 0;
}
//
void DetectorConstruction::SetMagField(G4double fieldValue)
{

  if(fieldValue<=0) return; 

  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  if(m_magField) delete m_magField;                //delete the existing magn field
  m_magField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));
  fieldMgr->SetDetectorField(m_magField);
  fieldMgr->CreateChordFinder(m_magField);
  fieldMgr->SetDetectorField(m_magField);  
}

void DetectorConstruction::SetDetModel(G4int model)
{
  if (model <= 0) return;
  std::cout << " -- Setting detector model to " << model << std::endl;
  model_ = model;
}

void DetectorConstruction::SetWThick(std::string thick)
{
  if (thick.size() <= 0) return;
  std::cout << " -- Setting W thick to " << thick << std::endl;
  std::vector<std::string> vec;
  boost::split(vec, thick, boost::is_any_of(","));
  absThickW_.resize(vec.size(),0);
  for (unsigned iE(0); iE<vec.size(); ++iE){//loop on elements
    std::istringstream(vec[iE])>>absThickW_[iE];
    std::cout << absThickW_[iE] << " ";
  }
  std::cout << std::endl;
}

void DetectorConstruction::SetPbThick(std::string thick)
{
  if (thick.size() <= 0) return;
  std::cout << " -- Setting Pb thick to " << thick << std::endl;
  std::vector<std::string> vec;
  boost::split(vec, thick, boost::is_any_of(","));
  absThickPb_.resize(vec.size(),0);
  for (unsigned iE(0); iE<vec.size(); ++iE){//loop on elements
    std::istringstream(vec[iE])>>absThickPb_[iE];
    std::cout << absThickPb_[iE] << " ";
  }
  std::cout << std::endl;
}

void DetectorConstruction::SetDropLayers(std::string layers)
{
  dropLayer_.resize(28,false);
  if (layers.size() <= 0) return;
  std::cout << " -- Dropping layers " << layers << std::endl;
  std::vector<std::string> vec;
  boost::split(vec, layers, boost::is_any_of(","));
  for (unsigned iE(0); iE<vec.size(); ++iE){//loop on elements
    unsigned layerId = 0;
    std::istringstream(vec[iE])>>layerId;
    if (layerId>0 && layerId<29) dropLayer_[layerId-1] = true;
    else std::cout << " -- invalid layer to drop, ignoring..." << std::endl;
  }
  for (unsigned iE(0); iE<dropLayer_.size(); ++iE){//loop on elements
    std::cout << dropLayer_[iE] << " ";
  }
  std::cout << std::endl;
}

G4VSolid *DetectorConstruction::constructSolid (std::string baseName, G4double thick, G4double zpos,const G4double & minL, const G4double & width){
  
  G4VSolid *solid;
  if (model_ == DetectorConstruction::m_FULLSECTION){
    double minR = tan(2*atan(exp(-m_maxEta)))*(zpos+m_z0pos+m_CalorSizeZ/2);
    double maxR = tan(2*atan(exp(-m_minEta)))*(zpos+m_z0pos+m_CalorSizeZ/2);
    //std::cout << " zpos = " << zpos+m_z0pos+m_CalorSizeZ/2 << " radius range " << minR << " " << maxR << std::endl;
    solid = new G4Tubs(baseName+"box",minR,maxR,thick/2,minL,width);
  }
  else if (model_ == DetectorConstruction::m_2016TB){
    std::cout << " zpos = " << zpos << "-" << zpos+thick << " hexagon with side " << m_maxRadius << std::endl;
    G4double zPlane[2] = { -thick/2,thick/2};
    G4double rInner[2] = { 0, 0 };
    G4double rOuter[2] = { m_minRadius, m_minRadius }; // feed center-to-side distance to G4Polyhedra
    solid = new G4Polyhedra(baseName+"hexa",0,2*pi,6,2,zPlane,rInner,rOuter); // startphi angle points to a corner
  }
  else{
    solid = new G4Box(baseName+"box", width/2, m_CalorSizeXY/2, thick/2 );
  }
  return solid;
}

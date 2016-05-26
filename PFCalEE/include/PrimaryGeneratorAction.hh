//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id$
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include <map>
#include <string>
#include <fstream>

class G4Event;
class G4ParticleGun;
//class HepMCG4AsciiReader;
//class HepMCG4PythiaInterface;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(G4int mod = 0);    
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* anEvent);

  // set methods
  bool SetInputFile(std::string filename);

  bool goodStream();

private:
  bool GetNextPrimaryStats();

private:
    
  DetectorConstruction*      fDetector;
  PrimaryGeneratorMessenger* fPrimaryGenMessenger;
    
  G4ParticleGun*             fParticleGun;
  std::ifstream*             fEventStream;
  int                        fNEventsRead;
  G4String                   fInputFile;
  
  std::vector<double>        energies;
  std::vector<int>           pdgs;
  std::vector<G4ThreeVector> positions;
  std::vector<G4ThreeVector> momenta;

  int model_;
  
};

#endif

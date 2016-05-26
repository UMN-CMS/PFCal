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

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "Randomize.hh"

#include "HepMCG4AsciiReader.hh"
#include "HepMCG4PythiaInterface.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(G4int mod)
: G4VUserPrimaryGeneratorAction(),
fParticleGun(0),
fEventStream(0),
fNEventsRead(0),
fInputFile("") {
    model_ = mod;

    // default generator is particle gun.
    fParticleGun = new G4ParticleGun();

    fDetector = (DetectorConstruction*)
            G4RunManager::GetRunManager()->GetUserDetectorConstruction();
    fPrimaryGenMessenger = new PrimaryGeneratorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
    delete fPrimaryGenMessenger;

    fEventStream->close();
    delete fEventStream;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool PrimaryGeneratorAction::SetInputFile(std::string filename) {
    // If stream was previously defined/linked to another file, close and delete
    // first.
    if (fEventStream != 0) {
        fEventStream->close();
        delete fEventStream;
    }
    fInputFile = filename;
    // Open new stream
    fEventStream = new std::ifstream(filename.c_str());
    fNEventsRead = 0;

    // Check it, exit if open failed.
    if (!goodStream()) {
        G4ExceptionDescription msg;
        msg << "Input file not set successfully, please check that file "
                << filename << " exists and is readable."
                << G4endl;
        G4Exception("PrimaryGeneratorAction::SetInputFile()",
                "MyCode0001", JustWarning, msg);

        return false;
    }

    // Advance to the beginning of events.  
    std::string line = "";
    while ((line.length() != 7 || line.substr(0, 7).compare("<event>") != 0) && goodStream()) {

        std::getline(*fEventStream, line);
    }
    return goodStream();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool PrimaryGeneratorAction::GetNextPrimaryStats() {

    std::string line = "";

    bool gotStats = false;
    while (std::getline(*fEventStream, line) && goodStream() && (line.length() != 8 || line.substr(0, 8) != "</event>")) {
        
        std::istringstream ss(line);
        int pid;
        double energy;
        double px = 0, py = 0, pz = 0;
        double x = 0, y = 0, z = 0;
        if (!(ss >> pid >> x >> y >> z >> px >> py >> pz >> energy)) { 
           break;
        }
        
        pdgs.push_back(pid);
        energies.push_back(energy*GeV);
        positions.push_back(G4ThreeVector(x*cm,y*cm,z*cm));
        momenta.push_back(G4ThreeVector(px,py,pz));
        
        gotStats = true;
    }
    
    std::getline(*fEventStream, line);

    if (line.substr(0, 9) == "</event>") {
        return false;
    }
    if (!gotStats) {
        G4ExceptionDescription msg;
        msg << "Failed to read primary momentum from file after " << fNEventsRead
                << " good Events" << G4endl;
        G4Exception("PrimaryGeneratorAction::GetNextPrimaryStats()",
                "MyCode0003", RunMustBeAborted, msg);
        return false;
    }

    ++fNEventsRead;

    if (!goodStream()) {
        G4ExceptionDescription msg;
        msg << "Error or EOF in reading primary events from file after " << fNEventsRead
                << " good Events" << G4endl;
        G4Exception("PrimaryGeneratorAction::GetNextPrimaryStats()",
                "MyCode0004", JustWarning, msg);
    }
    return true;
}

bool PrimaryGeneratorAction::goodStream() {
    if (fEventStream == 0) return false;
    return fEventStream->good();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

    if (fEventStream == 0) {
        G4ExceptionDescription msg;
        msg << "Must set input file using SetInputFile method or "
                << "/file/inputFilename/ UI command"
                << G4endl;
        G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
                "MyCode0006", RunMustBeAborted, msg);
        return;
    }

    if (!goodStream()) {
        G4ExceptionDescription msg;
        msg << "Cannot generate any more primary events because input file is unusable."
                << G4endl;
        G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
                "MyCode0005", RunMustBeAborted, msg);
        return;
    }

    if (!GetNextPrimaryStats()) {
        G4Exception("G4HEPEvtInterface::GeneratePrimaryVertex", "Event0202", JustWarning, "End-Of-File : HEPEvt input file");
        return;
    }

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    //Call the gun for each particle in the event
    for (size_t i = 0; i < pdgs.size(); i++) {
    
        fParticleGun->SetParticlePosition(positions[i]);
        fParticleGun->SetParticleMomentumDirection(momenta[i]);
        fParticleGun->SetParticleEnergy(energies[i]);

        G4ParticleDefinition* particle = particleTable->FindParticle(pdgs[i]);
        fParticleGun->SetParticleDefinition(particle);
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }

    pdgs.clear();
    energies.clear();
    positions.clear();
    momenta.clear();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


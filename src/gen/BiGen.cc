// RAT::BiGen
// 16-Jan-2015 Teal Pershing
// Original CfSource format by WGS

// See comments in RATBiGen.hh

#include <RAT/BiGen.hh>
#include <RAT/BiSource.hh>

#include <RAT/GLG4PosGen.hh>
#include <RAT/GLG4TimeGen.hh>
#include <RAT/Factory.hh>
#include <RAT/GLG4StringUtil.hh>

#include <G4Event.hh>
#include <G4PrimaryVertex.hh>
#include <G4PrimaryParticle.hh>
#include <G4ParticleDefinition.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4ThreeVector.hh>
#include <G4UnitsTable.hh>

#include <CLHEP/Vector/LorentzVector.h>

#include <cstring>

#define DEBUG

namespace RAT {

  BiGen::BiGen() : 
    stateStr(""), 
    isotope(214),
    posGen(0)
  {
    // As in the combo generator, use a default time generator if the
    // user does not supply one.
    timeGen = new GLG4TimeGen_Poisson();

    // Initialize the decay particles.
    electron = G4Electron::Electron();
    gamma   = G4Gamma::Gamma();
  }

  BiGen::~BiGen()
  {
    delete timeGen;
    delete posGen;
  }

  void BiGen::GenerateEvent(G4Event* event)
  {
    // Initialize a new Bi beta decay.
    BiSource biSource(isotope);

    // Generate the position of the isotope.  Note that, for now, we
    // don't change the position of the isotope as it decays.
    G4ThreeVector position;
    posGen->GeneratePosition(position);

    // Number of electrons and photons produced in the beta decay.
    int numberElectrons = biSource.GetNumElectron();
    int numberGammas   = biSource.GetNumGamma();

#ifdef DEBUG
    G4cout << "RAT::BiGen::GenerateEvent: "
	   << numberElectrons << " electrons, "
	   << numberGammas << " photons" << G4endl;
#endif

    // For each electron... since only one electron, can get rid of this later
    for ( int i = 0; i < numberElectrons; i++ ) 
      {
	double t = biSource.GetBiElectronTime(i);
        CLHEP::HepLorentzVector p = biSource.GetBiElectronMomentum(i);
	
	// Adjust the time from the t0 of the event.
	G4double time = NextTime() + t;

	// generate a vertex with a primary particle
	G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
	G4PrimaryParticle* particle = 
	  new G4PrimaryParticle(electron,
				p.px(), 
				p.py(), 
				p.pz());
	particle->SetMass(electron->GetPDGMass()); // Apparently this is useful in IBD code.
	vertex->SetPrimary(particle);
	event->AddPrimaryVertex(vertex);

#ifdef DEBUG
	G4cout << "RAT::BiGen::GenerateEvent: "
	       << "Electron " << i << " of " << numberElectrons
	       << "    time=" << G4BestUnit(time,"Time")
	       << ", position=" << G4BestUnit(position,"Length")
	       << ", momentum=" << G4BestUnit(p,"Energy")
	       << G4endl;
#endif

      } // for each electron

    // For each photon from beta decay...
    for ( int i = 0; i < numberGammas; i++ ) 
      {
        CLHEP::HepLorentzVector p = biSource.GetBiGammaMomentum(i);
	double t = biSource.GetBiGammaTime(i);

	// Adjust the time from the t0 of the event.
	G4double time = NextTime() + t;

	// generate a vertex with a primary particle
	G4PrimaryVertex* vertex = new G4PrimaryVertex(position, time);
	G4PrimaryParticle* particle = 
	  new G4PrimaryParticle(gamma,
				p.px(), 
				p.py(), 
				p.pz());
	particle->SetMass(gamma->GetPDGMass()); // Who knows?  Let's do this the same way as the others.
	vertex->SetPrimary(particle);
	event->AddPrimaryVertex(vertex);

#ifdef DEBUG
	G4cout << "RAT::BiGen::GenerateEvent: "
	       << "Gamma " << i << " of " << numberGammas
	       << "    time=" << G4BestUnit(time,"Time")
	       << ", position=" << G4BestUnit(position,"Length")
	       << ", momentum=" << G4BestUnit(p,"Energy")
	       << G4endl;
#endif

      } // for each photon from beta decay

  }


  void BiGen::ResetTime(double offset)
  {
    double eventTime = timeGen->GenerateEventTime();
    nextTime = eventTime + offset;
#ifdef DEBUG
    G4cout << "RAT::BiGen::ResetTime:"
	   << " eventTime=" << G4BestUnit(eventTime,"Time")
	   << ", offset=" << G4BestUnit(offset,"Time")
	   << ", nextTime=" << G4BestUnit(nextTime,"Time")
	   << G4endl;
#endif
  }
 
  void BiGen::SetState(G4String state)
  {
#ifdef DEBUG
    G4cout << "RAT::BiGen::SetState called with state='"
	   << state << "'" << G4endl;
#endif

    // Break the argument to the this generator into sub-strings
    // separated by ":".
    state = util_strip_default(state);
    std::vector<std::string> parts = util_split(state, ":");
    size_t nArgs = parts.size();

#ifdef DEBUG
    G4cout << "RAT::BiGen::SetState: nArgs="
	   << nArgs << G4endl;
#endif

    try {

      if ( nArgs >= 3 )
	{
	  // The last argument is an optional time generator
	  delete timeGen; timeGen = 0; // In case of exception in next line
	  timeGen = GlobalFactory<GLG4TimeGen>::New(parts[2]);
	}

      if ( nArgs >= 2 )
	{
	  // The first argument is the Bismuth isotope.  At
	  // present, only Bi214 is supported.
	  isotope = util_to_int( parts[0] );

	  if ( isotope != 214 ) 
	    {
	      G4cerr << "RAT::BiGen::SetState: Only Bi 214 is supported"
		     << G4endl;
	    }

	  // The second argument is a position generator.
	  delete posGen; posGen = 0;
	  posGen = GlobalFactory<GLG4PosGen>::New(parts[1]);
	}
      else
	{
	  G4Exception(__FILE__, "Invalid Parameter", FatalException, ("BiGen syntax error: '" +
								      state +
								      "' does not have a position generator").c_str());
	}

      stateStr = state; // Save for later call to GetState()
    } catch (FactoryUnknownID &unknown) {
      G4cerr << "Unknown generator \"" << unknown.id << "\"" << G4endl;
    }
  }

  G4String BiGen::GetState() const
  {
    return stateStr;
  }

  void BiGen::SetTimeState(G4String state)
  {
    if (timeGen)
      timeGen->SetState(state);
    else
      G4cerr << "BiGen error: Cannot set time state, no time generator selected" << G4endl;
  }

  G4String BiGen::GetTimeState() const
  {
    if (timeGen)
      return timeGen->GetState();
    else
      return G4String("BiGen error: no time generator selected");
  }

  void BiGen::SetPosState(G4String state)
  {
    if (posGen)
      posGen->SetState(state);
    else
      G4cerr << "BiGen error: Cannot set position state, no position generator selected" << G4endl;
  }

  G4String BiGen::GetPosState() const
  {
    if (posGen)
      return posGen->GetState();
    else
      return G4String("BiGen error: no position generator selected");
  }

} // namespace RAT

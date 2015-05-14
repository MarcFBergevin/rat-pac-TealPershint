// RAT::BiGen
// 16-March-2015 WGS

// Implements a GLG4Sim-style generator for the spontaneous fission of
// Bismuth.  The class we use is BiSource, prepared by Teal Pershing.  Original format (i.e. format of CfSource class) written by Matt Worcester.

// To use this generator, the command is:

// /generator/add bi ISOTOPE:POSITION[:TIME]

// (that is, the TIME is optional).  ISOTOPE should be 214, since that
// is the only one currently supported.  POSITION and TIME are the
// same as for the combo generator.  For example:

// /generator/add bi 214:fill

// Note that there is also no "defering" of any tracks of emitted
// particles into other events.  "TIME" refers to the t0 of the
// fission; the default is flat time distribution with a rate of 1 ns.

//Dependencies:
//beta-decays.dat: data file containing decay chains, beta decay end-points, and
//alpha decay energies.  Needed to generate the Bismuth decay_mode struct that
//generates events in BiSource.cc
//datpull.cc: c++ file that pulls the information from beta-decays.dat and
//puts it into structs which can be called in the BiSource file.

#ifndef __RAT_BiGen__
#define __RAT_BiGen__

#include <GLG4Gen.hh>

#include <globals.hh>

class G4Event;
class G4ParticleDefinition;
class GLG4TimeGen;
class GLG4PosGen;

namespace RAT {

  class BiGen : public GLG4Gen {
  public:
    BiGen();
    virtual ~BiGen();
    virtual void GenerateEvent(G4Event *event);
    virtual void ResetTime(double offset=0.0);
    virtual bool IsRepeatable() const { return true; };

    virtual void SetState(G4String state);
    virtual G4String GetState() const;

    virtual void SetTimeState(G4String state);
    virtual G4String GetTimeState() const;
    virtual void SetPosState(G4String state);
    virtual G4String GetPosState() const;

  protected:

    // Generator initialization, specified by the user.
    G4String stateStr;

    // Bismuth isotope.
    int isotope;

    // The time and position generators specified by the user.
    GLG4TimeGen* timeGen;
    GLG4PosGen* posGen;

    // The BiSource event model only generate electrons and photons.
    G4ParticleDefinition* electron;
    G4ParticleDefinition* gamma;
  };

} // namespace RAT

#endif // RAT_BiGen_h

/** \file BiSource.hh
 *  BiSource header file.
 *
 *  Author: Teal Pershing
 */
#ifndef __RAT_BiSource__
#define __RAT_BiSource__

#include <CLHEP/Vector/LorentzVector.h>

#include <cmath>
#include <iostream>
#include <vector>

/** BiSource generates the electron and different gammas from
 *  radioactive Bismuth decays.  The supported isotope is 214. 
 *  Update functionality as new features are added.
 */

namespace RAT {

  class BiSource{

  public:

    /** BiSource constructor.  Use a default isotope of Bi214, which
     * currently is the only isotope implement in this class. */
    BiSource(int newIsotope = 214);

    /** BiSource destructor. */
    ~BiSource();

    /** BiSource copy constructor. */
    BiSource(const BiSource& BiSource);

    /** BiSource overloaded = operator */
    BiSource& operator=(const BiSource& rhs);

    /** Returns the electron number.  Always one for single beta decays. */
    int GetNumElectron() const {return Nelectron;}


    /** Returns the prompt photon multiplicity */
    int GetNumGamma() const {return Ngamma;}

    /** Returns the energy of the electrons produced by each Bi decay.
     *  Called with the integer index for each electron in the electronE
     *  array, from 0 to the total number of electrons. */
    CLHEP::HepLorentzVector GetBiElectronMomentum(int n) const {return electronE[n];}
    double GetBiElectronTime(int n) const {return Telectron[n];}

    /** Returns the energy of the gammas produced by each Bi decay.
     *  Called with the integer index for each gamma in the gammaE
     *  array, from 0 to the total number of gammas/source. */
    CLHEP::HepLorentzVector GetBiGammaMomentum(int n) const {return gammaE[n];}
    double GetBiGammaTime(int n) const {return Tgamma[n];}

  private:

    int Isotope;

    static const int maxElectron = 1;  //Only single beta decays.
    static const int maxGamma = 6;    //Max gammas in beta-decays.dat
    int Nelectron, Ngamma;

    std::vector<CLHEP::HepLorentzVector> electronE;
    std::vector<float> Telectron;

    /**
     *  Array containing the momentum of the gammas from
     *  each Bi decay, indexed from 0 to the total number of
     *  gammas. 
    **/

    std::vector<CLHEP::HepLorentzVector> gammaE;
    std::vector<float> Tgamma;

    // G4 particle definitions.
    static double massElectron;

  };
} // namespace RAT

#endif

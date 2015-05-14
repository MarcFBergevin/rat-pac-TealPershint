/** \file BiSource.cc
 *  BiSource C++ file.  Modeled after BiSource.cc file written by
 *  Matthew Worcester. 
 *  Author: Teal Pershing
 */


#include <RAT/BiSource.hh>

#include <G4ParticleDefinition.hh>
#include <G4Gamma.hh>
#include <G4Electron.hh>

#include <CLHEP/Random/RandFlat.h>
#include <CLHEP/Random/RandGeneral.h>
#include <CLHEP/Vector/LorentzVector.h>
#include "datpull.hh"
#include <cmath>
#include <iostream>
#include <fstream>   // file I/O
#include <iomanip>   // format manipulation
#include <cstring>
#include <vector>

#define DEBUG

namespace RAT {


  double BiSource::massElectron = 0.; // allocate storage for static variable

  BiSource::BiSource(int newIsotope) :
    Isotope(newIsotope)
  {
    // Bi214
    if(Isotope == 214){

      // Verify that all maps and vectors are empty.
      Nelectron = 0;
      Ngamma = 0;
      electronE.clear();
      Telectron.clear();
      gammaE.clear();
      Tgamma.clear();

	/**Needs to be cleaned up. Some things to do:
	 * 1. have "theiso" pull the Isotope number that was inputted.
	 * 2. Write one clean function that gets a decay mode rather than
         *    the three inputs that are below.
	**/
 
	std::string theiso = "214Bi";
	std::list<Chain> thechains;
	std::list<Decay> thedecays;
	GetAllDecays(&thechains, &thedecays);

#ifdef DEBUG
	/**
	std::cout << "Print the names in the decays just for checking.." << std::endl;
	for (std::list<Decay>::iterator i = thedecays.begin(); i != thedecays.end(); ++i)
		cout << i->name << endl;
	**/
#endif
	

	Decay Bismuth = GetSpecificDecay(&thedecays, theiso);

	double r = CLHEP::RandFlat::shoot();
	Decay_Mode BiDecay = GetADecayMode(&Bismuth.decay_modes, r);	
	
	// Only one beta for all of beta_decays.dat, so keep as 1 for now.
	Nelectron = 1;
	double electronKE = BiDecay.beta_end_point;
	double energy = massElectron + electronKE;
	// Generate momentum direction uniformly in phi and cos(theta).
	double phi = CLHEP::RandFlat::shoot(0.,M_PI);
	double cosTheta = CLHEP::RandFlat::shoot(-1.,1.);
	double sinTheta = sqrt( 1. - cosTheta*cosTheta );
	double px = electronKE * sinTheta * cos(phi);
	double py = electronKE * sinTheta * sin(phi);
	double pz = electronKE * cosTheta;
#ifdef DEBUG
	std::cout << "BiSource::BiSource() - electron energy " 
		  << ", KE=" << electronKE
		  << ", (px,py,pz)=("
		  << px << "," << py << "," << pz << ")"
		  << std::endl;
#endif
	  CLHEP::HepLorentzVector momentum(px,py,pz,energy);
	  electronE.push_back( momentum );
	  Telectron.push_back( 0. );


      // pick a momentum for each gamma
      //
      double tote = 0.;
      Ngamma = BiDecay.num_gamma; 
      for(int nn=0; nn<Ngamma; nn++)
	{
	  double energy = BiDecay.excgamma[nn]; 
	  double phi = CLHEP::RandFlat::shoot(0.,M_PI);
	  double cosTheta = CLHEP::RandFlat::shoot(-1.,1.);
	  double sinTheta = sqrt( 1. - cosTheta*cosTheta );
	  double px = energy * sinTheta * cos(phi);
	  double py = energy * sinTheta * sin(phi);
	  double pz = energy * cosTheta;
#ifdef DEBUG
	  std::cout << "BiSource::BiSource() - gamma energy " 
		    << nn << " = " << energy
		    << ", (px,py,pz)=("
		    << px << "," << py << "," << pz << ")"
		    << std::endl;
#endif
	  CLHEP::HepLorentzVector momentum(px,py,pz,energy);
	  gammaE.push_back( momentum );
	  tote += energy;

	/**The times currently in place were for a Californium source.
	 * The Bismuth likely does not have the same timing for when they
	 * are emitted from the daughter nucleus.  Check what the times
	 * are for Bismuth.
	**/
	  
	  const size_t len2 = 2;
	  float rv[len2];
	  for ( size_t i=0; i<len2; i++ ) rv[i] = CLHEP::RandFlat::shoot();
	  //
	  // 80% of gammas have T_1/2 = 0.01 ns and 20% have T_1/2 = 1 ns
	  //
	  double halflife;
	  if(rv[0] < 0.8)
	    halflife = 0.01;
	  else
	    halflife = 1.0;
	  Tgamma.push_back( halflife*log(1/rv[1]) );
	}
      //std::cout << "          total energy = " << tote << std::endl;

    } // done with Bi214 

  }

  BiSource::~BiSource()
  {;}

  BiSource::BiSource(const BiSource& BiSource)
  {
    Isotope   = BiSource.Isotope;
    Nelectron  = BiSource.Nelectron;
    Ngamma    = BiSource.Ngamma;
    electronE  = BiSource.electronE;
    Telectron  = BiSource.Telectron;
    gammaE    = BiSource.gammaE;
    Tgamma    = BiSource.Tgamma;
  }    

  BiSource& BiSource::operator=(const BiSource& rhs){

    if (this != &rhs)
      {
	Isotope   = rhs.Isotope;  
	Nelectron  = rhs.Nelectron;
	Ngamma    = rhs.Ngamma;
	electronE  = rhs.electronE;
	Telectron  = rhs.Telectron;
	gammaE    = rhs.gammaE;
	Tgamma    = rhs.Tgamma;
      }
    return *this;
  }    
} // namespace RAT

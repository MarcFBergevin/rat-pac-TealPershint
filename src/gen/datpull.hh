#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
using std::string;
using std::getline;
using std::cout;
using std::endl;

typedef struct 
{
	string name;
	int branch;
        double br_ratio;
        int decay_mode;
        double half_life;
}Daughter;


typedef struct
{
	double prob_step;
	int beta_type;
	double beta_end_point;
	int num_gamma;
	double excgamma[6];
	double gamma1;
	double gamma2;
	double gamma3;
	double gamma4;
	double gamma5;
	double gamma6;
	double beta_decay_coeff_1;
	double beta_decay_coeff_2;
	double beta_decay_coeff_3;
}Decay_Mode;

typedef struct
{
	string name;
	std::list<Daughter> daughters;
}Chain;


typedef struct
{
	string name;
	int Z;
	int Amass;
	double half_life_s;
	std::list<Decay_Mode> decay_modes;
}Decay;


std::string trim(const std::string& str, const std::string& whitespace);
Decay_Mode GetEvent(std::list<Chain> chains, std::list<Decay> &decays, const std::string theiso, double a);
void GetAllDecays(std::list<Chain> *chain_list, std::list<Decay> *decay_list);
Decay GetSpecificDecay(std::list<Decay> *decays, const std::string isotope);
Decay_Mode GetADecayMode(std::list<Decay_Mode> *isodecaymodes, double a);




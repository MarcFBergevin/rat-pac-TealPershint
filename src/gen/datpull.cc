#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "datpull.hh"

using std::string;
using std::getline;
using std::cout;
using std::endl;

std::string trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    std::size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    std::size_t strEnd = str.find_last_not_of(whitespace);
    std::size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
char delim[] = " \t";



#define DEBUG
Decay GetSpecificDecay(std::list<Decay> *decays, const std::string isotope)
{
	Decay garbage;
	const char* ourbusiness = isotope.c_str();
	for (std::list<Decay>::iterator i = decays->begin(); i !=decays->end(); ++i)
	{
		const char* thename = i->name.c_str();
		//go Through the names of Decays and STRCMP with string put in, 
		if (!strcmp(thename, ourbusiness))
			return *i;
	}
	cout << "The isotope input does not match any isotope in the list of decays provided." <<
	        "Check your isotope format matches that in beta-decays.dat.  Returning garbage." <<
		"Sorry."  << endl;
	return garbage;
}


Decay_Mode GetEvent(std::list<Chain> chains, std::list<Decay> &decays, const std::string theiso, double a)
{
	Decay Bismuth = GetSpecificDecay(&decays, theiso);
	Decay_Mode themode=GetADecayMode(&Bismuth.decay_modes, a);
	return themode;
}



Decay_Mode GetADecayMode(std::list<Decay_Mode> *isodecaymodes, double a)
{
	Decay_Mode garbage;
	for (std::list<Decay_Mode>::iterator i = isodecaymodes->begin(); i !=isodecaymodes->end(); ++i)
	{
		if (i->prob_step > a)
			return *i;	
	}
	cout << "Error with probability step.  Either input probability is out of range or " <<
	        "a bad list of isotope decay modes was chosen.  Returning garbage.  Sorry." << endl;
	return garbage;
}


void GetAllDecays(std::list<Chain> *chain_list, std::list<Decay> *decay_list)
{
	std::ifstream myfile;
	string line;

    // See if the $RATDecayDataDir environment variable exists.
    char* path = getenv("RATDecayDataDir");
    std::string dirName;
    if ( path == 0 )
      {
	// RATDecayDataDir enviornment variable not found.  
	// Check to see if RATROOT is set
	path = getenv("RATROOT");
	if( path == 0 )
	  {
	    // Environment variable not found... so just use a name
	    // relative to the current directory.
	    dirName = "./data";
	  }
	else
	  {
	    // RATROOT enviornment variable found.
	    // Add data subdir to get the correct directory. 
	    // CURRENTLY VEEERY DEPENDENT ON POSITION.  MUST BE A MORE GENERAL WAY...
	       dirName = std::string(path) + "/data";
	  }
      }
    else
      {
	dirName = std::string(path);
      }
    std::string fileName = dirName + "/beta_decays.dat";
    //SetFileName(fileName);

	myfile.open(fileName.c_str());


	if(myfile.is_open())
	{
		while(getline(myfile,line))
		{
			string trimmed = trim(line);                  //Trims the leading and trailing whitespace

			if(trimmed.compare(""))                       //Is true if the two are NOT the same.  Odd.
			{
				if(trimmed[0]=='#' || trimmed[0]=='*')
					continue;
				char c_trimmed[100];
				strcpy(c_trimmed,trimmed.c_str());
				char *tok_ret = strtok(c_trimmed,delim);  //Pulls the character string from the file up to the specified delimiter
				if(!strcmp(tok_ret,"CHAIN:"))           //Get information for a chain of decays
				{
					Chain chain;
					chain.name = strtok(NULL,delim);  //strtok inserts NULLS as it goes along, starts from that NULL and goes to delimiter

					getline(myfile,line);
					int end = atoi(line.c_str());     //Number of objects on line after CHAIN

					for(int i = 0; i < end; ++i)      //Pull information for a daughter
					{
						getline(myfile,line);

						char c_trimmed_2[100];
						strcpy(c_trimmed_2,trim(line).c_str());
						char *tok_ret = strtok(c_trimmed_2,delim);

						Daughter daught;
						daught.name = tok_ret;
						daught.branch = atoi(strtok(NULL,delim));
						daught.br_ratio = atof(strtok(NULL,delim));
						daught.decay_mode = atoi(strtok(NULL,delim));
						daught.half_life = atof(strtok(NULL,delim));

						chain.daughters.push_back(daught);      //Put this Daughter struct into the Chain struct
					}

					chain_list->push_back(chain);
				}
				else if(!strcmp(tok_ret,"DECAY:") || !strcmp(tok_ret,"ALPHA:"))  //Pull information for a specific decay
				{
					Decay decay;
					decay.name = strtok(NULL,delim);

					getline(myfile,line);
					char c_trimmed_3[100];
					strcpy(c_trimmed_3,trim(line).c_str());
					char *tok_ret = strtok(c_trimmed_3,delim);


					decay.Z = atoi(tok_ret);
					decay.Amass = atoi(strtok(NULL,delim));
					decay.half_life_s = atof(strtok(NULL,delim));


					line = "garbage";
					while(trim(line) != "")
					{
						getline(myfile,line);

						if(trim(line) != "")
						{
							char c_trimmed_2[100];
							strcpy(c_trimmed_2,trim(line).c_str());
							char *tok_ret = strtok(c_trimmed_2,delim);

							Decay_Mode decay_mode;

							decay_mode.prob_step = atof(tok_ret);
							decay_mode.beta_type = atoi(strtok(NULL,delim));
							decay_mode.beta_end_point = atof(strtok(NULL,delim));
							decay_mode.num_gamma = atoi(strtok(NULL,delim));
							for (int i=0; i<6; ++i)
								decay_mode.excgamma[i] = atof(strtok(NULL, delim));
							/**
							decay_mode.gamma1 = atof(strtok(NULL,delim));
							decay_mode.gamma2 = atof(strtok(NULL,delim));
							decay_mode.gamma3 = atof(strtok(NULL,delim));
							decay_mode.gamma4 = atof(strtok(NULL,delim));
							decay_mode.gamma5 = atof(strtok(NULL,delim));
							decay_mode.gamma6 = atof(strtok(NULL,delim));
							**/
							decay_mode.beta_decay_coeff_1 = atof(strtok(NULL,delim));
							decay_mode.beta_decay_coeff_2 = atof(strtok(NULL,delim));
							decay_mode.beta_decay_coeff_3 = atof(strtok(NULL,delim));

							decay.decay_modes.push_back(decay_mode);
						}
					}

					decay_list->push_back(decay);
				}

				while((tok_ret=strtok(NULL,delim)));				
			}
		}
		myfile.close();
	}	
}




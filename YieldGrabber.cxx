//**********************************************************************************************************************//
// Author: Jack Henderson (TRIUMF)
// jhenderson@triumf.ca
//
// YieldGrabber.cxx 
// Class to grab TRIUMF yield data from .dat files corresponding to the available targets. Then allows for the plotting
// of yields vs N and Z, and printing of yields for individual isotopes through inputting of N and Z values, or through
// point-and-click functionality on yield histograms via communication with the MouseAction subclass (MouseAction.h)
//
// For use with the ROOT framework
//
//**********************************************************************************************************************//

#define YieldGrabber_cxx

#include "MouseAction.h"
#include "YieldGrabber.h"
#include "LinkDef.h"

void YieldGrabber::CreateIntensityMatrix(int source){

	cout << "Setting up average/maximum yields" << endl;

	double counter;
	double sum;
	double max;

	if(source > 0)
	{

		for(int i=0;i<146;i++){
			for(int j=0;j<94;j++){
				counter = 0;
				sum = 0;
				max = 0;
				Nuclei[i][j].avg_yield=0;
				Nuclei[i][j].max_yield=0;
				for(unsigned int k=0; k<Nuclei[i][j].yield.size(); k++){
					if(IonSourceComparison(Nuclei[i][j].ion_source.at(k)) == source){
						if(Nuclei[i][j].yield.at(k) > max)
							max = Nuclei[i][j].yield.at(k);
						sum = sum + Nuclei[i][j].yield.at(k);
						counter++;
					}
				}
				if(sum>0)
					Nuclei[i][j].avg_yield = (double) (sum/counter);
				Nuclei[i][j].max_yield = max;
			}
		}

	}
	else 
	{

		for(int i=0;i<146;i++){
			for(int j=0;j<94;j++){
				counter = 0;
				sum = 0;
				max = 0;
				Nuclei[i][j].avg_yield=0;
				Nuclei[i][j].max_yield=0;
				for(unsigned int k=0; k<Nuclei[i][j].yield.size(); k++){
					if(Nuclei[i][j].yield.at(k) > max)
						max = Nuclei[i][j].yield.at(k);
					sum = sum + Nuclei[i][j].yield.at(k);
					counter++;
				}
				if(sum > 0)
					Nuclei[i][j].avg_yield = (double) (sum/counter);
				if(max>0)
					Nuclei[i][j].max_yield = max;
			}
		}


	}

}

void YieldGrabber::ProcessData(){

	vector< std::string > line;
	std::string temp;
	std::string stab_test;
	std::string A_string;

	for(unsigned int i=0;i<data.size();i++){

		line = data.at(i);
		istringstream convert1(line.at(0));
		int z_val;
		if(!(convert1 >> z_val))
			z_val=0;
		temp = line.at(1);
		for(unsigned int j=0;j<temp.size()-1;j++)
			if(isdigit(temp[j]))
				A_string.push_back(temp[j]);

		int state_val;
		if(temp[temp.size()-2] == 'm'){
			state_val = (int)(temp[temp.size()-1] - '0');
			state_val++;
		}
		else
			state_val = 1;

		int a_val;
		istringstream convert2(A_string);
		if(!(convert2 >> a_val))
			a_val=0;
		int n_val = a_val - z_val;

		double d;
		temp = line.at(2);
		istringstream convert3(temp);
		if(!(convert3 >> d))
			d=0;

		istringstream convert4(line.at(3));
		int p_current;
		if(!(convert4 >> p_current))
			p_current=0;

		temp = line.at(4);
		if(temp.compare("Re")==0)
		{
			temp = "Re surface";
		}
		else if(temp.compare("Ta")==0)
		{
			temp = "Ta surface";
		}		
	
		A_string.clear();

		Nuclei[n_val-1][z_val-1].yield.push_back(d);
		Nuclei[n_val-1][z_val-1].proton_current.push_back(p_current);
		Nuclei[n_val-1][z_val-1].ion_source.push_back(temp);
		Nuclei[n_val-1][z_val-1].state.push_back(state_val);	
		Nuclei[n_val-1][z_val-1].tar_mat.push_back(target_mat.at(i));

	}

	line.clear();
	data_grabbed = true;

}

void YieldGrabber::PrintIsotopeInfo(){

	PrintIsotopeInfo(z_stored,n_stored);

}

void YieldGrabber::PrintIsotopeInfo(int z_in, int n_in){

	int z = z_in;
	int n = n_in;

	if(!data_grabbed)
		Start();
	
	PrintMeanIntensity(n_in-1,z_in-1);

	int n_states=0;
	for(unsigned int i=0;i<Nuclei[n-1][z-1].yield.size();i++){
		if(Nuclei[n-1][z-1].state.at(i) > n_states)
			n_states = Nuclei[n-1][z-1].state.at(i);
	}

	cout << "Number of states (ground + metastable): " << n_states << endl;
	
	for(int j=0;j<n_states;j++){
		if(j>0)
			cout << "Nucleus: " << z_in+n_in << element[z-1] << "m" << j << endl;
		else
			cout << "Nucleus: " << z_in+n_in << element[z-1] << endl;

		cout << "Yield:\t\t\t\tProton current:\t\tTarget:\t\t\tIon source:" << endl;
		for(unsigned int i=0;i<Nuclei[n-1][z-1].yield.size();i++){
			if(Nuclei[n-1][z-1].state.at(i) == j+1)
				cout << std::setw(10) << std::setprecision(5) << std::scientific << Nuclei[n-1][z-1].yield.at(i) << "\t\t\t" << Nuclei[n-1][z-1].proton_current.at(i) << "\t\t\t" << Nuclei[n-1][z-1].tar_mat.at(i)  << "\t\t\t" << Nuclei[n-1][z-1].ion_source.at(i) << endl;
		}
	}

}

void YieldGrabber::GrabData(const char* target, const char* datadir){



	char name[64];
	strcpy(name,datadir);
	strcat(name,target);
	strcat(name,".dat");

	ifstream infile;
	infile.open(name);
	if(infile.is_open()){

		int i=0;

		unsigned int startyields = data.size();

		std::string line;
		while(getline(infile,line))
		{
			std::stringstream linestream(line);

			if(line.length() == 0 )
				continue;

			if(i==0){
				i++;
				continue;	
			}
		
			std::string value;

			while(linestream >> value)
				line_vec.push_back(value);
		
			data.push_back(line_vec);
			target_mat.push_back(target);

			line_vec.clear();

			i++;

		}

		infile.close();
	
	}
	else{
		cout << "File: " << name << " failed to open" << endl;
		return;
	}

}


void YieldGrabber::Start(const char *inp){

	Clear();

	printf("Loading matrices\n");

	GrabData("SiC",inp);
	GrabData("TiC",inp);
	GrabData("NiO",inp);
	GrabData("ZrC",inp);
	GrabData("Nb",inp);
	GrabData("Ta",inp);
	GrabData("Th",inp);
	GrabData("U",inp);

	cout << "Processing data..." << endl;

	ProcessData();

	cout << "Data processed, " << data.size() << " yields loaded" << endl;

}

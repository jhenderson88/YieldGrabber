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

#ifndef YieldGrabber_h
#define YieldGrabber_h

using namespace std;

typedef struct _Isotope {																													// Data structure to define isotopes, only one required for each isotope
	vector<double> yield;																														// Vector of isotope yields corresponding to:
	vector<int> proton_current;																											// The proton current on target
	vector<std::string> ion_source;																									// The ion source used 
	vector<int>	state;																															// The state of the isotope (e.g. ground state = 1, first metastable = 2)
	vector<std::string> tar_mat;																										// The target material (SiC, TiC, etc.)
	double avg_yield;																																// The mean yield for the isotope
	double max_yield;																																// Maximum yield for the isotope
} Isotope;

class YieldGrabber {																																// The class itself
	
	RQ_OBJECT("YieldGrabber")																												// This allows for signal/slot communication with the MouseAction subclass


	private :	
		vector< vector< std::string > > data;																					// Data, line-by-line
		vector< std::string > line_vec;																								// Used for data input
		vector< string > target_mat;																									// Used to retain information on target material

		bool data_grabbed;																														// Flag: Is the data loaded?

		Isotope Nuclei[146][94];																											// Nuclear chart of isotopes [N][Z]

		int n_stored, z_stored;																												// Used to pass N and Z from mouse interaction with TCanvas
		MouseAction *mouseaction;																											// Instance of the MouseAction class, used to interact with the TCanvas

	public :
		
		YieldGrabber();																																// Constructor
		virtual ~YieldGrabber(){;}																											// Destructor

	public :

		void Clear();																																	// Basic clear function, resets vectors, etc.

		void PrintIsotopeInfo(int z, int n);																					// Print yields for the isotope given N and Z
		void PrintIsotopeInfo();																											// As above but uses n_stored and z_stored
		void GrabData(const char* target, const char* inp); 	
																																									// Grab the data from the .dat files - put it in the data vector
		void ProcessData();																														// Turn the data into Isotopes
		void CreateIntensityMatrix(int source = 0);																		// Calculate max and average yields, depending on the requested source
		void PlotMeanIntensity();																											// Plot TH2D of the mean intensity
		void PlotMaxIntensity();																											// Plot TH2D of the maximum intensity

		int IonSourceComparison(std::string input);																		// Turn ionsource into int (All = 0, Re surface = 1, Ta surface = 2, TRILIS = 3, IG-LIS = 4, FEBIAD = 5)

		void Connect(){																																// Create the connection between the TCanvas and the MouseAction class which interprets the events
			mouseaction = new MouseAction();
			c1->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","MouseAction",mouseaction,"EventInfo(Int_t,Int_t,Int_t,TObject*)");
			mouseaction->Connect("SetPxPy(Int_t,Int_t)","YieldGrabber",this,"SetPxPy(Int_t,Int_t)");
		}
		void SetPxPy(Int_t px, Int_t py){																							// Take the output from the MouseAction->YieldGrabber connection and turn it into something meaningful 
			n_stored = px; 																														// Get the N value and store it
			z_stored = py;																														// Get the Z value and store it
			cout << "N: " << n_stored << " Z: " << z_stored << endl;												
			cout << "Printing isotope info..." << endl;
			c1=(TCanvas*)gROOT->FindObject("c1");																				// Re-find the Canvas - otherwise you get segmentation faults when ROOT can't find it		
			PrintIsotopeInfo();																													// Print all of the yields corresponding to the selected isotope
		}	

		void Start(const char* inp = "./Data/");																			// Initialises everything and starts the data grabbomg

		void PrintMeanIntensity(int, int);																						// Prints the average intensity for a given isotope

		TH2D *avg_intensity_hist;																											// Mean intensity histogram
		TH2D *intensity_hist;																													// Max intensity histogram

		TCanvas *c1;																																	// THE canvas

};
#endif

#ifdef YieldGrabber_cxx

YieldGrabber::YieldGrabber()								
{ 
	data_grabbed = false;
	Clear();
} 

void YieldGrabber::Clear() 
{

	data.clear();																
	line_vec.clear();
	target_mat.clear();

	for(int i=0;i<146;i++){
		for(int j=0;j<94;j++){
			Nuclei[i][j].yield.clear();
			Nuclei[i][j].proton_current.clear();
			Nuclei[i][j].ion_source.clear();
			Nuclei[i][j].state.clear();
			Nuclei[i][j].tar_mat.clear();
			Nuclei[i][j].avg_yield = 0;
			Nuclei[i][j].max_yield = 0;
		}
	}

}

void YieldGrabber::PlotMaxIntensity(){

	char yn;
	bool selectionsource = false;
	cout << "Select ion source? (y/n)" << endl;
	cin >> yn;
	if(yn == 'y' || yn == 'Y')
		selectionsource = true;
	else if(yn == 'n' || yn == 'N')
		selectionsource = false;
	else{
		cout << "No valid input, setting false" << endl;
		selectionsource = false;
	}
	
	if(selectionsource){
		int is = 0;
		cout << "Choose ion source: 0 = All, 1 = Re Surface, 2 = Ta surface, 3 = TRILIS, 4 = IG-LIS, 5=FEBIAD" << endl;
		cin >> is;
		CreateIntensityMatrix(is);
	}
	else
		CreateIntensityMatrix();

	bool clickable = false;
	cout << "Enable select nucleus? (y/n)" << endl;
	cin >> yn;
	if(yn == 'y' || yn == 'Y')
		clickable = true;
	else if(yn == 'n' || yn == 'N')
		clickable = false;
	else{
		cout << "No valid input, setting false" << endl;
		clickable = false;
	}

	intensity_hist = new TH2D("Max_Intensity_Matrix","Maximum Intensity Matrix",160,1,161,110,1,111);
	for(int i=0;i<146;i++){
		for(int j=0;j<94;j++){
			intensity_hist->SetBinContent(i+1,j+1,Nuclei[i][j].max_yield);
		}
	}

  intensity_hist->SetTitle(Form("%s; Neutron Number; Proton Number;",intensity_hist->GetTitle()));
	intensity_hist->GetXaxis()->CenterTitle();
	intensity_hist->GetYaxis()->CenterTitle();

	c1 = new TCanvas("C1","c1",1000,600);
	c1->cd();
	TPad *p_max = new TPad("pad","pad",0,0,1,1);
	p_max->Draw();
	p_max->cd();
	p_max->SetLogz();
	intensity_hist->SetStats(0);
	intensity_hist->Draw("colz");
	p_max->Modified();
	p_max->Update();
	c1->Modified();
	c1->Update();
	
	if(clickable)
		Connect();
}

void YieldGrabber::PlotMeanIntensity(){

	char yn;
	bool selectionsource = false;
	cout << "Select ion source? (y/n)" << endl;
	cin >> yn;
	if(yn == 'y' || yn == 'Y')
		selectionsource = true;
	else if(yn == 'n' || yn == 'N')
		selectionsource = false;
	else{
		cout << "No valid input, setting false" << endl;
		selectionsource = false;
	}
	
	if(selectionsource){
		int is = 0;
		cout << "Choose ion source: 0 = All, 1 = Re Surface, 2 = Ta surface, 3 = TRILIS, 4 = IG-LIS, 5=FEBIAD" << endl;
		cin >> is;
		CreateIntensityMatrix(is);
	}
	else
		CreateIntensityMatrix();

	bool clickable = false;
	cout << "Enable select nucleus? (y/n)" << endl;
	cin >> yn;
	if(yn == 'y' || yn == 'Y')
		clickable = true;
	else if(yn == 'n' || yn == 'N')
		clickable = false;
	else{
		cout << "No valid input, setting false" << endl;
		clickable = false;
	}

	avg_intensity_hist = new TH2D("Avg_Intensity_Matrix","Average Yield",160,1,161,110,1,111);
	for(int i=0;i<146;i++){
		for(int j=0;j<94;j++){
			avg_intensity_hist->SetBinContent(i+1,j+1,Nuclei[i][j].avg_yield);
		}
	}

  avg_intensity_hist->SetTitle(Form("%s; Neutron Number; Proton Number;",avg_intensity_hist->GetTitle()));
	avg_intensity_hist->GetXaxis()->CenterTitle();
	avg_intensity_hist->GetYaxis()->CenterTitle();
	
	c1 = new TCanvas("C1","c1",1000,600);
	c1->cd();

	TPad *p_avg = new TPad("pad","pad",0,0,1,1);
	p_avg->Draw();
	p_avg->cd();
	p_avg->SetLogz();
	avg_intensity_hist->SetStats(0);
	avg_intensity_hist->Draw("colz");
	p_avg->Modified();
	p_avg->Update();
	gSystem->ProcessEvents();
	c1->Modified();
	c1->Update();

	
	if(clickable)
		Connect();

}

void YieldGrabber::PrintMeanIntensity(int N,int Z){

	CreateIntensityMatrix();
	cout << "Average intensity: " << Nuclei[N][Z].avg_yield << endl;

}

int YieldGrabber::IonSourceComparison(std::string input){

	if(input.compare("Re surface")==0)
		return 1;
	else if(input.compare("Ta surface")==0)
		return 2;
	else if(input.compare("TRILIS")==0)
		return 3;
	else if(input.compare("IG-LIS")==0)
		return 4;
	else if(input.compare("FEBIAD")==0)
		return 5;
	else
		return 0;
}

#endif

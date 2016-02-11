//**********************************************************************************************************************//
// Author: Jack Henderson (TRIUMF)
// jhenderson@triumf.ca
//
// Class to communicate with the data grabbing software - allows for point-and-click functionality on yield histograms
//
// Based on demonstration class by P. J. Davies (York) and ROOT example codes
//
//**********************************************************************************************************************//

#include "Declarations.h"

class MouseAction {

	//This is needed for the signal/slot stuff to work
	RQ_OBJECT("MouseAction")

	private:


	public:

		MouseAction(); 																																			// Constructor
		virtual ~MouseAction(){;} 																													// Destructor

		Int_t px_stored; 																																		// X position storage
		Int_t py_stored; 																																		// Y position storage

		void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);									// Event grabbing function
		void PrintEventInfo(Int_t px, Int_t py);																						// Uses the X and Y (N and Z) values to print the isotope selected

		void SetPxPy(Int_t px, Int_t py);  																									// This is the SIGNAL which is sent back to YieldGrabber
		Int_t GetPx();																																			// X position getter
		Int_t GetPy();																																			// Y position getter
  
};

MouseAction::MouseAction(){
	
	px_stored = 0;																																				// Reset X position to 0
	py_stored = 0;																																				// Reset Y position to 0

}


void MouseAction::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)					// Mimic the input of TCanvas EventInfo - grabs mouse movement, clicks, etc.
{	
   
	if ( event == kButton1Double) {																											// Wait for a double click of the mouse

		if(selected->InheritsFrom("TH2")){																								// If the object selected is a TH2 derivative
			TH2 *h = (TH2*)selected;																												// Copy the object
			Float_t xx = gPad->AbsPixeltoX(px);																							// Change X from absolute pixel to pad  
			Float_t x = gPad->PadtoX(xx);																										// ... and from pad to TH2 X
			Int_t binx = h->GetXaxis()->FindBin(x);																					// ... and finally, grab the bin corresponding to X
			Float_t yy = gPad->AbsPixeltoY(py);																							// Change Y from absolute pixel to pad  
			Float_t y = gPad->PadtoY(yy); 																									// ... and from pad to TH2 Y
			Int_t biny = h->GetYaxis()->FindBin(y);																					// ... and finally, grab the bin corresponding to Y
			SetPxPy(binx,biny);																															// Send the SIGNAL
			return;
		}

	}
	if ( event == kButton1Down) {																												// Wait for a single click of the mouse

		if(selected->InheritsFrom("TH2")){																								// If the object selected is a TH2 derivative
			TH2 *h = (TH2*)selected;																												// Copy the object
			Float_t xx = gPad->AbsPixeltoX(px);																							// Change X from absolute pixel to pad  
			Float_t x = gPad->PadtoX(xx);																										// ... and from pad to TH2 X
			Int_t binx = h->GetXaxis()->FindBin(x);																					// ... and finally, grab the bin corresponding to X
			Float_t yy = gPad->AbsPixeltoY(py);																							// Change Y from absolute pixel to pad  
			Float_t y = gPad->PadtoY(yy); 																									// ... and from pad to TH2 Y
			Int_t biny = h->GetYaxis()->FindBin(y);																					// ... and finally, grab the bin corresponding to Y
			PrintEventInfo(binx,biny);																											// Print the isotope selected
			return;
		}

	}

}

void MouseAction::PrintEventInfo(Int_t px, Int_t py){																	// Prints the isotope corresponding to X (N) and Y (Z)
	
	std::cout << "Isotope: " << px+py+2 << element[py] << std::endl;															// Uses the string array, elements (up to Pu)

}

void MouseAction::SetPxPy(Int_t px, Int_t py){																				// Function which emits the X and Y (N and Z) values from the double click
	
	long args[2];																																				// Array to pass the arguments back with the signal
	px_stored = args[0] = px; py_stored = args[1] = py;																	// Store the px and py info in the class
	Emit("SetPxPy(Int_t,Int_t)", args);																									// Signal a change

}

Int_t MouseAction::GetPx(){																														// X Getter

	return px_stored;

}

Int_t MouseAction::GetPy(){																														// Y Getter

	return py_stored;

}


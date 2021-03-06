# YieldGrabber
ROOT and python based code to grab ISAC yields from the yield database and output useful yield spectra

# Compilation 
To compile the ROOT code, simply enter ROOT and enter:

> .L YieldGrabber.cxx++

This will create the necessary libraries and only needs to be done once. From then on you can just load the YieldGrabber_cxx.so library file.

To run the code, in ROOT, simply create a new instance of the class as you would for any other ROOT class, eg:

> YieldGrabber t

> t.Start()

This will grab data from the .dat files stored in ./Data/ and arrange it into a reasonable format. In case you want to run elsewhere, use:

> t.Start("/home/user/YieldGrabber/Data/")

For example, to grab the data.

To, for example, plot average yields:

> t.PlotMeanIntensity()

Or, for maximum yields:

> t.PlotMaxIntensity()

You will then be prompted for a choice of ion source (if any), and asked whether you want to be able to interact with the histogram. If you choose to interact, double-clicking on an isotope on the yield histogram will output all yield information to the terminal.

# Easy running

To run easily, edit RunYields.C to have the correct absolute addresses and add the line:

> alias GrabISACYields='root -l ~/YieldGrabber/RunYields.C'

To your .bashrc. 

# Data grabbing

The shell script, GetYieldData.sh automatically grabs data from the TRIUMF yield database by using the wget command. This information is then parsed by the Read.py Python script into the .dat files required by the YieldGrabber software. This shell-script has to be run manually. Yield databases are updated reasonably regularly when ISAC is running, so it is recommended you run this script fairly regularly too.

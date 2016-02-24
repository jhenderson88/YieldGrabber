{

  gSystem->Load("/home/jhenderson/YieldGrabber/YieldGrabber_cxx.so"); // Edit this address to something appropriate
  YieldGrabber t;
  t.Start("/home/jhenderson/YieldGrabber/Data/"); // Edit this address to something appropriate
  t.PlotMeanIntensity();   

}

{

  gSystem->Load("/home/jhenderson/YieldGrabber/YieldGrabber_cxx.so");
  YieldGrabber t;
  t.Start("/home/jhenderson/YieldGrabber/Data/");
  t.PlotMeanIntensity();   

}

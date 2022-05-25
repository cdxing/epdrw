#include <iostream> 
#include <fstream> 


void write_epdGain2(int day=56, TString date = "01-02-2021 23:59:10") {
  char fn[256];
  sprintf(fn, "/star/u/skk317/EPD_Calibrate/Gains/Nmip_Day_%d.txt",day);

  // Covert day of the year to timestamp
  TDatime d1(2021,1,1,0,0,0);
  long dayinSec = ((day-1)*24*60*60)+1;// - 25*60*60;
  TDatime d2(d1.Convert()+dayinSec);
  TString storeTime = d2.AsSQLString(); // storeTime is begin time for validity range for WRITING DB

 /* 
  if (date != "13-13-1313 00:00:00")
  {
    storeTime = date;
    //storeTime = 1609651000;    
  }
 */ 

  cout<< d2.Convert()<<endl;

  cout << "\n\n\n Setting gain for the day= " << day <<
      " \nTimestamp = " << storeTime << " \nFrom file = "<< fn <<
	   "\n\n\n" << endl;

  gROOT->Macro("./loadlib.C");

  // Initiate c sturctue to fill in
  const Int_t MAX_DB_INDEX = 768;
  epdGain_st epdGain;
  // only mip and offset=0 will be set
  for(int i=0; i<MAX_DB_INDEX; i++){
    epdGain.ew[i] = -1;
    epdGain.pp[i] = -1;
    epdGain.tile[i] = -1;

    epdGain.vped[i] = -1;
    epdGain.mip[i] = -1;
    epdGain.qt_pedestals[i] = -1;
    epdGain.dark_current[i] = -1;

    epdGain.qt_pedestals_sigma[i] = -1;
    epdGain.offset[i] = 0;  //set to zero for Run18
  }

  //Read EPD gain from txt file
  FILE *fp = fopen(fn,"r");
  if(fp==NULL) {
    printf("Gain file does not exist");
    return;
  }

  short day1;
  short ew;
  short pp;
  short tile;
  float mip;
  float off;
  int n=0;


  char line[2550];
  while (fgets(line,2550,fp)) {
    if(line[0]=='#') continue;
    sscanf(&line[0],"%hd %hd %hd %hd %f %f", &day1, &ew, &pp, &tile, &mip, &off);
    if(tile == 1){
      epdGain.ew[n] = ew;
      epdGain.pp[n] = pp;
      epdGain.tile[n] = 0;
      epdGain.mip[n] = 0; 
      n++;
    }
    epdGain.ew[n] = ew;
    epdGain.pp[n] = pp;
    epdGain.tile[n] = tile;
    epdGain.mip[n] = mip;
    epdGain.offset[n] = off;
    n++;
  }
  fclose(fp);



  // print content of cstructure
  for(int i=0; i<MAX_DB_INDEX; i++){
    printf("%d               %d  %d  %d             %f %f %f %f %f %f \n",i,epdGain.ew[i],epdGain.pp[i],epdGain.tile[i],    epdGain.vped[i] , epdGain.mip[i] , epdGain.qt_pedestals[i] , epdGain.dark_current[i] , epdGain.qt_pedestals_sigma[i] , epdGain.offset[i],  epdGain.qt_pedestals_sigma[i] , epdGain.offset[i]  );
  }
  printf("Found %d entries (should be 768)\n",n);








  // un comment following to fill the DB after testing carefully

  /*  
     gSystem->Setenv("DB_ACCESS_MODE","write");
     StDbManager* mgr = StDbManager::Instance();
     StDbConfigNode* node = mgr->initConfig("Calibrations_epd");
     StDbTable* dbtable = node->addDbTable("epdGain");
     mgr->setStoreTime(storeTime.Data());    
     dbtable->SetTable((char*)&epdGain, 1);
     dbtable->setFlavor("ofl"); 
     mgr->storeDbTable(dbtable);    
     std::cout << "INFO: table saved to database" << std::endl;
  */ 

}

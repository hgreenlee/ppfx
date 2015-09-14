#include "ReweightDriver.h"
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace NeutrinoFluxReweight{
  
  ReweightDriver::ReweightDriver(int iuniv, const ParameterTable& cv_pars, const ParameterTable& univ_pars, std::string fileIn)
    : iUniv(iuniv), cvPars(cv_pars), univPars(univ_pars), fileOptions(fileIn)
  {
    ParseOptions();
    Configure();
    
  }
  
  void ReweightDriver::Configure(){
    
    //Creating the vector of reweighters:
    
    if(doMIPPNumi)MIPP_NUMI_PION_Universe = new MIPPNumiPionYieldsReweighter(iUniv,cvPars,univPars);
    if(doMIPPNumi)MIPP_NUMI_KAON_Universe = new MIPPNumiKaonYieldsReweighter(iUniv,cvPars,univPars);

    TARG_ATT_Universe = new TargetAttenuationReweighter(iUniv,cvPars,univPars);    
    VOL_ABS_IC_Universe = new AbsorptionICReweighter(iUniv,cvPars,univPars);
    VOL_ABS_DPIP_Universe = new AbsorptionDPIPReweighter(iUniv,cvPars,univPars);
    VOL_ABS_DVOL_Universe = new AbsorptionDVOLReweighter(iUniv,cvPars,univPars);
    VOL_ABS_NUCLEON_Universe = new NucleonAbsorptionOutOfTargetReweighter(iUniv,cvPars,univPars);
    VOL_ABS_OTHER_Universe = new OtherAbsorptionOutOfTargetReweighter(iUniv,cvPars,univPars);

    THINTARGET_PC_PION_Universe = new ThinTargetpCPionReweighter(iUniv,cvPars,univPars);
    THINTARGET_PC_KAON_Universe = new ThinTargetpCKaonReweighter(iUniv,cvPars,univPars);
    THINTARGET_NC_PION_Universe = new ThinTargetnCPionReweighter(iUniv,cvPars,univPars);
    THINTARGET_PC_NUCLEON_Universe = new ThinTargetpCNucleonReweighter(iUniv,cvPars,univPars);
    THINTARGET_MESON_INCIDENT_Universe = new ThinTargetMesonIncidentReweighter(iUniv,cvPars,univPars);
    THINTARGET_NUCLEON_A_Universe = new ThinTargetnucleonAReweighter(iUniv,cvPars,univPars);
    OTHER_Universe = new OtherReweighter(iUniv,cvPars,univPars);
    
  }
  
  void ReweightDriver::ParseOptions(){
    //Parsing the file input:
    using boost::property_tree::ptree;
    ptree top;
    std::string val = "";
    read_xml(fileOptions.c_str(),top,2); // option 2 removes comment strings
    ptree& options = top.get_child("inputs.Settings");
    
    val = options.get<std::string>("Reweighters");
    if(val=="MIPPNuMIOn")doMIPPNumi = true;
    else doMIPPNumi = false;
    
  }
  double ReweightDriver::calculateWeight(const InteractionChainData& icd){

    double tot_wgt = 1.0;
    
    //Boolean flags: 
    const int nnodes=icd.interaction_chain.size();
    std::vector<bool> interaction_nodes(nnodes,false);
    std::vector<bool> attenuation_nodes(nnodes,false);
    std::vector<bool> absorption_nodes(nnodes,false);

    /// ----- PROCESS INTERACTION NODES ----- ///
    
    //First we look at MIPP and look absorption chain:
    bool has_mipp = false;
    mipp_pion_wgt = 1.0;
    if(doMIPPNumi){
      interaction_nodes = MIPP_NUMI_PION_Universe->canReweight(icd);
      for(int ii=0;ii<interaction_nodes.size();ii++){
	if(interaction_nodes[ii]==true){
	  has_mipp = true;
	  mipp_pion_wgt = MIPP_NUMI_PION_Universe->calculateWeight(icd);
	  break; 
	}
      }
      tot_wgt *= mipp_pion_wgt;
    }

    mipp_kaon_wgt = 1.0;
    if(!has_mipp && doMIPPNumi){
      interaction_nodes = MIPP_NUMI_KAON_Universe->canReweight(icd);

      for(int ii=0;ii<interaction_nodes.size();ii++){
	if(interaction_nodes[ii]==true){
	  has_mipp = true;
	  mipp_kaon_wgt = MIPP_NUMI_KAON_Universe->calculateWeight(icd);
	  break; 
	}
      }
      tot_wgt *= mipp_kaon_wgt;
    }
    
    pC_pi_wgt = 1.0;
    
    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_PC_PION_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_PC_PION_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  pC_pi_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= pC_pi_wgt;
       
    pC_k_wgt = 1.0;
    
    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_PC_KAON_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_PC_KAON_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  pC_k_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= pC_k_wgt;
    
    nC_pi_wgt = 1.0;
 
    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_NC_PION_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_NC_PION_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  nC_pi_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= nC_pi_wgt;
      

    pC_nu_wgt = 1.0;

    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_PC_NUCLEON_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_PC_NUCLEON_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  pC_nu_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= pC_nu_wgt;
      
    meson_inc_wgt = 1.0;
    
    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_MESON_INCIDENT_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_MESON_INCIDENT_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  meson_inc_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
	}
      else{
	break;
      } 
    }
    tot_wgt *= meson_inc_wgt;    

    nuA_wgt = 1.0;
 
    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = THINTARGET_NUCLEON_A_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = THINTARGET_NUCLEON_A_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  nuA_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= nuA_wgt;
     
    other_wgt = 1.0;

    for(int ii=(interaction_nodes.size()-1);ii>=0;ii--){	
      if(interaction_nodes[ii]==false){
	bool is_rew = OTHER_Universe->canReweight((icd.interaction_chain)[ii]);
	if(is_rew){
	  double rewval = OTHER_Universe->calculateWeight((icd.interaction_chain)[ii]);
	  other_wgt *= rewval;
	  interaction_nodes[ii]=true;
	}
      }
      else{
	break;
      } 
    }
    tot_wgt *= other_wgt;
    
    //Looking for target attenuation correction:
    att_wgt = 1.0;

    attenuation_nodes = TARG_ATT_Universe->canReweight(icd);   
    //we just see for the first position (prmary proton)
    if(attenuation_nodes[0]==true){
      att_wgt *= TARG_ATT_Universe->calculateWeight(icd);
    }
    tot_wgt *= att_wgt;
    
    
    //ABS:
    tot_abs_wgt = 1.0;
    
    // Looking for the correction of the pi & K absorption in volumes (Al)    
    abs_ic_wgt = 1.0;
    
    absorption_nodes = VOL_ABS_IC_Universe->canReweight(icd);
    if(absorption_nodes[0]==true){
      abs_ic_wgt *= VOL_ABS_IC_Universe->calculateWeight(icd);
    }
    tot_wgt     *= abs_ic_wgt;
    tot_abs_wgt *= abs_ic_wgt;
 
    //Looking for the correction of the pi & K absorption in volumes (Fe)
    abs_dpip_wgt = 1.0;
    
    absorption_nodes = VOL_ABS_DPIP_Universe->canReweight(icd);
    if(absorption_nodes[0]==true){
      abs_dpip_wgt *= VOL_ABS_DPIP_Universe->calculateWeight(icd);
    }
    tot_wgt     *= abs_dpip_wgt;
    tot_abs_wgt *= abs_dpip_wgt;
    
    
    //Looking for the correction of the pi & K absorption in volumes (He)
    abs_dvol_wgt = 1.0;
    
    absorption_nodes = VOL_ABS_DVOL_Universe->canReweight(icd);
    if(absorption_nodes[0]==true){
      abs_dvol_wgt *= VOL_ABS_DVOL_Universe->calculateWeight(icd);
    }
    tot_wgt     *= abs_dvol_wgt;
    tot_abs_wgt *= abs_dvol_wgt;
    
    //Looking for the correction of nucleons on Al, Fe and He.
    abs_nucleon_wgt = 1.0;
    
    absorption_nodes = VOL_ABS_NUCLEON_Universe->canReweight(icd);
    if(absorption_nodes[0]==true){
      abs_nucleon_wgt *= VOL_ABS_NUCLEON_Universe->calculateWeight(icd);
    }
    tot_wgt     *= abs_nucleon_wgt;
    tot_abs_wgt *= abs_nucleon_wgt;
    
    //Looking for the correction of any other particle on Al, Fe and He.
    abs_other_wgt = 1.0;
    
    absorption_nodes = VOL_ABS_OTHER_Universe->canReweight(icd);
    if(absorption_nodes[0]==true){
      abs_other_wgt *= VOL_ABS_OTHER_Universe->calculateWeight(icd);
    }
    tot_wgt     *= abs_other_wgt;
    tot_abs_wgt *= abs_other_wgt;
    
    if(tot_wgt!=tot_wgt){
      std::cout<<"Alert nan total wgt... check!!!"<<std::endl;
      return 1.0;
    }
    return tot_wgt;
  }
  
};

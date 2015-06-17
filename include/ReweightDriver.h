#ifndef REWEIGHTDRIVER_H
#define REWEIGHTDRIVER_H

#include "NA49Reweighter.h"
#include "MIPPThinTargetReweighter.h"
#include "TheoryThinTargetReweighter.h"

#include "MIPPNumiYieldsReweighter.h"
#include "MIPPNumiKaonsYieldsReweighter.h"
#include "TargetAttenuationReweighter.h"

#include "AbsorptionICReweighter.h"
#include "AbsorptionDPIPReweighter.h"
#include "AbsorptionDVOLReweighter.h"
#include "NucleonAbsorptionOutOfTargetReweighter.h"
#include "OtherAbsorptionOutOfTargetReweighter.h"

#include "tmp_TSRew.h"
#include "tmp_TARew.h"
#include "OtherReweighter.h"

#include "InteractionChainData.h"

namespace NeutrinoFluxReweight{
    
  /*! \class ReweightDriver
   *  \brief A class to manage and drive the weight calculation procedure. 
   *
   * There is a one-to-one correspondence between ReweightDrivers and universes.
   */

  class ReweightDriver{
  public:  
    /*!
     * the constructor
    */
    ReweightDriver(int iuniv, const ParameterTable& cv_pars, const ParameterTable& univ_pars,std::string fileIn);
    

    /*!
     * Calculate weight for this event, given a set of central value parameters
     * and a set of randomly varied parameters corresponding to the universe
     * that this ReweightDriver is responsible for.
     *
     * - Call mipp_yields on input chain. record interactions covered with a weight in interaction_covered.
     * - Call attenuation on input chain
     * - Call thin target reweighters. 
     *    -# Call na49 for interactions not yet covered. record in interaction_covered
     *    -# ditto. call MIPP, then theory
     */
    double calculateWeight(const InteractionChainData& icd);
    
    /*!  MIPP NuMI yield weight   */
    double mipp_wgt;

     /*!   MIPP NuMI kaons yield weight   */
    double mipp_kaons_wgt;
    
    /*!  Target attenuation weight   */
    //double att_wgt;
    double att_wgt;
    double sur_wgt;

    /*!  Absorption weight   */
    double abs_ic_wgt;
    double abs_dpip_wgt;
    double abs_dvol_wgt;
    double abs_nucleon_wgt;
    double abs_other_wgt;
    double tot_abs_wgt;
       
    /*!   NA49 weight   */
    double na49_wgt;
    
    /*!   MIPP thin target weight   */
    double mipp_thin_wgt;
    
    /*!   Theory (model) prediction weight   */
    //double theory_wgt;
    double other_wgt;
    
  private:    
    /*!
     * Configures each of the reweighing tools.
     */
    void Configure();
    /*!
     * Parse the option to select which reweighter use
     */
    void ParseOptions();
    int iUniv;
    ParameterTable cvPars;
    ParameterTable univPars;

    MIPPNumiYieldsReweighter* MIPP_NUMI_Universe;
    MIPPNumiKaonsYieldsReweighter* MIPP_NUMI_KAONS_Universe;
    
    //    TargetAttenuationReweighter* TARG_ATT_Universe;
    tmp_TSRew* tmp_TSRew_Universe;
    tmp_TARew* tmp_TARew_Universe;
    
    AbsorptionICReweighter*   VOL_ABS_IC_Universe;
    AbsorptionDPIPReweighter* VOL_ABS_DPIP_Universe;
    AbsorptionDVOLReweighter* VOL_ABS_DVOL_Universe;
    NucleonAbsorptionOutOfTargetReweighter* VOL_ABS_NUCLEON_Universe;
    OtherAbsorptionOutOfTargetReweighter* VOL_ABS_OTHER_Universe;

    OtherReweighter* OTHER_Universe;

    NA49Reweighter* NA49_Universe;
    MIPPThinTargetReweighter* MIPP_THIN_Universe;
    //    TheoryThinTargetReweighter* THEORY_Universe;

    //Flag to select the reweighters:
    bool doMIPPNumiYields;
    bool doMIPPNumiKaonsYields;

    //    bool doTargetAttenuation;
    bool doTA;
    bool doTS;
    
    bool doAbsorptionIC;
    bool doAbsorptionDPIP;
    bool doAbsorptionDVOL;
    bool doAbsorptionNucleon;
    bool doAbsorptionOther;

    //    bool doTheoryThinTarget;
    bool doOther;

    bool doNA49;
    bool doMIPPThinTarget;
    
    std::string fileOptions;
    
  };

  
};
#endif

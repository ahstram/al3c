#include<unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <sstream>


#include "../include/al3c.hpp"

struct param_t {

	float MigrationRate_AfrToEur,
	      MigrationRate_AfrToAsn,
	      MigrationRate_EurToAsn,
	      EffectivePopulationSize_Afr,
	      GrowthRate_Eur,
	      GrowthRate_Asn,
	      PastEvent_AfrToEurProportion;

}; 



struct param_summary_t {
	float MigrationRate_AfrToEur_Variance,
	      MigrationRate_AfrToAsn_Variance,
	      MigrationRate_EurToAsn_Variance,
	      EffectivePopulationSize_Afr_Variance,
	      GrowthRate_Eur_Variance,
	      GrowthRate_Asn_Variance,
	      PastEvent_AfrToEurProportion_Variance;

};


void user_t::prior() {

//	param->MigrationRate_AfrToEur=u01()*0.4+0.84; // Unif[0.84,1.24] --> [2.1,3.1]
	param->MigrationRate_AfrToEur=u01()*1.0+2.1; // Unif[2.1,3.1]

//	param->MigrationRate_AfrToAsn=u01()*0.32+0.16; // Unif[0.16,0.48] --> [0.4, 1.2]
	param->MigrationRate_AfrToAsn=u01()*0.8+0.4; // Unif[0.16,0.48] --> [0.4, 1.2]

//	param->MigrationRate_EurToAsn=u01()*0.84+0.72; // Unif[0.72,1.56] --> [1.8, 3.9]
	param->MigrationRate_EurToAsn=u01()*2.1+1.8; // Unif[1.8, 3.9]


	param->EffectivePopulationSize_Afr=u01()*0.2994+1.319; // Unif[1.319, 1.6184]
	param->GrowthRate_Eur=u01()*0.31+0.28; // Unif[0.28, 0.59]
	param->GrowthRate_Asn=u01()*0.45+0.30; // Unif[0.30, 0.75]


	//param->PastEvent_AfrToEurProportion=u01()*2.8+4.8; // Unif[4.8,7.6] --> [12, 19]
	param->PastEvent_AfrToEurProportion=u01()*7+12; //  Unif[12, 19]

};
float user_t::prior_density() {
//	if (0.84<=param->MigrationRate_AfrToEur && param->MigrationRate_AfrToEur<=1.24
//		 && 0.16<=param->MigrationRate_AfrToAsn && param->MigrationRate_AfrToAsn<=0.48
//		 && 0.72<=param->MigrationRate_EurToAsn && param->MigrationRate_EurToAsn<=1.56
//		 && 1.319<=param->EffectivePopulationSize_Afr && param->EffectivePopulationSize_Afr<=1.6184
//		&& 0.28<=param->GrowthRate_Eur && param->GrowthRate_Eur<=0.59
//		 && 0.3<=param->GrowthRate_Asn && param->GrowthRate_Asn<=0.75
//		&& 4.8<=param->PastEvent_AfrToEurProportion && param->PastEvent_AfrToEurProportion<=7.6)

if (2.1<=param->MigrationRate_AfrToEur && param->MigrationRate_AfrToEur<=3.1
		 && 0.4<=param->MigrationRate_AfrToAsn && param->MigrationRate_AfrToAsn<=1.2
		 && 1.8<=param->MigrationRate_EurToAsn && param->MigrationRate_EurToAsn<=3.9
		 && 1.319<=param->EffectivePopulationSize_Afr && param->EffectivePopulationSize_Afr<=1.6184
		&& 0.28<=param->GrowthRate_Eur && param->GrowthRate_Eur<=0.59
		 && 0.3<=param->GrowthRate_Asn && param->GrowthRate_Asn<=0.75
		&& 12<=param->PastEvent_AfrToEurProportion && param->PastEvent_AfrToEurProportion<=19)


		return 1;
	else
		return 0;

	return 1;

}

void user_t::perturb() {

	param->MigrationRate_AfrToEur+=(u01()-0.5f)*sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12);
	param->MigrationRate_AfrToAsn+=(u01()-0.5f)*sqrt(2*param_summary->MigrationRate_AfrToAsn_Variance*12);
	param->MigrationRate_EurToAsn+=(u01()-0.5f)*sqrt(2*param_summary->MigrationRate_EurToAsn_Variance*12);
	param->EffectivePopulationSize_Afr+=(u01()-0.5f)*sqrt(2*param_summary->EffectivePopulationSize_Afr_Variance*12);
	param->GrowthRate_Eur+=(u01()-0.5f)*sqrt(2*param_summary->GrowthRate_Eur_Variance*12);
	param->GrowthRate_Asn+=(u01()-0.5f)*sqrt(2*param_summary->GrowthRate_Asn_Variance*12);
	param->PastEvent_AfrToEurProportion+=(u01()-0.5f)*sqrt(2*param_summary->PastEvent_AfrToEurProportion_Variance*12);


}

float user_t::perturb_density(param_t *old_param) {
/*
	if ( fabs(param->MigrationRate_AfrToEur - old_param->MigrationRate_AfrToEur) > sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->MigrationRate_AfrToAsn - old_param->MigrationRate_AfrToAsn) > sqrt(2*param_summary->MigrationRate_AfrToAsn_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->MigrationRate_EurToAsn - old_param->MigrationRate_AfrToEur) > sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->EffectivePopulationSize_Afr - old_param->MigrationRate_AfrToEur) > sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->GrowthRate_Eur - old_param->MigrationRate_AfrToEur) > sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->GrowthRate_Asn - old_param->MigrationRate_AfrToEur) > sqrt(2*param_summary->MigrationRate_AfrToEur_Variance*12)/2.f ) 
		return 0.f;
	if ( fabs(param->PastEvent_AfrToEurProportion - old_param->PastEvent_AfrToEurProportion) > sqrt(2*param_summary->PastEvent_AfrToEurProportion_Variance*12)/2.f ) 
		return 0.f;

*/

	return 1.f;

}
void user_t::simulate() {

	//const int MAX_PATH_LEN=10000;
	//char cwd[MAX_PATH_LEN];
	//getcwd(cwd,MAX_PATH_LEN);
	ostringstream cmd;
	uint seed=u01()*UINT_MAX;

//  run this command:
//  It is assumed that MaCS is in the search path
//  ./macs <parameters> | allele_spectrum | cut -f3- | sed 1d

	cmd<<"macs/macs.so 718 100000 -s "<<seed<<" -t .001 -I 3 176 170 372 0 -m 2 1 "<<param->MigrationRate_AfrToEur<<" -m 3 1 "<<param->MigrationRate_AfrToAsn<<" -m 3 2 "<<param->MigrationRate_EurToAsn<<" -n 1 "<<param->EffectivePopulationSize_Afr<<" -g 2 "<<param->GrowthRate_Eur<<" -g 3 "<<param->GrowthRate_Asn<<" -eg .0230000 2 0 -eg .0230001 3 0 -ej .0230002 3 2 -em .0230003 2 1 "<<param->PastEvent_AfrToEurProportion<<" -en .0230004 2 0.1861 -ej .051 2 1 -en .148 1 0.731 -r 0.0006  2>/dev/null |   macs/allele_spectrum | cut -f3- | sed 1d";

	exec_cmd(cmd.str().c_str());

}

float user_t::distance() {

float r=0;

for (uint n=0;n<N;n++)
	for (uint d=0;d<D;d++)
		r+=pow(S[n][d]-O[n][d],2);

return sqrt(r);

}

void user_summary_t::summarize(){

	float m1=0,m2=0;

	for (uint a=0;a<A;a++) {
		m1+=params[a]->MigrationRate_AfrToEur;
		m2+=params[a]->MigrationRate_AfrToEur*params[a]->MigrationRate_AfrToEur;
	} m1/=(float)A; m2/=(float)A;
	summary->MigrationRate_AfrToEur_Variance=m2-m1*m1;

	m1=0,m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->MigrationRate_AfrToAsn;
		m2+=params[a]->MigrationRate_AfrToAsn*params[a]->MigrationRate_AfrToAsn;
	} m1/=(float)A; m2/=(float)A;
	summary->MigrationRate_AfrToAsn_Variance=m2-m1*m1;

	m1=0,m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->MigrationRate_EurToAsn;
		m2+=params[a]->MigrationRate_EurToAsn*params[a]->MigrationRate_EurToAsn;
	} m1/=(float)A; m2/=(float)A;
	summary->MigrationRate_EurToAsn_Variance=m2-m1*m1;
	
	m1=0,m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->EffectivePopulationSize_Afr;
		m2+=params[a]->EffectivePopulationSize_Afr*params[a]->EffectivePopulationSize_Afr;
	} m1/=(float)A; m2/=(float)A;
	summary->EffectivePopulationSize_Afr_Variance=m2-m1*m1;

	m1=0, m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->GrowthRate_Eur;
		m2+=params[a]->GrowthRate_Eur*params[a]->GrowthRate_Eur;
	} m1/=(float)A; m2/=(float)A;
	summary->GrowthRate_Eur_Variance=m2-m1*m1;

	m1=0,m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->GrowthRate_Asn;
		m2+=params[a]->GrowthRate_Asn*params[a]->GrowthRate_Asn;
	} m1/=(float)A; m2/=(float)A;
	summary->GrowthRate_Asn_Variance=m2-m1*m1;

	m1=0,m2=0;
	for (uint a=0;a<A;a++) {
		m1+=params[a]->PastEvent_AfrToEurProportion;
		m2+=params[a]->PastEvent_AfrToEurProportion*params[a]->PastEvent_AfrToEurProportion;
	} m1/=(float)A; m2/=(float)A;
	summary->PastEvent_AfrToEurProportion_Variance=m2-m1*m1;

}


void user_t::print(ofstream& output, bool header) {

	if (header) {
		output<<"#distance	weight	MigrationRate_AfrToEur	MigrationRate_AfrToAsn	MigrationRate_EurToAsn	EffectivePopulationSize_Afr	GrowthRate_Eur	GrowthRate_Asn	PastEvent_AfrToEurProportion\n";
		return;
	}
	
	output<<"d="<<(*d)<<"\tw="<<(*w)
	<<"\t"<<param->MigrationRate_AfrToEur
	<<"\t"<<param->MigrationRate_AfrToAsn
	<<"\t"<<param->MigrationRate_EurToAsn
	<<"\t"<<param->EffectivePopulationSize_Afr
	<<"\t"<<param->GrowthRate_Eur
	<<"\t"<<param->GrowthRate_Asn
	<<"\t"<<param->PastEvent_AfrToEurProportion<<"\n";

	for (uint n=0;n<N;n++) {
		output<<"#";
		for (uint d=0;d<D;d++) 
			output<<"\t"<<S[n][d];
		output<<"\n";
	}


}

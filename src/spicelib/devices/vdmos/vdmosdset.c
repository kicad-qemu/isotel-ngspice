/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1988 Jaijeet S Roychowdhury
**********/

#include "ngspice/ngspice.h"
#include "ngspice/cktdefs.h"
#include "ngspice/devdefs.h"
#include "vdmosdefs.h"
#include "ngspice/distodef.h"
#include "ngspice/const.h"
#include "ngspice/sperror.h"
#include "ngspice/suffix.h"

int
VDMOSdSetup(GENmodel *inModel, CKTcircuit *ckt)
        /* actually load the current value into the 
         * sparse matrix previously provided 
         */
{
    VDMOSmodel *model = (VDMOSmodel *) inModel;
    VDMOSinstance *here;
    double Beta;
    double DrainSatCur;
    double SourceSatCur;
    double gm;
    double gds;
    double gb;
    double ebd;
    double vgst;
    double evbs;
    double vbd;
    double vbs;
    double vds;
    double vdsat;
    double vgd;
    double vgs;
    double vt;
    double lgbs;
    double lgbs2;
    double lgbs3;
    double lgbd;
    double lgbd2;
    double lgbd3;
    double gm2;
    double gds2;
    double gb2;
    double gmds;
    double gmb;
    double gbds;
    double gm3;
    double gds3;
    double gb3;
    double gm2ds;
    double gmds2;
    double gm2b;
    double gmb2;
    double gb2ds;
    double gbds2;
    double lcapgb2;
    double lcapgb3;
    double lcapgs2;
    double lcapgs3;
    double lcapgd2;
    double lcapgd3;
    double lcapbs2 = 0;
    double lcapbs3 = 0;
    double lcapbd2 = 0;
    double lcapbd3 = 0;
    double gmbds = 0.0;


    /*  loop through all the VDMOS device models */
    for( ; model != NULL; model = VDMOSnextModel(model)) {
        /* loop through all the instances of the model */
        for (here = VDMOSinstances(model); here != NULL ;
                here=VDMOSnextInstance(here)) {

            vt = CONSTKoverQ * here->VDMOStemp;

            DrainSatCur = here->VDMOSm * here->VDMOStSatCur;
            SourceSatCur = here->VDMOSm * here->VDMOStSatCur;

            Beta = here->VDMOStTransconductance * here->VDMOSm *
                    here->VDMOSw/here->VDMOSl;

                    vbs = model->VDMOStype * ( 
                        *(ckt->CKTrhsOld+here->VDMOSbNode) -
                        *(ckt->CKTrhsOld+here->VDMOSsNodePrime));
                    vgs = model->VDMOStype * ( 
                        *(ckt->CKTrhsOld+here->VDMOSgNode) -
                        *(ckt->CKTrhsOld+here->VDMOSsNodePrime));
                    vds = model->VDMOStype * ( 
                        *(ckt->CKTrhsOld+here->VDMOSdNodePrime) -
                        *(ckt->CKTrhsOld+here->VDMOSsNodePrime));

                /* now some common crunching for some more useful quantities */

                vbd=vbs-vds;
                vgd=vgs-vds;

            /*
             * bulk-source and bulk-drain diodes
             *   here we just evaluate the ideal diode current and the
             *   corresponding derivative (conductance).
             */
	    if(vbs <= 0) {
                lgbs = SourceSatCur/vt;
                lgbs += ckt->CKTgmin;
		lgbs2 = lgbs3 = 0;
            } else {
                evbs = exp(MIN(MAX_EXP_ARG,vbs/vt));
                lgbs = SourceSatCur*evbs/vt + ckt->CKTgmin;
		lgbs2 = model->VDMOStype *0.5 * (lgbs - ckt->CKTgmin)/vt;
		lgbs3 = model->VDMOStype *lgbs2/(vt*3);

            }
            if(vbd <= 0) {
                lgbd = DrainSatCur/vt;
                lgbd += ckt->CKTgmin;
		lgbd2 = lgbd3 = 0;
            } else {
                ebd = exp(MIN(MAX_EXP_ARG,vbd/vt));
                lgbd = DrainSatCur*ebd/vt +ckt->CKTgmin;
		lgbd2 = model->VDMOStype *0.5 * (lgbd - ckt->CKTgmin)/vt;
		lgbd3 = model->VDMOStype *lgbd2/(vt*3);
            }

            /* now to determine whether the user was able to correctly
             * identify the source and drain of his device
             */
            if(vds >= 0) {
                /* normal mode */
                here->VDMOSmode = 1;
            } else {
                /* inverse mode */
                here->VDMOSmode = -1;
            }

            /*
             *     this block of code evaluates the drain current and its 
             *     derivatives using the shichman-hodges model and the 
             *     charges associated with the gate, channel and bulk for 
             *     mosfets
             *
             */

            /* the following variables are local to this code block until 
             * it is obvious that they can be made global 
             */
	     {
            double betap;
	    double dvondvbs;
	    double d2vondvbs2;
	    double d3vondvbs3;

		    dvondvbs = d2vondvbs2 = d3vondvbs3 = 0.0;

                vgst=(here->VDMOSmode==1?vgs:vgd);
                vdsat=MAX(vgst,0);

                if (vgst <= 0) {
                    /*
                     *     cutoff region
                     */
		    /* cdrain = 0 */
                    gm=0;
                    gds=0;
                    gb=0;
		    gm2=gb2=gds2=0;
		    gmds=gbds=gmb=0;
		    gm3=gb3=gds3=0;
		    gm2ds=gmds2=gm2b=gmb2=gb2ds=gbds2=0;
                } else{
                    /*
                     *     saturation region
                     */

                    betap=Beta*(1+model->VDMOSlambda*(vds*here->VDMOSmode));
			/* cdrain = betap * vgst * vgst * 0.5; */
                    if (vgst <= (vds*here->VDMOSmode)){
                        gm=betap*vgst;
                        gds=model->VDMOSlambda*Beta*vgst*vgst*.5;
                   /*     gb=here->VDMOSgm*arg; */
			gb= -gm*dvondvbs;
			gm2 = betap;
			gds2 = 0;
			gb2 = -(gm*d2vondvbs2 - betap*dvondvbs*dvondvbs);
			gmds = vgst*model->VDMOSlambda*Beta;
			gbds = - gmds*dvondvbs;
			gmb = -betap*dvondvbs;
			gm3 = 0;
			gb3 = -(gmb*d2vondvbs2 + gm*d3vondvbs3 -
				betap*2*dvondvbs*d2vondvbs2);
			gds3 = 0;
			gm2ds = Beta * model->VDMOSlambda;
			gm2b = 0;
			gmb2 = -betap*d2vondvbs2;
			gb2ds = -(gmds*d2vondvbs2 - dvondvbs*dvondvbs*
				Beta * model->VDMOSlambda);
			gmds2 = 0;
			gbds2 = 0;
			gmbds = -Beta * model->VDMOSlambda*dvondvbs;


                    } else {
                    /*
                     *     linear region
                     */
			/* cdrain = betap * vds * (vgst - vds/2); */
                        gm=betap*(vds*here->VDMOSmode);
                        gds= Beta * model->VDMOSlambda*(vgst*
				vds*here->VDMOSmode - vds*vds*0.5) +
				betap*(vgst - vds*here->VDMOSmode);
                        /* gb=gm*arg; */
			gb = - gm*dvondvbs;
			gm2 = 0;
			gb2 = -(gm*d2vondvbs2);
			gds2 = 2*Beta * model->VDMOSlambda*(vgst - 
				vds*here->VDMOSmode) - betap;
			gmds = Beta * model->VDMOSlambda* vds *
				here->VDMOSmode + betap;
			gbds = - gmds*dvondvbs;
			gmb=0;
			gm3=0;
			gb3 = -gm*d3vondvbs3;
			gds3 = -Beta*model->VDMOSlambda*3.;
			gm2ds=gm2b=gmb2=0;
			gmds2 = 2*model->VDMOSlambda*Beta;
			gb2ds = -(gmds*d2vondvbs2);
			gbds2 = -gmds2*dvondvbs;
			gmbds = 0;
                    }
                }
                /*
                 *     finished
                 */
            } /* code block */


            /*
             *  COMPUTE EQUIVALENT DRAIN CURRENT SOURCE
             */
                /* 
                 * now we do the hard part of the bulk-drain and bulk-source
                 * diode - we evaluate the non-linear capacitance and
                 * charge
                 *
                 * the basic equations are not hard, but the implementation
                 * is somewhat long in an attempt to avoid log/exponential
                 * evaluations
                 */
                /*
                 *  charge storage elements
                 *
                 *.. bulk-drain and bulk-source depletion capacitances
                 */
            /*
             *     meyer's capacitor model
             */
	/*
	 * the meyer capacitance equations are in DEVqmeyer
	 * these expressions are derived from those equations.
	 * these expressions are incorrect; they assume just one
	 * controlling variable for each charge storage element
	 * while actually there are several;  the VDMOS small
	 * signal ac linear model is also wrong because it 
	 * ignores controlled capacitive elements. these can be 
	 * corrected (as can the linear ss ac model) if the 
	 * expressions for the charge are available
	 */

	 
{


    double phi;
    double cox;
    double vddif;
    double vddif1;
    double vddif2;
    /* von, vgst and vdsat have already been adjusted for 
        possible source-drain interchange */

    phi = here->VDMOStPhi;
    cox = 0;/*FIXME: can we do disto without knowing the oxide thickness?*/
    if (vgst <= -phi) {
    lcapgb2=lcapgb3=lcapgs2=lcapgs3=lcapgd2=lcapgd3=0;
    } else if (vgst <= -phi/2) {
    lcapgb2= -cox/(4*phi);
    lcapgb3=lcapgs2=lcapgs3=lcapgd2=lcapgd3=0;
    } else if (vgst <= 0) {
    lcapgb2= -cox/(4*phi);
    lcapgb3=lcapgs3=lcapgd2=lcapgd3=0;
    lcapgs2 = cox/(3*phi);
    } else  {			/* the VDMOSmodes are around because 
					vds has not been adjusted */
        if (vdsat <= here->VDMOSmode*vds) {
	lcapgb2=lcapgb3=lcapgs2=lcapgs3=lcapgd2=lcapgd3=0;
        } else {
            vddif = 2.0*vdsat-here->VDMOSmode*vds;
            vddif1 = vdsat-here->VDMOSmode*vds/*-1.0e-12*/;
            vddif2 = vddif*vddif;
	    lcapgd2 = -vdsat*here->VDMOSmode*vds*cox/(3*vddif*vddif2);
	    lcapgd3 = - here->VDMOSmode*vds*cox*(vddif - 6*vdsat)/(9*vddif2*vddif2);
	    lcapgs2 = -vddif1*here->VDMOSmode*vds*cox/(3*vddif*vddif2);
	    lcapgs3 = - here->VDMOSmode*vds*cox*(vddif - 6*vddif1)/(9*vddif2*vddif2);
	    lcapgb2=lcapgb3=0;
        }
    }
    }

		/* the b-s and b-d diodes need no processing ...  */
	here->capbs2 = lcapbs2;
	here->capbs3 = lcapbs3;
	here->capbd2 = lcapbd2;
	here->capbd3 = lcapbd3;
	here->gbs2 = lgbs2;
	here->gbs3 = lgbs3;
	here->gbd2 = lgbd2;
	here->gbd3 = lgbd3;
	here->capgb2 = model->VDMOStype*lcapgb2;
	here->capgb3 = lcapgb3;
                /*
                 *   process to get Taylor coefficients, taking into
		 * account type and mode.
                 */

	if (here->VDMOSmode == 1)
		{
		/* normal mode - no source-drain interchange */

 here->cdr_x2 = gm2;
 here->cdr_y2 = gb2;;
 here->cdr_z2 = gds2;;
 here->cdr_xy = gmb;
 here->cdr_yz = gbds;
 here->cdr_xz = gmds;
 here->cdr_x3 = gm3;
 here->cdr_y3 = gb3;
 here->cdr_z3 = gds3;
 here->cdr_x2z = gm2ds;
 here->cdr_x2y = gm2b;
 here->cdr_y2z = gb2ds;
 here->cdr_xy2 = gmb2;
 here->cdr_xz2 = gmds2;
 here->cdr_yz2 = gbds2;
 here->cdr_xyz = gmbds;

		/* the gate caps have been divided and made into
			Taylor coeffs., but not adjusted for type */

	here->capgs2 = model->VDMOStype*lcapgs2;
	here->capgs3 = lcapgs3;
	here->capgd2 = model->VDMOStype*lcapgd2;
	here->capgd3 = lcapgd3;
} else {
		/*
		 * inverse mode - source and drain interchanged
		 */

here->cdr_x2 = -gm2;
here->cdr_y2 = -gb2;
here->cdr_z2 = -(gm2 + gb2 + gds2 + 2*(gmb + gmds + gbds));
here->cdr_xy = -gmb;
here->cdr_yz = gmb + gb2 + gbds;
here->cdr_xz = gm2 + gmb + gmds;
here->cdr_x3 = -gm3;
here->cdr_y3 = -gb3;
here->cdr_z3 = gm3 + gb3 + gds3 + 
	3*(gm2b + gm2ds + gmb2 + gb2ds + gmds2 + gbds2) + 6*gmbds ;
here->cdr_x2z = gm3 + gm2b + gm2ds;
here->cdr_x2y = -gm2b;
here->cdr_y2z = gmb2 + gb3 + gb2ds;
here->cdr_xy2 = -gmb2;
here->cdr_xz2 = -(gm3 + 2*(gm2b + gm2ds + gmbds) +
					gmb2 + gmds2);
here->cdr_yz2 = -(gb3 + 2*(gmb2 + gb2ds + gmbds) +
					gm2b + gbds2);
here->cdr_xyz = gm2b + gmb2 + gmbds;

          here->capgs2 = model->VDMOStype*lcapgd2;
	  here->capgs3 = lcapgd3;

	  here->capgd2 = model->VDMOStype*lcapgs2;
	  here->capgd3 = lcapgs3;

}

/* now to adjust for type and multiply by factors to convert to Taylor coeffs. */

here->cdr_x2 = 0.5*model->VDMOStype*here->cdr_x2;
here->cdr_y2 = 0.5*model->VDMOStype*here->cdr_y2;
here->cdr_z2 = 0.5*model->VDMOStype*here->cdr_z2;
here->cdr_xy = model->VDMOStype*here->cdr_xy;
here->cdr_yz = model->VDMOStype*here->cdr_yz;
here->cdr_xz = model->VDMOStype*here->cdr_xz;
here->cdr_x3 = here->cdr_x3/6.;
here->cdr_y3 = here->cdr_y3/6.;
here->cdr_z3 = here->cdr_z3/6.;
here->cdr_x2z = 0.5*here->cdr_x2z;
here->cdr_x2y = 0.5*here->cdr_x2y;
here->cdr_y2z = 0.5*here->cdr_y2z;
here->cdr_xy2 = 0.5*here->cdr_xy2;
here->cdr_xz2 = 0.5*here->cdr_xz2;
here->cdr_yz2 = 0.5*here->cdr_yz2;


		}
        }
    return(OK);
    }

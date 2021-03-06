/* Anima system body modeller */
#include "movelib.h"
#include "translib.h"
//#include <malloc.h>
#include <stdlib.h>
//#include <stdio.h>

anifloat MinimalY(Point3 ** pointlist,int * nr);
int  TranslateY(Point3 ** pointlist,anifloat dy);
int  TranslateXZ(Point3 ** pointlist,anifloat dx,anifloat dz);

#include "drawbody.h"
#include "bdyshape.h"

Point3 * CurrentShape=BasicShape;
Point3 BasePos;

#define MAXPOINT LASTPOINT+5
#define AddPoint(p) pointlist[np++]=BodyShape+p


anifloat MinimalY(Point3 ** pointlist,int * nr) {
	int n=0;
	anifloat y,min=10000.0;
	while (pointlist[n] != 0L ) {
		if ((y=pointlist[n]->y) < min ) { min=y;*nr=n;}
		n++;
	}
	return min;
}

int  TranslateY(Point3 ** pointlist,anifloat dy) {
	int n;
	for (n=0;pointlist[n] != 0L; n++ )  pointlist[n]->y+=dy;
	return 0;
}

int  TranslateXZ(Point3 ** pointlist,anifloat dx,anifloat dz) {
	int n;
	for (n=0;pointlist[n] != 0L; n++ ){
		pointlist[n]->x+=dx;
		pointlist[n]->z+=dz;
	}
	return 0;
}


/********************************************************************************/
int gLockPosFlag;  // From mBodyProc
int basenr,dlx=0,dlz=0;

void ModelBody_Q(BODYANGLES_Q PosData, Point3 * * theShapeP ,anifloat * lockxp,anifloat * lockzp,int * locknrp) {
/*  Generate geometry data, to be sent to rendering procedure */
/* Feed in the body angles and generate position of different parts */

	Point3 * pointlist[MAXPOINT];
	Point3 * BodyShape;
	Quat q[10];
	Quat * qPos;
	int i,np,NRightWrist=4,NLeftWrist=3,NUBack=2,NPelvis=1, transformBase;

	qPos=(Quat *)PosData;

	if (*theShapeP == NULL)
		*theShapeP=(Point3 *)malloc(sizeof(BODYSHAPE));
	
	BodyShape=*theShapeP;
	for (np=0;np<MAXPOINT;np++) pointlist[np]=0L;
	for (i=0;i< LASTPOINT ;i++) BodyShape[i]=CurrentShape[i];

	np=0;

	/* And the location on the stage */
	BodyShape[BODYLOCATION].x+= qPos[BODYLOCATION].x;
	BodyShape[BODYLOCATION].y+= qPos[BODYLOCATION].y;
	BodyShape[BODYLOCATION].z+= qPos[BODYLOCATION].z;

	AddPoint(PELVIS);
	/* Add PELVISLOCATION to BodyShape[PELVIS] */
	BodyShape[PELVIS].x+= qPos[PELVISLOC].x;
	BodyShape[PELVIS].y+= qPos[PELVISLOC].y;
	BodyShape[PELVIS].z+= qPos[PELVISLOC].z;

	q[0]=qPos[BODYTURN];
	trfQuat(q,&BodyShape[BODYLOCATION],pointlist);

	transformBase=np;	
	AddPoint(WAIST);
	AddPoint(RWAIST);
	AddPoint(LWAIST);
	AddPoint(RBUTTOCK);
	AddPoint(LBUTTOCK);
	AddPoint(RHIP);
	AddPoint(LHIP);

	/* Now everything is rotated and translated together with pelvis */
	mulQuatQuat(q+NPelvis,qPos+PELVIS,q);
	trfQuat(q+1,BodyShape+PELVIS,pointlist+transformBase);

	transformBase=np;
	AddPoint(UPPERBACK);
	AddPoint(LBREAST);
	AddPoint(RBREAST);
	mulQuatQuat(q,qPos+WAIST,q+NPelvis);
	trfQuat(q,BodyShape+WAIST,pointlist+transformBase);

	transformBase=np;
	AddPoint(NECK);
	AddPoint(RSHOULDER);
	AddPoint(LSHOULDER);
	AddPoint(MSHOULDER);
	mulQuatQuat(q+NUBack,qPos+UPPERBACK,q);
	trfQuat(q+NUBack,BodyShape+UPPERBACK,pointlist+transformBase);

	transformBase=np;
	AddPoint(HEAD);
	mulQuatQuat(q,qPos+NECK,q+NUBack);
	trfQuat(q,BodyShape+NECK,pointlist+transformBase);

	transformBase=np;
	AddPoint(CROWN);
	AddPoint(NOSE);
	AddPoint(HEADCENTER);
	AddPoint(LEYE);
	AddPoint(REYE);
	mulQuatQuat(q,qPos+HEAD,q);
	trfQuat(q,BodyShape+HEAD,pointlist+transformBase);

	transformBase=np;
	AddPoint(LARM);
	mulQuatQuat(q,qPos+LSHOULDER,q+NUBack);
	trfQuat(q,BodyShape+LSHOULDER,pointlist+transformBase);

	transformBase=np;	
	AddPoint(LELBOW);
	mulQuatQuat(q,qPos+LARM,q);
	trfQuat(q,BodyShape+LARM,pointlist+transformBase);
	
	transformBase=np;
	AddPoint(LWRIST);
	mulQuatQuat(q,qPos+LELBOW,q);
	trfQuat(q,BodyShape+LELBOW,pointlist+transformBase);
	
	transformBase=np;   AddPoint(LFINGERS);   AddPoint(LTHUMB);
	mulQuatQuat(q+NLeftWrist,qPos+LWRIST,q);
	trfQuat(q+NLeftWrist,BodyShape+LWRIST,pointlist+transformBase);
	
	transformBase=np;   AddPoint(LFINGER1);   AddPoint(LFINGER4);
	mulQuatQuat(q,qPos+LFINGERS,q+NLeftWrist);
	trfQuat(q,BodyShape+LFINGERS,pointlist+transformBase);
	
	transformBase=np;   AddPoint(LTHUMBNAIL);
	mulQuatQuat(q,qPos+LTHUMB,q+NLeftWrist);
	trfQuat(q,BodyShape+LTHUMB,pointlist+transformBase);
	
	transformBase=np;AddPoint(RARM);
	mulQuatQuat(q,qPos+RSHOULDER,q+NUBack);
	trfQuat(q,BodyShape+RSHOULDER,pointlist+transformBase);
	
	transformBase=np;AddPoint(RELBOW);
	mulQuatQuat(q,qPos+RARM,q);
	trfQuat(q,BodyShape+RARM,pointlist+transformBase);
	
	transformBase=np;   AddPoint(RWRIST);
	mulQuatQuat(q,qPos+RELBOW,q);
	trfQuat(q,BodyShape+RELBOW,pointlist+transformBase);
	
	transformBase=np;   AddPoint(RFINGERS);   AddPoint(RTHUMB);
	mulQuatQuat(q+NRightWrist,qPos+RWRIST,q);
	trfQuat(q+NRightWrist,BodyShape+RWRIST,pointlist+transformBase);
	
	transformBase=np;   AddPoint(RFINGER1);   AddPoint(RFINGER4);
	mulQuatQuat(q,qPos+RFINGERS,q+NRightWrist);
	trfQuat(q,BodyShape+RFINGERS,pointlist+transformBase);
	
	transformBase=np;   AddPoint(RTHUMBNAIL);
	mulQuatQuat(q,qPos+RTHUMB,q+NRightWrist);
	trfQuat(q,BodyShape+RTHUMB,pointlist+transformBase);

	transformBase=np;
	AddPoint(LKNEE);
	AddPoint(LTHIGH);
	mulQuatQuat(q,qPos+LHIP,q+NPelvis);
	trfQuat(q,BodyShape+LHIP,pointlist+transformBase);

	transformBase=np;
	AddPoint(LANKLE);
	mulQuatQuat(q,qPos+LKNEE,q);
	trfQuat(q,BodyShape+LKNEE,pointlist+transformBase);

	transformBase=np;
	AddPoint(LTOE);
	mulQuatQuat(q,qPos+LANKLE,q);
	trfQuat(q,BodyShape+LANKLE,pointlist+transformBase);

	transformBase=np;	
	AddPoint(RKNEE);
	AddPoint(RTHIGH);
	mulQuatQuat(q,qPos+RHIP,q+NPelvis);
	trfQuat(q,BodyShape+RHIP,pointlist+transformBase);

	transformBase=np;
	AddPoint(RANKLE);
	mulQuatQuat(q,qPos+RKNEE,q);
	trfQuat(q,BodyShape+RKNEE,pointlist+transformBase);

	transformBase=np;
	AddPoint(RTOE);
	mulQuatQuat(q,qPos+RANKLE,q);
	trfQuat(q,BodyShape+RANKLE,pointlist+transformBase);

/* Here comes the AutoY height adjustment code */
//BasePos=BodyShape[BODYLOCATION];
if (qPos[BODYLOCATION].y==0) {
	BasePos.y=-MinimalY(pointlist,& basenr);
	if (BasePos.y != 0.0) {
	    TranslateY(pointlist,BasePos.y);
	  }

/* Testing lock function */
	if (gLockPosFlag) {
	/* All glidning samlat n�r l�set sl�pper */
		if ((basenr != *locknrp)){
	   		*locknrp=basenr;
	   		if (*locknrp > -1) {
	   			*lockxp=pointlist[*locknrp]->x;
	   			*lockzp=pointlist[*locknrp]->z;
	   			}
	   		}
		else {
	   		TranslateXZ(pointlist,*lockxp-pointlist[*locknrp]->x,*lockzp-pointlist[*locknrp]->z);
	   		}
	}
      }
else 	*locknrp=-1;

}








#define _CRT_SECURE_NO_WARNINGS
#include "mainFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "structure.h"
#include "mainFunctions_PEUL.h"
#define ALIASING 1
#define PI 3,1415926535
#define MORE

int compare(double const *a, double const *b) {
	if (floor(*a) > floor(*b)) {
		return 1;
	}
	else if (floor(*a) < floor(*b)) {
		return -1;
	}
	else if ((*a - floor(*a)) > (*b - floor(*b))) {
		return 1;
	}
	else if ((*a - floor(*a)) < (*b - floor(*b))) {
		return -1;
	}
	return 0;
}

void sort(double *t) {
	for (int i = 1; i < t[0]; i++) {
		for (int j = i; j < t[0]; j++) {
			if (compare(&t[i], &t[j + 1])==1) {
				double tmp = t[i];
				t[i] = t[j + 1];
				t[j + 1] = tmp;
			}
		}
	}
}

void showTab(double *t) {
	for (int i = 1; i <= t[0]; i++) {
		printf("%.15f ", t[i]);
	}
	printf("\n");
}

double* listingTimes(sParam param, double *t) {
	int nbT = 0;
	for (int i = 0; i < param.nbPolyhedrons; i++) {
		nbT += param.poly[i].nbFaces;
	}
	t = (double*)malloc((nbT + 1) * sizeof(double));
	int cpt = 1;
	t[0] = nbT;
	for (int i = 0; i < param.nbPolyhedrons; i++) {
		for (int j = 0; j < param.poly[i].nbFaces; j++) {
			t[cpt] = -((param.light.paramEqua.x[1] * param.poly[i].face[j].planEqua.a + param.light.paramEqua.y[1] * param.poly[i].face[j].planEqua.b + param.light.paramEqua.z[1] * param.poly[i].face[j].planEqua.c + param.poly[i].face[j].planEqua.d) / (param.poly[i].face[j].planEqua.a * param.light.paramEqua.x[0] + param.poly[i].face[j].planEqua.b * param.light.paramEqua.y[0] + param.poly[i].face[j].planEqua.c * param.light.paramEqua.z[0]));
			cpt++;
		}
	}
	//qsort(t + 1, t[0], sizeof(t), compare); // Ne marche pas
	sort(t);
	return t;
}

void equaParamLight(sParam *param, double X, double Y) {
	/*sFace planImage;
	planImage.nbPeaks = 0;
	planImage.planEqua.a = 1;
	planImage.planEqua.b = 0;
	planImage.planEqua.c = 0;
	planImage.planEqua.d = -1;*/
	double nWidth = (param->image.width) / 256;
	double nHeight = (param->image.height) / 256;
	param->light.paramEqua.x[1] = param->viewerPos.x;  //AAAAAAAAAAAAAAAAAAAH
	param->light.paramEqua.x[0] = 1;
	param->light.paramEqua.y[1] = param->viewerPos.y;
	param->light.paramEqua.y[0] = (nHeight / 2) - (X / 256);
	param->light.paramEqua.z[1] = param->viewerPos.z;
	param->light.paramEqua.z[0] = (nWidth / 2) - (Y / 256);
}

sPos* intersectLight(sParam param, double t) {
	double x = param.light.paramEqua.x[0] * t + param.light.paramEqua.x[1];
	double y = param.light.paramEqua.y[0] * t + param.light.paramEqua.y[1];
	double z = param.light.paramEqua.z[0] * t + param.light.paramEqua.z[1];
	sPos *pos = (sPos*)malloc(sizeof(sPos));
	pos->x = x;
	pos->y = y;
	pos->z = z;
	return pos;
}

void* doesCollide(sParam param, double *t) {
	for (int i = 0; i < param.nbPolyhedrons; i++) {
		for (int j = 1; j <= t[0]; j++) {
			sPosFace *pos = (sPosFace*)malloc(sizeof(sPosFace));
			pos->position = intersectLight(param, t[j]);
			double theta = 0;
			for (int k = 0; k < param.poly[i].nbFaces; k++) {
				if (i == 0 && k == 6) {
					i = 0;
				}
				for (int l = 0; l < param.poly[i].face[k].nbPeaks; l++) {
					if (l + 1 < param.poly[i].face[k].nbPeaks) {
						double xps = param.poly[i].face[k].peak[l].x - pos->position->x;
						double yps = param.poly[i].face[k].peak[l].y - pos->position->y;
						double zps = param.poly[i].face[k].peak[l].z - pos->position->z;
						double xpt = param.poly[i].face[k].peak[l + 1].x - pos->position->x;
						double ypt = param.poly[i].face[k].peak[l + 1].y - pos->position->y;
						double zpt = param.poly[i].face[k].peak[l + 1].z - pos->position->z;
						double lengthPs = pow(xps, 2) + pow(yps, 2) + pow(zps, 2);
						double lengthPt = pow(xpt, 2) + pow(ypt, 2) + pow(zpt, 2);
						theta += acos((xps*xpt + yps*ypt + zps*zpt) / sqrt(lengthPs*lengthPt));
					}
					else {
						double xps = param.poly[i].face[k].peak[l].x - pos->position->x;
						double yps = param.poly[i].face[k].peak[l].y - pos->position->y;
						double zps = param.poly[i].face[k].peak[l].z - pos->position->z;
						double xpt = param.poly[i].face[k].peak[0].x - pos->position->x;
						double ypt = param.poly[i].face[k].peak[0].y - pos->position->y;
						double zpt = param.poly[i].face[k].peak[0].z - pos->position->z;
						double lengthPs = (pow(xps, 2) + pow(yps, 2) + pow(zps, 2));
						double lengthPt = (pow(xpt, 2) + pow(ypt, 2) + pow(zpt, 2));
						theta += acos((xps*xpt + yps*ypt + zps*zpt) / sqrt(lengthPs*lengthPt));
					}
				}
				theta /= 2 * PI;  //precision environ egale a 2.6646.10^-15
				//printf("%.16f\n", theta);
				if (theta > 1.047197/*1.047197551196*/ && theta < 1.047198/*1.047197551197*/) {  //1.0471975511965976
					pos->iFace = k;
					pos->iPoly = i;
					return pos;
				}
				theta = 0;
			}
			free(pos);
		}
	}
	return false;
}

#ifdef MORE

sColor pixelAvg(sColor pixel, sFile *imageStart, sFile *imageEnd, int w, int h, sParam param) {
	int averageR = 0;
	int averageG = 0;
	int averageB = 0;
	int nbColMax = param.image.width;
	int nbLigneMax = param.image.height;
	int n = 0;
	for (int i = w - ALIASING; i <= w + ALIASING; i++) {
		for (int j = h - ALIASING; j <= h + ALIASING; j++) {
			if ((i >= 0 && j >= 0) && i != nbColMax + 1 && j != nbLigneMax + 1 && (i != w || j != h)) {
				//if(imageStart->data[nbColMax * j + i].r - )
				averageR = averageR + imageStart->data[nbColMax * j + i].r;
				averageG = averageG + imageStart->data[nbColMax * j + i].g;
				averageB = averageB + imageStart->data[nbColMax * j + i].b;
				n++;
			}
		}
	}
	if (n != 0) {
		pixel.r = (int)(averageR / n);
		pixel.g = (int)(averageG / n);
		pixel.b = (int)(averageB / n);
		return pixel;
	}
}

int createImage(sPos posLight, sParam param) {
	sFile* I = newBMP(param.image.width, param.image.height);
	int cptPoly = -1;
	int cptFace = -1;
	int cptSphere = -1;
	for (int w = 1; w <= param.image.width; w++) {
		for (int h = 1; h <= param.image.height; h++) {
			double *t = NULL;
			equaParamLight(&param, w, h); ///////BOUCLE
			t = listingTimes(param, t);
			//showTab(t);
			sPosFace *posPointObjet = NULL;
			sPosSphere *posPointSphere = NULL;
			sColor p;
			float lightFactor = param.light.lightFactor;
			if ((posPointObjet = doesCollide(param, t)) || (posPointSphere = doesCollideSphere(param))) {
				if (posPointObjet && !posPointSphere) {
					cptPoly = posPointObjet->iPoly;
					cptFace = posPointObjet->iFace;
					if (isInTheShadow(*(posPointObjet->position), param)) {
						lightFactor -= 0.3;
					}
					p.r = param.poly[cptPoly].face[cptFace].color.r * lightFactor;
					p.g = param.poly[cptPoly].face[cptFace].color.g * lightFactor;
					p.b = param.poly[cptPoly].face[cptFace].color.b * lightFactor;
					setcolor(I, w - 1, h - 1, p);
				}
				else if (posPointSphere && !posPointObjet) {
					cptSphere = posPointSphere->iSphere;
					if (isInTheShadow(*(posPointSphere->position), param)) {
						lightFactor -= 0.3;
					}
					p.r = param.sphere[cptSphere].color.r * lightFactor;
					p.g = param.sphere[cptSphere].color.g * lightFactor;
					p.b = param.sphere[cptSphere].color.b * lightFactor;
					setcolor(I, w - 1, h - 1, p);
				}
				else if (posPointSphere != NULL && posPointObjet != NULL) {
					if (posPointObjet->position->x > posPointSphere->position->x) {
						p.r = param.sphere[cptSphere].color.r * lightFactor;
						p.g = param.sphere[cptSphere].color.g * lightFactor;
						p.b = param.sphere[cptSphere].color.b * lightFactor;
						setcolor(I, w - 1, h - 1, p);
					}
					else {
						p.r = param.poly[cptPoly].face[cptFace].color.r * lightFactor;
						p.g = param.poly[cptPoly].face[cptFace].color.g * lightFactor;
						p.b = param.poly[cptPoly].face[cptFace].color.b * lightFactor;
						setcolor(I, w - 1, h - 1, p);
					}
				}
				else if (posPointObjet == NULL) {
					p.r = param.sphere[cptSphere].color.r * lightFactor;
					p.g = param.sphere[cptSphere].color.g * lightFactor;
					p.b = param.sphere[cptSphere].color.b * lightFactor;
					setcolor(I, w - 1, h - 1, p);
				}
				else {
					p.r = param.poly[cptPoly].face[cptFace].color.r * lightFactor;
					p.g = param.poly[cptPoly].face[cptFace].color.g * lightFactor;
					p.b = param.poly[cptPoly].face[cptFace].color.b * lightFactor;
					setcolor(I, w - 1, h - 1, p);
				}
				free(posPointSphere);
				free(t);
				free(posPointObjet);
			}
			else {
				p.r = param.image.background.r;
				p.g = param.image.background.g;
				p.b = param.image.background.b;
				setcolor(I, w - 1, h - 1, p);
			}
		}
	}
	char nameI[50];
	char nameJ[50];
	///
	sFile* J = newBMP(param.image.width, param.image.height);
	sColor p;
	for (int w = 0; w < param.image.width; w++) {
		for (int h = 0; h < param.image.height; h++) {
			p = I->data[(int)param.image.height * w + h];
			p = pixelAvg(p, I, J, w, h, param);
			setcolor(J, w, h, p);
		}
	}
	///
	strcpy(nameI, param.image.name);
	strcat(nameI, ".bmp");
	saveBMP(I, nameI);
	strcpy(nameJ, param.image.name);
	strcat(nameJ, "2.bmp");
	saveBMP(J, nameJ);
	deleteBMP(I);
	deleteBMP(J);
	return 1;
}

#endif // MORE


//TPS ESTIME 80 000px par seconde
//TPS ESTIME 16 350px par seconde

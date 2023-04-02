// *****************************************************************************
// *****************************************************************************
// Copyright 2014 - 2017, Cadence Design Systems
// 
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8. 
// 
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
// 
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
// 
//  $Author$
//  $Revision$
//  $Date$
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#ifdef WIN32
#pragma warning (disable : 4786)
#endif

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#ifndef WIN32
#   include <unistd.h>
#else
#   include <windows.h>
#endif /* not WIN32 */
#include "lefrReader.hpp"
#include "lefwWriter.hpp"
#include "lefiDebug.hpp"
#include "lefiEncryptInt.hpp"
#include "lefiUtil.hpp"


#include "stdlib.h"

#include <boost/tokenizer.hpp>


using namespace std;
using namespace LefDef;

vector<MacroPin> _macroPins;

static char defaultName[128];
static char defaultOut[128];
static FILE* fout; //--> move to circuit.h
int printing = 0;     // Printing the output.
int parse65nm = 0;
int parseLef58Type = 0;
int isSessionles = 0;

// TX_DIR:TRANSLATION ON

static void dataError() { fprintf(fout, "ERROR: returned user data is not correct!\n"); }

void checkType(lefrCallbackType_e c) 
{
	if (c >= 0 && c <= lefrLibraryEndCbkType) 
	{
		// OK
	} 
	else 
	{
		fprintf(fout, "ERROR: callback type is out of bounds!\n");
	}
}

static char* orientStr(int orient) {
    switch (orient) {
        case 0: return ((char*)"N");
        case 1: return ((char*)"W");
        case 2: return ((char*)"S");
        case 3: return ((char*)"E");
        case 4: return ((char*)"FN");
        case 5: return ((char*)"FW");
        case 6: return ((char*)"FS");
        case 7: return ((char*)"FE");
    };
    return ((char*)"BOGUS");
}

void lefVia(lefiVia *via) 
{
	int i, j;

	/* Read MacroVia */
	lefrSetCaseSensitivity(1);
    
	if (via->lefiVia::hasDefault())
	{
	}
	else if (via->lefiVia::hasGenerated())
	{
		//    fprintf(fout, "GENERATED");
	}
       
	if (via->lefiVia::numLayers() > 0) 
	{
		for (i = 0; i < via->lefiVia::numLayers(); i++) 
		{
			for (j = 0; j < via->lefiVia::numRects(i); j++)
			{
				if (via->lefiVia::rectColorMask(i, j)) 
				{
				} 
				else 
				{
				}
			}
		}
	}

	/* Store */ 
   
	return;
}

void lefSpacing(lefiSpacing* spacing)          { return; }
void lefViaRuleLayer(lefiViaRuleLayer* vLayer) { return; }

void prtGeometry(lefiGeometries *geometry) {
    int                 numItems = geometry->lefiGeometries::numItems();
    int                 i, j;
    lefiGeomPath        *path;
    lefiGeomPathIter    *pathIter;
    lefiGeomRect        *rect;
    lefiGeomRectIter    *rectIter;
    lefiGeomPolygon     *polygon;
    lefiGeomPolygonIter *polygonIter;
    lefiGeomVia         *via;
    lefiGeomViaIter     *viaIter;

    for (i = 0; i < numItems; i++) {
        switch (geometry->lefiGeometries::itemType(i)) {
            case  lefiGeomClassE:
                fprintf(fout, "CLASS %s ",
                        geometry->lefiGeometries::getClass(i));
                break;
            case lefiGeomLayerE:
                fprintf(fout, "      LAYER %s ;\n",
                        geometry->lefiGeometries::getLayer(i));
                break;
            case lefiGeomLayerExceptPgNetE:
                fprintf(fout, "      EXCEPTPGNET ;\n");
                break;
            case lefiGeomLayerMinSpacingE:
                fprintf(fout, "      SPACING %g ;\n",
                        geometry->lefiGeometries::getLayerMinSpacing(i));
                break;
            case lefiGeomLayerRuleWidthE:
                fprintf(fout, "      DESIGNRULEWIDTH %g ;\n",
                        geometry->lefiGeometries::getLayerRuleWidth(i));
                break;
            case lefiGeomWidthE:
                fprintf(fout, "      WIDTH %g ;\n",
                        geometry->lefiGeometries::getWidth(i));
                break;
            case lefiGeomPathE:
                path = geometry->lefiGeometries::getPath(i);
                if (path->colorMask != 0) {
                    fprintf(fout, "      PATH MASK %d ", path->colorMask);
                } else {
                    fprintf(fout, "      PATH ");
                }
                for (j = 0; j < path->numPoints; j++) {
                    if (j + 1 == path->numPoints) // last one on the list
                        fprintf(fout, "      ( %g %g ) ;\n", path->x[j], path->y[j]);
                    else
                        fprintf(fout, "      ( %g %g )\n", path->x[j], path->y[j]);
                }
                break;
            case lefiGeomPathIterE:
                pathIter = geometry->lefiGeometries::getPathIter(i);
                if (pathIter->colorMask != 0) {
                    fprintf(fout, "      PATH MASK %d ITERATED ", pathIter->colorMask);
                } else {
                    fprintf(fout, "      PATH ITERATED ");
                }
                for (j = 0; j < pathIter->numPoints; j++)
                    fprintf(fout, "      ( %g %g )\n", pathIter->x[j],
                            pathIter->y[j]);
                fprintf(fout, "      DO %g BY %g STEP %g %g ;\n", pathIter->xStart,
                        pathIter->yStart, pathIter->xStep, pathIter->yStep);
                break;
            case lefiGeomRectE:
                rect = geometry->lefiGeometries::getRect(i);
                if (rect->colorMask != 0) {
                    fprintf(fout, "      RECT MASK %d ( %f %f ) ( %f %f ) ;\n",
                            rect->colorMask, rect->xl,
                            rect->yl, rect->xh, rect->yh);
                } else {
                    fprintf(fout, "      RECT ( %f %f ) ( %f %f ) ;\n", rect->xl,
                            rect->yl, rect->xh, rect->yh);
                }
                break;
            case lefiGeomRectIterE:
                rectIter = geometry->lefiGeometries::getRectIter(i);
                if (rectIter->colorMask != 0) {
                    fprintf(fout, "      RECT MASK %d ITERATE ( %f %f ) ( %f %f )\n",
                            rectIter->colorMask,
                            rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
                } else {
                    fprintf(fout, "      RECT ITERATE ( %f %f ) ( %f %f )\n",
                            rectIter->xl, rectIter->yl, rectIter->xh, rectIter->yh);
                }
                fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                        rectIter->xStart, rectIter->yStart, rectIter->xStep,
                        rectIter->yStep);
                break;
            case lefiGeomPolygonE:
                polygon = geometry->lefiGeometries::getPolygon(i);
                if (polygon->colorMask != 0) {
                    fprintf(fout, "      POLYGON MASK %d ", polygon->colorMask);
                } else {
                    fprintf(fout, "      POLYGON ");
                }
                for (j = 0; j < polygon->numPoints; j++) {
                    if (j + 1 == polygon->numPoints) // last one on the list
                        fprintf(fout, "      ( %g %g ) ;\n", polygon->x[j],
                                polygon->y[j]);
                    else
                        fprintf(fout, "      ( %g %g )\n", polygon->x[j],
                                polygon->y[j]);
                }
                break;
            case lefiGeomPolygonIterE:
                polygonIter = geometry->lefiGeometries::getPolygonIter(i);
                if (polygonIter->colorMask != 0) {
                    fprintf(fout, "       POLYGON MASK %d ITERATE ", polygonIter->colorMask);
                } else {
                    fprintf(fout, "      POLYGON ITERATE");
                }
                for (j = 0; j < polygonIter->numPoints; j++)
                    fprintf(fout, "      ( %g %g )\n", polygonIter->x[j],
                            polygonIter->y[j]);
                fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                        polygonIter->xStart, polygonIter->yStart,
                        polygonIter->xStep, polygonIter->yStep);
                break;
            case lefiGeomViaE:
                via = geometry->lefiGeometries::getVia(i);
                if (via->topMaskNum != 0 || via->bottomMaskNum != 0 || via->cutMaskNum !=0) {
                    fprintf(fout, "      VIA MASK %d%d%d ( %g %g ) %s ;\n",
                            via->topMaskNum, via->cutMaskNum, via->bottomMaskNum,
                            via->x, via->y,
                            via->name);

                } else {
                    fprintf(fout, "      VIA ( %g %g ) %s ;\n", via->x, via->y,
                            via->name);
                }
                break;
            case lefiGeomViaIterE:
                viaIter = geometry->lefiGeometries::getViaIter(i);
                if (viaIter->topMaskNum != 0 || viaIter->cutMaskNum != 0 || viaIter->bottomMaskNum != 0) {
                    fprintf(fout, "      VIA ITERATE MASK %d%d%d ( %g %g ) %s\n",
                            viaIter->topMaskNum, viaIter->cutMaskNum, viaIter->bottomMaskNum,
                            viaIter->x,
                            viaIter->y, viaIter->name);
                } else {
                    fprintf(fout, "      VIA ITERATE ( %g %g ) %s\n", viaIter->x,
                            viaIter->y, viaIter->name);
                }
                fprintf(fout, "      DO %g BY %g STEP %g %g ;\n",
                        viaIter->xStart, viaIter->yStart,
                        viaIter->xStep, viaIter->yStep);
                break;
            default:
                fprintf(fout, "BOGUS geometries type.\n");
                break;
        }
    }
}

int antennaCB(lefrCallbackType_e c, double value, lefiUserData) {
    checkType(c);
    // if ((long)ud != userData) dataError();

    switch (c) {
        case lefrAntennaInputCbkType:
            //fprintf(fout, "ANTENNAINPUTGATEAREA %g ;\n", value);
            break;
        case lefrAntennaInoutCbkType:
            //fprintf(fout, "ANTENNAINOUTDIFFAREA %g ;\n", value);
            break;
        case lefrAntennaOutputCbkType:
            //fprintf(fout, "ANTENNAOUTPUTDIFFAREA %g ;\n", value);
            break;
        case lefrInputAntennaCbkType:
            //fprintf(fout, "INPUTPINANTENNASIZE %g ;\n", value);
            break;
        case lefrOutputAntennaCbkType:
            //fprintf(fout, "OUTPUTPINANTENNASIZE %g ;\n", value);
            break;
        case lefrInoutAntennaCbkType:
            //fprintf(fout, "INOUTPINANTENNASIZE %g ;\n", value);
            break;
        default:
            //fprintf(fout, "BOGUS antenna type.\n");
            break;
    }
    return 0;
}

int arrayBeginCB(lefrCallbackType_e c, const char* name, lefiUserData) 
{
	int  status;

	checkType(c);
	status = lefwStartArray(name);
	if (status != LEFW_OK)
		return status;
	return 0;
}

int arrayCB(lefrCallbackType_e c, lefiArray* a, lefiUserData) {
    int              status, i, j, defCaps;
    lefiSitePattern* pattern;
    lefiTrackPattern* track;
    lefiGcellPattern* gcell;

    checkType(c);
    // if ((long)ud != userData) dataError();

    if (a->lefiArray::numSitePattern() > 0) {
        for (i = 0; i < a->lefiArray::numSitePattern(); i++) {
            pattern = a->lefiArray::sitePattern(i);
            status = lefwArraySite(pattern->lefiSitePattern::name(),
                    pattern->lefiSitePattern::x(),
                    pattern->lefiSitePattern::y(),
                    pattern->lefiSitePattern::orient(),
                    pattern->lefiSitePattern::xStart(),
                    pattern->lefiSitePattern::yStart(),
                    pattern->lefiSitePattern::xStep(),
                    pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }
    if (a->lefiArray::numCanPlace() > 0) {
        for (i = 0; i < a->lefiArray::numCanPlace(); i++) {
            pattern = a->lefiArray::canPlace(i);
            status = lefwArrayCanplace(pattern->lefiSitePattern::name(),
                    pattern->lefiSitePattern::x(),
                    pattern->lefiSitePattern::y(),
                    pattern->lefiSitePattern::orient(),
                    pattern->lefiSitePattern::xStart(),
                    pattern->lefiSitePattern::yStart(),
                    pattern->lefiSitePattern::xStep(),
                    pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }
    if (a->lefiArray::numCannotOccupy() > 0) {
        for (i = 0; i < a->lefiArray::numCannotOccupy(); i++) {
            pattern = a->lefiArray::cannotOccupy(i);
            status = lefwArrayCannotoccupy(pattern->lefiSitePattern::name(),
                    pattern->lefiSitePattern::x(),
                    pattern->lefiSitePattern::y(),
                    pattern->lefiSitePattern::orient(),
                    pattern->lefiSitePattern::xStart(),
                    pattern->lefiSitePattern::yStart(),
                    pattern->lefiSitePattern::xStep(),
                    pattern->lefiSitePattern::yStep());
            if (status != LEFW_OK)
                dataError();
        }
    }

    if (a->lefiArray::numTrack() > 0) {
        for (i = 0; i < a->lefiArray::numTrack(); i++) {
            track = a->lefiArray::track(i);
            fprintf(fout, "  TRACKS %s, %g DO %d STEP %g\n",
                    track->lefiTrackPattern::name(),
                    track->lefiTrackPattern::start(), 
                    track->lefiTrackPattern::numTracks(), 
                    track->lefiTrackPattern::space()); 
            if (track->lefiTrackPattern::numLayers() > 0) {
                fprintf(fout, "  LAYER ");
                for (j = 0; j < track->lefiTrackPattern::numLayers(); j++)
                    fprintf(fout, "%s ", track->lefiTrackPattern::layerName(j));
                fprintf(fout, ";\n"); 
            }
        }
    }

    if (a->lefiArray::numGcell() > 0) {
        for (i = 0; i < a->lefiArray::numGcell(); i++) {
            gcell = a->lefiArray::gcell(i);
            fprintf(fout, "  GCELLGRID %s, %g DO %d STEP %g\n",
                    gcell->lefiGcellPattern::name(),
                    gcell->lefiGcellPattern::start(), 
                    gcell->lefiGcellPattern::numCRs(), 
                    gcell->lefiGcellPattern::space()); 
        }
    }

    if (a->lefiArray::numFloorPlans() > 0) {
        for (i = 0; i < a->lefiArray::numFloorPlans(); i++) {
            status = lefwStartArrayFloorplan(a->lefiArray::floorPlanName(i));
            if (status != LEFW_OK)
                dataError();
            for (j = 0; j < a->lefiArray::numSites(i); j++) {
                pattern = a->lefiArray::site(i, j);
                status = lefwArrayFloorplan(a->lefiArray::siteType(i, j),
                        pattern->lefiSitePattern::name(),
                        pattern->lefiSitePattern::x(),
                        pattern->lefiSitePattern::y(),
                        pattern->lefiSitePattern::orient(),
                        (int)pattern->lefiSitePattern::xStart(),
                        (int)pattern->lefiSitePattern::yStart(),
                        pattern->lefiSitePattern::xStep(),
                        pattern->lefiSitePattern::yStep());
                if (status != LEFW_OK)
                    dataError();
            }
            status = lefwEndArrayFloorplan(a->lefiArray::floorPlanName(i));
            if (status != LEFW_OK)
                dataError();
        }
    }

    defCaps = a->lefiArray::numDefaultCaps();
    if (defCaps > 0) {
        status = lefwStartArrayDefaultCap(defCaps);
        if (status != LEFW_OK)
            dataError();
        for (i = 0; i < defCaps; i++) {
            status = lefwArrayDefaultCap(a->lefiArray::defaultCapMinPins(i),
                    a->lefiArray::defaultCap(i));
            if (status != LEFW_OK)
                dataError();
        }
        status = lefwEndArrayDefaultCap();
        if (status != LEFW_OK)
            dataError();
    }
    return 0;
}

int arrayEndCB(lefrCallbackType_e c, const char* name, lefiUserData) {
	int  status;

	checkType(c);
	// if ((long)ud != userData) dataError();
	// use the lef writer to write the data out
	status = lefwEndArray(name);
	if (status != LEFW_OK)
		return status;
	return 0;
}

int busBitCharsCB(lefrCallbackType_e c, const char* busBit, lefiUserData)
{
	checkType(c);
	return 0;
}

int caseSensCB(lefrCallbackType_e c, int caseSense, lefiUserData) {
    checkType(c);
    // if ((long)ud != userData) dataError();

    if (caseSense == TRUE)
        fprintf(fout, "NAMESCASESENSITIVE ON ;\n");
    else
        fprintf(fout, "NAMESCASESENSITIVE OFF ;\n");
    return 0;
}

int fixedMaskCB(lefrCallbackType_e c, int fixedMask, lefiUserData) {
    checkType(c);

    if (fixedMask == 1) 
        fprintf(fout, "FIXEDMASK ;\n");
    return 0;
}

int clearanceCB(lefrCallbackType_e c, const char* name, lefiUserData) {
    checkType(c);
    return 0;
}

int dividerCB(lefrCallbackType_e c, const char* name, lefiUserData) {
    checkType(c);
    return 0;
}

int noWireExtCB(lefrCallbackType_e c, const char* name, lefiUserData) {
    checkType(c);
    return 0;
}

int noiseMarCB(lefrCallbackType_e c, lefiNoiseMargin *, lefiUserData) {
    checkType(c);
    return 0;
}

int edge1CB(lefrCallbackType_e c, double name, lefiUserData) {
    checkType(c);
    return 0;
}

int edge2CB(lefrCallbackType_e c, double name, lefiUserData) {
    checkType(c);
    return 0;
}

int edgeScaleCB(lefrCallbackType_e c, double name, lefiUserData) {
    checkType(c);
    return 0;
}

int noiseTableCB(lefrCallbackType_e c, lefiNoiseTable *, lefiUserData) {
    checkType(c);
    return 0;
}

int correctionCB(lefrCallbackType_e c, lefiCorrectionTable *, lefiUserData) {
    checkType(c);
    return 0;
}

int dielectricCB(lefrCallbackType_e c, double dielectric, lefiUserData) {
    checkType(c);
    return 0;
}

int irdropBeginCB(lefrCallbackType_e c, void*, lefiUserData){
    checkType(c);
    return 0;
}

int irdropCB(lefrCallbackType_e c, lefiIRDrop* irdrop, lefiUserData) {
    int i;
    checkType(c);
    return 0;
}

int irdropEndCB(lefrCallbackType_e c, void*, lefiUserData){
    checkType(c);
    return 0;
}

int layerCB(lefrCallbackType_e c, lefiLayer* _layer, lefiUserData) {

	int i, j, k;
    int numPoints, propNum;
    double *widths, *current;
    lefiLayerDensity* density;
    lefiAntennaPWL* pwl;
    lefiSpacingTable* spTable;
    lefiInfluence* influence;
    lefiParallel* parallel;
    lefiTwoWidths* twoWidths;
    char pType;
    int numMinCut, numMinenclosed;
    lefiAntennaModel* aModel;
    lefiOrthogonal*   ortho;



    checkType(c);
    // if ((long)ud != userData) dataError();

    lefrSetCaseSensitivity(0);

    // Call parse65nmRules for 5.7 syntax in 5.6
    if (parse65nm)
        _layer->lefiLayer::parse65nmRules();

    // Call parseLef58Type for 5.8 syntax in 5.7
    if (parseLef58Type)
        _layer->lefiLayer::parseLEF58Layer();

    //fprintf(fout, "LAYER %s\n", _layer->lefiLayer::name());
    if (_layer->lefiLayer::hasType())
    {
    }
        
    if (_layer->lefiLayer::hasLayerType())
    {
    }
    
    if (_layer->lefiLayer::hasMask())
    {
    }
        
    if (_layer->lefiLayer::hasPitch())
    {
    }
    else if (_layer->lefiLayer::hasXYPitch()) 
    {
	}
    
    if (_layer->lefiLayer::hasOffset())
    {
    }
    else if (_layer->lefiLayer::hasXYOffset()) 
    {
	}

    if (_layer->lefiLayer::hasDiagPitch())
		{
		}
    else if (_layer->lefiLayer::hasXYDiagPitch())
		{
		}
    if (_layer->lefiLayer::hasDiagWidth())
		{
		}

    if (_layer->lefiLayer::hasDiagSpacing())
		{
		}
    
    if (_layer->lefiLayer::hasWidth())
    {
    }
    if (_layer->lefiLayer::hasArea())
    {
    }
    
    if (_layer->lefiLayer::hasMinwidth()) 
    {
    }

    if (_layer->lefiLayer::hasSpacingNumber()) 
    {
			for (i = 0; i < _layer->lefiLayer::numSpacing(); i++) 
      {
				if (_layer->lefiLayer::hasSpacingEndOfLine(i)) 
        {       
					if (_layer->lefiLayer::hasSpacingParellelEdge(i)) 
					{
          }
        }
      }
    }

    if (_layer->lefiLayer::hasDirection()) { }

    /* Spacing Table */
    for (i = 0; i < _layer->lefiLayer::numSpacingTable(); i++) {
        spTable = _layer->lefiLayer::spacingTable(i);
        
        if (spTable->lefiSpacingTable::isInfluence()) 
        {
            influence = spTable->lefiSpacingTable::influence();
        } 
        else if(spTable->lefiSpacingTable::isParallel())
        {
            parallel = spTable->lefiSpacingTable::parallel();
            int num_width = parallel->lefiParallel::numWidth();
            int num_length = parallel->lefiParallel::numLength();

            for (j = 0; j < parallel->lefiParallel::numWidth(); j++) 
						{
							for (k = 0; k < parallel->lefiParallel::numLength(); k++) 
							{
              }
            }
        } 
        else 
        {    
            // 5.7 TWOWIDTHS
            twoWidths = spTable->lefiSpacingTable::twoWidths();
        }
    }

    // Set it to case sensitive from here on
    lefrSetCaseSensitivity(1);

    /* Store Layer */
    return 0;
}

int macroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData) {
    checkType(c);
	
    Macro* macro = stdCellLib()->createMacro(macroName);
   
    return 0;
}

int macroFixedMaskCB(lefrCallbackType_e c, int, lefiUserData) 
{
    checkType(c);
    return 0;
}

int macroClassTypeCB(lefrCallbackType_e c, const char* macroClassType, lefiUserData) 
{
    checkType(c);
    return 0;
}

int macroOriginCB(lefrCallbackType_e c, lefiNum, lefiUserData) 
{
    checkType(c);
    return 0;
}

int macroSizeCB(lefrCallbackType_e c, lefiNum, lefiUserData) 
{
    checkType(c);
    return 0;
}

int macroCB(lefrCallbackType_e c, lefiMacro* _macro, lefiUserData) 
{
    lefiSitePattern* pattern;
    int              propNum, i, hasPrtSym = 0;

    checkType(c);

    /* Read Macro */
    Macro* macro = stdCellLib()->getMacro(_macro->lefiMacro::name());

	if(_macro->lefiMacro::hasClass()) 
	{
	}

	if(_macro->lefiMacro::hasXSymmetry()) 
	{
	}

	if(_macro->lefiMacro::hasYSymmetry()) 
	{
	}
    
	if(_macro->lefiMacro::has90Symmetry()) 
	{
	}
    
	if (_macro->lefiMacro::hasSiteName()) 
	{
	}
    
   
	if (_macro->lefiMacro::hasSize()) 
	{
	}
    
    
	if (_macro->lefiMacro::hasForeign()) 
	{
		for (i = 0; i < _macro->lefiMacro::numForeigns(); i++) 
		{                

			if (_macro->lefiMacro::hasForeignPoint(i)) 
			{
      }
    }
  }

	if (_macro->lefiMacro::hasOrigin()) 
	{
	}
    
	if (_macro->lefiMacro::hasPower())
	{
	}
    
	return 0;
}

int macroEndCB(lefrCallbackType_e c, const char* macroName, lefiUserData) 
{
	checkType(c);

	// JKIm

	Macro* macro = stdCellLib()->getMacro(macroName);

	for(int i=0; i < _macroPins.size(); i++) 
	{
		MacroPin* temp = &_macroPins[i];
        
		MacroPin* macroPin = macro->createMacroPin(temp->getName());
		macroPin->setDirection(temp->getDirection());
		macroPin->setUseType(temp->getUseType());
	}
	
	_macroPins.clear();
	vector<MacroPin>().swap(_macroPins);

	return 0;
}

int manufacturingCB(lefrCallbackType_e c, double num, lefiUserData) 
{
	checkType(c);
	return 0;
}

int maxStackViaCB(lefrCallbackType_e c, lefiMaxStackVia* maxStack, lefiUserData) 
{
	checkType(c);
	return 0;
}

int minFeatureCB(lefrCallbackType_e c, lefiMinFeature* min, lefiUserData) 
{
	checkType(c);
	return 0;
}

int nonDefaultCB(lefrCallbackType_e c, lefiNonDefault* def, lefiUserData) {
	int          i;
	lefiVia*     via;
	lefiSpacing* spacing;

	checkType(c);
    
	// handle via in nondefaultrule
	for (i = 0; i < def->lefiNonDefault::numVias(); i++) 
	{
		via = def->lefiNonDefault::viaRule(i);
		lefVia(via);
	}

	// handle spacing in nondefaultrule
	for (i = 0; i < def->lefiNonDefault::numSpacingRules(); i++) 
	{
		spacing = def->lefiNonDefault::spacingRule(i);
		lefSpacing(spacing);
	}

	return 0;
}

int obstructionCB(lefrCallbackType_e c, lefiObstruction* obs, lefiUserData) 
{
	lefiGeometries* geometry;

	checkType(c);
	// if ((long)ud != userData) dataError();
	geometry = obs->lefiObstruction::geometries();
	for(int i=0; i < geometry->numItems(); i++) 
	{
		string _lName = geometry->getLayer(i);
		double x1 = geometry->getRect(i)->xl;
		double y1 = geometry->getRect(i)->yl;
		double x2 = geometry->getRect(i)->xh;
		double y2 = geometry->getRect(i)->yh;
	}
  
	return 0;
}

int pinCB(lefrCallbackType_e c, lefiPin* _pin, lefiUserData) 
{
	int                  numPorts, i, j;
	lefiGeometries*      geometry;
	lefiPinAntennaModel* aModel;
	checkType(c);

	// JKIm

	MacroPin macroPin;
	macroPin.setName(_pin->lefiPin::name());
    
	if (_pin->lefiPin::hasDirection())
	{
		macroPin.setDirection(toDirecType(_pin->lefiPin::direction()));
	}

	if (_pin->lefiPin::hasUse()) 
	{
		macroPin.setUseType(toUseType(_pin->lefiPin::use()));
	}  
	else
	{
		//cout << newPin.name << " SIGNAL" << endl;
		macroPin.setUseType(UseType::SIGNAL);
	}

	if (_pin->lefiPin::hasShape())
	{
	}

	if (_pin->lefiPin::hasAntennaDiffArea()) 
	{
		for (i = 0; i < _pin->lefiPin::numAntennaDiffArea(); i++) 
		{
			double _area = _pin->lefiPin::antennaDiffArea(i);
			string _layer = "";
			if (_pin->lefiPin::antennaDiffAreaLayer(i))
				_layer = _pin->lefiPin::antennaDiffAreaLayer(i);
		}
	}

	/*
	if(ckt->clkPort == "")
	{
		// only TSMC 65nm
		if(newPin.name == "CP")
		{
			newPin.use = UseType::CLOCK;
		}
	}
	else
	{
		// 
		if(newPin.name == ckt->clkPort)
		{
			newPin.use = UseType::CLOCK;
		}
	}
	*/

	numPorts = _pin->lefiPin::numPorts();
    
	//cout << newPin.name << " -> " << endl;
	for (int i = 0; i < numPorts; i++) 
	{
		geometry = _pin->lefiPin::port(i);
		int numItems = geometry->lefiGeometries::numItems();
		
		for (int j = 0; j < numItems; j++) 
		{
			switch (geometry->lefiGeometries::itemType(j)) 
			{
				case lefiGeomLayerE:
					// push the previous pair
					if(j != 0) 
					{
					}
					break;
                
				case lefiGeomRectE:
					lefiGeomRect* rect = geometry->lefiGeometries::getRect(j);
					break;
			}
		}
       
        // push the last pair
		if(numItems > 0) 
		{
		}

  }
    
	_macroPins.push_back(macroPin);
	return 0;  
}

int densityCB(lefrCallbackType_e c, lefiDensity* density, lefiUserData) 
{
	checkType(c);
	return 0;
}

int propDefBeginCB(lefrCallbackType_e c, void*, lefiUserData) 
{
	checkType(c);
	return 0;
}

int propDefCB(lefrCallbackType_e c, lefiProp* prop, lefiUserData) 
{
	checkType(c);
	return 0;
}

int propDefEndCB(lefrCallbackType_e c, void*, lefiUserData) 
{
	checkType(c);
	return 0;
}

int siteCB(lefrCallbackType_e c, lefiSite* _site, lefiUserData) 
{
	int hasPrtSym = 0;
	int i;
	//Site newSite;

	checkType(c);
	// if ((long)ud != userData) dataError();

	if (_site->lefiSite::hasClass())
	{
		//newSite.type = _site->lefiSite::siteClass();
	}
	if (_site->lefiSite::hasXSymmetry()) 
	{
		//newSite.symmetries.push_back("X");
	}
	if (_site->lefiSite::hasYSymmetry()) 
	{
		//newSite.symmetries.push_back("Y");
	}
    
	if (_site->lefiSite::has90Symmetry()) 
	{
	//newSite.symmetries.push_back("R90");
	}
    
	if (_site->lefiSite::hasSize())   
	{
	}

	return 0;
}

int spacingBeginCB(lefrCallbackType_e c, void*, lefiUserData)
{
	checkType(c);
	return 0;
}

int spacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData) 
{
	checkType(c);
	// if ((long)ud != userData) dataError();
	lefSpacing(spacing);
	return 0;
}

int spacingEndCB(lefrCallbackType_e c, void*, lefiUserData)
{
	checkType(c);
	// if ((long)ud != userData) dataError();
	//fprintf(fout, "END SPACING\n");
	return 0;
}

int timingCB(lefrCallbackType_e c, lefiTiming* timing, lefiUserData) 
{
	int i;
	checkType(c);
	return 0;
}

int unitsCB(lefrCallbackType_e c, lefiUnits* unit, lefiUserData) 
{
	checkType(c);
	// if ((long)ud != userData) dataError();
	if (unit->lefiUnits::hasDatabase())
	{
		int LEFdist2Microns = unit->lefiUnits::databaseNumber();
	}
    
	if (unit->lefiUnits::hasCapacitance())
	{
		int LEF1Cap = unit->lefiUnits::capacitance();
	}
	if (unit->lefiUnits::hasVoltage())
	{
		int LEF1Volt = unit->lefiUnits::voltage();
	}  
	return 0;
}

int useMinSpacingCB(lefrCallbackType_e c, lefiUseMinSpacing* spacing,
        lefiUserData) {
    checkType(c);

	int LEFMinSpacingOBS = spacing->lefiUseMinSpacing::value();

    return 0;
}

int versionCB(lefrCallbackType_e c, double num, lefiUserData) {
    checkType(c);
    // if ((long)ud != userData) dataError();
    //fprintf(fout, "VERSION %g ;\n", num);
	string LEFVersion = to_string(num);
    return 0;
}

int versionStrCB(lefrCallbackType_e c, const char* versionName, lefiUserData) {
    checkType(c);
    // if ((long)ud != userData) dataError();
    //fprintf(fout, "VERSION %s ;\n", versionName);
    return 0;
}

int viaCB(lefrCallbackType_e c, lefiVia* via, lefiUserData) {
    checkType(c);
    // if ((long)ud != userData) dataError();
    lefVia(via);
    return 0;
}

int viaRuleCB(lefrCallbackType_e c, lefiViaRule* viaRule, lefiUserData) {
    int               numLayers, numVias, i;
    lefiViaRuleLayer* vLayer;

    checkType(c);
    // if ((long)ud != userData) dataError();
    //fprintf(fout, "VIARULE %s", viaRule->lefiViaRule::name());
    //if (viaRule->lefiViaRule::hasGenerate())
    //    fprintf(fout, " GENERATE");
    //if (viaRule->lefiViaRule::hasDefault())
    //    fprintf(fout, " DEFAULT");
    //fprintf(fout, "\n");

    numLayers = viaRule->lefiViaRule::numLayers();
    // if numLayers == 2, it is VIARULE without GENERATE and has via name
    // if numLayers == 3, it is VIARULE with GENERATE, and the 3rd layer is cut
    for (i = 0; i < numLayers; i++) {
        vLayer = viaRule->lefiViaRule::layer(i); 
        lefViaRuleLayer(vLayer);
    }

    if (numLayers == 2 && !(viaRule->lefiViaRule::hasGenerate())) {
        // should have vianames
        numVias = viaRule->lefiViaRule::numVias();
        //if (numVias == 0)
        //    fprintf(fout, "Should have via names in VIARULE.\n");
        //else {
        //    for (i = 0; i < numVias; i++)
        //        fprintf(fout, "  VIA %s ;\n", viaRule->lefiViaRule::viaName(i));
        //}
    }
    if (viaRule->lefiViaRule::numProps() > 0) {
        //fprintf(fout, "  PROPERTY ");
        for (i = 0; i < viaRule->lefiViaRule::numProps(); i++) {
            //fprintf(fout, "%s ", viaRule->lefiViaRule::propName(i));
            //if (viaRule->lefiViaRule::propValue(i))
            //    fprintf(fout, "%s ", viaRule->lefiViaRule::propValue(i));
            switch (viaRule->lefiViaRule::propType(i)) {
                case 'R': fprintf(fout, "REAL ");
                          break;
                case 'I': fprintf(fout, "INTEGER ");
                          break;
                case 'S': fprintf(fout, "STRING ");
                          break;
                case 'Q': fprintf(fout, "QUOTESTRING ");
                          break;
                case 'N': fprintf(fout, "NUMBER ");
                          break;
            } 
        }
        //fprintf(fout, ";\n");
    }
    return 0;
}

int extensionCB(lefrCallbackType_e c, const char* extsn, lefiUserData) 
{
	checkType(c);
	return 0;
}

int doneCB(lefrCallbackType_e c, void*, lefiUserData) 
{
	checkType(c);
	return 0;
}

void errorCB(const char* msg)         { printf ("%s : %s\n", lefrGetUserData(), msg); }
void warningCB(const char* msg)       { printf ("%s : %s\n", lefrGetUserData(), msg); }
void* mallocCB(int size)              { return malloc(size);        }
void* reallocCB(void* name, int size) { return realloc(name, size); }

static void freeCB(void* name) 
{
	free(name);
	return;
}

void lineNumberCB(int lineNo) 
{
	return;
}

static void printWarning(const char *str)
{
	fprintf(stderr, "%s\n", str);
}

int 
LefDef::readLEF(const char* input) 
{
	const char* inFile[100];
	char* outFile;
	FILE* f;
	int res;
	int noCalls = 0;
	//  long start_mem;
	int num;
	int status;
	int retStr = 0;
	int numInFile = 0;
	int fileCt = 0;
	int relax = 0;
	const char* version = "N/A";
	int setVer = 0;
	char* userData;
	int msgCb = 0;
	int test1 = 0;
	int test2 = 0;
	int ccr749853 = 0;
	int ccr1688946 = 0;
	int ccr1709089 = 0;
	int verbose = 0;
	
	// start_mem = (long)sbrk(0);
	
	userData = strdup ("(lefrw-5100)");
	strcpy(defaultName,"lef.in");
	strcpy(defaultOut,"list");
	inFile[0] = defaultName;
	outFile = defaultOut;
	fout = stdout;
	//  userData = 0x01020304;
	
#ifdef WIN32
	// Enable two-digit exponent format
	_set_output_format(_TWO_DIGIT_EXPONENT);
#endif

	inFile[numInFile++] = input;

	// sets the parser to be case sensitive...
	// default was supposed to be the case but false...
	// lefrSetCaseSensitivity(true);
	if (isSessionles) 
	{
		lefrSetOpenLogFileAppend();
	}

	lefrInitSession(isSessionles ? 0 : 1);

	if (noCalls == 0) 
	{
		lefrSetWarningLogFunction(printWarning);
		lefrSetAntennaInputCbk(antennaCB);
		lefrSetAntennaInoutCbk(antennaCB);
		lefrSetAntennaOutputCbk(antennaCB);
		lefrSetArrayBeginCbk(arrayBeginCB);
		lefrSetArrayCbk(arrayCB);
		lefrSetArrayEndCbk(arrayEndCB);
		lefrSetBusBitCharsCbk(busBitCharsCB);
		lefrSetCaseSensitiveCbk(caseSensCB);
		lefrSetFixedMaskCbk(fixedMaskCB);
		lefrSetClearanceMeasureCbk(clearanceCB);
		lefrSetDensityCbk(densityCB);
		lefrSetDividerCharCbk(dividerCB);
		lefrSetNoWireExtensionCbk(noWireExtCB);
		lefrSetNoiseMarginCbk(noiseMarCB);
		lefrSetEdgeRateThreshold1Cbk(edge1CB);
		lefrSetEdgeRateThreshold2Cbk(edge2CB);
		lefrSetEdgeRateScaleFactorCbk(edgeScaleCB);
		lefrSetExtensionCbk(extensionCB);
		lefrSetNoiseTableCbk(noiseTableCB);
		lefrSetCorrectionTableCbk(correctionCB);
		lefrSetDielectricCbk(dielectricCB);
		lefrSetIRDropBeginCbk(irdropBeginCB);
		lefrSetIRDropCbk(irdropCB);
		lefrSetIRDropEndCbk(irdropEndCB);
		lefrSetLayerCbk(layerCB);
		lefrSetLibraryEndCbk(doneCB); 
		lefrSetMacroBeginCbk(macroBeginCB);
		lefrSetMacroCbk(macroCB);
		lefrSetMacroClassTypeCbk(macroClassTypeCB);
		lefrSetMacroOriginCbk(macroOriginCB);
		lefrSetMacroSizeCbk(macroSizeCB);
		lefrSetMacroFixedMaskCbk(macroFixedMaskCB);
		lefrSetMacroEndCbk(macroEndCB);
		lefrSetManufacturingCbk(manufacturingCB);
		lefrSetMaxStackViaCbk(maxStackViaCB);
		lefrSetMinFeatureCbk(minFeatureCB);
		lefrSetNonDefaultCbk(nonDefaultCB);
		lefrSetObstructionCbk(obstructionCB);
		lefrSetPinCbk(pinCB);
		lefrSetPropBeginCbk(propDefBeginCB);
		lefrSetPropCbk(propDefCB);
		lefrSetPropEndCbk(propDefEndCB);
		lefrSetSiteCbk(siteCB);
		lefrSetSpacingBeginCbk(spacingBeginCB);
		lefrSetSpacingCbk(spacingCB);
		lefrSetSpacingEndCbk(spacingEndCB);
		lefrSetTimingCbk(timingCB);
		lefrSetUnitsCbk(unitsCB);
		lefrSetUseMinSpacingCbk(useMinSpacingCB);
		lefrSetUserData((void*)3);
		if (!retStr)
			lefrSetVersionCbk(versionCB);
		else
			lefrSetVersionStrCbk(versionStrCB);
		lefrSetViaCbk(viaCB);
		lefrSetViaRuleCbk(viaRuleCB);
		lefrSetInputAntennaCbk(antennaCB);
		lefrSetOutputAntennaCbk(antennaCB);
		lefrSetInoutAntennaCbk(antennaCB);
		
		if (msgCb) 
		{
			lefrSetLogFunction(errorCB);
			lefrSetWarningLogFunction(warningCB);
		}
		
		lefrSetMallocFunction(mallocCB);
		lefrSetReallocFunction(reallocCB);
		lefrSetFreeFunction(freeCB);
		
		//lefrSetLineNumberFunction(lineNumberCB);
		//lefrSetDeltaNumberLines(10000);
		
		lefrSetRegisterUnusedCallbacks();
		
		if (relax)
			lefrSetRelaxMode();
		
		if (setVer)
			(void)lefrSetVersionValue(version);
		
		lefrSetAntennaInoutWarnings(30);
		lefrSetAntennaInputWarnings(30);
		lefrSetAntennaOutputWarnings(30);
		lefrSetArrayWarnings(30);
		lefrSetCaseSensitiveWarnings(30);
		lefrSetCorrectionTableWarnings(30);
		lefrSetDielectricWarnings(30);
		lefrSetEdgeRateThreshold1Warnings(30);
		lefrSetEdgeRateThreshold2Warnings(30);
		lefrSetEdgeRateScaleFactorWarnings(30);
		lefrSetInoutAntennaWarnings(30);
		lefrSetInputAntennaWarnings(30);
		lefrSetIRDropWarnings(30);
		lefrSetLayerWarnings(30);
		lefrSetMacroWarnings(30);
		lefrSetMaxStackViaWarnings(30);
		lefrSetMinFeatureWarnings(30);
		lefrSetNoiseMarginWarnings(30);
		lefrSetNoiseTableWarnings(30);
		lefrSetNonDefaultWarnings(30);
		lefrSetNoWireExtensionWarnings(30);
		lefrSetOutputAntennaWarnings(30);
		lefrSetPinWarnings(30);
		lefrSetSiteWarnings(30);
		lefrSetSpacingWarnings(30);
		lefrSetTimingWarnings(30);
		lefrSetUnitsWarnings(30);
		lefrSetUseMinSpacingWarnings(30);
		lefrSetViaRuleWarnings(30);
		lefrSetViaWarnings(30);
		
    (void) lefrSetShiftCase();  // will shift name to uppercase if caseinsensitive
    // is set to off or not set
    if (!isSessionles) {
        lefrSetOpenLogFileAppend();
    }

    if (ccr749853) {
        lefrSetTotalMsgLimit (5);
        lefrSetLimitPerMsg (1618, 2);
    }

    if (ccr1688946) {
        lefrRegisterLef58Type("XYZ", "CUT");
        lefrRegisterLef58Type("XYZ", "CUT");
    }


    if (test1) {  // for special tests
        for (fileCt = 0; fileCt < numInFile; fileCt++) {
            lefrReset();

            if ((f = fopen(inFile[fileCt],"r")) == 0) {
                fprintf(stderr,"Couldn't open input file '%s'\n", inFile[fileCt]);
                return(2);
            }

            (void)lefrEnableReadEncrypted();

            status = lefwInit(fout); // initialize the lef writer,
            // need to be called 1st
            if (status != LEFW_OK)
                return 1;

            res = lefrRead(f, inFile[fileCt], (void*)userData);

            if (res)
                fprintf(stderr, "Reader returns bad status.\n", inFile[fileCt]);

            (void)lefrPrintUnusedCallbacks(fout);
            (void)lefrReleaseNResetMemory();
            //(void)lefrUnsetCallbacks();
            (void)lefrUnsetLayerCbk();
            (void)lefrUnsetNonDefaultCbk();
            (void)lefrUnsetViaCbk();

        }
    }
    else if (test2) {  // for special tests
        // this test is design to test the 3 APIs, lefrDisableParserMsgs,
        // lefrEnableParserMsgs & lefrEnableAllMsgs
        // It uses the file ccr566209.lef.  This file will parser 3 times
        // 1st it will have lefrDisableParserMsgs set to both 2007 & 2008
        // 2nd will enable 2007 by calling lefrEnableParserMsgs
        // 3rd enable all msgs by call lefrEnableAllMsgs

        int nMsgs = 3;
        int dMsgs[3];
        if (numInFile != 1) {
            fprintf(stderr,"Test 2 mode needs only 1 file\n");
            return 2;
        } 

        for (int idx=0; idx<5; idx++) {
            if (idx == 0) {  // msgs 2005 & 2011
                fprintf(stderr,"\nPass 0: Disabling 2007, 2008, 2009\n");
                dMsgs[0] = 2007;
                dMsgs[1] = 2008;
                dMsgs[2] = 2009;
                lefrDisableParserMsgs (3, (int*)dMsgs);
            } else if (idx == 1) { // msgs 2007 & 2005, 2011 did not print because
                fprintf(stderr,"\nPass 1: Enable 2007\n");
                dMsgs[0] = 2007;       // lefrUnsetLayerCbk() was called
                lefrEnableParserMsgs (1, (int*)dMsgs);
            } else if (idx == 2) { // nothing were printed
                fprintf(stderr,"\nPass 2: Disable all\n");
                lefrDisableAllMsgs();
            } else if (idx == 3) { // nothing were printed, lefrDisableParserMsgs
                fprintf(stderr,"\nPass 3: Enable All\n");
                lefrEnableAllMsgs();
            } else if (idx == 4) { // msgs 2005 was printed
                fprintf(stderr,"\nPass 4: Set limit on 2007 up 2\n");
                lefrSetLimitPerMsg (2007, 2);
            } 

            if ((f = fopen(inFile[fileCt],"r")) == 0) {
                fprintf(stderr,"Couldn't open input file '%s'\n", inFile[fileCt]);
                return(2);
            }

            (void)lefrEnableReadEncrypted();

            status = lefwInit(fout); // initialize the lef writer,
            // need to be called 1st
            if (status != LEFW_OK)
                return 1;

            res = lefrRead(f, inFile[fileCt], (void*)userData);

            if (res)
                fprintf(stderr, "Reader returns bad status.\n", inFile[fileCt]);

            (void)lefrPrintUnusedCallbacks(fout);
            (void)lefrReleaseNResetMemory();
            //(void)lefrUnsetCallbacks();
            (void)lefrUnsetLayerCbk();
            (void)lefrUnsetNonDefaultCbk();
            (void)lefrUnsetViaCbk();

        }
    } else {
        for (fileCt = 0; fileCt < numInFile; fileCt++) {
            lefrReset();

            if ((f = fopen(inFile[fileCt],"r")) == 0) {
                fprintf(stderr,"Couldn't open input file '%s'\n", inFile[fileCt]);
                return(2);
            }

            (void)lefrEnableReadEncrypted();

            status = lefwInit(fout); // initialize the lef writer,
            // need to be called 1st
            if (status != LEFW_OK)
                return 1;

            if (ccr1709089) {
                // CCR 1709089 test.
                // Non-initialized lefData case.
                lefrSetLimitPerMsg(10000, 10000);
            }

			//fprintf(fout,"==================================");
			//cout << " ---- lef read start ---- " << endl;
            res = lefrRead(f, inFile[fileCt], (void*)userData);
			//cout << " ---- lef read end ---- " << endl;
            
			if (ccr1709089) {
                // CCR 1709089 test.
                // Initialized lefData case.
                lefrSetLimitPerMsg(10000, 10000);
            }
            if (res)
                fprintf(stderr, "Reader returns bad status.\n", inFile[fileCt]);

            (void)lefrPrintUnusedCallbacks(fout);
            (void)lefrReleaseNResetMemory();

        }
        (void)lefrUnsetCallbacks();
    }
    // Unset all the callbacks
    void lefrUnsetAntennaInputCbk();
    void lefrUnsetAntennaInoutCbk();
    void lefrUnsetAntennaOutputCbk();
    void lefrUnsetArrayBeginCbk();
    void lefrUnsetArrayCbk();
    void lefrUnsetArrayEndCbk();
    void lefrUnsetBusBitCharsCbk();
    void lefrUnsetCaseSensitiveCbk();
    void lefrUnsetFixedMaskCbk();
    void lefrUnsetClearanceMeasureCbk();
    void lefrUnsetCorrectionTableCbk();
    void lefrUnsetDensityCbk();
    void lefrUnsetDielectricCbk();
    void lefrUnsetDividerCharCbk();
    void lefrUnsetEdgeRateScaleFactorCbk();
    void lefrUnsetEdgeRateThreshold1Cbk();
    void lefrUnsetEdgeRateThreshold2Cbk();
    void lefrUnsetExtensionCbk();
    void lefrUnsetInoutAntennaCbk();
    void lefrUnsetInputAntennaCbk();
    void lefrUnsetIRDropBeginCbk();
    void lefrUnsetIRDropCbk();
    void lefrUnsetIRDropEndCbk();
    void lefrUnsetLayerCbk();
    void lefrUnsetLibraryEndCbk();
    void lefrUnsetMacroBeginCbk();
    void lefrUnsetMacroCbk();
    void lefrUnsetMacroClassTypeCbk();
    void lefrUnsetMacroEndCbk();
    void lefrUnsetMacroOriginCbk();
    void lefrUnsetMacroSizeCbk();
    void lefrUnsetManufacturingCbk();
    void lefrUnsetMaxStackViaCbk();
    void lefrUnsetMinFeatureCbk();
    void lefrUnsetNoiseMarginCbk();
    void lefrUnsetNoiseTableCbk();
    void lefrUnsetNonDefaultCbk();
    void lefrUnsetNoWireExtensionCbk();
    void lefrUnsetObstructionCbk();
    void lefrUnsetOutputAntennaCbk();
    void lefrUnsetPinCbk();
    void lefrUnsetPropBeginCbk();
    void lefrUnsetPropCbk();
    void lefrUnsetPropEndCbk();
    void lefrUnsetSiteCbk();
    void lefrUnsetSpacingBeginCbk();
    void lefrUnsetSpacingCbk();
    void lefrUnsetSpacingEndCbk();
    void lefrUnsetTimingCbk();
    void lefrUnsetUseMinSpacingCbk();
    void lefrUnsetUnitsCbk();
    void lefrUnsetVersionCbk();
    void lefrUnsetVersionStrCbk();
    void lefrUnsetViaCbk();
    void lefrUnsetViaRuleCbk();

    //fclose(fout);

    // Release allocated singleton data.
    lefrClear();    

    /*
    for(int i=0; i < macros.size(); i++) {
        macro* theMacro = &macros[i];
        dense_hash_map<string,macroPin>::iterator it;
        for( it = theMacro->pins.begin(); it != theMacro->pins.end(); ++it) {
            macroPin* thePin = &it->second;
            for(int j=0; j < thePin->ports.size(); j++) {
                double xLL = 2000;
                double yLL = 2000;
                double xUR = -2000;
                double yUR = -2000;
                for(int k=0; k < thePin->ports[j].second.size(); k++) {
                   dRect pin_seg = thePin->ports[j].second[k];
                   xLL = min(xLL,pin_seg.ll.x);
                   yLL = min(yLL,pin_seg.ll.y);
                   xUR = max(xUR,pin_seg.ur.x);
                   yUR = max(yUR,pin_seg.ur.y);
                }
                dRect box;
                box.ll.x = xLL;
                box.ll.y = yLL;
                box.ur.x = xUR;
                box.ur.y = yUR;
                thePin->bbox.push_back(make_pair(thePin->ports[j].first,box));
            }
        }

    }

    for(int i=0; i < macroVias.size(); i++) {
        macroVia* theVia = &macroVias[i];
        string lower_layer_name = theVia->cuts[0].first;
        theVia->lowLayer = atoi(lower_layer_name.substr(5).c_str());
    }
    */

    return 0;
}

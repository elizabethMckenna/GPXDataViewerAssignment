/**
 * @file GPXHelpers.h
 * @author Elizabeth McKenna
 * GuelphID: 1053037
 * course: CIS2750, W21
 * @date March 2021
 * @brief:  header file containing helper funtions used in GPXParser.c and GPXParserA2.c
 * @sources:  createValidGPX function is based on code found on http://www.xmlsoft.org/examples/tree1.c, 
 *            haversine funtion is based on code found on https://www.movable-type.co.uk/scripts/latlong.html 
 *            validateSchema funtion is based on code found on http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 */

#ifndef GPX_HELPERS_H
#define GPX_HELPERS_H

#include "GPXParser.h"

#ifndef R
    #define R 6371e3
#endif
#ifndef TO_RAD
    #define TO_RAD M_PI/180
#endif
#ifndef A2_MOD3_DELTA
    #define A2_MOD3_DELTA 10 //in meters
#endif

// ---- A1 ----

//module 1
GPXData *initializeGPXData ();
int isGPXdataValid(GPXData * gpxData);
int initAndParseGPXData(xmlNode * cur_node,List * listToAddTo);

Waypoint *initializeWaypoint ();
int isWaypointValid(Waypoint * waypoint);
int initAndParseWaypoint(xmlNode * cur_node, List * listToAddTo);

Route *initializeRoute ();
int isRouteValid(Route * route);
int initAndParseRoute(xmlNode * cur_node, List * listToBeAddedTo);

TrackSegment *initializeTrackSegment ();
int isTrackSegmentValid(TrackSegment * trackSegment);
int initAndParseTrackSegment(xmlNode * cur_node, List * listToBeAddedTo);

Track *initializeTrack () ;
int isTrackValid(Track * track);
int initAndParseTrack(xmlNode * cur_node, List * listToBeAddedTo);

int isGPXDocNodeValid (GPXdoc * gpxDocNode);
GPXdoc *initializeGPXdoc ();
int parseGPXnode (xmlNode* cur_node, GPXdoc* newGPXdoc);
int gpxParseHelp (xmlNode* a_node, GPXdoc *newGPXdoc);

//module 2
int countWaypointListGPXData(List* listOfWaypoints);
int countRouteListGPXData (List* listOfRoutes);
int countTrackSegmentListGPXData (List* listOfTrackSegments);
int countTrackListGPXData (List* listOfTracks);


// ---- A2 ----

//module 1
bool validateFilename(char * filename, int mode);
bool validateSchema (xmlDocPtr doc, char* gpxSchemaFile);
xmlDocPtr GPXtoXML ( GPXdoc * gpxDoc );
//module 2
float min (float num1, float num2);
float haversine (float lat1, float lon1, float lat2, float lon2);
void deleteDummy(void* data);

#endif
/**
 * @file GPXParser.c
 * @author Elizabeth McKenna
 * GuelphID: 1053037
 * course: CIS2750, W21
 * @date February 2021
 * @brief File containing the function definitions to implemment functions contained in GPXParser.h
 *        Also includes helper funcitons which assist in executing the funtioned declared in the header.
 * @sources:  createGPX function is based on code found on http://www.xmlsoft.org/examples/tree1.c, 
 *            
 */



#include "GPXParser.h"
#include "GPXHelpers.h"
#include "assert.h"


//  ---------------------    GPXData Functions    ---------------------  //


/** 
 * creates and initializes a new waypoint
 *  returns a pointer to the waypoint created
**/ 
GPXData *initializeGPXData () {
    GPXData * newGPXData = (GPXData*)malloc(sizeof(GPXData));

    //initialize waypoint values
    newGPXData->name[0] = '\0';

    return (newGPXData);
}


/** 
 * checks validity of the gpxData passed to it
 * returns: 0 (if valid), (-1 if invalid)
**/
int isGPXdataValid(GPXData * gpxData){ 
    if (gpxData == NULL){
        return -1;
    }
    if (strcmp(gpxData->name,"\0") == 0 ){
        return -1;
    }
    else if (strcmp(gpxData->value, "\0") == 0){
        return -1;
    }
    return 0;
}

/** 
 * initilaizes new GPXData struct and parses xml node passed and adds 
 * struct as node to end of 'listToAddTo' param 
 * assumes that xmlNode is of GPXData type.
 * @returns 0 on success, -1 on failure (invalid)
*/
int initAndParseGPXData(xmlNode * cur_node,List * listToAddTo)
{
    if (cur_node == NULL || listToAddTo == NULL){
        return -1;
    }
    xmlNode * temp_node = cur_node->next;
    char *name = NULL;
    xmlChar *value = NULL;

    for (temp_node = cur_node->children; temp_node != NULL; temp_node = temp_node->next){
        value = xmlNodeGetContent(temp_node);
        //get name of gpxData element
        name = (char*) cur_node->name;
        
        char * valueString = (char*)value;

        GPXData * newGPXData = (GPXData*)malloc(sizeof(GPXData) + (sizeof(char) * strlen(valueString)+1));

        strcpy(newGPXData->name, name);
       // printf("     -- name: '%s'\n", newGPXData->name);

        strcpy(newGPXData->value, valueString);
       // printf("     -- value: '%s'\n", newGPXData->value);
  
        xmlFree(value);  

        if (isGPXdataValid(newGPXData) == 0){
            insertBack(listToAddTo, (void*)newGPXData);
            //printf("List length: %d\n", getLength(newGPXdoc->waypoints));
        } 
        else {
            deleteGpxData(newGPXData);
            return -1;
        }
    } 
    return 0;
}

/** 
 * frees and individual GPXData struct
 *  assumes that data is of type GPXData
**/
void deleteGpxData( void* data){
    if (data != NULL){    
        GPXData * gpxData = (GPXData*)data;
        free(gpxData);
    }
}


/** 
 * returns a pointer to a string representeation of a GPXData struct (on success)
 * returns NULL (on failure)
 * assumes that data passed to it is of type 
**/
char* gpxDataToString( void* data){
    if (data == NULL){
        return NULL;
    }
    GPXData * gpxData = (GPXData *)data;
    if (isGPXdataValid(gpxData) == -1){
        return NULL;
    }
    int numChars = 0;
    char* gpxDataString = NULL;

    numChars = numChars + strlen("    <>  <>\n");
    numChars = numChars + strlen(gpxData->name);
    numChars = numChars + strlen("      -- value: \n");
    numChars = numChars + strlen(gpxData->value);

    gpxDataString = (char*)malloc(numChars+2 * sizeof(char));

    strcpy(gpxDataString, "    <> ");
    strcat(gpxDataString,gpxData->name);
    strcat(gpxDataString," <>\n      -- value: ");
    strcat(gpxDataString,gpxData->value);
    strcat(gpxDataString,"\n");
    
    return(gpxDataString); 
}

int compareGpxData(const void *first, const void *second){return(0);}















//  ---------------------    Waypoint Functions    ---------------------  //

/** creates and initializes a new waypoint
 *  returns a pointer to the waypoint created
**/ 
Waypoint *initializeWaypoint () {
    Waypoint * newWaypoint = (Waypoint*)malloc(sizeof(Waypoint));

    //initialize waypoint values
    newWaypoint->name = NULL;
    newWaypoint->latitude = 1000;   // -90 ≤ validLatitude ≤ 90
    newWaypoint->longitude = 1000;  // -180 ≤ validLongitude ≤ 180
    newWaypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return (newWaypoint);
}

/** 
 * checks validity of the waypoint passed to it
 * returns: 0 (if valid), (-1 if invalid)
 * **/
int isWaypointValid(Waypoint * waypoint){
    if (waypoint == NULL || waypoint->name == NULL || waypoint->otherData == NULL ){
        return -1;
    }
    //if list is non empty, iterate through list and check each GPX data point is valid
    if (getLength(waypoint->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(waypoint->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isGPXdataValid((GPXData *)elem) == -1){
                return -1;
            }
        }
    }
    return 0;
}

/** 
 * initilaizes new Waypoint struct and parses xml node passed and adds 
 * struct as node to end of 'listToAddTo' param 
 * assumes that xmlNode is of Waypoint type.
 * @returns 0 on success, -1 on failure (invalid)
*/
int initAndParseWaypoint(xmlNode * cur_node, List * listToAddTo)
{
    if (cur_node == NULL || listToAddTo == NULL){
        return -1;
    }
    Waypoint * newWaypoint = initializeWaypoint();
    xmlAttr *attr;
    xmlNode * temp_node;
    xmlChar *name = NULL;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode* value = attr->children;
        char *attrName = (char*)attr->name;
        char *content = (char *)(value->content);
        if (strcmp(attrName,"lat") == 0){
            newWaypoint->latitude = atof(content);
           // printf("  -- latitude set to: %f\n", newWaypoint->latitude);   
        }
        else if (strcmp(attrName,"lon") == 0) {
            newWaypoint->longitude = atof(content);
           // printf("  -- longitude set to: %f\n", newWaypoint->longitude);
        }
    }
    //check for non-attributes
    for (temp_node = cur_node->children; temp_node != NULL; temp_node = temp_node->next){
        name = xmlNodeGetContent(temp_node);
        if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "name") == 0){
            char * nameString = (char*)malloc((strlen((char*)name)+1) * sizeof(char));
            strcpy(nameString, (char*)name);
            newWaypoint->name = nameString;
            //printf("  -- name: '%s'\n", newWaypoint->name);
        }
        else if (temp_node->type == XML_ELEMENT_NODE){
            initAndParseGPXData(temp_node, newWaypoint->otherData );
        }
        xmlFree(name);
    }
    //if name has not been set after loop has finsihed, initialize to 
    if (newWaypoint->name == NULL){
        char * nameString = (char*)malloc(sizeof(char));
            strcpy(nameString, "\0");
            newWaypoint->name = nameString;
           // printf("   -- name:'%s'\n", newWaypoint->name);
    }    
    if (isWaypointValid(newWaypoint) == 0){
        insertBack(listToAddTo, (void*)newWaypoint);
       // printf("\n List length: %d\n", getLength(newGPXdoc->routes));
    } 
    else {
        deleteWaypoint(newWaypoint);
        return -1;
    }
    return 0;
}

/** 
 * frees the waypoint passed to it
 * assumes data passed is of type Waypoint
 **/
void deleteWaypoint(void* data){
    if (data != NULL){
        Waypoint * waypoint = (Waypoint*)data;
        //free the list
        if (waypoint->otherData != NULL){
        freeList(waypoint->otherData);
        }
        //free memory allocated for the name 
        if (waypoint->name != NULL){
        free(waypoint->name);
        }
        //free the waypoint itself
        free(waypoint);
    }
}


/** 
 * returns a char* to the string of waypoint
 * returns pointer to waypoint string on success, and NULL on failure
 * assumes data passed is a Waypoint struct
**/
char* waypointToString( void* data){
    Waypoint * waypoint = (Waypoint *)data;
    if ( waypoint == NULL || isWaypointValid(waypoint) == -1){
        return NULL;
    }
    char latTemp[100];
    char lonTemp[100];
    int numChars = 0;
    int numSubstringGets = 0;  //number of times gpxData strings added to waypoint string
    char* waypointString = NULL;

    numChars = numChars + strlen("<> wpt <>\n");
    numChars = numChars + sprintf(latTemp,"  -- latitude: %f\n", waypoint->latitude);
    numChars = numChars + sprintf(lonTemp,"  -- longitude: %f\n", waypoint->longitude);
    numChars = numChars + strlen("  -- name: \n") + strlen(waypoint->name);
    //if list is non empty, iterate through GPX data point and add to numChars
    if (getLength(waypoint->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(waypoint->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)gpxDataToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }

    waypointString = (char*)malloc(numChars+2 + numSubstringGets * sizeof(char)); 
      // + 2 is for \n at end and one for '\0'
    strcpy(waypointString,"<> wpt <>\n");
    strcat(waypointString,latTemp);
    strcat(waypointString,lonTemp);
    strcat(waypointString,"  -- name: ");
    strcat(waypointString,waypoint->name);
    strcat(waypointString,"\n");
    //loop through other data and print each node
    if (getLength(waypoint->otherData) != 0){
        char * gpxDataString = toString(waypoint->otherData);
        strcat(waypointString, gpxDataString);
        free(gpxDataString);
    }
    strcat(waypointString,"\n");
    return(waypointString);
}


/** compare names of waypoints **/
int compareWaypoints(const void *first, const void *second){
   return(0);
}


/**
 * Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
 * Returns NULL if the waypoint does not exist/not found
**/
Waypoint* getWaypoint(const GPXdoc* doc, char* name){
    if (doc == NULL || name == NULL){
        return NULL;
    }
    if (getLength(doc->waypoints) != 0){
        void * elem;
        ListIterator iterate = (createIterator(doc->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (strcmp(((Waypoint*)elem)->name,name) == 0){
               // printf("Waypoint found: %s\n", waypointToString(elem));
                return (Waypoint*) elem;
            }
        }
    }
    return NULL;
 }

















//  ---------------------    Route Functions    ---------------------  //

/** creates and initializes a new route
 *  returns a pointer to the route created
**/ 
Route *initializeRoute () {
    Route * newRoute = (Route*)malloc(sizeof(Route));
    //initialize route values
    newRoute->name = NULL;
    newRoute->waypoints =initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    newRoute->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return (newRoute);
}


/** 
 * checks validity of the route passed to it
 * returns: 0 (if valid), (-1 if invalid)
**/
int isRouteValid(Route * route){
    if (route == NULL || route->name == NULL || route->waypoints == NULL || route->otherData == NULL ){
        return -1;
    }
    //check all waypoints and gpxDatas are valid
    if (getLength(route->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(route->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isWaypointValid((Waypoint *)elem) == -1){
                return -1;
            }
        }
    }
    else if (getLength(route->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(route->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isGPXdataValid((GPXData *)elem) == -1){
                return -1;
            }
        }
    }
    return 0;
}


/** 
 * initilaizes new Rote struct and parses xml node passed and adds 
 * struct as node to end of 'listToAddTo' param 
 * assumes that xmlNode is of Route type.
 * @returns 0 on success, -1 on failure (invalid)
*/
int initAndParseRoute(xmlNode * cur_node, List * listToBeAddedTo)
{
    if (cur_node == NULL || listToBeAddedTo == NULL){
        return -1;
    }
    Route * newRoute = initializeRoute();
    xmlNode * temp_node;
    xmlChar *name = NULL;
    //check for elements
    for (temp_node = cur_node->children; temp_node != NULL; temp_node = temp_node->next){
        name = xmlNodeGetContent(temp_node);
       // printf("in here - name: <> %s <>\n", (char*)name);
        if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "name") == 0){
            char * nameString = (char*)malloc((strlen((char*)name)+1) * sizeof(char));
            strcpy(nameString, (char*)name);
            newRoute->name = nameString;
           // printf("  -- name: '%s'\n", newRoute->name);
        }
        else if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "rtept") == 0){
            //initialize a waypoint list node
            initAndParseWaypoint(temp_node, newRoute->waypoints);
  
        }
        else if (temp_node->type == XML_ELEMENT_NODE){
            initAndParseGPXData(temp_node,newRoute->otherData);
        }
        xmlFree(name);
    }
    //if name has not been set after loop has finsihed, initialize to empty string
    if (newRoute->name == NULL){
        char * nameString = (char*)malloc(sizeof(char));
            strcpy(nameString, "\0");
            newRoute->name = nameString;
           // printf("   -- name:'%s'\n", newRoute->name);
    } 

    if (isRouteValid(newRoute) == 0){
         //printf("ADDING ROUTE TO BACK OF LIST\n");
        insertBack(listToBeAddedTo, (void*)newRoute);
    }   
    else {
        deleteRoute(newRoute);
        return -1;
    }  
    return 0;     
}


/**
 * frees the route passed to it
**/
void deleteRoute(void* data){
    if (data == NULL){
        return;
    }
    Route * route = (Route*)data;
    if (route == NULL){
        return;
    }
    //free the lists in waypoints and other data
    if (route->otherData != NULL){
        freeList(route->otherData);
    }
    if (route->waypoints != NULL){
        freeList(route->waypoints);
    }
    if (route->name != NULL){
        //free memory allocated for the name 
        free(route->name);
    }
    if (route != NULL){
        //free the waypoint itself
        free(route);
    }
}


/** 
 * returns a char* to the string of route
 * returns pointer to route string on success, and NULL on failure
 * assumes data passed is a Route struct
**/
char* routeToString(void* data){
    Route * route = (Route *)data;
    if (route == NULL || isRouteValid(route) == -1){
        return NULL;
    }
    int numChars = 0;
    int numSubstringGets = 0;  //number of times waypoint/otherdata strings added to route string
    char* routeString = NULL;

    numChars = numChars + strlen("<> rte <>\n");
    numChars = numChars + strlen("  -- name: \n") + strlen(route->name);
    numChars = numChars + strlen("  -- rtepts: \n\n") ;
    //if otherData list is non empty, iterate through GPX data point and add to numChars
    if (getLength(route->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(route->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)gpxDataToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }
    numChars = numChars + strlen("  -- otherData: \n");
    //if waypoint list is non empty, iterate through GPX data point and add to numChars
    if (getLength(route->waypoints) != 0){
        void * elem;
        ListIterator iterate = (createIterator(route->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)waypointToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }
    routeString = (char*)malloc(numChars+2 + numSubstringGets * sizeof(char)); 
      // + 2 is for \n at end and one for '\0'

    strcpy(routeString,"<> rte <>\n");
    strcat(routeString,"  -- name: ");
    strcat(routeString,route->name);
    strcat(routeString,"\n  -- otherData: ");
    //loop through other data and print each node
   // printf("route otherdata list length is: %d\n", getLength(route->otherData ));
    if (getLength(route->otherData) != 0){
        char * gpxDataString = toString(route->otherData);
        strcat(routeString, gpxDataString);
        free(gpxDataString);
    }
    strcat(routeString,"\n  -- rtepts: \n");
    if (getLength(route->waypoints) != 0){
        char * waypointString = toString(route->waypoints);
        strcat(routeString, waypointString);
        free(waypointString);
    }
    strcat(routeString,"\n");
    return(routeString);
}


/** compare names of routes **/
int compareRoutes(const void *first, const void *second){
    return(0);
}



/** Function that returns a route with the given name. 
 *  If more than one exists, return the first one.  
 *  Return NULL if the route does not exist
**/
Route* getRoute(const GPXdoc* doc, char* name){
     if (doc == NULL || name == NULL){
        return NULL;
    }
    if (getLength(doc->routes) != 0){
        void * elem;
        ListIterator iterate = (createIterator(doc->routes));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (strcmp(((Route *)elem)->name,name) == 0){
                return (Route *) elem;
            }
        }
    }
    return NULL;
}














//  ---------------------    TrackSegment Functions    ---------------------  //



/** creates and initializes a new route
 *  returns a pointer to the route created
**/ 
TrackSegment *initializeTrackSegment () {
    TrackSegment * newTrackSegment = (TrackSegment*)malloc(sizeof(TrackSegment));
    //initialize track segments waypoint list
    newTrackSegment->waypoints =initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

    return (newTrackSegment);
}


/** 
 * checks validity of the route passed to it
 * returns: 0 (if valid), (-1 if invalid)
 * **/
int isTrackSegmentValid(TrackSegment * trackSegment){
    if (trackSegment == NULL){
        return -1;
    }
    if (getLength(trackSegment->waypoints) != 0){
        void * elem;
        ListIterator iterate = (createIterator(trackSegment->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isWaypointValid((Waypoint *)elem) == -1){
                return -1;
            }
        }
    }
    return 0;
}


/** 
 * initilaizes new TrackSegment struct and parses xml node passed and adds 
 * struct as node to end of 'listToAddTo' param 
 * assumes that xmlNode is of TrackSegment type.
 * @returns 0 on success, -1 on failure (invalid)
*/
int initAndParseTrackSegment(xmlNode * cur_node, List * listToBeAddedTo)
{
    if ( cur_node == NULL || listToBeAddedTo == NULL ){
        return -1;
    }
    TrackSegment * newTrackSegment = initializeTrackSegment();
    xmlNode * temp_node;
    xmlChar *name = NULL;
    //add each trkpt into the segment waypoint list
    for (temp_node = cur_node->children; temp_node != NULL; temp_node = temp_node->next){
        name = xmlNodeGetContent(temp_node);
        if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "trkpt") == 0){
            initAndParseWaypoint(temp_node, newTrackSegment->waypoints);
        }
        xmlFree(name);
    }
    if (isTrackSegmentValid(newTrackSegment) == 0){
         //printf("ADDING ROUTE TO BACK OF LIST\n");
        insertBack(listToBeAddedTo, (void*)newTrackSegment);
    }     
    else {
        deleteTrackSegment(newTrackSegment);
        return -1;
    } 
    return 0;   
}


/**
 * frees the trackSegment passed to it
**/
void deleteTrackSegment(void* data){
    TrackSegment * trackSegment = (TrackSegment*)data;
    //free list in waypoints
    if (trackSegment->waypoints != NULL){
        freeList(trackSegment->waypoints);
    }
    //free the trackSegment itself
    if (trackSegment != NULL){
        free(trackSegment);
    }
}


/** 
 * returns a char* to the string of trackSegment
 * returns pointer to trackSegment string on success, and NULL on failure
 * assumes data passed is a trackSegment struct
**/
char* trackSegmentToString(void* data){
    TrackSegment * trackSegment = (TrackSegment *)data;
    if ( trackSegment == NULL || isTrackSegmentValid(trackSegment) == -1){
        return NULL;
    }
    int numChars = 0;
    int numSubstringGets = 0;  //number of times waypoint/otherdata strings added to route string
    char* trackSegmentString = NULL;

    numChars = numChars + strlen("<> trkseg <>\n");
    numChars = numChars + strlen("  -- trkpts: \n\n") ;
    //if otherData list is non empty, iterate through GPX data point and add to numChars
    if (getLength(trackSegment->waypoints) != 0){
        void * elem;
        ListIterator iterate = (createIterator(trackSegment->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)waypointToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }

    trackSegmentString = (char*)malloc(numChars+2 + numSubstringGets * sizeof(char)); 
      // + 2 is for \n at end and one for '\0'
    strcpy(trackSegmentString ,"<> trkseg <>\n");
    //loop through waypoint list and print each node
    strcat(trackSegmentString ,"\n  -- trkpts: \n");
    if (getLength(trackSegment->waypoints) != 0){
        char * waypointString = toString(trackSegment->waypoints);
        strcat(trackSegmentString, waypointString);
        free(waypointString);
    }
    strcat(trackSegmentString ,"\n");

    return(trackSegmentString);
}


/** 
 * compare names of trackSegments 
**/
int compareTrackSegments(const void *first, const void *second){return(0);}

















//  ---------------------    Track Functions    ---------------------  //


/** 
 * creates and initializes a new track
 * returns a pointer to the track created
**/ 
Track *initializeTrack () {
    Track * newTrack = (Track*)malloc(sizeof(Track));

    //initialize route values
    newTrack->name = NULL;
    newTrack->segments =initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    newTrack->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return (newTrack);
}


/** 
 * checks validity of the route passed to it
 * returns: 0 (if valid), (-1 if invalid)
**/
int isTrackValid(Track * track){
    if (track == NULL || track->name == NULL || track->segments == NULL || track->otherData == NULL ){
        return -1;
    }
    if (getLength(track->segments) != 0){
        void * elem;
        ListIterator iterate = (createIterator(track->segments));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isTrackSegmentValid((TrackSegment *)elem) == -1){
                return -1;
            }
        }
    }
    if (getLength(track->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(track->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (isGPXdataValid((GPXData *)elem) == -1){
                return -1;
            }
        }
    }
    return 0;
}


/** 
 * initilaizes new Track struct and parses xml node passed and adds 
 * struct as node to end of 'listToAddTo' param 
 * assumes that xmlNode is of Track type.
 * @returns 0 on success, -1 on failure (invalid)
*/
int initAndParseTrack(xmlNode * cur_node, List * listToBeAddedTo)
{
    if (cur_node == NULL || listToBeAddedTo == NULL ){
        return -1;
    }
    Track * newTrack = initializeTrack();
    xmlNode * temp_node;
    xmlChar *name = NULL;
    //check for elements
    for (temp_node = cur_node->children; temp_node != NULL; temp_node = temp_node->next){
        name = xmlNodeGetContent(temp_node);
       // printf("in here - name: <> %s <>\n", (char*)name);
        if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "name") == 0){
            char * nameString = (char*)malloc((strlen((char*)name)+1) * sizeof(char));
            strcpy(nameString, (char*)name);
            newTrack->name = nameString;
        }
        else if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "trkseg") == 0){
            initAndParseTrackSegment(temp_node, newTrack->segments);
        }
        //dont parse if a track point since it is already being handled when trackSegment is parsed
        else if (temp_node->type == XML_ELEMENT_NODE && strcmp((char*)temp_node->name, "trkpt") != 0){ 
            initAndParseGPXData(temp_node,newTrack->otherData);
        }
        xmlFree(name);
    }
    //if name has not been set after loop has finsihed, initialize to empty string
    if (newTrack->name == NULL){
        char * nameString = (char*)malloc(sizeof(char));
            strcpy(nameString, "\0");
            newTrack->name = nameString;
           // printf("   -- name:'%s'\n", newTrack->name);
    } 
    if (isTrackValid(newTrack) == 0){
         //printf("ADDING ROUTE TO BACK OF LIST\n");
        insertBack(listToBeAddedTo, (void*)newTrack);
    }  
    else {
        deleteTrack(newTrack);
        return -1;
    }    
    return 0;    
}


/** frees the track passed to it **/
void deleteTrack(void* data){
    if (data != NULL){
        Track * track = (Track*)data;
        //free the lists in struct
        if (track->otherData != NULL){
            freeList(track->otherData);
        }
        if (track->segments != NULL ){
            freeList(track->segments);
        }
        //free memory allocated for the name 
        if (track->name != NULL) {
            free(track->name);
        }
        //free the track itself
        free(track);
    }
}


/** 
 * @returns a char* to the string of track
 * @returns: pointer to track string on success, and NULL on failure
 * @assumes: data passed is a track struct
**/
char* trackToString(void* data){
    Track * track = (Track *)data;
    if ( track == NULL || isTrackValid(track) == -1) {
        return NULL;
    }
    int numChars = 0;
    int numSubstringGets = 0;  //number of times waypoint/otherdata strings added to track string
    char* trackString = NULL;

    numChars = numChars + strlen("<> trk <>\n");
    numChars = numChars + strlen("  -- name: \n") + strlen(track->name);
    numChars = numChars + strlen("  -- trksegs: \n\n") ;
    //if otherData list is non empty, iterate through GPX data point and add to numChars
    if (getLength(track->otherData) != 0){
        void * elem;
        ListIterator iterate = (createIterator(track->otherData));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)gpxDataToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }
    numChars = numChars + strlen("  -- otherData: \n");
    //if waypoint list is non empty, iterate through GPX data point and add to numChars
    if (getLength(track->segments) != 0){
        void * elem;
        ListIterator iterate = (createIterator(track->segments));
        while ((elem = nextElement(&iterate)) != NULL) {
            numSubstringGets++;
            char * subStringAdding = (char*)trackSegmentToString(elem);
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }

    trackString = (char*)malloc(numChars+2 + numSubstringGets * sizeof(char)); 
      // + 2 is for \n at end and one for '\0'
    strcpy(trackString,"<> trk <>\n");
    strcat(trackString,"  -- name: ");
    strcat(trackString,track->name);
    strcat(trackString,"\n  -- otherData: ");
    //loop through other data and print each node
    if (getLength(track->otherData) != 0){
        char * gpxDataString = toString(track->otherData);
        strcat(trackString, gpxDataString);
        free(gpxDataString);
    }
    strcat(trackString,"\n  -- trksegs: ");
    if (getLength(track->segments) != 0){
        char * trackSegmentString = toString(track->segments);
        strcat(trackString, trackSegmentString);
        free(trackSegmentString);
    }
    strcat(trackString,"\n");
    return(trackString);
}


/** 
 * compare names of tracks
**/
int compareTracks(const void *first, const void *second){
    return(0);
}


/**
 * Function that returns a track with the given name.  
 * If more than one exists, return the first one. 
 * Return NULL if the track does not exist 
**/
Track* getTrack(const GPXdoc* doc, char* name){
    if (doc == NULL || name == NULL){
        return NULL;
    }
    if (getLength(doc->tracks) != 0){
        void * elem;
        ListIterator iterate = (createIterator(doc->tracks));
        while ((elem = nextElement(&iterate)) != NULL) {
            if (strcmp(((Track*)elem)->name,name) == 0){
                //printf("Track found: %s\n", trackToString(elem));
                return (Track*) elem;
            }
        }
    }
    //printf("Track '%s' not found\n", name);
    return NULL;
}


















//  ---------------------    GPXDoc Functions    ---------------------  //

/** 
 * checks validity of the gpxDocNode passed to it
 * returns: 0 (if valid), (-1 if invalid)
**/
int isGPXDocNodeValid (GPXdoc * gpxDocNode){
    if (gpxDocNode == NULL){
        return -1;
    } else if (strcmp(gpxDocNode->namespace, "\0") == 0 ){
        return -1;   //gpxDocNode namespace is invalid
    } else if (gpxDocNode->version < 0 ){
        return -1;   //gpxDocNode version is invalid
    } else if (gpxDocNode->creator == NULL || strcmp(gpxDocNode->creator,"\0") == 0 ){
        return -1;   //gpxDocNode creator is invalid
    } else if (gpxDocNode->waypoints == NULL){
        return -1;   //gpxDocNode waypoints is invalid
    } else if (gpxDocNode->routes == NULL){
        return -1;   //gpxDocNode routes is invalid
    } else if (gpxDocNode->tracks == NULL){
        return -1;   //gpxDocNode tracks is invalid
    }
    return 0;
}


/** allocates space for a new GPXdoc object and initializes docValues to invalid values.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A GPXdoc object has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pointer to the new struct or NULL (on malloc failure)
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc *initializeGPXdoc (){
    GPXdoc *newGPXdoc = NULL;
    newGPXdoc = (GPXdoc*)malloc(sizeof(GPXdoc));
    if (newGPXdoc == NULL){
        return NULL;
    }
    /*initialize values of newGPXdoc //to values that cause an error, 
    so if not initialized in createGPXdoc function, know that there was an issue*/
    newGPXdoc->namespace[0] = '\0'; 
    newGPXdoc->version = -1;
    newGPXdoc->creator = NULL;
    List *waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    if (waypointList == NULL){
        freeList(waypointList);
        return NULL;
    }
    List *routeList = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    if (routeList == NULL){
        freeList(waypointList);
        freeList(routeList);
        return NULL;
    }
    List *trackList = initializeList(&trackToString, &deleteTrack, &compareTracks);
    if (trackList == NULL){
        freeList(waypointList);
        freeList(routeList);
        freeList(trackList);
        return NULL;
    }
    newGPXdoc->waypoints = waypointList;
    newGPXdoc->routes = routeList;
    newGPXdoc->tracks = trackList;

    return newGPXdoc;
}


/** 
 * parses the GPXDoc node from metadata at top of file
 * assumes that xmlNode is of cur_node is of GPX type, and thats the file exists and is valid GPX/XML
*/
int parseGPXnode (xmlNode* cur_node, GPXdoc* newGPXdoc){
    if (cur_node == NULL || newGPXdoc == NULL){
        return -1;
    }
    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode* value = attr->children;
        char *attrName = (char*)attr->name;
        char *content = (char *)(value->content);
        strcpy(newGPXdoc->namespace, (char*)cur_node->ns->href);
        if (strcmp(attrName,"version") == 0) {
            newGPXdoc->version = atof(content);
           // printf("  -- version set to: %f\n", newGPXdoc->version);
        }
        else if (strcmp(attrName,"creator") == 0) {
            //allocate space for string -- as long as the creator name
            newGPXdoc->creator = (char*)malloc((strlen(content)+1)*sizeof(char));
            if (newGPXdoc->creator == NULL){ //if malloc has failed
                return -1; 
            }
            strcpy((newGPXdoc->creator),content);
            //printf("  -- creator set to: %s\n", newGPXdoc->creator);
        }
    }
    if (isGPXDocNodeValid(newGPXdoc) != 0){
        return -1;
    }
    return 0;
}





/** parses though a gpx file when given the root node of an xml file, and intializes values inside of GPXdoc struct
 *@pre GPXdoc object must exist
       a_node is reffering to the root node of the xml file that is to be parsed
 *@post A GPXdoc struct object has been parsed and its information is now contained in 
 *@usageNote:  MUST CHECK FOR VALIDITY AFTER USE!
 *@return 0 (on success), -1 (on failure)
 *@param fileName - a string containing the name of the GPX file
**/
int gpxParseHelp (xmlNode* a_node, GPXdoc *newGPXdoc){
    if (a_node == NULL || newGPXdoc == NULL){
        return -1;
    }
    xmlNode *cur_node = NULL;

   for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {   
        ///parse gpx type node
        if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"gpx") == 0) {
            //printf("\n>>>>>>>>>gpxParseHelp - GPX element loop\n\n");
            if (parseGPXnode(cur_node,newGPXdoc) != 0) {
                return -1;
            }
        }
        //parse waypoint type node
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"wpt") == 0) {
            //printf("\n>>>>>>>>>gpxParseHelp - WAYPOINT element loop\n\n");
            if (initAndParseWaypoint(cur_node, newGPXdoc->waypoints) != 0){
                return -1;
            }
        }
        //parse route type node
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"rte") == 0) {
           // printf("\n>>>>>>>>>gpxParseHelp - ROUTE element loop\n\n");
            if (initAndParseRoute(cur_node, newGPXdoc->routes) != 0 ){
                return -1;
            }
        }
        //parse track type node
        else if (cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name,"trk") == 0) {
            //printf("\n>>>>>>>>>gpxParseHelp - TRACK element loop\n\n");
            if (initAndParseTrack(cur_node, newGPXdoc->tracks) != 0 ){
                return -1;
            }
        }
        //recursively get next node
        gpxParseHelp(cur_node->children, newGPXdoc);
    }
    return 0;
}






/** Function initializes a new object of GPX type, and returns a pointer to the created GPXdoc
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pointer to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
 * sources: code largely based on code found at http://www.xmlsoft.org/examples/tree1.c
**/
GPXdoc* createGPXdoc(char* fileName){
    if (fileName == NULL){
        return NULL;
    }

    xmlDoc *doc = NULL;
    
    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL) {  //check malloc was successful
        xmlCleanupParser();
        return NULL;
    }
    GPXdoc *gpxDoc = initializeGPXdoc();
    xmlNode *root_element = NULL;
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if (gpxParseHelp(root_element,gpxDoc) != 0){
        //the has been some sort of issue/invalid data while parsing, get outta here
        xmlCleanupParser();
        xmlFreeDoc(doc);
        deleteGPXdoc(gpxDoc);
        return NULL;
    }
    
    //free 
    xmlFreeDoc(doc);
    xmlCleanupParser();

    if (isGPXDocNodeValid(gpxDoc) == -1){
        return NULL;
    }
    return(gpxDoc);
}


/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc){
    char temp[100];
    int numChars = 0;
    int numSubstringGets = 0;
    char* GPXdocString = NULL;
    
    //CHECK GPXdoc VALIDITY BEOFRE beginning
    if ( doc == NULL || isGPXDocNodeValid(doc) == -1 ){
        return NULL;
    }

    //find total number of chars in string that is to be returned, allocate the memory, then populate with the string
    numChars = numChars + strlen("<> gpx <>\n");
    numChars = numChars + strlen("  -- namespace: \n") + strlen(doc->namespace);
    numChars = numChars + sprintf(temp,"  -- version: %.1f\n", doc->version);
    numChars = numChars + strlen("  -- creator: \n") + strlen(doc->creator);
    if (getLength(doc->waypoints) != 0) {
        void * elem;
        ListIterator iterate = (createIterator(doc->waypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
           // printf("NumChars is : %d\n", numChars);
            char * subStringAdding = (char*)waypointToString(elem);
            numSubstringGets ++;
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }
    if (getLength(doc->routes) != 0) {
        void * elem;
        ListIterator iterate = (createIterator(doc->routes));
        while ((elem = nextElement(&iterate)) != NULL) {
           // printf("NumChars is : %d\n", numChars);
            char * subStringAdding = (char*)routeToString(elem);
            numSubstringGets ++;
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }
    if (getLength(doc->tracks) != 0) {
        void * elem;
        ListIterator iterate = (createIterator(doc->tracks));
        while ((elem = nextElement(&iterate)) != NULL) {
            //printf("NumChars is : %d\n", numChars);
            char * subStringAdding = (char*)trackToString(elem);
            numSubstringGets ++;
            numChars = numChars + strlen(subStringAdding);
            free(subStringAdding);
        }
    }

   // printf("NUMCHARS: %d\n", numChars);
    GPXdocString = (char*)malloc(numChars+2 + numSubstringGets * sizeof(char));
        //+2 b/c 1 for '\0' & 1 for '\n'

    strcpy(GPXdocString,"<> gpx <>\n");
    strcat(GPXdocString,"  -- namespace: ");
    strcat(GPXdocString,doc->namespace);
    strcat(GPXdocString,"\n");
    strcat(GPXdocString,temp); //contains version info
    strcat(GPXdocString,"  -- creator: ");
    strcat(GPXdocString,doc->creator);
    strcat(GPXdocString,"\n");
    if (getLength(doc->waypoints) != 0) {
        char * waypointString = toString(doc->waypoints);
        strcat(GPXdocString,waypointString);
        free(waypointString);
    }
    if (getLength(doc->routes) != 0) {
        char * routeString = toString(doc->routes);
        strcat(GPXdocString,routeString);
        free(routeString);
    }
    if (getLength(doc->tracks) != 0) {
        char * trackString = toString(doc->tracks);
        strcat(GPXdocString,trackString);
        free(trackString);
    }
    strcat(GPXdocString,"\n");

    return(GPXdocString);
}

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/
void deleteGPXdoc(GPXdoc* doc){
    //free string allocated for creator 
    if (doc->creator != NULL){
        free(doc->creator);
    }
    //free list - Free list function uses freeWaypoint function to free each waypoint
    if (doc->waypoints != NULL){
        freeList(doc->waypoints);
    }
    //free list - Free list function uses freeRoutes fucntion to free each route
    if (doc->routes != NULL){
        freeList(doc->routes);
    }
    //free list - Free list function uses freeRoutes fucntion to free each track
    if (doc->tracks != NULL){
        freeList(doc->tracks);
    }
    if (doc != NULL){ 
        free(doc);
    }
}




//      vvvvvvvvv   MODULE 2   vvvvvvvv       //


/* For the five "get..." functions below, return the count of specified entities from the file.  
They all share the same format and only differ in what they have to count.
 
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
 */




//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){
    int numWaypoints = 0 ;
    if ( doc != NULL && doc->waypoints!= NULL ){
       numWaypoints = getLength(doc->waypoints);
       return(numWaypoints);
    }
    else {
        return 0;
    }
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
    int numRoutes = 0 ;
    if ( doc != NULL && doc->routes!= NULL ){
       numRoutes = getLength(doc->routes);
       return(numRoutes);
    }
    else {
        return 0;
    }
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
    int numTracks = 0;
    if ( doc != NULL && doc->tracks!= NULL ){
       numTracks = getLength(doc->tracks);
       return(numTracks);
    }
    else {
        return 0;
    }
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){
    int numSegments = 0;
    if ( doc != NULL && doc->tracks != NULL ){   
        if (getLength((doc->tracks)) != 0){
            void * elem;
            ListIterator iterate = (createIterator((doc->tracks)));
            while ((elem = nextElement(&iterate)) != NULL && ((Track*)elem)->segments != NULL ) {
                numSegments = numSegments + getLength(((Track*)elem)->segments);
            }
        }
       return(numSegments);
    }
    else {
        return 0;
    }
}



/**
 * @return: counts the number of GPX data structs in listOFWaypoints
 * @assumes: that list that is passed is a list of waypoints
**/
int countWaypointListGPXData(List* listOfWaypoints){
    int count = 0;
    if (listOfWaypoints != NULL && getLength(listOfWaypoints) != 0){
        void * elem;
        ListIterator iterate = (createIterator(listOfWaypoints));
        while ((elem = nextElement(&iterate)) != NULL) {
            //count number nodes in other data list
            count = count + getLength(((Waypoint*)elem)->otherData);
            
            //count number of waypoints with a non empty name element
            if (strcmp(((Waypoint*)elem)->name, "\0") != 0 ){
                count ++;
            }
        }
    }
    return count;
}


/**
 * @return: counts the number of GPX data structs in listOFRoutes
 * @assumes: that list that is passed is a list of routes
**/
int countRouteListGPXData (List* listOfRoutes){
    int count = 0;
    if (listOfRoutes != NULL && getLength(listOfRoutes) != 0){
        void * elem;
        ListIterator iterate = (createIterator(listOfRoutes));
        while ((elem = nextElement(&iterate)) != NULL) {
            //count number nodes in other data list
            count = count + countWaypointListGPXData(((Route*)elem)->waypoints);
            //count number of routes with a non empty name element
            if (strcmp(((Route*)elem)->name, "\0") != 0 ){
                count ++;
            }
            if (getLength(((Route*)elem)->otherData) ){
               // printf("OTHERDATA NOT BEING READ - count: %d  -  Name: %s\n", count, ((Route*)elem)->name);
                count = count + getLength(((Route*)elem)->otherData);
            }
        }      
    }
    return count;
}


/**
 * @return: counts the number of GPX data structs in listOFTrackSegments
 * @assumes: that list that is passed is a list of track segments
**/
int countTrackSegmentListGPXData (List* listOfTrackSegments){
    int count = 0;
    if (listOfTrackSegments != NULL && getLength(listOfTrackSegments) != 0){
        void * elem;
        ListIterator iterate = (createIterator(listOfTrackSegments));
        while ((elem = nextElement(&iterate)) != NULL) {
            //count number nodes in other data list
            count = count + countWaypointListGPXData(((TrackSegment*)elem)->waypoints);
        }
    }
    return count;
}


/**
 * @return: counts the number of GPX data structs in listOFTracks
 * @assumes: that list that is passed is a list of tracks
**/
int countTrackListGPXData (List* listOfTracks){
    int count = 0;
    if (listOfTracks != NULL && getLength(listOfTracks) != 0){
        void * elem;
        ListIterator iterate = (createIterator(listOfTracks));
        while ((elem = nextElement(&iterate)) != NULL) {
            //count number nodes in other data list  
            count = count + countTrackSegmentListGPXData(((Track*)elem)->segments);
            //count number of routes with a non empty name element
            if (strcmp(((Track*)elem)->name, "\0") != 0 ){
                count ++;
            }
            if (getLength(((Track*)elem)->otherData) ){
                count = count + getLength(((Track*)elem)->otherData);
            }
        }
    }
    return count;
}


/**
 * @return: Total number of GPXData elements in the document
 * @assumes: that list that doc exists and is valid
**/
int getNumGPXData(const GPXdoc* doc){
    int count = 0;
    if (doc != NULL){    
    //from waypoint list
    count = countWaypointListGPXData(doc->waypoints);
    //from route list
    count = count + countRouteListGPXData (doc->routes);
    //from track list
    count = count + countTrackListGPXData (doc->tracks);
    }
    return(count);
}





















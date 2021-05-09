/**
 * @file GPXParser.c
 * @author Elizabeth McKenna
 * GuelphID: 1053037
 * course: CIS2750, W21
 * @date March 2021
 * @brief File containing the function definitions to implemment functions contained in GPXParser.h for Assignment 2
 *        Also includes helper funcitons which assist in executing the funtioned declared in the header.
 * @sources:  createValidGPX function is based on code found on http://www.xmlsoft.org/examples/tree1.c, 
 *            haversine funtion is based on code found on https://www.movable-type.co.uk/scripts/latlong.html 
 *            validateSchema funtion is based on code found on http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 */



#include "GPXParser.h"
#include "GPXHelpers.h"
#include "assert.h"




/* ###################################################################################
 *                                  A2
 * ################################################################################### */




// =============================== MODULE 1 ==========================================

/** validates strings as filenames for gpx and xsd files
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@return "true" is file name is valid, "false" if invalid
 *@param fileName - a string containing the name of the GPX file
  @param mode - (1) validates schemaDoc ('xsd') file extension
                (2) validates gpxDoc ('gpx') file extension
**/
bool validateFilename(char * filename, int mode){
    if (filename == NULL){
        return false;
    }
    //get second token (becuase it should be file extension)
    char copy[strlen(filename)];
    strcpy( copy, filename );
    char * ext = strtok( copy, ".");
    ext = strtok( NULL, "." );
    if ( mode == 1 && strcmp( ext, "xsd" ) == 0 ){
        return true;
    }
    else if ( mode == 2 && strcmp(ext, "gpx" ) == 0 ){
        return true;
    }
    return false;
}

/** validates an xmlDoc against a schema file
 *@pre File name cannot be an empty string or NULL, and correct
       File represented by this name must exist and must be readable.
       Doc exists
 *@return "true" if is valid, "false" if is invalid
 *@param fileName - a string containing the name of the GPX file
  @param doc - a xmlDoc pointer
  funtion based on code from http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
**/
bool validateSchema (xmlDocPtr doc, char* gpxSchemaFile){
    if (gpxSchemaFile == NULL || doc == NULL ){
        return false;
    }
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    int ret;
    bool valid = true; //chNge to false as needed
    
    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema); //To print schema dump

    if (doc == NULL) {
    valid = false;
    }
    else {
    xmlSchemaValidCtxtPtr ctxt;
    ctxt = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    ret = xmlSchemaValidateDoc(ctxt, doc);
    if (ret == 0) {
    valid = true;
    }
    else if (ret > 0){
        valid = false;
    }
    else {
        valid = false;
    }
    xmlSchemaFreeValidCtxt(ctxt);
    }
    
    // free resources
    if(schema != NULL){
        xmlSchemaFree(schema);
    }
    xmlSchemaCleanupTypes();
    xmlMemoryDump();

    return(valid);
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
 *@param schemaFile - a string containing the name of the XSD schema file
 * sources: code largely based on code found at http://www.xmlsoft.org/examples/tree1.c
**/
GPXdoc * createValidGPXdoc(char* fileName, char * gpxSchemaFile){
    if (fileName == NULL || gpxSchemaFile == NULL ){
        return NULL;
    }
    validateFilename( gpxSchemaFile, 1 );
    validateFilename( fileName, 2 );
    xmlDoc *doc = NULL;
    
    // parse the file and get the DOM 
    doc = xmlReadFile(fileName, NULL, 0);
    if (doc == NULL) {  //check malloc was successful
        xmlCleanupParser();
        return NULL;
    }
    if (validateSchema(doc, gpxSchemaFile) == false){  
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }
    GPXdoc *gpxDoc = initializeGPXdoc();
    xmlNode *root_element = NULL;
    // Get the root element node 
    root_element = xmlDocGetRootElement(doc);
    if (gpxParseHelp(root_element,gpxDoc) != 0){
        // has been some sort of issue/invalid data while parsing  
        xmlFreeDoc(doc);
        deleteGPXdoc(gpxDoc);
        xmlCleanupParser();
        return NULL;
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    if (isGPXDocNodeValid(gpxDoc) == -1){
        return NULL;
    }

    return(gpxDoc);
}





/**  Converts GPX file to and XMLDoc
 *@pre gpxDoc cannot be NULL and should be valid.
       gpxDoc must exist and must be readable.
 *@post Either:
        A valid XMLdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pointer to an xmlDoc containing information from gpxDoc
 *@param gpcDoc - a GPXDoc struct that is valid and not null
**/
xmlDocPtr GPXtoXML ( GPXdoc * gpxDoc ) {
    if (gpxDoc == NULL){
        return NULL;
    }
    xmlDocPtr newDoc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL, node = NULL, node1 = NULL, node2 = NULL;/* node pointers */
    char buff[256];

    ListIterator iterateotherdata;
    ListIterator iterateWpt;
    ListIterator iterateTrk;
    ListIterator iterateTrkSeg;
    ListIterator iterateRte;

    void * tmpWpt;
    void * tmpgpxdata;
    void * tmpRte;
    void * tmpTrk;
    void * tmpTrkSeg;

    char * waypointAttrArray[18] = { "ele", "time", "magvar", "geoidheight"
                                          "name", "cmt", "desc", "src" ,
                                          "link", "sym", "type", "fix" ,
                                          "sat", "hdop", "vdop", "pdop",
                                          "ageofdgpsdata", "dgpsid", "extensions"};
    char * routeAndTrackAttrArray[7] = { "cmt", "desc", "src", "link", 
                                      "number", "type", "extensions" };
    


    //make new xmlDoc
    newDoc = xmlNewDoc(BAD_CAST "1.0");

    //GPX NODE ... is the root node
    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    //root element stuff
    sprintf(buff, "%.1f", gpxDoc->version);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST buff );
    xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST gpxDoc->creator);
    //namespace
    xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST gpxDoc->namespace, NULL);
    xmlSetNs(root_node, ns);
    xmlDocSetRootElement(newDoc, root_node);

    //WAYPOINTS
    iterateWpt = (createIterator(gpxDoc->waypoints));
    while ((tmpWpt = nextElement(&iterateWpt)) != NULL) {
        node = xmlNewChild(root_node, NULL, BAD_CAST "wpt", NULL);
        //latitude
        sprintf(buff, "%f", ((Waypoint*)tmpWpt)->latitude);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST buff );
        //longitude
        sprintf(buff, "%f", ((Waypoint*)tmpWpt)->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST buff );
        //other data (pre name)
        for (int i = 0; i < 4; i ++){
            iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
            while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                    node1 = xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                }  
            }
        }
        //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
        if ( ((Waypoint*)tmpWpt)->name != NULL && strcmp( ((Waypoint*)tmpWpt)->name, "\0" ) != 0 ){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Waypoint*)tmpWpt)->name);
        }
        //other data (post name vals)
        for (int i = 4; i < 18; i ++){
            iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
            while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){ 
                if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                    node1 = xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                } 
            }
        }
    }

    //ROUTES
    iterateRte = (createIterator(gpxDoc->routes));
    while ((tmpRte = nextElement(&iterateRte)) != NULL ) {
        node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);
        //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
        if ( ((Route*)tmpRte)->name != NULL  && strcmp( ((Route*)tmpRte)->name, "\0") != 0 ){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Route*)tmpRte)->name);
        }
        //other data
        for (int i = 0; i < 7; i++){
            iterateotherdata = (createIterator(((Route*)tmpRte)->otherData));                 
            while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                if ( strcmp(((GPXData*)tmpgpxdata)->name, routeAndTrackAttrArray[i] ) == 0 ){
                    node1 = xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                }
            }
        }
        //route points
        iterateWpt = (createIterator(((Route*)tmpRte)->waypoints));
        while ((tmpWpt = nextElement(&iterateWpt)) != NULL) {
            node1 = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);
            //latitude
            sprintf(buff, "%f", ((Waypoint*)tmpWpt)->latitude);
            xmlNewProp(node1, BAD_CAST "lat", BAD_CAST buff );
            //longitude
            sprintf(buff, "%f", ((Waypoint*)tmpWpt)->longitude);
            xmlNewProp(node1, BAD_CAST "lon", BAD_CAST buff );

            //other data (pre name)
            for (int i = 0; i < 4; i ++){
                iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
                while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                    if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                        xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                    }  
                }
            }
            //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
            if ( ((Waypoint*)tmpWpt)->name != NULL && strcmp( ((Waypoint*)tmpWpt)->name, "\0" ) != 0 ){
                xmlNewChild(node1, NULL, BAD_CAST "name", BAD_CAST ((Waypoint*)tmpWpt)->name);
            }
            //other data (post name vals)
            for (int i = 4; i < 18; i ++){
                iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
                while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){ 
                    if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                        xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                    } 
                }
            }
        } 
    }


    //TRACKS
    iterateTrk = (createIterator(gpxDoc->tracks));
    while ((tmpTrk = nextElement(&iterateTrk)) != NULL ){
        node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);
    
        //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
        if ( ((Track*)tmpTrk)->name != NULL  && strcmp( ((Track*)tmpTrk)->name, "\0") != 0 ){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST ((Track*)tmpTrk)->name);
        }
        //other data (added in proper order)
        for (int i = 0; i < 7; i++){
            iterateotherdata = (createIterator(((Track*)tmpTrk)->otherData));                 
            while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                if ( strcmp(((GPXData*)tmpgpxdata)->name, routeAndTrackAttrArray[i] ) == 0 ){
                    xmlNewChild(node, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                }
            }
        }

        //track segments
        iterateTrkSeg = createIterator(((Track*)tmpTrk)->segments);
        while ((tmpTrkSeg = nextElement(&iterateTrkSeg)) != NULL ) {
            node1 = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
            //track points
            iterateWpt = (createIterator(((TrackSegment*)tmpTrkSeg)->waypoints));
            while ((tmpWpt = nextElement(&iterateWpt)) != NULL) {
                node2 = xmlNewChild(node1, NULL, BAD_CAST "trkpt", NULL);
                //latitude
                sprintf(buff, "%f", ((Waypoint*)tmpWpt)->latitude);
                xmlNewProp(node2, BAD_CAST "lat", BAD_CAST buff );
                //longitude
                sprintf(buff, "%f", ((Waypoint*)tmpWpt)->longitude);
                xmlNewProp(node2, BAD_CAST "lon", BAD_CAST buff );


                //other data (pre name)
                for (int i = 0; i < 4; i ++){
                    iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
                    while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                        if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                            xmlNewChild(node2, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                        }  
                    }
                }
                //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
                if ( ((Waypoint*)tmpWpt)->name != NULL && strcmp( ((Waypoint*)tmpWpt)->name, "\0" ) != 0 ){
                    xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST ((Waypoint*)tmpWpt)->name);
                }
                //other data (post name vals)
                for (int i = 4; i < 18; i ++){
                    iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
                    while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){ 
                        if ( strcmp(((GPXData*)tmpgpxdata)->name, waypointAttrArray[i] ) == 0 ){
                            xmlNewChild(node2, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                        } 
                    }
                }
                


                /*
                //name - must not be null, may be empty (but if it is, dont actually need to write in xml file)
                if ( ((Waypoint*)tmpWpt)->name != NULL && strcmp( ((Waypoint*)tmpWpt)->name, "\0" ) != 0 ){
                    xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST ((Waypoint*)tmpWpt)->name);
                }
                //other data
                iterateotherdata = (createIterator(((Waypoint*)tmpWpt)->otherData));                 
                while ((tmpgpxdata = nextElement(&iterateotherdata)) != NULL ){
                    xmlNewChild(node2, NULL, BAD_CAST ((GPXData *)tmpgpxdata)->name,  BAD_CAST ((GPXData *)tmpgpxdata)->value);
                } */




            } 
        }
    }
    // xmlSaveFormatFileEnc("please.txt", newDoc, "UTF-8", 1);
    // xmlFreeDoc(newDoc);
    // xmlCleanupParser();
    // xmlMemoryDump();
    return(newDoc);
}



/** Function to writing a GPXdoc into a file in GPX format.
 *@pre
    GPXdoc object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post GPXdoc has not been modified in any way, and a file representing the
    GPXdoc contents in GPX format has been created
 *@return a boolean value indicating success or failure of the write
 *@param doc - a pointer to a GPXdoc struct
  @param filenae - the name of the output file
 **/
bool writeGPXdoc (GPXdoc * doc, char * filename) {
    //file must be valid and not have NULL arguments then
    if (doc == NULL || filename == NULL ){
        return false;
    }
    //convert doc to xml tree
    xmlDocPtr xmlDoc = GPXtoXML(doc);
    if (xmlSaveFormatFileEnc(filename, xmlDoc, "UTF-8", 1) == -1){
        //file write has failed
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }
    //free the xml stuff that dont need anymore
    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();
    xmlMemoryDump();
    return true;
}


/** Function to validating an existing a GPXobject object against a GPX schema file
 *@pre 
    GPXdoc object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post GPXdoc has not been modified in any way
 *@return the boolean aud indicating whether the GPXdoc is valid
 *@param doc - a pointer to a GPXdoc struct
 *@param gpxSchemaFile - the name of a schema file
 **/
bool validateGPXDoc(GPXdoc* gpxDoc, char* gpxSchemaFile){
    if ( gpxDoc == NULL || gpxSchemaFile == NULL ){
        return false;
    }
    //convert gpxDoc into xml tree
    xmlDocPtr xmlDoc = GPXtoXML(gpxDoc);
    
    //check is a valid gpx image (using the schemaFile)
    if (validateSchema(xmlDoc, gpxSchemaFile) == false){  
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return false;
    }
    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();

    //now check with contraints in gpxParser.h
    if (isGPXDocNodeValid(gpxDoc) == -1 ){
        return false;
    }
    void * temp;
    ListIterator iter = createIterator(gpxDoc->waypoints);
    while ((temp = nextElement(&iter)) != NULL ) {
        if (isWaypointValid((Waypoint *)temp) == -1 ){
            return false;
        }
    }
    iter = createIterator(gpxDoc->routes);
    while ((temp = nextElement(&iter)) != NULL ) {
        if (isRouteValid((Route *)temp) == -1 ){
            return false;
        }
    }
    iter = createIterator(gpxDoc->tracks);
    while ((temp = nextElement(&iter)) != NULL ) {
        if (isTrackValid((Track *)temp) == -1 ){
            return false;
        }
    }
    return true;
}





// =============================== MODULE 2 ==========================================

/** Function that rounds the length of a track or a route to the nearest 10m
 *@pre Length is not negative
  *@return length rounded to the nearest 10m
 *@param len - length
**/
float round10 (float len) {
    int lenFloor = floor(len);
    char buff[256];
    int i = 0;
    int lastDig;

    sprintf(buff, "%f", len);
    //find the last digit before the decimal
    while (buff[i] != '.'){
        i ++;
    }
    lastDig = buff[i-1] - '0';
    //round based on last digit
    if (lastDig >= 5){ //round up to nearest 10
        return lenFloor + (10 - lastDig);
    }
    else { //last dig < 5 --> round down to nearest 10
        return lenFloor - lastDig;
    }
}

//returns the min of the two values passed as parameters
float min (float num1, float num2){
    if (num1 < num2){
        return num1;
    }
    return num2;
}



/**
 * Funtion calculates the distance between two points using the haversine formula.
 * @pre: all paramters are within valud range of a latitude or longitude
 * @return: the distance between the two points
 * @param lat1 - latitude of point 1
 * @param lon1 - longitude of point 1
 * @param lat2 - latitude of point 2
 * @param lon2 - longitude of point 2
 * based on code from https://www.movable-type.co.uk/scripts/latlong.html 
**/
float haversine (float lat1, float lon1, float lat2, float lon2){
    float phi1 = lat1 * TO_RAD;
    float phi2 = lat2 * TO_RAD;
    float deltaPhi = ( lat2 - lat1 ) * TO_RAD;
    float deltaLambda = ( lon2 - lon1 ) * TO_RAD;

    float a = ( sin(deltaPhi/2) * sin(deltaPhi/2) ) + ( cos(phi1) * cos(phi2) * sin(deltaLambda/2) * sin(deltaLambda/2) );
    float c =  2 * atan2(sqrt(a), sqrt(1-a));
    //return ( R * 2 * atan2(sqrt(a), sqrt(1-a)));
    return R * c;
}


/** Function that returns the length of a Route
 *@pre Route object exists, is not null, and has not been freed
 *@post Route object had been freed
 *@return length of the route in meters
 *@param rt - a pointer to a Route struct
**/
float getRouteLen(const Route *rt){
    if (rt == NULL){
        return 0;
    }
    float sum = 0;
    void * temp;
    float pt1Lat;
    float pt1Lon;
    float pt2Lat;
    float pt2Lon; 
    
    if ( rt->waypoints != NULL && getLength(rt->waypoints) != 0){
        ListIterator iter = createIterator(rt->waypoints);
        temp = nextElement(&iter);
        pt2Lat = ((Waypoint*)temp)->latitude;
        pt2Lon = ((Waypoint*)temp)->longitude;
        while ((temp = nextElement(&iter)) != NULL ) {
            //second waypoint becomes first waypoint, next waypoint becomes second waypoint
            pt1Lat = pt2Lat;
            pt1Lon = pt2Lon;
            pt2Lat = ((Waypoint*)temp)->latitude;
            pt2Lon = ((Waypoint*)temp)->longitude;
            //calculate distance between waypoints
            sum = sum + haversine(pt1Lat, pt1Lon, pt2Lat, pt2Lon);
        }
    }
    return sum;
}


/** Function that returns the length of a Track
 *@pre Track object exists, is not null, and has not been freed
 *@post Track object had been freed
 *@return length of the track in meters
 *@param tr - a pointer to a Track struct
**/
float getTrackLen(const Track *tr){
    if (tr == NULL){
        return 0;
    }
    float sum = 0;
    void * segElem;
    void * wptElem;
    float pt1Lat;
    float pt1Lon;
    float pt2Lat;
    float pt2Lon; 
    bool firstSeg = true;
    
    //iterate through each track segment
    if (tr->segments != NULL && getLength(tr->segments) != 0){
        ListIterator segIter = createIterator(tr->segments);
        while ((segElem = nextElement(&segIter)) != NULL ) {
            //iterate through all waypoints in each segment
            if(((TrackSegment*)segElem)->waypoints != NULL && getLength(((TrackSegment*)segElem)->waypoints) != 0) {
                ListIterator wptIter = createIterator(((TrackSegment*)segElem)->waypoints);
                if (firstSeg == true){ //only set if first seg, other wise keep old waypoint data so distance between segs is measured
                    wptElem = nextElement(&wptIter);
                    pt2Lat = ((Waypoint*)wptElem)->latitude;
                    pt2Lon = ((Waypoint*)wptElem)->longitude;
                    firstSeg = false;
                }
                while ((wptElem = nextElement(&wptIter)) != NULL ) {
                    //second waypoint becomes first waypoint, next waypoint becomes second waypoint
                    pt1Lat = pt2Lat;
                    pt1Lon = pt2Lon;
                    pt2Lat = ((Waypoint*)wptElem)->latitude;
                    pt2Lon = ((Waypoint*)wptElem)->longitude;
                    //calculate distance between waypoints
                    sum = sum + haversine(pt1Lat, pt1Lon, pt2Lat, pt2Lon);
                }
            }
            //at end of track segment, last node in segment will be in pt2lat/pt2lon, so when next seg starts looping, distance b/w will be added in sum 
        }
    }
    return sum;
}


/** Function that returns the number routes with the specified length, using the provided tolerance 
 * to compare route lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of routes with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search route length
 *@param delta - the tolerance used for comparing route lengths
**/
int numRoutesWithLength(const GPXdoc* doc, float len, float delta) {
    int routeLen;
    int diff;
    int numRoutes = 0;
    void * elem;
    if (doc == NULL || delta < 0 || len < 0){
        return 0;
    }
    ListIterator iter = createIterator(doc->routes);
    while ((elem = nextElement(&iter)) != NULL ){
        routeLen = getRouteLen((Route*)elem);
        //find difference
        diff = routeLen - len;
        if (diff < 0){  //force diff to be positive
            diff = (-1) * diff;
        }
        if (diff <= delta){
            numRoutes ++;
        }
    }
    return numRoutes;
}

/** Function that returns the number tracks with the specified length, using the provided tolerance 
 * to compare track lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of tracks with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search track length
 *@param delta - the tolerance used for comparing track lengths
**/
int numTracksWithLength(const GPXdoc* doc, float len, float delta) {
    int trackLen;
    int diff;
    int numTracks = 0;
    void * elem;
    if (doc == NULL || delta < 0 || len < 0){
        return 0;
    }
    ListIterator iter = createIterator(doc->tracks);
    while ((elem = nextElement(&iter)) != NULL ){
        trackLen = getTrackLen((Track*)elem);
        //find difference
        diff = trackLen - len;
        if (diff < 0){  //force diff to be positive
            diff = (-1) * diff;
        }
        if (diff <= delta){
            numTracks ++;
        }
    }
    return numTracks;
}


/** Function that checks if the current route is a loop
 *@pre Route object exists, is not null
 *@post Route object exists, is not null, has not been modified
 *@return true if the route is a loop, false otherwise
 *@param route - a pointer to a Route struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopRoute(const Route* route, float delta) {
    if (route == NULL || delta < 0 || getLength(route->waypoints) < 4 ) {
        return false;
    }
    //get first and last nodes to compare
    Waypoint * frontNode = getFromFront(route->waypoints);
    Waypoint * endNode = getFromBack(route->waypoints);
    float distance = haversine(frontNode->latitude, frontNode->longitude, endNode->latitude, endNode->longitude);
    if (distance <= delta) {
        return true;
    }
    return false;
}


/** Function that checks if the current track is a loop
 *@pre Track object exists, is not null
 *@post Track object exists, is not null, has not been modified
 *@return true if the track is a loop, false otherwise
 *@param track - a pointer to a Track struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopTrack(const Track *tr, float delta) {
    int numWaypoints = 0;
    void * elem;

    if (tr == NULL || delta < 0 ) {
        return false;
    }
    //num waypoints in one or more track segs
    if (getLength(tr->segments) >= 0 ){ // check theres one or more track segs
        ListIterator iter = createIterator(tr->segments);
        while ((elem = nextElement(&iter)) != NULL ){
            numWaypoints = numWaypoints + getLength( ((TrackSegment *)elem)->waypoints);
        }
    }
    if (numWaypoints < 4){
        return false;
    }
    //get first and last nodes to compare
    TrackSegment * frontSeg = getFromFront(tr->segments);
    TrackSegment * endSeg = getFromBack(tr->segments);
    Waypoint * frontNode = getFromFront(frontSeg->waypoints);
    Waypoint * endNode = getFromBack(endSeg->waypoints);
    float distance = haversine(frontNode->latitude, frontNode->longitude, endNode->latitude, endNode->longitude);
    if (distance <= delta) {
        return true;
    }
    return false;
}


//dummy funtion to pass to a list
void deleteDummy(void* data){
    //this is a dummy function to pass to initialize list so nodes are not actually deleted.
}

/** Function that returns all routes between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Route structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || doc->routes == NULL || getLength(doc->routes) == 0 ){
        return NULL;
    }
    float distanceFromSource;
    float distanceFromDest;
    Waypoint * frontNode;
    Waypoint * endNode;
    void * elem;
    if (getLength(doc->routes) != 0 ){
    List * routesBetween = initializeList(&routeToString, &deleteDummy, &compareRoutes);
    //for each route in doc, check if first waypoint and last waypoint are both within to source and destination respectively
        ListIterator iter = createIterator(doc->routes);
        while ((elem = nextElement(&iter)) != NULL ){  //for each route, check if between source and dest
            if (((Route *)elem)->waypoints == NULL || getLength(((Route *)elem)->waypoints) == 0 ){
                continue;
            }
            frontNode = getFromFront(((Route*)elem)->waypoints);
            endNode = getFromBack(((Route*)elem)->waypoints);
            distanceFromSource = haversine(frontNode->latitude, frontNode->longitude, sourceLat, sourceLong);
            distanceFromDest = haversine(endNode->latitude, endNode->longitude, destLat, destLong);
            //force distance values positive
            if (distanceFromSource < 0){
                distanceFromSource = distanceFromSource * (-1);
            }
            if (distanceFromDest < 0){
                distanceFromDest = distanceFromDest * (-1);
            }
            //add to list if within delta of both source and dest
            if (distanceFromSource <= delta && distanceFromDest <= delta){
                insertFront(routesBetween, elem);
            }
        }
        //if list is empty after loop finished, nothing in list, so free it and return null
        if (getLength(routesBetween) == 0 ){
            freeList(routesBetween);
            return NULL;
        }
        else {
            return routesBetween;
        }
    }
    return NULL;
}


/** Function that returns all Tracks between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Track structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || getLength(doc->tracks) == 0 ){
        return NULL;
    }
    float distanceFromSource;
    float distanceFromDest;
    TrackSegment * frontSeg;
    TrackSegment * endSeg;
    Waypoint * frontNode;
    Waypoint * endNode;
    void * trElem;
    if (getLength(doc->tracks) != 0 ){
    List * tracksBetween = initializeList(&trackToString, &deleteDummy, &compareTracks);
    //for each track, get first waypoint in first segment and last waypoint in last segment
        ListIterator trIter = createIterator(doc->tracks);
        while ((trElem = nextElement(&trIter)) != NULL ){  //for each track seg, check if between source and dest
            if (((Track *)trElem)->segments == NULL || getLength(((Track *)trElem)->segments) == 0 ){
                continue;
            }
            frontSeg = getFromFront(((Track*)trElem)->segments);
            endSeg = getFromBack(((Track*)trElem)->segments);
            if (frontSeg == NULL || getLength(frontSeg->waypoints) == 0 ){
                continue;
            }
            if (endSeg == NULL || getLength(endSeg->waypoints) == 0 ){
                continue;
            }
            frontNode = getFromFront(frontSeg->waypoints);
            endNode = getFromBack(endSeg->waypoints);
            distanceFromSource = haversine(frontNode->latitude, frontNode->longitude, sourceLat, sourceLong);
            distanceFromDest = haversine(endNode->latitude, endNode->longitude, destLat, destLong);
            //force distance values positive
            if (distanceFromSource < 0){
                distanceFromSource = distanceFromSource * (-1);
            }
            if (distanceFromDest < 0){
                distanceFromDest = distanceFromDest * (-1);
            }
            //add to list if within delta of both source and dest
            if (distanceFromSource <= delta && distanceFromDest <= delta){
                insertFront(tracksBetween, trElem);
            }
        }
        if (getLength(tracksBetween) == 0 ){
            freeList(tracksBetween);
            return NULL;
        }
        else {
            return tracksBetween;
        }
    }
    return NULL;
}




// =============================== MODULE 3 ==========================================


/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/
char* routeToJSON(const Route *rt) {
    char * string = NULL;
    char numVal[2048];
    char routeLen[2048];
    bool isLoop;
    int numChars = 55; //19 filler chars + 20 chars for nameThings + 1 for null terminator

    if (rt == NULL){
    string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
    strcpy(string, "{}");
    return string;
    }
    //get length of string going to make
    // -- add name
    if (strcmp(rt->name, "\0") == 0){
        numChars += strlen("None");
    } else {
        numChars += strlen(rt->name);
    }
    // -- add numVal (num waypoints)
    sprintf(numVal, "%d", getLength(rt->waypoints));
    numChars += strlen(numVal);
    // -- add routeLen
    sprintf(routeLen, "%.1f", round10(getRouteLen(rt)));
    numChars += strlen(routeLen);
    // -- add loop stat
    if (isLoopRoute(rt, A2_MOD3_DELTA) == true){
        numChars += strlen("true");
        isLoop = true;
    } else {
        numChars += strlen("false");
        isLoop = false;
    }
    // -- add other data string
    char * otherDataStr = gpxRouteDataToJSON(rt);
    numChars += strlen(otherDataStr);

    //malloc the string 
    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add name
    strcpy( string, "{\"name\":\"" );
    if (strcmp(rt->name, "\0") == 0){
        strcat( string,"None" );
    } else {
        strcat(string, rt->name );
    }
    // -- add numVal (num waypoints)
    strcat( string, "\",\"numPoints\":" );
    strcat( string, numVal );
    // -- add routeLen
    strcat( string, ",\"len\":" );
    strcat( string, routeLen);
    // -- add loop stat
    strcat( string, ",\"loop\":" );
    if (isLoop == true){
        strcat( string, "true" );
    } else {
        strcat( string, "false" );
    }
    strcat( string, ",\"other\":\"" );
    strcat( string, otherDataStr );
    strcat( string, "\"}");
fprintf(stderr, "%s\n", string);
    return string;
}

/** Function to converting a Track into a JSON string
 *@pre Track is not NULL
 *@post Track has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Track struct
 **/
char* trackToJSON(const Track *tr) {
    char * string = NULL;
    char numVal[2048];
    char trackLen[2048];
    bool isLoop;
    int numChars = 55; //19 filler chars + 20 chars for nameThings + 1 for null terminator

    if (tr == NULL){
    string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
    strcpy(string, "{}");
    return string;
    }

    //find number of points:
    //sum num waypoints in each segment
    int numPts = 0;
    void * elem;
    if (getLength(tr->segments) >= 0 ){ // check theres one or more track segs
        ListIterator iter = createIterator(tr->segments);
        while ((elem = nextElement(&iter)) != NULL ){
            numPts = numPts + getLength( ((TrackSegment *)elem)->waypoints);
        }
    }


    
    //get length of string going to make
    // -- add name
    if (strcmp(tr->name, "\0") == 0){
        numChars += strlen("None");
    } else {
        numChars += strlen(tr->name);
    }
    // -- add numVal (num waypoints)
    sprintf(numVal, "%d", numPts);
    numChars += strlen(numVal);
    // -- add routeLen
    sprintf(trackLen, "%.1f", round10(getTrackLen(tr)));
    numChars += strlen(trackLen);
    // -- add loop stat
    if (isLoopTrack(tr, A2_MOD3_DELTA) == true){
        numChars += strlen("true");
        isLoop = true;
    } else {
        numChars += strlen("false");
        isLoop = false;
    }
    // -- add other data string
    char * otherDataStr = gpxTrackDataToJSON(tr);
    numChars += strlen(otherDataStr);

    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add name
    strcpy( string, "{\"name\":\"" );
    if (strcmp(tr->name, "\0") == 0){
        strcat( string,"None" );
    } else {
        strcat(string, tr->name );
    }
    // -- add numVal (num waypoints)
    strcat( string, "\",\"numPoints\":" );
    strcat( string, numVal );
    // -- add routeLen
    strcat( string, ",\"len\":" );
    strcat( string, trackLen);
    // -- add loop stat
    strcat( string, ",\"loop\":" );
    if (isLoop == true){
        strcat( string, "true" );
    } else {
        strcat( string, "false" );
    }
    strcat( string, ",\"other\":\"" );
    strcat( string, otherDataStr );
    strcat( string, "\"}");
fprintf(stderr, "%s\n", string);
    return string;
}


/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* routeListToJSON(const List *list) {
    if (list == NULL || getLength((List*)list) == 0 ){
        return NULL;
    }
    char * string = NULL;
    void * elem;
    char * temp;
    char * temp2;
    bool firstElem = true;
    int numChars = 3 + getLength((List*)list); //2 square brackets + (getLength(list) - 1) commas + 1 for null terminator = 2 + getLength(list)

    //get length of string going to make
    ListIterator iter = createIterator((List*)list);
    while ((elem = nextElement(&iter)) != NULL ) { 
        //add length of routeJSON for each route in list
        temp = routeToJSON((Route *)elem);
        numChars += strlen(temp);  
        if (temp != NULL)
            free(temp);
    }
    string = (char*)malloc(numChars * sizeof(char));
    //iterate through list again to make output string
    strcpy( string, "[");
    ListIterator iter2 = createIterator((List*)list);
    while ((elem = nextElement(&iter2)) != NULL ) { 
        if (firstElem == true){
            firstElem = false;
        } else {
            strcat( string, ",");
        }
        temp2 = routeToJSON((Route *)elem);
        strcat( string, temp2 );
        if (temp2 != NULL)
            free(temp2);
    }
    strcat( string, "]");
    //printf("\n\n'%s'\n", string);
    return string;
}

/** Function to converting a list of Track structs into a JSON string
 *@pre Track list is not NULL
 *@post Track list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* trackListToJSON(const List *list) {
    if (list == NULL || getLength((List*)list) == 0){
        return NULL;
    }
    char * string = NULL;
    void * elem;
    char * temp;
    bool firstElem = true;
    int numChars = 2 + getLength((List*)list); //2 square brackets + (getLength(list) - 1) commas + 1 for null terminator = 2 + getLength(list)

    //get length of string going to make
    ListIterator iter = createIterator((List*)list);
    while ((elem = nextElement(&iter)) != NULL ) { 
        //add length of routeJSON for each route in list
        temp = trackToJSON((Track *)elem);
        numChars += strlen(temp);
        free(temp);
    }
    string = (char*)malloc(numChars * sizeof(char));
    //iterate through list again to make output string
    strcpy( string, "[");
    iter = createIterator((List*)list);
    while ((elem = nextElement(&iter)) != NULL ) { 
        if (firstElem == true){
            firstElem = false;
        } else {
            strcat( string, ",");
        }
        temp = trackToJSON((Track *)elem);
        strcat( string, temp );
        free(temp);
    }
    strcat( string, "]");
    return string;
}


/** Function to converting a GPXdoc into a JSON string
 *@pre GPXdoc is not NULL
 *@post GPXdoc has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a GPXdoc struct
 **/
char* GPXtoJSON(const GPXdoc* gpx) {
    char * string = NULL;
    char ver[2048];
    char numW[2048];
    char numR[2048];
    char numT[2048];
    int numChars = 68; //23 filler chars + 44 chars for nameThings + 1 for null terminator
    if (gpx == NULL){
        string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
        strcpy(string, "{}");
        return string;
    }
    //get length of string going to make
    // -- add version
    sprintf(ver, "%.1f", gpx->version);
    numChars += strlen(ver);
    // -- add creator
    numChars += strlen(gpx->creator);
    // -- add numWaypoints
    numChars += sprintf(numW, "%d", getLength(gpx->waypoints));
    // -- add numRoutes
    numChars += sprintf(numR, "%d", getLength(gpx->routes));
    // -- add numTracks
    numChars += sprintf(numT, "%d", getLength(gpx->tracks));

    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add version
    strcpy( string, "{\"version\":");
    strcat( string, ver );
    //add creator
    strcat( string, ",\"creator\":\"");
    strcat( string, gpx->creator);
    // -- add numWaypoints
    strcat( string, "\",\"numWaypoints\":");
    strcat( string, numW );
    // -- add numRoutes
    strcat( string, ",\"numRoutes\":");
    strcat( string, numR );
    // -- add numTracks
    strcat( string, ",\"numTracks\":");
    strcat( string, numT );
    strcat( string, "}" );
    
    return string;
}











// A2 BONUS FUNCTIONS -------------------------------


/** Function to adding an Waypont struct to an existing Route struct
 *@pre arguments are not NULL
 *@post The new waypoint has been added to the Route's waypoint list
 *@return N/A
 *@param rt - a Route struct
 *@param pr - a Waypoint struct
 **/
void addWaypoint(Route * rt, Waypoint * pt) {
    if (rt != NULL || pt != NULL){  
        insertBack(rt->waypoints, (void*)pt);
    }
}

/** Function to adding an Route struct to an existing GPXdoc struct
 *@pre arguments are not NULL
 *@post The new route has been added to the GPXdoc's routes list
 *@return N/A
 *@param doc - a GPXdoc struct
 *@param rt - a Route struct
 **/
void addRoute(GPXdoc * doc, Route * rt) {
    if (doc != NULL || rt != NULL){
        insertBack(doc->routes, (void*)rt);
    }
}

/** Function to converting a JSON string into an GPXdoc struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized GPXdoc struct
 *@param str - a pointer to a string
 **/
GPXdoc* JSONtoGPX(const char* gpxString) {
    //GPX string format:
    // {"version":ver,"creator":"creatorValue"}
    if (gpxString == NULL){
        return NULL;
    }
    char jsonString[strlen(gpxString)];
    float ver;
    char creatorValue[strlen(gpxString)];
    GPXdoc *gpxDoc;
    char * token;
    char namespaceString[] = "http://www.topografix.com/GPX/1/1";

    //make a copy of the string to keep original string unchanged while being tokenized
    strcpy( jsonString, gpxString );
    if ( (token = strtok( jsonString, ":," )) != NULL ){
        //get version (second token)
        if ((token = strtok( NULL, ":,")) != NULL){
            ver = atof(token);
        } else {
            return NULL;
        }
        //get creator token (discard, we want the value after the colon) 
        if ((token = strtok( NULL, ":,")) != NULL){
            if ((token = strtok( NULL, "\"")) != NULL){
                strcpy(creatorValue, token);
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    }
    
    gpxDoc = initializeGPXdoc();
    gpxDoc->version = ver;
    gpxDoc->creator = (char*)malloc((strlen(creatorValue)+1)*sizeof(char));
    if (gpxDoc->creator == NULL){ //if malloc has failed
        return NULL; 
    }
    strcpy((gpxDoc->creator),creatorValue);
    strcpy((gpxDoc->namespace),namespaceString);

    return gpxDoc;
}

/** Function to converting a JSON string into an Waypoint struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Waypoint struct
 *@param str - a pointer to a string
 **/
Waypoint* JSONtoWaypoint(const char* gpxString) {
    //Wpt string format:
    // {"lat":latVal,"lon":lonVal}
    if (gpxString == NULL){
        return NULL;
    }
    char string[strlen(gpxString)];
    float latVal;
    float lonVal;
    char name[] = "\0";
    char * token;
    Waypoint * newWpt;
    //make a copy of the string to keep original string unchanged while being tokenized
    strcpy( string, gpxString );

    //first token is lat
    if ( (token = strtok( string, ":," )) != NULL ){
        //get latVal (second token)
        if ((token = strtok( NULL, ":,")) != NULL){
            latVal = atof(token);
            //printf("latVal token is '%.1f'\n", latVal);
        } else {
            return NULL;
        }
        //third token is lon
        if ((token = strtok( NULL, ":\",")) != NULL){
            //forth token is lonVal
            if ((token = strtok( NULL, ":}")) != NULL) {
                lonVal = atof(token);
            } else {
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }

    newWpt = initializeWaypoint();
    //name will always be null since no name param in gpxString
    newWpt->name = (char*)malloc(sizeof(char));
    strcpy( newWpt->name, name );
    newWpt->latitude = latVal;
    newWpt->longitude = lonVal;

    return newWpt;
}

/** Function to converting a JSON string into an Route struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Route struct
 *@param str - a pointer to a string
 **/
Route* JSONtoRoute(const char* gpxString) {
    //Rte string format:
    // {"name":"nameVal"}
    if (gpxString == NULL){
        return NULL;
    }
    char string[strlen(gpxString)];
    char nameVal[strlen(gpxString)];
    char * token;
    Route * newRte;

    //make a copy of the string to keep original string unchanged while being tokenized
    strcpy( string, gpxString );

    //first token is name
    if ( (token = strtok( string, ":," )) != NULL ){
        //second token is nameVal
        //printf("token1 is: %s\n", token);
        if ((token = strtok( NULL, ":,\"")) != NULL) {
            strcpy(nameVal, token);
            //printf("token2 is: %s\n", token);
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }

    newRte = initializeRoute();
    //name will always be null since no name param in gpxString
    newRte->name = (char*)malloc((strlen(nameVal) + 1 ) * sizeof(char));
    strcpy( newRte->name, nameVal );

    return newRte;
}





//#define SCHEMAFILE "gpx.xsd"


//A3 functions


/** Function to converting a GPXdoc into a JSON string
 *@pre GPXdoc is not NULL
 *@post GPXdoc has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a GPXdoc struct
 **/
char* GPXPlusFileNametoJSON(const GPXdoc* gpx, char * filename) {
    char * string = NULL;
    char ver[2048];
    char numW[2048];
    char numR[2048];
    char numT[2048];
    int numChars = 82; //23 filler chars + 44 chars for nameThings + 1 for null terminator
    if (gpx == NULL){
        string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
        strcpy(string, "{}");
        return string;
    }
    //get length of string going to make
    // -- add filename
    numChars += strlen(filename);
    // -- add version
    sprintf(ver, "%.1f", gpx->version);
    numChars += strlen(ver);
    // -- add creator
    numChars += strlen(gpx->creator);
    // -- add numWaypoints
    numChars += sprintf(numW, "%d", getLength(gpx->waypoints));
    // -- add numRoutes
    numChars += sprintf(numR, "%d", getLength(gpx->routes));
    // -- add numTracks
    numChars += sprintf(numT, "%d", getLength(gpx->tracks));

    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add filename
    strcpy( string, "{\"filename\":\"");
    strcat( string, filename );
    // -- add version
    strcat( string, "\",\"version\":");
    strcat( string, ver );
    //add creator
    strcat( string, ",\"creator\":\"");
    strcat( string, gpx->creator);
    // -- add numWaypoints
    strcat( string, "\",\"numWaypoints\":");
    strcat( string, numW );
    // -- add numRoutes
    strcat( string, ",\"numRoutes\":");
    strcat( string, numR );
    // -- add numTracks
    strcat( string, ",\"numTracks\":");
    strcat( string, numT );
    strcat( string, "}" );
    
    return string;
}








char * gpxFileToJSON(char * filename, char * schemaName, char *shortenedFilename){
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr = GPXPlusFileNametoJSON(doc, shortenedFilename);
    deleteGPXdoc(doc);
    return JSONstr;
}

char * createGPXreturnJSON( char * version, char * creator, char * filename, char * namespace){
    GPXdoc * doc = initializeGPXdoc();
    doc->version = atof(version);
    doc->creator = creator;
    strcpy(doc->namespace,namespace);

    if (isGPXDocNodeValid(doc) == 0){ //if valid
        if (writeGPXdoc (doc, filename) == false){
            fprintf(stderr,"error 1\n");
            return "{}";
        }
        else {
            fprintf(stderr,"file write worked\n");
        }
    } else {
        //deleteGPXdoc(doc);
        fprintf(stderr,"error 2\n");
        return "{}";
    }
    
    char * JSONstr = GPXtoJSON(doc);
    //fprintf(stderr,"Meep: %s\n", JSONstr);
    //deleteGPXdoc(doc);
    return JSONstr;
}

char * getRouteListArray ( char * filename , char * schemaName ){
    //create doc
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr ;
    if (getLength(doc->routes) > 0){
        JSONstr = routeListToJSON(doc->routes);
        fprintf(stderr, "RoutesList JSON: %s", JSONstr);
    }
    else {
        fprintf(stderr, "No routes.");
        deleteGPXdoc(doc);
        return ("{}");
    }
    deleteGPXdoc(doc);
    return JSONstr;
}

char * getTrackListArray ( char * filename , char * schemaName ){
    //create doc
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr ;
    if (getLength(doc->tracks) > 0){
        JSONstr = trackListToJSON(doc->tracks);
        fprintf(stderr, "TracksList JSON: %s", JSONstr);
    }
    else {
        fprintf(stderr, "No tracks.");
        deleteGPXdoc(doc);
        return ("{}");
    }
    deleteGPXdoc(doc);
    return JSONstr;
}








char * gpxRouteDataToJSON (const Route *rt){
    char * dataStr;
    void * elem;
    int numChars = 0;

    if (getLength(rt->otherData) == 0){
        dataStr = (char*)malloc(strlen("None") * sizeof(char));
        strcpy(dataStr, "None");
        fprintf(stderr, "%s", dataStr);
        return dataStr;
    }

    ListIterator iter = createIterator((List*)rt->otherData);
    while ((elem = nextElement(&iter)) != NULL ) { 
        numChars += strlen(((GPXData *)elem)->name);
        numChars += 5;  // for ': ' and '\n' at end, & \0
        numChars += strlen(((GPXData *)elem)->value);
    }
    dataStr = (char *)malloc(numChars * sizeof(char));
    ListIterator iter2 = createIterator((List*)rt->otherData);
    strcpy(dataStr, "");
    while ((elem = nextElement(&iter2)) != NULL ) { 
        char temp[strlen(((GPXData *)elem)->value)];
        strcpy( temp, ((GPXData *)elem)->value);
        for (int i = 0; i < strlen(temp) ; i++){
            if (temp[i] == '\n'){
                temp[i] = '\0';
            }
        }

        strcat(dataStr, ((GPXData *)elem)->name);
        strcat(dataStr, ": ");
        strcat(dataStr, temp);
        strcat( dataStr, " ");
    }
    fprintf(stderr, "%s", dataStr);
    return dataStr;
}


char * gpxTrackDataToJSON (const Track * tr){
    char * dataStr;
    void * elem;
    int numChars = 0;

    if (getLength(tr->otherData) == 0){
        dataStr = (char*)malloc(strlen("None") * sizeof(char));
        strcpy(dataStr, "None");
        fprintf(stderr, "%s", dataStr);
        return dataStr;
    }

    ListIterator iter = createIterator((List*)tr->otherData);
    while ((elem = nextElement(&iter)) != NULL ) { 
        numChars += strlen(((GPXData *)elem)->name);
        numChars += 5;  // for ': ' and '\n' at end, & \0
        numChars += strlen(((GPXData *)elem)->value);
    }
    dataStr = (char *)malloc(numChars * sizeof(char));
    ListIterator iter2 = createIterator((List*)tr->otherData);
    strcpy(dataStr, "");
    while ((elem = nextElement(&iter2)) != NULL ) { 
        char temp[strlen(((GPXData *)elem)->value)];
         strcpy( temp, ((GPXData *)elem)->value);
        for (int i = 0; i < strlen(temp) ; i++){
            if (temp[i] == '\n'){
                temp[i] = '\0';
            }
        }

        strcat(dataStr, ((GPXData *)elem)->name);
        strcat(dataStr, ": ");
        strcat(dataStr, temp);
        strcat( dataStr, " ");
    }
    fprintf(stderr, "%s", dataStr);
    return dataStr;
}


char * tracksBetweenJSON ( char * filename, char * schemaName, char *  sourceLat, char *  sourceLong, char *  destLat, char *  destLong, char *  delta){
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr;
    if (getLength(doc->tracks) > 0){
        List * tracksBtw = getTracksBetween(doc, atof(sourceLat), atof(sourceLong), atof(destLat), atof(destLong), atof(delta));
        if (tracksBtw == NULL){
            fprintf(stderr, "\nTRACK LIST IS NULL \n\n\n");
            deleteGPXdoc(doc);
            return ("{}");
        }
        if (getLength(tracksBtw) == 0){
            fprintf(stderr, "No tracks between.\n");
            deleteGPXdoc(doc);
            return ("{}");
        }
        JSONstr = trackListToJSON(tracksBtw);
        fprintf(stderr, "\n\nTracksList JSON: %s\n\n", JSONstr);
    }
    else {
        fprintf(stderr, "No tracks.");
        deleteGPXdoc(doc);
        return ("{}");
    }
    deleteGPXdoc(doc);
    return JSONstr;
}


char * routesBetweenJSON ( char * filename, char * schemaName, char *  sourceLat, char *  sourceLong, char *  destLat, char *  destLong, char *  delta){
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr;
    if (getLength(doc->routes) > 0){
        List * routesBtw = getRoutesBetween(doc, atof(sourceLat), atof(sourceLong), atof(destLat), atof(destLong), atof(delta));
        if (routesBtw == NULL){
            fprintf(stderr, "\nROUTE LIST IS NULL \n\n\n");
            deleteGPXdoc(doc);
            return ("{}");
        }
        if (getLength(routesBtw) == 0){
            fprintf(stderr, "No routes between.\n");
            deleteGPXdoc(doc);
            return ("{}");
        }
        JSONstr = routeListToJSON(routesBtw);
        fprintf(stderr, "\n\nRoutesList JSON: %s\n\n", JSONstr);
    }
    else {
        fprintf(stderr, "No routes.");
        deleteGPXdoc(doc);
        return ("{}");
    }
    deleteGPXdoc(doc);
    return JSONstr;
}


/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/
char* waypointToJSON(const Waypoint * wpt) {
    char * string = NULL;

    char lat[2048];
    char lon[2048];
    int numChars = 26; //19 filler chars + 20 chars for nameThings + 1 for null terminator

    if (wpt == NULL){
    string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
    strcpy(string, "{}");
    return string;
    }
    //get length of string going to make
    // -- add name
    if (strcmp(wpt->name, "\0") == 0){
        numChars += strlen("None");
    } else {
        numChars += strlen(wpt->name);
    }
    // -- add latitude
    sprintf(lat, "%f", wpt->latitude);
    numChars += strlen(lat);
    // -- add latitude
    sprintf(lon, "%f", wpt->longitude);
    numChars += strlen(lon);
    
    //malloc the string 
    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add name
    strcpy( string, "{'name':'" );
    if (strcmp(wpt->name, "\0") == 0){
        strcat( string,"None" );
    } else {
        strcat(string, wpt->name );
    }
    // -- add lat
    strcat( string, "','lat':" );
    strcat( string, lat );
    // -- add lon
    strcat( string, ",'lon':" );
    strcat( string, lon);
    strcat( string, "}");
//fprintf(stderr, "%s\n", string);
    return string;
}


/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* waypointListToJSON(const List *list) {
    if (list == NULL || getLength((List*)list) == 0 ){
        return NULL;
    }
    char * string = NULL;
    void * elem;
    char * temp;
    char * temp2;
    bool firstElem = true;
    int numChars = 3 + getLength((List*)list); //2 square brackets + (getLength(list) - 1) commas + 1 for null terminator = 2 + getLength(list)

    //get length of string going to make
    ListIterator iter = createIterator((List*)list);
    while ((elem = nextElement(&iter)) != NULL ) { 
        //add length of routeJSON for each route in list
        temp = waypointToJSON((Waypoint *)elem);
        numChars += strlen(temp);  
        if (temp != NULL)
            free(temp);
    }
    string = (char*)malloc(numChars * sizeof(char));
    //iterate through list again to make output string
    strcpy( string, "[");
    ListIterator iter2 = createIterator((List*)list);
    while ((elem = nextElement(&iter2)) != NULL ) { 
        if (firstElem == true){
            firstElem = false;
        } else {
            strcat( string, ",");
        }
        temp2 = waypointToJSON((Waypoint *)elem);
        strcat( string, temp2 );
        if (temp2 != NULL)
            free(temp2);
    }
    strcat( string, "]");
    //printf("\n\n'%s'\n", string);
    return string;
}



/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/
char* routeToJSONwithWaypoints(const Route *rt) {
    char * string = NULL;
    char numVal[2048];
    char routeLen[2048];
    bool isLoop;
    int numChars = 79; //19 filler chars + 20 chars for nameThings + 1 for null terminator

    if (rt == NULL){
    string = (char*)malloc((strlen("{}") + 1) * sizeof(char));
    strcpy(string, "{}");
    return string;
    }
    //get length of string going to make
    // -- add name
    if (strcmp(rt->name, "\0") == 0){
        numChars += strlen("None");
    } else {
        numChars += strlen(rt->name);
    }
    // -- add numVal (num waypoints)
    sprintf(numVal, "%d", getLength(rt->waypoints));
    numChars += strlen(numVal);
    // -- add routeLen
    sprintf(routeLen, "%.1f", round10(getRouteLen(rt)));
    numChars += strlen(routeLen);
    // -- add loop stat
    if (isLoopRoute(rt, A2_MOD3_DELTA) == true){
        numChars += strlen("true");
        isLoop = true;
    } else {
        numChars += strlen("false");
        isLoop = false;
    }
    // -- add other data string
    char * otherDataStr = gpxRouteDataToJSON(rt);
    numChars += strlen(otherDataStr);
    // -- add waylpoint list string
    char * waypointArray = waypointListToJSON(rt->waypoints);
    numChars += strlen(waypointArray);

    //malloc the string 
    string = (char*)malloc(numChars * sizeof(char));

    //write data to string
    // -- add name
    strcpy( string, "{\"name\":\"" );
    if (strcmp(rt->name, "\0") == 0){
        strcat( string,"None" );
    } else {
        strcat(string, rt->name );
    }
    // -- add numVal (num waypoints)
    strcat( string, "\",\"numPoints\":" );
    strcat( string, numVal );
    // -- add routeLen
    strcat( string, ",\"len\":" );
    strcat( string, routeLen);
    // -- add loop stat
    strcat( string, ",\"loop\":" );
    if (isLoop == true){
        strcat( string, "true" );
    } else {
        strcat( string, "false" );
    }
    strcat( string, ",\"other\":\"" );
    strcat( string, otherDataStr );
    strcat( string, "\",\"waypointList\":\"");
    strcat( string, waypointArray );
    strcat( string, "\"}");
//fprintf(stderr, "%s\n", string);
    return string;
}



/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* routeListToJSONwithWaypoints(const List *list) {
    if (list == NULL || getLength((List*)list) == 0 ){
        return NULL;
    }
    char * string = NULL;
    void * elem;
    char * temp;
    char * temp2;
    bool firstElem = true;
    int numChars = 3 + getLength((List*)list); //2 square brackets + (getLength(list) - 1) commas + 1 for null terminator = 2 + getLength(list)

    //get length of string going to make
    ListIterator iter = createIterator((List*)list);
    while ((elem = nextElement(&iter)) != NULL ) { 
        //add length of routeJSON for each route in list
        temp = routeToJSONwithWaypoints((Route *)elem);
        numChars += strlen(temp);  
        if (temp != NULL)
            free(temp);
    }
    string = (char*)malloc(numChars * sizeof(char));
    //iterate through list again to make output string
    strcpy( string, "[");
    ListIterator iter2 = createIterator((List*)list);
    while ((elem = nextElement(&iter2)) != NULL ) { 
        if (firstElem == true){
            firstElem = false;
        } else {
            strcat( string, ",");
        }
        temp2 = routeToJSONwithWaypoints((Route *)elem);
        strcat( string, temp2 );
        if (temp2 != NULL)
            free(temp2);
    }
    strcat( string, "]");
    printf("\n\n'%s'\n", string);
    return string;
}












char * getRouteListArraywithWaypoints ( char * filename , char * schemaName ){
    //create doc
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{}");
    }
    char * JSONstr ;
    if (getLength(doc->routes) > 0){
        JSONstr = routeListToJSONwithWaypoints(doc->routes);
        fprintf(stderr, "\nRoutesList JSON: %s", JSONstr);
    }
    else {
        fprintf(stderr, "No routes.");
        deleteGPXdoc(doc);
        return ("{}");
    }
    deleteGPXdoc(doc);
    return JSONstr;
}















char * renameRoute ( char * filename, char * schemaName, int index, char * newName){
  //create doc
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{\"successful\":false}");
    }
    int curIndex = 0;
    void * elem;
    if (getLength(doc->routes) >= index){
         ListIterator iter = createIterator(doc->routes);
        while ((elem = nextElement(&iter)) != NULL){
            if (curIndex == index){
                free(((Route*)elem)->name);

                ((Route*)elem)->name = (char*)malloc((strlen((char*)newName)+1) * sizeof(char));
                strcpy( ((Route*)elem)->name , newName);
                printf("new name set to : '%s'\n", ((Route*)elem)->name );
            }
            curIndex++;
        }
    }
    else {
        deleteGPXdoc(doc);
        return ("{\"successful\":false}");
    }
    writeGPXdoc (doc, filename);
    deleteGPXdoc(doc);
    return ("{\"successful\":true}");
}








char * renameTrack ( char * filename, char * schemaName, int index,  char * newName){
      //create doc
    GPXdoc * doc = createValidGPXdoc(filename, schemaName);
    if (doc == NULL){
        return ("{\"successful\":false}");
    }
    int curIndex = 0;
    void * elem;
    if (getLength(doc->tracks) >= index){
         ListIterator iter = createIterator(doc->tracks);
        while ((elem = nextElement(&iter)) != NULL){
            if (curIndex == index){
                free(((Track*)elem)->name);

                ((Track*)elem)->name = (char*)malloc((strlen((char*)newName)+1) * sizeof(char));
                strcpy( ((Track*)elem)->name , newName);
                printf("new name set to : '%s'\n", ((Track*)elem)->name );
            }
            curIndex++;
        }
    }
    else {
        deleteGPXdoc(doc);
        return ("{\"successful\":false}");
    }
    writeGPXdoc (doc, filename);
    deleteGPXdoc(doc);
    return ("{\"successful\":true}");
}
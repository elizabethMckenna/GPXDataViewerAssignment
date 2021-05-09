#include "GPXParser.h"
#include "GPXHelpers.h"


/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */
int
main(int argc, char **argv)
{
    if (argc != 3){
        printf("wrong num command args\n");
        return(1);
    }


    GPXdoc *doc = NULL;
    int filenameLength = strlen(argv[1]) + 1 ;
    int schemanameLength = strlen(argv[2]) + 1 ;
    char fileNameCommandLine[filenameLength];
    char schemaNameCommandLine[schemanameLength];
    strcpy(fileNameCommandLine,argv[1]);
    strcpy(schemaNameCommandLine,argv[2]);
    printf("\n\nFilename on ARGV[1] is : '%s' -- %d\n", fileNameCommandLine, filenameLength);
    printf("Schema name on ARGV[2] is : '%s' -- %d\n", schemaNameCommandLine, schemanameLength);

printf("getNumWaypoints(NULL DOC POINTER): %d\n", getNumWaypoints(doc));
printf("getNumRoutes(NULL DOC POINTER): %d\n", getNumRoutes(doc));
printf("getNumTracks(NULL DOC POINTER): %d\n", getNumTracks(doc));
 printf("getNumSegments(NULL DOC POINTER): %d\n", getNumSegments(doc));
 printf("getNumGPXData(NULL DOC POINTER): %d\n", getNumGPXData(doc));

    doc = createValidGPXdoc(fileNameCommandLine, schemaNameCommandLine);
    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
        return -1;
    }
    char* temp = (GPXdocToString(doc));
    printf(temp);

    /*Get the root element node */
    //root_element = xmlDocGetRootElement(doc);

printf("WAYPOINT LIST: %d\n", getLength(doc->waypoints));
if (getLength(doc->waypoints) == 0){
    printf("Nope\n");
}
   char* docString = toString(doc->waypoints);

    printf("getNumWaypoints(AFTER PARSING DOC): %d\n", getNumWaypoints(doc));
    printf("getNumRoutes(AFTER PARSING DOC): %d\n", getNumRoutes(doc));
    printf("getNumTracks(AFTER PARSING DOC): %d\n", getNumTracks(doc));
    printf("getNumSegments(AFTER PARSING DOC): %d\n", getNumSegments(doc));
    printf("getNumGPXData(AFTER PARSING DOC): %d\n", getNumGPXData(doc));
    

    printf("\n\n\n\n");
    getWaypoint(doc,"Raven Dance Peak 1647m");
    getWaypoint(doc, "Foo");
    printf("\n\n");

    getTrack(doc, "Mount Steele Trail");
    getTrack(doc, "Some other trail");
    printf("\n\n");


    getRoute(doc, "Some route");
    printf("\n\n");

printf("namespace: %s\n", doc->namespace);

//xmlDocPtr meep = GPXtoXML (doc);
writeGPXdoc (doc, "oink.gpx");
validateGPXDoc(doc, schemaNameCommandLine);

printf("round10(403.27): %.1f\n", round10(403.27));
printf("round10(404.5): %.1f\n", round10(404.5));
printf("round10(405): %.1f\n", round10(405));
printf("round10(409): %.1f\n", round10(409));
printf("haversine:  lat=43.532623 lon=-80.226235 && lat=43.532756 lon=-80.226043 :  %.5f\n ", haversine(43.532623,-80.226235,43.532756,-80.226043));
ListIterator iter = createIterator(doc->routes);
void * potato = nextElement(&iter);
printf("here: %d\n", getLength(doc->routes));
printf("getRouteLen: %f\n", getRouteLen((Route*)potato));
ListIterator iter2 = createIterator(doc->tracks);
potato = nextElement(&iter2);
printf("here: %d\n", getLength(doc->tracks));
printf("getTrackLen: %f\n", getTrackLen((Track*)potato));
List * routesBetweenList = getRoutesBetween( doc, 44.59590, -123.62087,  44.5972, -123.6202, 1000000 );
if (routesBetweenList == NULL){
    printf("routes between list is null\n");
}
else {
    printf("Routes between list ->\n");
    char * meepString = toString(routesBetweenList);
    printf("%s", meepString);
    free(meepString);
}
List * tracksBetweenList = getTracksBetween( doc, 44.59590, -123.62087,  44.5972, -123.6202, 1000000 );
if (tracksBetweenList == NULL){
    printf("tracks between list is null\n");
}
else {
    printf("tracks between list ->\n");
    char * moopString = toString(tracksBetweenList);
    printf("%s", moopString);
    free(moopString);
}

char alpaca[] = "{\"version\":1.1,\"creator\":\"WebTool\"}" ;
GPXdoc * llama = JSONtoGPX(alpaca);

char cat[] = "{\"lat\":43.537299,\"lon\":-80.218267}";
Waypoint * kitten = JSONtoWaypoint(cat);
deleteWaypoint(kitten);

char gecko[] = "{\"name\":\"Reynolds walk\"}";
Route * lizard = JSONtoRoute(gecko);
deleteRoute(lizard);

char * boop = gpxFileToJSON(fileNameCommandLine);


    freeList(tracksBetweenList);
    freeList(routesBetweenList);
    free(docString);
    free(temp);

        deleteGPXdoc(doc);

        deleteGPXdoc(llama);

    return 0;
}




var isLoggedIntoDatabase = false;


// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {

  /*  //TEMP: preset db setting
    document.getElementById("databaseLoginUsername").value= "mckennae";
    document.getElementById("databaseLoginPassword").value= "1053037";
    document.getElementById("databaseLoginDatabaseName").value= "mckennae";
*/
    document.getElementById("databaseLoginUsername").value= "";
    document.getElementById("databaseLoginPassword").value= "";
    document.getElementById("databaseLoginDatabaseName").value= "";

    $('#databaseOpLabel').html(""); //clear db op label text
    if (isLoggedIntoDatabase == false){
        document.getElementById("databaseLoginForm").style.display = "block";
        document.getElementById("databaseLogoutBtn").style.display = "none";
        document.getElementById("databaseOpButtonPanel").style.display = "none";
    } else {
        document.getElementById("databaseLoginForm").style.display = "none";
        document.getElementById("databaseLogoutBtn").style.display = "block";
        document.getElementById("databaseOpButtonPanel").style.display = "block";
        document.getElementById("queryButtonPanel").style.display = "block";
        
    }

    //hide all query search panels
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";

    document.getElementById("queryResultPanel").style.display = "none";
    document.getElementById("queryButtonPanel").style.display = "none";

    //hide rename panel until logged in, and clear text field
    document.getElementById("renamePanel").style.display = "none";
    document.getElementById("newNameText").value = "";
    document.getElementById("renamePanelNotLoggedInNotice").style.display = "block";



    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/loadGPXFilesOnPageLoad',   //The server endpoint we are connecting to
        data: {
        },
        success: function (data) {
            console.log("Back");
            console.log(data);

            if (data.length > 0){
               for (var i = 0; i < data.length; i++){
                   console.log("GPXarray at[" + i + "]" + JSON.stringify(data[i]));
                   var currentString = JSON.stringify(data[i]);
                   var currentGPX = JSON.parse(currentString);

                   //add each file to file log panel
                   $('#fileLogPanelTable').append(
                    '<tr>'
                        + "<td> <a href=\"" + currentGPX.filename + "\" download \" >" + currentGPX.filename + "</a> </td>"
                        + '<td> ' + currentGPX.version + '</td>'
                        + '<td> ' + currentGPX.creator + '</td>'
                        + '<td> ' + currentGPX.numWaypoints + '</td>'
                        + '<td> ' + currentGPX.numRoutes + '</td>'
                        + '<td> ' + currentGPX.numTracks + '</td>'
                    + '</tr>'
                    );
                    console.log("File Log Table successfully populated on load.");
                    //add each file to drop down list
                    $('#file-select-dropdown').append(
                     '<option value=\"' + currentGPX.filename + "\">" + currentGPX.filename + "</option>"
                    );
                    console.log("File drop down list successfully populated on load.");
               }
            }
            else {    
                $('#filelogMessageText').html("File log is empty");
                //We write the object to the console to show that the request was successful
                console.log("File log is empty because no valid files contained in uploads folder"); 
                //$('#fileLogPanelTable').append('<tr> <td> No Files </td> </tr>' );
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });


    // No redirects if possible
    $('#fileLogPanel').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //console.log("MEOW!");
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint 
        });
    });


    // 'CREATE GPX' BUTTON CLICKED
    $('#newGPXform').submit(function(e){
        $('#addRoutesMessageBox').html("Attempting to Create New Route");
        e.preventDefault();
        console.log("Create GPX BUtton Pushed");
        console.log(document.querySelector("#createGPXversionNumBox").value)
        console.log(document.querySelector("#createGPXfileNameBox").value)
        console.log(document.querySelector("#createGPXcreatorNameBox").value)
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getDataFromCreateGPXForm',   //The server endpoint we are connecting to
            data: {
                version: document.querySelector("#createGPXversionNumBox").value,
                filename: document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_"), //replace whitespace with _
                creatorName: document.querySelector("#createGPXcreatorNameBox").value
            },
            success: function (data) {
                console.log("LOOK HERE!");
                console.log(data);
                console.log(JSON.stringify(data));
                // if data object is empty, file not created
                if (data.length <= 0){
                    alert("Oh No!\n'" 
                    + document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") 
                    + ".gpx' Was not uploaded.");
                    $('#createNewGPXTextOut').html("");
                } else { 
                    $('#createNewGPXTextOut').html("File " + document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") + ".gpx was uploaded" );
                    console.log("File " +  document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") + ".gpx uploaded successfully"); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });
    });


    


     // 'LOGIN TO DATABASE' BUTTON CLICKED
     $('#databaseLoginForm').submit(function(e){
        e.preventDefault();
       // $('#databaseLoginSubmitLabel').html("Trying to connect to database.");
        e.preventDefault();
        console.log("Opening connection to database.");
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getDataFromDatabaseLoginForm',   //The server endpoint we are connecting to
            data: {
                username: document.querySelector("#databaseLoginUsername").value,
                password: document.querySelector("#databaseLoginPassword").value, //replace whitespace with _
                databaseName: document.querySelector("#databaseLoginDatabaseName").value
            },
            success: function (data) {
                console.log("Returned from database login button");
                console.log(data);
                if (data.successful == true){
                    console.log("Database accessed successfully");

                } else if (data.successful == false ){
                    console.log("Database accessed unsuccessfully");
                    //alert user
                    alert("\t\t\t\t   ERROR:\n Database was not able to be accessed successfully.\n" 
                         + "Please ensure login information is correct and valid.");
                    // clear message panel
                   // $('#databaseLoginSubmitLabel').html(""); 

                }
                if (numberOfValidFiles() == 0 ){
                    document.getElementById("databaseLoginForm").style.display = "none";
                    document.getElementById("storeAllFilesButton").style.display= "none";
                    $('#databaseOpLabel').html("Logged in. \nCurrently No Files in Database.\n");
                    console.log("No files in uploads");
                    getDatabaseStatusButtonClicked(); 
                } else if (data.successful == false){
                    //do nothing
                } else {
                    // hide login panel, show logout button
                    $('#databaseOpLabel').html("Logged in. \n");
                    console.log("Logged in to DB");
                    document.getElementById("databaseLoginForm").style.display = "none";
                    document.getElementById("databaseLogoutBtn").style.display = "block";
                    document.getElementById("databaseOpButtonPanel").style.display = "block";
                    document.getElementById("queryButtonPanel").style.display = "block";
                    //hide clear all data if tables are all empty
                    if (data.numRowsFILEdb == 0 && data.numRowsROUTEdb == 0 && data.numRowsPOINTdb == 0 ){
                        document.getElementById("clearAllTablesButton").style.display = "none";
                        document.getElementById("queryButtonPanel").style.display = 'none';
                    }
                    // show rename routes panel, hide login for more options memo clear text box
                    document.getElementById("renamePanel").style.display = "block";
                    document.getElementById("newNameText").value = "";
                    document.getElementById("renamePanelNotLoggedInNotice").style.display = "none";

                    getDatabaseStatusButtonClicked(); 
                }                    
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });
        document.querySelector("#databaseLoginUsername").value = "";
        document.querySelector("#databaseLoginPassword").value = "";
        document.querySelector("#databaseLoginDatabaseName").value = "";
    });

    //empty all the fields to be in og state
    emptyTable(pathsFoundTable);
    emptyTable(viewPanelTable);


/*
    // 'ADD ROUTE' BUTTON CLICKED
    $('#newGPXform').submit(function(e){
        e.preventDefault();
        console.log("ADD ROUTE BUTTON PUSHED");
        var waypointTable = document.querySelector("#waypointsInputTable");
        var numWaypoints = waypointTable.rows.length;
        console.log("Number waypoints in Route: " + numWaypoints);
        var filename = document.getElementById("file-select-dropdown").value
    
        //error checking:
            //if issue with file name display alert
        if (filename == undefined || filename == null || filename == "" || filename == "--Please choose a file--"){
            //if file was not selected, send error
            alert("Error: No File Has Been selected from dropdown menu.");
            return ;
        }
    
        
       $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getDataFromAddRouteForm',   //The server endpoint we are connecting to
        data: {
            filename: filename,
            numWaypoints: numWaypoints,
            table: waypointTable
        },
    
        success: function (data) {
            console.log("LOOK HERE!");
            
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
       // return false;
});*/

        /*
        $('#createNewGPXTextOut').html("Attempting to Create New GPX");
        e.preventDefault();
        console.log("ADD ROUTE BUTTON PUSHED");
        var numWaypoints = document.getElementById("#waypointsInputTable").rows.length;
        var fileSelected = document.getElementById("file-select-dropdown").value;
        console.log("File Selected: " + fileSelected);
        console.log("Number waypoints in Route: " + numWaypoints);
        




     /*   console.log("Create GPX BUtton Pushed");
        console.log(document.querySelector("#createGPXversionNumBox").value)
        console.log(document.querySelector("#createGPXfileNameBox").value)
        console.log(document.querySelector("#createGPXcreatorNameBox").value)
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getDataFromCreateGPXForm',   //The server endpoint we are connecting to
            data: {
                version: document.querySelector("#createGPXversionNumBox").value,
                filename: document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_"), //replace whitespace with _
                creatorName: document.querySelector("#createGPXcreatorNameBox").value
            },

            success: function (data) {
                console.log("LOOK HERE!");
                console.log(data);
                console.log(JSON.stringify(data));
                // if data object is empty, file not created
                if (data.length <= 0){
                    alert("Oh No!\n'" 
                    + document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") 
                    + ".gpx' Was not uploaded.");
                    $('#createNewGPXTextOut').html("");
                } else { 
                    $('#createNewGPXTextOut').html("File " + document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") + ".gpx was uploaded" );
                    console.log("File " +  document.querySelector("#createGPXfileNameBox").value.replace(/[ .]+/g,"_") + ".gpx uploaded successfully"); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });*/
    //});

    
    

    




// 'FIND PATHS' BUTTON CLICKED
$('#pathsBetweenForm').submit(function(e){
    e.preventDefault();
    $('#pathsBetweenFormMessageBar').html("Searching for Paths Between");
    e.preventDefault();
    console.log("Find paths between button pressed");
    console.log("latStart: " + document.querySelector("#latNumBoxStart").value);
    console.log("lonStart: " + document.querySelector("#lonNumBoxStart").value);
    console.log("latEnd: " + document.querySelector("#latNumBoxEnd").value);
    console.log("lonEnd: " + document.querySelector("#lonNumBoxEnd").value);
    console.log('delta: ' + document.querySelector("#deltaVal").value );
    var filename = document.getElementById("file-select-dropdown").value;
    var sourceLat = document.querySelector("#latNumBoxStart").value;
    var sourceLon = document.querySelector("#lonNumBoxStart").value;
    var destLat = document.querySelector("#latNumBoxEnd").value;
    var destLon = document.querySelector("#lonNumBoxEnd").value;
    var delta = document.querySelector("#deltaVal").value;
    if (filename == null || filename == undefined || filename == "--Please choose a file--"  ){
        alert("Error: No File Has Been selected from dropdown menu.");
        return ;
    }

    //Pass data to the Ajax call, so it gets passed to the server
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getDataFromPathBetweenForm',   //The server endpoint we are connecting to
        data: {
            filename: filename,
            sourceLat: sourceLat,
            sourceLon: sourceLon,
            destLat: destLat,
            destLon: destLon,
            delta: delta
        },

        success: function (data) {
            emptyTable(pathsFoundTable);
            console.log("LOOK HERE!");
            console.log(data);
            console.log(data.tracksBetweenArray);
            console.log("Tracks Between: " + data.tracksBetweenArray.length);
            console.log(data.routesBetweenArray);
            console.log("Routes Between: " + data.routesBetweenArray.length);
            
            var routesBetween = data.routesBetweenArray[i]; 
            if (! data.tracksBetweenArray.length ){
                console.log("No tracks between");
            } else {
                for (var i = 0; i < data.tracksBetweenArray.length; i++){
                    var tracksBetween = data.tracksBetweenArray[i];
                    $('#pathsFoundTable').append(
                        '<tr>'
                            + "<td>" + "Track " + (i+1) + "</td>"
                            + '<td> ' + tracksBetween.name + '</td>'
                            + '<td> ' + tracksBetween.numPoints + '</td>'
                            + '<td> ' + tracksBetween.len + '</td>'
                            + '<td> ' + tracksBetween.loop + '</td>'
                            +'<td> <input type="button" onclick="alert('+ "'" + tracksBetween.other + "')" + '" value="Show Other Data"> </td>'
                        + '</tr>'
                    ); 
                }
            }
            if (! data.routesBetweenArray.length ){
                console.log("No routes between");
            } else {
                for (var i = 0; i < data.routesBetweenArray.length; i++){
                    var routesBetween = data.routesBetweenArray[i];
                    $('#pathsFoundTable').append(
                        '<tr>'
                            + "<td>" + "Route " + (i+1) + "</td>"
                            + '<td> ' + routesBetween.name + '</td>'
                            + '<td> ' + routesBetween.numPoints + '</td>'
                            + '<td> ' + routesBetween.len + '</td>'
                            + '<td> ' + routesBetween.loop + '</td>'
                            +'<td> <input type="button" onclick="alert('+ "'" + routesBetween.other + "')" + '" value="Show Other Data"> </td>'
                        + '</tr>'
                    ); 
                }
            }

            if (! data.routesBetweenArray.length && ! data.tracksBetweenArray.length ){
                $('#pathsFoundTable').append(
                    '<tr>'
                        + "<td>" + "No paths between were found. " + "</td>"
                    + '</tr>'
                ); 
            }

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
});

}); //goes with top of ready funtion


// populate dropdown with components from GPX View Table
function populateDropdownWithComponentsFromGPXView( filename ) {
    console.log("populating dropdown with files from DB");
    //empty dropdown of data that might already be in it
    $('rename-dropdown-box').empty();
    //add placeholder line to top
    $('rename-dropdown-box').append('<option placeholder="">--Please choose a component--</option>' );

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/populateViewPanelTable',   //The server endpoint we are connecting to
        data: {
            filename: filename
        },
        success: function (data) {
            console.log("MEEP MEEP");
            console.log(data[0]);
            retObj = data[0];
            
            //routes
            if (retObj.routeListArray.length > 0){
                for (var i = 0; i < retObj.routeListArray.length; i++){
                    $('#rename-dropdown-box').append('<option value=\'{\"type\":\"rte\", \"index\":' + (i) + "}\'> Route " + (i+1) + "</option>" );      
                }
            } 
            //tracks
            if (retObj.trackListArray.length > 0){

                for (var i = 0; i < retObj.trackListArray.length; i++){
                    $('#rename-dropdown-box').append('<option value=\'{\"type\":\"trk\",\"index\":' + (i) + "}\'> Track " + (i+1) + "</option>" );      
                }
            } 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}




function fileSelectSubmit(){
    console.log("IN THE FUNCTION");
    $('rename-dropdown-box').empty();
    $('rename-dropdown-box').append('<option placeholder="">--Please choose a component--</option>' );
    
    //GET FILE SELECTED FROM DROPDOWN MENU

    var fileSelected = document.getElementById("file-select-dropdown").value;
    console.log(fileSelected);
    if (fileSelected == "--Please choose a file--"){
        alert("Please choose one of the files from the drop down list.\n");
        emptyTable(viewPanelTable);
        $('#viewPanelTable').append('<tr> <td> No File Selected </td> </tr>' );
    
    } else {
        $('#addRouteFormMessageBox').html("Adding Route to File: " + fileSelected);
        $('#pathsBetweenFormMessageBar').html("Searching in File: " + fileSelected);
        emptyTable(viewPanelTable);
        populateViewPanelTable(fileSelected);
        $('rename-dropdown-box').empty();
        $('rename-dropdown-box').append('<option placeholder="">--Please choose a component--</option>' );
        populateDropdownWithComponentsFromGPXView(fileSelected);
    }
}


function emptyTable( tablePassed){
    while( tablePassed.rows.length > 1)
     tablePassed.rows[1].remove();
}

function emptyTableFull( tablePassed){
    while( tablePassed.rows.length > 0)
     tablePassed.rows[0].remove();
}

function populateViewPanelTable( filename ){
    console.log("File Selected: " + filename);
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/populateViewPanelTable',   //The server endpoint we are connecting to
        data: {
            filename: filename
        },
        success: function (data) {
            console.log("LOOK HERE!");
            console.log(data[0]);
            retObj = data[0];
            //console.log("num tracks: " + retObj.trackListArray.length);
            
            //tracks
            if (retObj.trackListArray.length > 0){
                for (var i = 0; i < retObj.trackListArray.length; i++){
                    currentTrk = retObj.trackListArray[i];
                    var name = currentTrk.name;
                    
                    console.log("NAME IS: " + name);
                    if (name.localeCompare("None") == 0) {
                        $('#viewPanelTable').append(
                            '<tr>'
                                + "<td>" + "Track " + (i+1) + "</td>"
                                + '<td> ' + " " + '</td>'
                                + '<td> ' + currentTrk.numPoints + '</td>'
                                + '<td> ' + currentTrk.len + '</td>'
                                + '<td> ' + currentTrk.loop + '</td>'
                                +'<td> <input type="button" onclick="alert('+ "'" + currentTrk.other + "')" + '" value="Show Other Data"> </td>'
                            + '</tr>'
                        ); 
                    } else {
                        //add each track to view panel
                        $('#viewPanelTable').append(
                            '<tr>'
                                + "<td>" + "Track " + (i+1) + "</td>"
                                + '<td> ' + currentTrk.name + '</td>'
                                + '<td> ' + currentTrk.numPoints + '</td>'
                                + '<td> ' + currentTrk.len + '</td>'
                                + '<td> ' + currentTrk.loop + '</td>'
                                +'<td> <input type="button" onclick="alert('+ "'" + currentTrk.other + "')" + '" value="Show Other Data"> </td>'
                            + '</tr>'
                        ); 
                    }      
                }
            } else {
                console.log("There are no tracks to add to table.");
            }
            //routes
            if (retObj.routeListArray.length > 0){
                for (var i = 0; i < retObj.routeListArray.length; i++){
                    currentRte = retObj.routeListArray[i];     
                    var rteName = currentRte.name; 
                    
                    
                    console.log("OtherData: " + currentRte.other);           
                   //add each track to view panel
                    if (rteName.localeCompare("None") == 0){
                        var otherStr = currentRte.other;
                        $('#viewPanelTable').append(
                            '<tr>'
                                + "<td>" + "Route " + (i+1) + "</td>"
                                + '<td> ' + " " + '</td>'
                                + '<td> ' + currentRte.numPoints + '</td>'
                                + '<td> ' + currentRte.len + '</td>'
                                + '<td> ' + currentRte.loop + '</td>'
                                +'<td> <input type="button" onclick="alert('+ "'" + currentRte.other + "')" + '" value="Show Other Data"> </td>'
                            + '</tr>'
                        ); 
                    } else {
                        var otherStr = currentRte.other;
                        $('#viewPanelTable').append(
                            '<tr>'
                                + "<td>" + "Route " + (i+1) + "</td>"
                                + '<td> ' + currentRte.name + '</td>'
                                + '<td> ' + currentRte.numPoints + '</td>'
                                + '<td> ' + currentRte.len + '</td>'
                                + '<td> ' + currentRte.loop + '</td>'
                                +'<td> <input type="button" onclick="alert('+ "'" + currentRte.other + "')" + '" value="Show Other Data"> </td>'
                            + '</tr>'
                        ); 
                    }
                }
            } else {
                console.log("There are no tracks to add to table.");
            }

            //add message for empty files
            if (retObj.routeListArray.length == undefined && retObj.trackListArray.length == undefined  ){
                $('#viewPanelTable').append(
                    '<tr>'
                        + "<td>" + "File contains no tracks or routes"+ "</td>"
                    + '</tr>'
                    ); 
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
    return;
}

function addWaypointRows(){
    console.log("INSIDE AddWaypointRows Function");
    var numWaypointsToAdd = document.querySelector("#numWaypointChoice").value;
    console.log("Add " + numWaypointsToAdd + "waypoints to add route form");
    // add selected number of waypoints to table
    emptyTableFull(waypointsInputTable);
    for (var i = 0; i < numWaypointsToAdd; i++){
        $('#waypointsInputTable').append(
            '<tr> '
                + '<td> '+ (i+1) + ' </td>'
                + '<td> <input type="number" class="form-control" id="latitudeNumBox"' + (i+1) +' placeholder="Latitude" step=.00000001 min=-90 max=90 required> </td>'
                + '<td> <input type="number" class="form-control" id="longitudeNumBox"' + (i+1) + ' placeholder="Longitude" step=.00000001 min=-180 max=180 required> </td>'
            + '</tr>'
        ); 
    }
    //clear input box
    document.querySelector("#numWaypointChoice").value = document.querySelector("#waypointsInputTable").rows.length;
    return;
}


function addRouteFormSubmitBtnPressed(){
    
    console.log("ADD ROUTE BUTTON PUSHED");
    var waypointTable = document.querySelector("#waypointsInputTable");
    var numWaypoints = waypointTable.rows.length;
    console.log("Number waypoints in Route: " + numWaypoints);
    var filename = document.getElementById("file-select-dropdown").value

    //error checking:
        //if issue with file name display alert
    if (filename === undefined || filename === null || filename === "" || filename === "--Please choose a file--"){
        //if file was not selected, send error
        alert("Error: No File Has Been selected from dropdown menu.");
        return ;
    }

    
   $.ajax({
    type: 'get',            //Request type
    dataType: 'json',       //Data type - we will use JSON for almost everything 
    url: '/getDataFromAddRouteForm',   //The server endpoint we are connecting to
    data: {
        filename: filename,
        numWaypoints: numWaypoints,
        table: waypointTable
    },
        success: function (data) {
            console.log("Ajax Returned"); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}


function logoutDatabase(){
    console.log("Logging out of Current Database");
    alert("Logging out of Current Database");
    isLoggedIntoDatabase = false;
    document.getElementById("databaseLoginForm").style.display = "block";
    document.getElementById("databaseLogoutBtn").style.display = "none";
    document.getElementById("databaseOpButtonPanel").style.display = "none";
    // hide rename routes panel, show login memo clear text box
    document.getElementById("renamePanel").style.display = "none";
    document.getElementById("newNameText").value = "";
    document.getElementById("renamePanelNotLoggedInNotice").style.display = "block";
    // set all database serach values to hidden
     //hide all query search panels
     document.getElementById("displayAllRoutesPanel").style.display = "none";
     document.getElementById("displayRoutesFromFilePanel").style.display = "none";
     document.getElementById("displayPointsFromRoutePanel").style.display = "none";
     document.getElementById("displayPointsFromFilePanel").style.display = "none";
     document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
 
     document.getElementById("queryResultPanel").style.display = "none";
     document.getElementById("queryButtonPanel").style.display = "none";
     $('#databaseOpLabel').html("");
}






function getTableLengths(){
    var retObj = {};
    console.log("getting db table lengths...");
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getDBTableLengths',   //The server endpoint we are connecting to
        data: {
        },
        success: function (data) {
            console.log("returned lengths are: ");
            console.log("DATA OBJEECT");
            console.log(data);  
            retObj.successful = data.successful;
            retObj.numRowsFILEdb = data.numRowsFILEdb;
            retObj.numRowsROUTEdb = data.numRowsROUTEdb;
            retObj.numRowsPOINTdb = data.numRowsPOINTdb;
            console.log("RETOBJ OBJEECT");
            console.log(retObj); 
            return retObj;
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}



// 'CLEAR ALL TABLES' BUTTON CLICKED
function clearAllDBTablesButtonClicked(){
    //set query panel stuff invisible
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
    
    $('#databaseOpLabel').html("Clearing all Tables.");
    console.log("Clearing tables");
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/clearAllDBTables',   //The server endpoint we are connecting to
        data: {
        },
        success: async function (data) {
            console.log("Returned from ajax");
            dbTableLens = getTableLengths();
            console.log(dbTableLens);
            document.getElementById("clearAllTablesButton").style.display = "none"; 
            document.getElementById("queryButtonPanel").style.display = 'none';
            getDatabaseStatusButtonClicked(); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }  
    });
}


function getDatabaseStatusButtonClicked() {
    console.log("GETTING DB STATUS...");
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getDBTableLengths',   //The server endpoint we are connecting to
        data: {
        },
        success: function (data) {
            console.log("returned lengths are: ");
            console.log("DATA OBJEECT");
            console.log(data);  
            alert("Database has " + data.numRowsFILEdb + " files, " 
                          + data.numRowsROUTEdb + " routes, and " 
                          + data.numRowsPOINTdb + " points." );
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}



function numberOfValidFiles(){
    var num = 0;
    for (var i = document.getElementById('fileLogPanelTable').rows.length; i > 1; i--){
        num++;
    }
    console.log("numberOfFiles Function retuning " + num);
    return num; 
}



// 'STORE ALL FILES' BUTTON CLICKED
function storeAllFilesBtnClicked(){
    //set query panel stuff invisible
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";

    $('#databaseOpLabel').html("Loading all Files into Database.");
    console.log("Opening connection to database.");
  $.ajax({
    type: 'get',            //Request type
    dataType: 'json',       //Data type - we will use JSON for almost everything 
    url: '/storeAllFilesFILEtable',  // '/loadGPXFilesOnPageLoad',   //The server endpoint we are connecting to
    data: {
    },
    success: function (data) {
        const fileDataArray = data;
        console.log("List of Files added to FILE table:");
        console.log(fileDataArray);

        if (data.length > 0){
            for (var i = 0; i < data.length; i++){
                // call ajax to add all routes from each file in list returned
              $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/storeAllFilesROUTEtable',  // '/loadGPXFilesOnPageLoad',   //The server endpoint we are connecting to
                data: {
                    listOfFiles: fileDataArray
                },
                success: function (data) {
                    console.log("List of Files added to ROUTE table:");
                    console.log(data);
        
                    if (data.length == 0){
                        alert("Data base is up to date.");
                    }
                    else {    
                        //if data array empty, no files being added, so do nothing more
                    }
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    $('#errorMessageBar').html("On page load, received error from server");
                    console.log(error); 
                }
              });
            }
            getDatabaseStatusButtonClicked();
            //be sure clear button visible
            document.getElementById("clearAllTablesButton").style.display = "inline";
            document.getElementById("queryButtonPanel").style.display = "block";
        }
        else {    
            //if data array empty, no files being added, so do nothing more
            alert("All Files are up to date; Nothing added to database.");
            getDatabaseStatusButtonClicked();
        }
    },
    fail: function(error) {
        // Non-200 return, do something with error
        $('#errorMessageBar').html("On page load, received error from server");
        console.log(error); 
    }
  });
}








// populate dropdown with files from database
function populateDropdownWithFilesFromDB( dropdownBoxID ){
    console.log("populating dropdown with files from DB");
    //empty dropdown of data that might already be in it
    $(dropdownBoxID).empty();
    //add placeholder line to top
    $(dropdownBoxID).append('<option placeholder="">--Please choose a file--</option>' );
    $.ajax({
    type: 'get',            //Request type
    dataType: 'json',       //Data type - we will use JSON for almost everything 
    url: '/getFilenamesAndIDsFromDB',   //The server endpoint we are connecting to
    data: {
    },
    success: function (data) {
        console.log(data);
        for (var i = 0; i < data.length; i++){
            var currentFilename = data[i].file_name;
            var currentGPXid = data[i].gpx_id;
            //add file to drop down list
            $(dropdownBoxID).append('<option value=\"' + currentGPXid + "\">" + currentFilename + "</option>" );
            console.log("File drop down list successfully populated with files from db.");
        }
    },
    fail: function(error) {
        // Non-200 return, do something with error
        $('#errorMessageBar').html("On page load, received error from server");
        console.log(error); 
    }   
});
}

   
// populate dropdown with routes from database
function populateDropdownWithRoutesFromDB( dropdownBoxID ) {
    console.log("populating dropdown with files from DB");
    //empty dropdown of data that might already be in it
    $(dropdownBoxID).empty();
    //add placeholder line to top
    $(dropdownBoxID).append('<option placeholder="">--Please choose a route--</option>' );
    $.ajax({
    type: 'get',            //Request type
    dataType: 'json',       //Data type - we will use JSON for almost everything 
    url: '/getRouteNamesAndIDsFromDB',   //The server endpoint we are connecting to
    data: {
    },
    success: function (data) {
        console.log(data);
        for (var i = 0; i < data.length; i++){
            var currentRoutename = data[i].route_name;
            if (currentRoutename == null){
                currentRoutename = 'Unnamed Route';
            }
            var currentRouteID = data[i].route_id;
            //add file to drop down list
            $(dropdownBoxID).append('<option value=\"' + currentRouteID + "\">" + currentRoutename + " ID(" + currentRouteID + ") </option>" );
            console.log("Route drop down list successfully populated with routes from db.");
        }
    },
    fail: function(error) {
        // Non-200 return, do something with error
        $('#errorMessageBar').html("On page load, received error from server");
        console.log(error); 
    }   
    });
}







// visibility stuff for query panel: when button clicked, show only the assocciated panel -----------------------------------

function displayAllRoutesButtonClicked(){
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "block";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
}


function displayRoutesFromFileButtonClicked(){
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display  routes from file panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "block";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
    //populate dropdown with filenames (from DB)
    populateDropdownWithFilesFromDB(routeFromDbDropdownRouteFromFile);
}

function displayAllPointsFromRouteButtonClicked(){
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all from routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "block";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
    //populate dropdown with routes (from DB)
    populateDropdownWithRoutesFromDB(routeFromDbDropdownPointsFromRoute);
}

function displayAllPointsFromFileButtonClicked(){
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "block";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "none";
    //populate dropdown with filenames (from DB)
    populateDropdownWithFilesFromDB(filesFromDbDropdownPointsFromFile);
}

function displayShortestLongestRoutesButtonClicked(){
    emptyTableFull(queryResultTable);
    document.getElementById("queryResultPanel").style.display = "none";
    //hide all panels other than display all routes panel
    document.getElementById("displayAllRoutesPanel").style.display = "none";
    document.getElementById("displayRoutesFromFilePanel").style.display = "none";
    document.getElementById("displayPointsFromRoutePanel").style.display = "none";
    document.getElementById("displayPointsFromFilePanel").style.display = "none";
    document.getElementById("displayShortestLongestRoutesPanel").style.display = "block";
    //populate dropdown with routes (from DB)
    populateDropdownWithFilesFromDB(filesFromDbDropdownPointsFromFileShortestLongest);
}


// query functions to populate result table -----------------------------------

function displayAllRoutesQueryFunction() {
    //check all selections have been made
    var sortBy;
    if  (document.getElementById('displayAllRoutesSortByName').checked){
        sortBy = 'route_name';
    } else if (document.getElementById('displayAllRoutesSortByLength').checked){
        sortBy = 'route_len';
    }
    //clear results table and populate headers
    emptyTableFull(queryResultTable);
    //add each file to file log panel
    $('#queryResultTable').append(
        '<tr>'
            + '<th> Name </th>'
            + '<th> Route Length </th>'
            + '<th> From File </th>'
        + '</tr>');
    document.getElementById('queryResultPanel').style.display = "block";

    console.log("DISPLAYING ALL ROUTES, sorted by " + sortBy );
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getAllRoutesFromROUTEtable',   //The server endpoint we are connecting to
        data: {
            sortBy: sortBy
        },
        success: function (data) {
            if (sortBy == 'route_name'){
                data.sort(function(a, b) {
                    var textA = a.route_name.toUpperCase();
                    var textB = b.route_name.toUpperCase();
                    return (textA < textB) ? -1 : (textA > textB) ? 1 : 0;
                    });
            }
            console.log(data);
            for (var i = 0; i < data.length; i++){
                var currentRoutename = data[i].route_name;
                var currentRouteLen = data[i].route_len;
                var currentFilename = data[i].file_name;

                $('#queryResultTable').append(
                    '<tr>'
                        + '<td>' + currentRoutename + '</td>'
                        + '<td>' + currentRouteLen+ '</td>'
                        + '<td>' + currentFilename + '</td>'
                    + '</tr>'); 
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}






function displayRoutesFromFileQueryFunction(){
    console.log("DISPLAYING ROUTES FROM FILE");
    //clear results table and populate headers
    emptyTableFull(queryResultTable);
    //get sort By selection
    var sortBy;
    if  (document.getElementById('displayRoutesSortByNameQuery2').checked){
        sortBy = 'route_name';
    } else if (document.getElementById('displayRoutesSortByLengthQuery2').checked){
        sortBy = 'route_len';
    }
    //get gpx_id from dropdown list --> gpx_id should be the value of selected option
    var gpx_idOfFileSelected = document.getElementById("routeFromDbDropdownRouteFromFile").value;
    console.log(gpx_idOfFileSelected);
    if (gpx_idOfFileSelected == "--Please choose a file--"){
        alert("Please choose one of the files from the drop down list.\n");
    } else {
        //add each file to file log panel
        $('#queryResultTable').append(
            '<tr>'
                + '<th> Name </th>'
                + '<th> Route Length </th>'
                + '<th> From File </th>'
            + '</tr>');

        document.getElementById('queryResultPanel').style.display = "block";

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getRoutesForSelectedFileFromROUTEtable',   //The server endpoint we are connecting to
            data: {
                sortBy: sortBy,
                gpx_id: gpx_idOfFileSelected
            },
            success: function (data) {
                console.log(data);
                var unnamedRouteNameCounter = 1;
                for (var i = 0; i < data.length; i++){
                    var currentRoutename = data[i].route_name;
                    if (currentRoutename == null ){
                        currentRoutename = 'Unnamed Route ' + unnamedRouteNameCounter;
                        unnamedRouteNameCounter++;
                    }
                    var currentRouteLen = data[i].route_len;
                    var currentFilename = data[i].file_name;

                    $('#queryResultTable').append(
                        '<tr>'
                            + '<td>' + currentRoutename + '</td>'
                            + '<td>' + currentRouteLen+ '</td>'
                            + '<td>' + currentFilename + '</td>'
                        + '</tr>'); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });
    }
}





function displayPointsFromRouteQueryFunction(){
    console.log("DISPLAYING POINTS FROM ROUTE");
    //clear results table and populate headers
    emptyTableFull(queryResultTable);
    //get route_id from dropdown list --> route_id should be the value of selected option
    var idOfRouteSelected = document.getElementById("routeFromDbDropdownPointsFromRoute").value;
    console.log(idOfRouteSelected);
    if (idOfRouteSelected == "--Please choose a route--"){
        alert("Please choose one of the routes from the drop down list.\n" 
                + " If none are visible, there may be no routes that exist in database");
    } else {
        //add each point ot query panel
        $('#queryResultTable').append(
            '<tr>'
                + '<th> Index ----- </th>'
                + '<th> Latitude ----- </th>'
                + '<th> Longitude ----- </th>'
                + '<th> Name of Point </th>'
            + '</tr>');
        document.getElementById('queryResultPanel').style.display = "block";

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getPointsOfSpecificRouteFromPOINTtable',   //The server endpoint we are connecting to
            data: {
                route_id: idOfRouteSelected
            },
            success: function (data) {
                console.log(data);
                var unnamedPointNameCounter = 1;
                for (var i = 0; i < data.length; i++){
                    var currentPointName = data[i].point_name;
                    if (currentPointName == null ){
                        currentPointName = 'Unnamed Point ' + unnamedPointNameCounter;
                        unnamedPointNameCounter++;
                    }

                    $('#queryResultTable').append(
                        '<tr>'
                            + '<td>' + data[i].point_index + '</td>'
                            + '<td>' + data[i].latitude + '</td>'
                            + '<td>' + data[i].longitude + '</td>'
                            + '<td>' + currentPointName + '</td>'
                        + '</tr>'); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });
    }
}   







function displayPointsFromFileQueryFunction(){
    console.log("DISPLAYING POINTS FROM FILE");
    //clear results table and populate headers
    emptyTableFull(queryResultTable);
    
    //get sort By selection
    var sortBy;
    if  (document.getElementById('displayPointsFromFileSortByRouteName').checked){
        sortBy = 'route_name';
    } else if (document.getElementById('displayAllRoutesSortByRouteLength').checked){
        sortBy = 'route_len';
    }

    //get gpx_id from dropdown list --> gpx_id should be the value of selected option
    var gpx_idOfFileSelected = document.getElementById("filesFromDbDropdownPointsFromFile").value;
    console.log(gpx_idOfFileSelected);
    if (gpx_idOfFileSelected == "--Please choose a file--"){
        alert("Please choose one of the files from the drop down list.\n");
    } else {
        // add each point ot query panel
        $('#queryResultTable').append(
            '<tr>'
                + '<th> Index ----- </th>'
                + '<th> Latitude ----- </th>'
                + '<th> Longitude ----- </th>'
                + '<th> Name of Point ----- </th>'
                + '<th> Name of Route </th>'
            + '</tr>');
        document.getElementById('queryResultPanel').style.display = "block";
        
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getPointsFromSelectedFile',   //The server endpoint we are connecting to
            data: {
                sortBy: sortBy,
                gpx_id: gpx_idOfFileSelected
            },
            success: function (data) {
                console.log(data);
                var unnamedPointNameCounter = 1;
                var unnamedRouteNameCounter = 0;
                var routeIDofLastUnnamedRoute = -1;
                for (var i = 0; i < data.length; i++){
                    var currentPointName = data[i].point_name;
                    if (currentPointName == null){
                        currentPointName = 'Unnamed Point ' + unnamedPointNameCounter;
                        unnamedPointNameCounter++;
                    }
                    var currentRouteName = data[i].route_name;
                    var currentRouteID = data[i].route_id;
                    // if still same null route as last 
                    if (currentRouteName == null && currentRouteID == routeIDofLastUnnamedRoute){
                        currentRouteName = 'Unnamed Route ' + unnamedRouteNameCounter;
                        routeIDofLastUnnamedRoute = currentRouteID;
                        console.log("meow1");
                        console.log(data[i]);
                    } 
                    // if new route (diff id), but also unnamed
                    else if (currentRouteName == null && currentRouteID != routeIDofLastUnnamedRoute){
                        unnamedRouteNameCounter++;
                        currentRouteName = 'Unnamed Route ' + unnamedRouteNameCounter ;
                        
                        routeIDofLastUnnamedRoute = currentRouteID;
                        console.log("meow2");
                        console.log(data[i]);
                    }

                    $('#queryResultTable').append(
                        '<tr>'
                            + '<td>' + data[i].point_index + '</td>'
                            + '<td>' + data[i].latitude + '</td>'
                            + '<td>' + data[i].longitude + '</td>'
                            + '<td>' + currentPointName + '</td>'
                            + '<td>' + currentRouteName + '</td>'
                        + '</tr>'); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        });
    }
}


function displayShortestLongestQueryFunction(){
    console.log("DISPLAYING N SHORTEST/LONGEST FROM FILE");
    emptyTableFull(queryResultTable);
    
    //get sort By selection
    var sortBy;
    if  (document.getElementById('displayShortestLongestSortByRoute').checked){
        sortBy = 'route_name';
    } else if (document.getElementById('displayShortestLongestSortByLength').checked){
        sortBy = 'route_len';
    }

    //get shortest/longest selection
    var shortLong;
    if  (document.getElementById('displayShortestRoutesRadio').checked){
        shortLong = 'asc';
    } else if (document.getElementById('displayLongestRoutesRadio').checked){
        shortLong = 'desc';
    }

    //get number of Routes to be displayed
    var numRoutesToDisplay = document.getElementById('numRoutesToDisplayShortLong').value;

    //get gpx_id from dropdown list --> gpx_id should be the value of selected option
    var gpx_idOfFileSelected = document.getElementById("filesFromDbDropdownPointsFromFileShortestLongest").value;
    console.log(gpx_idOfFileSelected);
    if (gpx_idOfFileSelected == "--Please choose a file--"){
        alert("Please choose one of the files from the drop down list.\n");
    } else {
        // add each point ot query panel
        $('#queryResultTable').append(
            '<tr>'
                + '<th> Route Name ----- </th>'
                + '<th> Route Length ----- </th>'
                + '<th> From File </th>'
            + '</tr>');
        document.getElementById('queryResultPanel').style.display = "block";  

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/getNShortestLongestRoutesFromFile',   //The server endpoint we are connecting to
            data: {
                sortBy: sortBy,
                shortLong: shortLong,
                gpx_id: gpx_idOfFileSelected,
                numRoutesToDisplay: numRoutesToDisplay
            },
            success: function (data) {
                console.log("back after ajax");
                console.log(data);
                for (var i = 0; i < data.length; i++){
                    
                    $('#queryResultTable').append(
                        '<tr>'
                            + '<td>' + data[i].route_name + '</td>'
                            + '<td>' + data[i].route_len + '</td>'
                            + '<td>' + data[i].file_name + '</td>'
                        + '</tr>'); 
                }
            },
            fail: function(error) {
                // Non-200 return, do something with error
                $('#errorMessageBar').html("On page load, received error from server");
                console.log(error); 
            }   
        }); 
    }
}





function renameComponent() {
    // if this is placeholder val, no components will exist
    var filename = document.getElementById("file-select-dropdown").value;
    
    //check a compnent is selected from drop down menu
    var componentName = document.getElementById("rename-dropdown-box").value;
    if (componentName == null || componentName == undefined || componentName == "--Please choose a component--"  ){
        alert("Error: No Component Has Been selected from dropdown menu.");
        return ;
    }
    //check new name field is not empty
    var newName = document.getElementById('newNameText').value;
    newName = newName.trim();
    if ( newName == null || newName == undefined || newName === ""){
        newName = "None";
    } 
    //parse component name into type and number
    var compObj = {};
    compObj = JSON.parse(componentName);
    console.log(compObj);
    console.log("RENAMING COMPONENT " + componentName + " to '" + newName + "'");
    //console.log("NAme: " + name +", Index: " + index );
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/renameComponentGPX',   //The server endpoint we are connecting to
        data: {
            type: compObj.type,
            index: compObj.index,
            filename: filename,
            newName: newName
        },
        success: function (data) {
            console.log("RENAME CALL RESONSE")
            console.log(data);
            if (data.successful == true){
                //display alert saying name changed successfull
                //alert("Successfully Renamed to '" + newName + "'!");
                //clear and refill gpx view table
                emptyTable(viewPanelTable);
                populateViewPanelTable(filename);
                $('#viewPanelTable').append('<tr> <td> No File Selected </td> </tr>' );
                
                if (compObj.type == 'rte'){
                    console.log("ATTEMPTING TO UPDATE ROUTE NAME IN DATABASE");
                    // modify row in table in data base
                    $.ajax({
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything 
                        url: '/renameComponentGPXinDB',   //The server endpoint we are connecting to
                        data: {
                            index: compObj.index,
                            filename: filename,
                            newName: newName
                        },
                        success: function (data) {
                            console.log("ATTEMPTING TO MODIFY IN DB TABLES")
                            console.log(data);
                            if (data.updateSuccessful == true){
                                alert("Route successfully Renamed to '" + newName + "'. \n"
                                        + " Route name in database is up to date");
                                    //clear rename textbox
                                    document.getElementById("newNameText").value = "";
                            } else {
                                alert("Route successfully Renamed to '" + newName + "'. \n" 
                                        + "Database not updated. Please ensure all uploaded files have been stored (ie 'Store All Routes') " );
                                //clear rename textbox
                                document.getElementById("newNameText").value = "";
                                }
                        },
                        fail: function(error) {
                            // Non-200 return, do something with error
                            $('#errorMessageBar').html("On page load, received error from server");
                            console.log(error); 
                        }   
                    });
                } else if (compObj.type == 'trk'){
                    alert("Track successfully Renamed to '" + newName + "'. \n"
                    + " Track names are not stored in database.");
                    //clear rename textbox
                    document.getElementById("newNameText").value = "";
                }
            }
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#errorMessageBar').html("On page load, received error from server");
            console.log(error); 
        }   
    });
}
    



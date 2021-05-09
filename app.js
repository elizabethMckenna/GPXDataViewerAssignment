'use strict'

// C library API
const ffi = require('ffi-napi');



// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { response } = require('express');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let gpxparser = ffi.Library('./parser/bin/libgpxparser', {
   'createGPXreturnJSON': [ 'string', [ 'string' , 'string' , 'string' , 'string'] ],
   'getRouteListArray' : [ 'string', [ 'string' , 'string' ]],
   'getTrackListArray' : [ 'string' ,[ 'string' , 'string' ]],
   'gpxFileToJSON' : [ 'string' , [ 'string' , 'string' , 'string' ] ],
   'tracksBetweenJSON' : [ 'string', [ 'string' , 'string' , 'string' , 'string' , 'string' , 'string' , 'string' ] ],
   'routesBetweenJSON' : [ 'string', [ 'string' , 'string' , 'string' , 'string' , 'string' , 'string' , 'string' ] ],
   'getRouteListArraywithWaypoints' : [ 'string', [ 'string' , 'string' ]],
   'renameRoute' : [ 'string' , [ 'string', 'string', 'int', 'string' ] ],
   'renameTrack' : [ 'string' , [ 'string', 'string', 'int', 'string' ] ]
  });



//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


app.get('/loadGPXFilesOnPageLoad', function ( req, res){
  //joining path of directory 
  const directoryPath = path.join(__dirname, 'uploads');
  let numValidFilesCount = 0;
  //passsing directoryPath and callback function
  fs.readdir(directoryPath, function (err, files) {
      //handling error
      if (err) {
          return console.log('Unable to scan directory: ' + err);
      } 
      let gpxArray = [];
      //listing all files using forEach
      for(let file of files) {
          // run file though 
          console.log("file:",file); 
          let gpxObj = JSON.parse(gpxparser.gpxFileToJSON(directoryPath + "/" + file, __dirname + "/gpx.xsd", file));
          //console.log(gpxparser.gpxFileToJSON(directoryPath + "/" + file));
          if (!(JSON.stringify(gpxObj) === '{}')){
            console.log("JSON:", JSON.stringify(gpxObj) );
            numValidFilesCount += 1;
            gpxArray.push(gpxObj);
          }
          else {
            console.log("File not valid.");
          }
      };
      console.log("\nnumValidFilesCount: " + numValidFilesCount);
      for (var i = 0; i < numValidFilesCount; i++){
        console.log(gpxArray[i]);
      }
      res.send(gpxArray);

  });
});




app.get('/getDataFromCreateGPXForm', function ( req, res){
  var version = req.query.version;
  var filename = req.query.filename;
  var creator = req.query.creatorName;
  var namespace = "http://www.topografix.com/GPX/1/1";

  const directoryPath = path.join(__dirname, 'uploads');
  var gpxArray = [];
  var gpxObj = {};

      console.log("Create GPX BUtton Pushed");
      console.log("WRITING FILE AT:" + directoryPath + "/" + filename + ".gpx");
      
      console.log("Version: " + version + 
                  "\nFilename: " + filename + 
                  "\nCreator: " + creator) + 
                  "\nNamespace: " + namespace;
      gpxObj = JSON.parse(gpxparser.createGPXreturnJSON(version, creator, directoryPath + "/" + filename + ".gpx", namespace));
      console.log("TEST1");
      console.log(JSON.stringify(gpxObj));
      gpxArray.push(gpxObj);
      console.log("after:");
      console.log(gpxObj);
  
    return res.send(gpxArray);
});


app.get('/populateViewPanelTable', function ( req, res){
  const directoryPath = path.join(__dirname, 'uploads');
  var filenameWithPath = directoryPath + "/" + req.query.filename;
  console.log("Full file name & path is:" + filenameWithPath + "\n");
  var fileDataArray = [];
  var routeListArray =[];
  var trackListArray = [];
  var fileObj = {};
  
  //populate route list
  routeListArray = JSON.parse(gpxparser.getRouteListArray(filenameWithPath, __dirname + "/gpx.xsd"));
  console.log("Routes - arraylength is: " + routeListArray.length);
  for (var i = 0; i < routeListArray.length; i++)
    console.log(routeListArray[i]);
  //populate tracklist
  trackListArray = JSON.parse(gpxparser.getTrackListArray(filenameWithPath, __dirname + "/gpx.xsd"));
  console.log("Tracks - arraylength is: " + trackListArray.length);
  for (var i = 0; i < trackListArray.length; i++)
    console.log(trackListArray[i]);

  fileObj.trackListArray = trackListArray;
  fileObj.routeListArray = routeListArray;

  fileDataArray.push(fileObj);

  res.send(fileDataArray);
});


app.get('/getDataFromAddRouteForm', function ( req, res){
  console.log("In getDataFromAddRouteForm app.get !!!");
  const directoryPath = path.join(__dirname, 'uploads');
  var filenameWithPath = directoryPath + "/" + req.query.filename;
  console.log("\n\nFull file name & path is:" + filenameWithPath + "\n\n");
});



app.get('/getDataFromPathBetweenForm', function ( req, res){
  console.log("In getDataFromPathBetweenForm app.get !!!");
  const directoryPath = path.join(__dirname, 'uploads');
  var filenameWithPath = directoryPath + "/" + req.query.filename;
  var sourceLat = req.query.sourceLat;
  var sourceLon = req.query.sourceLon;
  var destLat = req.query.destLat;
  var destLon = req.query.destLon;
  var delta = req.query.delta;

  var routesBetweenArray = [];
  var tracksBetweenArray = [];
  var pathObj = {};
  var routeListArray =[];
  var trackListArray = [];

  //populate route list
  routeListArray = JSON.parse(gpxparser.getRouteListArray(filenameWithPath, __dirname + "/gpx.xsd"));
  console.log("Routes - arraylength is: " + routeListArray.length);
  
  //populate tracklist
  trackListArray = JSON.parse(gpxparser.getTrackListArray(filenameWithPath, __dirname + "/gpx.xsd"));
  console.log("Tracks - arraylength is: " + trackListArray.length);
  
  //print the data to console
  console.log("\n\n -- TRACKS --");
  for (var i = 0; i < trackListArray.length; i++)
    console.log(trackListArray[i]);
  console.log(" -- ROUTES --");
  for (var i = 0; i < routeListArray.length; i++)
    console.log(routeListArray[i]);

  //get tracksBetween
  tracksBetweenArray = JSON.parse(gpxparser.tracksBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta));
  console.log("TracksBetween - arraylength is: " + tracksBetweenArray.length);
  console.log(" -- TRACKS BETWEEN --");

  console.log(JSON.stringify(gpxparser.tracksBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta)));
  if (tracksBetweenArray){
    for (var i = 0; i < tracksBetweenArray.length; i++)
    console.log(tracksBetweenArray[i]);
  }

  //get routesBetween
  routesBetweenArray = JSON.parse(gpxparser.routesBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta));
  console.log("RoutesBetween - arraylength is: " + routesBetweenArray.length);
  console.log(" -- ROUTES BETWEEN --");

  console.log(JSON.stringify(gpxparser.routesBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta)));
  if (routesBetweenArray){
    for (var i = 0; i < routesBetweenArray.length; i++)
    console.log(routesBetweenArray[i]);
  }

  pathObj.tracksBetweenArray = tracksBetweenArray;
  pathObj.routesBetweenArray = routesBetweenArray;

  res.send(pathObj);
  
});


/*
app.get('/getDataFromPathBetweenForm', function ( req, res){
  console.log("In getDataFromPathBetweenForm app.get !!!");
  const directoryPath = path.join(__dirname, 'uploads');
  
  var sourceLat = req.query.sourceLat;
  var sourceLon = req.query.sourceLon;
  var destLat = req.query.destLat;
  var destLon = req.query.destLon;
  var delta = req.query.delta;
  

  var routesBetweenArray = [];
  var tracksBetweenArray = [];
  //var pathsArray = [];
  var pathObj = {};
  pathObj.routesBetweenArray = routesBetweenArray;
  pathObj.tracksBetweenArray = tracksBetweenArray;

  var trackListStatsObj = {};

  var trkNameArray = [];
  trackListStatsObj.name = trkNameArray;
  var trkNumPtsArray = [];
  trackListStatsObj.numPoints = trkNumPtsArray;
  var trkLengthArray = [];
  trackListStatsObj.len = trkLengthArray;
  var trkLoopArray = [];
  trackListStatsObj.loop = trkLoopArray;

  var routeListArray =[];
  var trackListArray = [];


  fs.readdir(directoryPath, function (err, files) {
    
    //handling error
    if (err) {
        return console.log('Unable to scan directory: ' + err);
    } 

    //listing all files using forEach
    for(let file of files) {
      console.log("IN HERE!!!!!");
      var filenameWithPath = directoryPath + "/" + file;
      
      //populate route list
      routeListArray = JSON.parse(gpxparser.getRouteListArray(filenameWithPath, __dirname + "/gpx.xsd"));
      console.log("Routes - arraylength is: " + routeListArray.length);
      
      //populate tracklist
      trackListArray = JSON.parse(gpxparser.getTrackListArray(filenameWithPath, __dirname + "/gpx.xsd"));
      console.log("Tracks - arraylength is: " + trackListArray.length);
      
      //get tracksBetween
      tracksBetweenArray = JSON.parse(gpxparser.tracksBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta));
      console.log("TracksBetween - arraylength is: " + tracksBetweenArray.length);
      console.log(" -- TRACKS BETWEEN --");

      //console.log(JSON.stringify(gpxparser.tracksBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta)));
      if (tracksBetweenArray){
        for (var i = 0; i < tracksBetweenArray.length; i++)
        console.log(tracksBetweenArray[i]);
        
      }

      //get routesBetween
      routesBetweenArray = JSON.parse(gpxparser.routesBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta));
      console.log("RoutesBetween - arraylength is: " + routesBetweenArray.length);
      console.log(" -- ROUTES BETWEEN --");

      console.log(JSON.stringify(gpxparser.routesBetweenJSON(filenameWithPath, __dirname + "/gpx.xsd", sourceLat, sourceLon, destLat, destLon, delta)));
      if (routesBetweenArray){
        for (var i = 0; i < routesBetweenArray.length; i++)
        console.log(routesBetweenArray[i]);
      }

      pathObj.tracksBetweenArray.push(tracksBetweenArray);
      pathObj.routesBetweenArray.push(routesBetweenArray);

    };
    
    res.send(pathObj);
  
  });  
});
*/







// DATA BASE STUFF STARTS HERE

let dbConf = {
  host     : 'dursley.socs.uoguelph.ca',
  user     : '',
  password : '',
  database : ''
};
//let insRec = "INSERT INTO FILE (file_name, ver, creator) VALUES ('file1',1.1,'Kermit the Frog'),('file2',1.1,'Bob the Builder')";



app.get('/getDataFromDatabaseLoginForm', async( req, res) => {
  dbConf.user = req.query.username;
  dbConf.password = req.query.password;
  dbConf.database = req.query.databaseName;

  const mysql = require('mysql2/promise');
  
  let connection;
  var numFilesCount = 0;
  var numRoutesCount = 0;
  var numPointsCount = 0;

  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)

      // create FILE table
      connection.execute("create table if not exists FILE ( gpx_id int auto_increment," 
                                                        + " file_name varchar(60) not null,"
                                                        + " ver decimal(2,1) not null,"
                                                        + " creator varchar(256) not null, "
                                                        + " primary key(gpx_id) )");
      // create ROUTE table
      connection.execute("create table if not exists ROUTE ( route_id int auto_increment," 
                                                        + " route_name varchar(256),"
                                                        + " route_len float(15,7) not null,"
                                                        + " gpx_id int not null,"
                                                        + " primary key(route_id),"
                                                        + " foreign key (gpx_id) references FILE (gpx_id) on delete cascade )" );
      // create POINT table
      connection.execute("create table if not exists POINT ( point_id int auto_increment," 
                                                        + " point_index int not null,"
                                                        + " latitude decimal(11,7) not null,"
                                                        + " longitude decimal(11,7) not null,"
                                                        + " point_name varchar(256),"
                                                        + " route_id int not null,"
                                                        + " primary key(point_id),"
                                                        + " foreign key (route_id) references ROUTE (route_id) on delete cascade )" );
                  
      var [rowsFILEdb, fieldsFILEdb] = await connection.execute("select * from FILE");
      var [rowsROUTEdb, fieldsROUTEdb] = await connection.execute("select * from ROUTE");
      var [rowsPOINTdb, fieldsPOINTdb] = await connection.execute("select * from POINT");
     
      for (let row of rowsFILEdb){
        numFilesCount += 1;
      }
      for (let row of rowsROUTEdb){
        numRoutesCount += 1;
      }
      for (let row of rowsPOINTdb){
        numPointsCount += 1;
      }

      console.log("NUM ROWS IN FILE TABLE: " + numFilesCount);
      console.log("NUM ROWS IN ROUTE TABLE: " + numRoutesCount);
      console.log("NUM ROWS IN POINT TABLE: " + numPointsCount);
      
      //Populate the table
     // await connection.execute(insRec);

      //Run select query, wait for results
      //const [rows1, fields1] = await connection.execute('SELECT * from `student` ORDER BY `last_name`');

      /*console.log("\nSorted by last name:");
      for (let row of rows1){
          console.log("ID: "+row.id+" Last name: "+row.last_name+" First name: "+row.first_name+" mark: "+row.mark );
      }

      //Run select query, wait for results
      console.log("\nSorted by first name:");
      const [rows2, fields2] = await connection.execute('SELECT * from `student` ORDER BY `first_name`');
      for (let row of rows2){
          console.log("ID: "+row.id+" Last name: "+row.last_name+" First name: "+row.first_name+" mark: "+row.mark );
      }

      await connection.execute("DELETE FROM student");
      */
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          console.log("DataBase connection was successful");
         res.send(JSON.stringify({successful:true, numRowsFILEdb:numFilesCount, numRowsROUTEdb:numRoutesCount, numRowsPOINTdb:numPointsCount}));
        } else {
          console.log("Database connection was unsuccessful");
          res.send(JSON.stringify({successful:false, numRowsFILEdb:numFilesCount, numRowsROUTEdb:numRoutesCount, numRowsPOINTdb:numPointsCount}));
        }
    }
});





app.get('/clearAllDBTables', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)

      await connection.execute("delete from POINT");
      await connection.execute("delete from ROUTE");
      await connection.execute("delete from FILE");
      
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          console.log("Tables emptied successfully");
         res.send(JSON.stringify({successful:true}));
        } else {
          console.log("Oops... an issue occured emptying the tables");
          res.send(JSON.stringify({successful:false}));
        }
    }
});


app.get('/getDBTableLengths', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var numFilesCount = 0;
  var numRoutesCount = 0;
  var numPointsCount = 0;
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)

      var [rowsFILEdb, fieldsFILEdb] = await connection.execute("select * from FILE");
      var [rowsROUTEdb, fieldsROUTEdb] = await connection.execute("select * from ROUTE");
      var [rowsPOINTdb, fieldsPOINTdb] = await connection.execute("select * from POINT");
      
      for (let row of rowsFILEdb){
        numFilesCount += 1;
      }
      for (let row of rowsROUTEdb){
        numRoutesCount += 1;
      }
      for (let row of rowsPOINTdb){
        numPointsCount += 1;
      }
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          console.log("Table legths fetched successfully");
          res.send(JSON.stringify({successful:true, numRowsFILEdb:numFilesCount, numRowsROUTEdb:numRoutesCount, numRowsPOINTdb:numPointsCount}));
        } else {
          console.log("Oops... an issue occured fetching table lengths");
          res.send(JSON.stringify({successful:false, numRowsFILEdb:numFilesCount, numRowsROUTEdb:numRoutesCount, numRowsPOINTdb:numPointsCount}));
        }
    }
});





app.get('/storeAllFilesFILEtable', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  const directoryPath = path.join(__dirname, 'uploads');
  let numValidFilesCount = 0;
  let gpxArray = [];

  try{
    //passsing directoryPath and callback function
    fs.readdir(directoryPath, async (err, files) => {
      //handling error
      if (err) {
          return console.log('Unable to scan directory: ' + err);
      } 
      connection = await mysql.createConnection(dbConf)
      //listing all files using forEach
      for(let file of files) {
          // run file though 
          console.log("file:",file); 
          let gpxObj = JSON.parse(gpxparser.gpxFileToJSON(directoryPath + "/" + file, __dirname + "/gpx.xsd", file));
          //console.log(gpxparser.gpxFileToJSON(directoryPath + "/" + file));
          if (!(JSON.stringify(gpxObj) === '{}')){
            console.log("JSON:", JSON.stringify(gpxObj) );
            
            //query to see if file name already exists
            const [rows1, fields1] = await connection.execute("SELECT * from FILE WHERE file_name = '" + gpxObj.filename + "'");
            if (rows1.length == 0){
              console.log("RETRUN IS EMPTY... adding " + gpxObj.filename + " to FILE table");
              numValidFilesCount += 1;
              //add file to FILE table, add gpx_id to gpxObj info, then add gpxObject to array so knows which file to go through for routes
              await connection.execute("INSERT INTO FILE (file_name, ver, creator) "
                                      + "VALUES ('" + gpxObj.filename + "'," + gpxObj.version + ", '" + gpxObj.creator + "')");
              const [rows2, fields2] = await connection.execute("select gpx_id from FILE where file_name = '" + gpxObj.filename + "'");
              gpxObj.gpx_id = rows2[0].gpx_id;

              // ADD ALL ROUTES FROM FILE TO ROUTE TABLE
              var routeListArray =[];
              
              //populate route list
              routeListArray = JSON.parse(gpxparser.getRouteListArraywithWaypoints(directoryPath + "/" + gpxObj.filename, __dirname + "/gpx.xsd"));
              //add each route from list into ROUTE table
              for (var i = 0; i < routeListArray.length; i++){
                var waypointListArray = [];
                
                //go through string and replace all ' with ", then parse into object array for waypoints
                routeListArray[i].waypointList = routeListArray[i].waypointList.replace(/'/g, "\"");
                routeListArray[i].waypointList = JSON.parse(routeListArray[i].waypointList);
                console.log(waypointListArray);
                
                if (routeListArray[i].name == 'None'){
                  console.log("NO NAME FIELD");
                  await connection.execute("INSERT INTO ROUTE (route_name, route_len, gpx_id) "
                                      + "VALUES (NULL," + routeListArray[i].len + ", " + gpxObj.gpx_id + ")");
                } else {
                  console.log("NAME FIELD is " + routeListArray[i].name);
                  await connection.execute("INSERT INTO ROUTE (route_name, route_len, gpx_id) "
                  + "VALUES ('" + routeListArray[i].name + "'," + routeListArray[i].len + ", " + gpxObj.gpx_id + ")");
                }

                //loop through all the waypoints in the current route and add to POINT table in db
                for (var j = 0; j < routeListArray[i].waypointList.length; j++){
                  let [rows2, fields2] = await connection.execute("select route_id from ROUTE where route_id=(select max(route_id) from ROUTE)" );
                  var stringBack1 = JSON.stringify(rows2[0]);
                  var rowsBack = JSON.parse(stringBack1);
                  routeListArray[i].route_id = rowsBack.route_id;
                  if (routeListArray[i].waypointList[j].name == 'None'){
                    await connection.execute("INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) "
                    + "VALUES (" + (j+1) + ", " //point index 
                    + routeListArray[i].waypointList[j].lat + ", " //latitude
                    + routeListArray[i].waypointList[j].lon + ", "  //longitude
                    + "NULL,"   //point name
                    + routeListArray[i].route_id + ")" ); //route id
                  } else {
                    await connection.execute("INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) "
                    + "VALUES (" + (j+1) + ", " //point index 
                    + routeListArray[i].waypointList[j].lat + ", " //latitude
                    + routeListArray[i].waypointList[j].lon + ", '"  //longitude
                    + routeListArray[i].waypointList[j].name + "',"   //point name
                    + routeListArray[i].route_id + ")" ); //route id
                  }
                }
              }
              gpxObj.routeListArray = routeListArray;
            
              console.log(gpxObj);
              gpxArray.push(gpxObj);
            } else {
              console.log("File name " + gpxObj.filename + " matches a file already in table, so was not added to FILE table");
            }
          }
          else {
            console.log("File not valid.");
          }
      };

      console.log("\nnumValidFilesCount: " + numValidFilesCount);
      if (connection && connection.end) {
        connection.end();
        console.log("Stored all files successfully");
        //go through all files that were valid, and add them to FILE table is their name is unique
        console.log("printing gpxArray before sending it:");
        for (var i = 0; i < numValidFilesCount; i++){
          console.log("GPXARRAY at "+ i);
          console.log(gpxArray[i]);
        }
        res.send(gpxArray);
      } else {
        console.log("Oh no... an issue occured storing files");
        //go through all files that were valid, and add them to FILE table is their name is unique
        console.log("printing gpxArray before sending it:");
        for (var i = 0; i < numValidFilesCount; i++){
          console.log("GPXARRAY at "+ i);
          console.log(gpxArray[i]);
        }
        res.send(gpxArray);
      }
    });
  }catch(e){
      console.log("Query error: "+e);
  }
});







app.get('/getFilenamesAndIDsFromDB', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var fileArray = [];
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT file_name, gpx_id from FILE");
      // add all rows to filename array
      for (let row of rowsDB){
        var parsePt1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parsePt1);
        console.log("filename is: " );
        console.log(dataFromRow);
        fileArray.push(dataFromRow);
      }     
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(fileArray);
        } else {
          res.send(fileArray);
        }
    }
});


app.get('/getRouteNamesAndIDsFromDB', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var routeArray = [];
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT route_name, route_id from ROUTE");
      // add all rows to filename array
      for (let row of rowsDB){
        var parsePt1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parsePt1);
        console.log(dataFromRow);
        routeArray.push(dataFromRow);
      } 
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(routeArray);
        } else {
          res.send(routeArray);
        }
    }
});




app.get('/getAllRoutesFromROUTEtable', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var routeArray = [];
  var sortBy = req.query.sortBy;
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT * from ROUTE ORDER BY " + sortBy + " DESC");
      // add all rows to filename array
      var unnamedRouteNameCounter = 1;
      for (let row of rowsDB){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        var [rows2DB, fileds2DB] = await connection.execute("SELECT file_name from FILE WHERE gpx_id = " + dataFromRow.gpx_id );
        if (row.route_name == null){
          dataFromRow.route_name = 'Unnamed Route ' +unnamedRouteNameCounter;
          unnamedRouteNameCounter++;
        }

        var parse2 = JSON.stringify(rows2DB[0]);
        var dataFromFileTable = JSON.parse(parse2);
        dataFromRow.file_name = dataFromFileTable.file_name;

        console.log(dataFromRow);
        routeArray.push(dataFromRow);
      }
      //order routes by specified identifier
      if (req.query.sortBy == 'route_name'){
        routeArray.sort(function(a, b) {
          var textA = a.route_name.toUpperCase();
          var textB = b.route_name.toUpperCase();
          return (textA < textB) ? -1 : (textA > textB) ? 1 : 0;
        });
      } 
      else if ( req.query.sortBy == 'route_len'){
        routeArray.sort(function(a, b) {
          var lenA = a.route_len;
          var lenB = b.route_len;
          return lenA - lenB;
        });
      }
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(routeArray);
        } else {
          res.send(routeArray);
        }
    }
});





app.get('/getRoutesForSelectedFileFromROUTEtable', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var routeArray = [];
  var sortBy = req.query.sortBy;
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT * from ROUTE WHERE gpx_id= " + req.query.gpx_id + " ORDER BY " + sortBy );
      // add all rows to filename array
      for (let row of rowsDB){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        var [rows2DB, fileds2DB] = await connection.execute("SELECT file_name from FILE WHERE gpx_id = " + dataFromRow.gpx_id );

        var parse2 = JSON.stringify(rows2DB[0]);
        var dataFromFileTable = JSON.parse(parse2);
        dataFromRow.file_name = dataFromFileTable.file_name;
        
        console.log(dataFromRow);
        routeArray.push(dataFromRow);
      }
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(routeArray);
        } else {
          res.send(routeArray);
        }
    }
});




app.get('/getPointsOfSpecificRouteFromPOINTtable', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var pointArray = [];
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT * from POINT WHERE route_id= " + req.query.route_id + " ORDER BY point_index ASC");
      // add all rows to return array
      for (let row of rowsDB){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        
        console.log(dataFromRow);
        pointArray.push(dataFromRow);
      }
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(pointArray);
        } else {
          res.send(pointArray);
        }
    }
});





app.get('/getPointsFromSelectedFile', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var pointArray = [];
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      var [rowsDB, fieldsDB] = await connection.execute("SELECT route_name, route_len, ROUTE.route_id, point_index, point_name, latitude, longitude "
                                                        + "from ROUTE, POINT WHERE ROUTE.route_id = POINT.route_id  AND ROUTE.gpx_id = " 
                                                        + req.query.gpx_id + " ORDER BY " + req.query.sortBy + ", ROUTE.route_id, point_index");
      
      for (let row of rowsDB){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        
        console.log(dataFromRow);
        pointArray.push(dataFromRow);
      } 
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(pointArray);
        } else {
          res.send(pointArray);
        }
    }
});





app.get('/getNShortestLongestRoutesFromFile', async( req, res) => {
  const mysql = require('mysql2/promise');
  let connection;
  var routeArray = [];
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)


      var [rowsDB, fieldsDB] = await connection.execute("SELECT * from ROUTE"
                                                        + " WHERE gpx_id=" + req.query.gpx_id
                                                        + " ORDER BY route_len " + req.query.shortLong + ", route_id"
                                                        + " LIMIT " + req.query.numRoutesToDisplay );
 
      var unnamedCounter = 1;                                                
      for (let row of rowsDB){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        if (dataFromRow.route_name == null){
          dataFromRow.route_name = "Unnamed Route " + unnamedCounter;
          unnamedCounter++;
        }
        var [rows2DB, fileds2DB] = await connection.execute("SELECT file_name from FILE WHERE gpx_id = " + dataFromRow.gpx_id );
        
        var parse2 = JSON.stringify(rows2DB[0]);
        var dataFromFileTable = JSON.parse(parse2);
        dataFromRow.file_name = dataFromFileTable.file_name;
        
        console.log(dataFromRow);
        routeArray.push(dataFromRow);
      }
      
      //order routes by specified identifier
      if (req.query.sortBy == 'route_name'){
        routeArray.sort(function(a, b) {
          var textA = a.route_name.toUpperCase();
          var textB = b.route_name.toUpperCase();
          return (textA < textB) ? -1 : (textA > textB) ? 1 : 0;
        });
      } 
      else if ( req.query.sortBy == 'route_len' && req.query.shortLong == 'asc'){
        routeArray.sort(function(a, b) {
          var lenA = a.route_len;
          var lenB = b.route_len;
          return lenA - lenB;
        });
      }
      else if ( req.query.sortBy == 'route_len' && req.query.shortLong == 'desc'){
        routeArray.sort(function(a, b) {
          var lenA = a.route_len;
          var lenB = b.route_len;
          return lenB - lenA;
        });
      }
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send(routeArray);
        } else {
          res.send(routeArray);
        }
    }
});






app.get('/renameComponentGPX', function ( req, res){
  console.log("In renameComponentGPX app.get !!!");
  const directoryPath = path.join(__dirname, 'uploads');
  var filenameWithPath = directoryPath + "/" + req.query.filename;
  var type = req.query.type;
  var index = req.query.index;
  var newName = req.query.newName;

  if (req.query.type == 'rte'){
    var retVal = gpxparser.renameRoute( filenameWithPath, __dirname + "/gpx.xsd", index , newName);
  } else {
    var retVal = gpxparser.renameTrack(filenameWithPath, __dirname + "/gpx.xsd", index , newName );
  }
  res.send(retVal);
});


app.get('/renameComponentGPXinDB', async( req, res) => {
  console.log("\nLOOK AT ME!  IM AM IN HERE AHHHHHHHHHHH\n");
  const mysql = require('mysql2/promise');
  let connection;
  var routeArray = [];
 
  try{
      // create the connection
      connection = await mysql.createConnection(dbConf)
      //select all rows from 
      console.log("FAIL!");
      var [rowsDB1, fieldsDB1] = await connection.execute("SELECT gpx_id from FILE WHERE file_name= '" + req.query.filename + "'");
      // find number of rows returned:  if 0, file not in data base, so return failed
     console.log("TEST1");
      
      if ( rowsDB1[0] == undefined || rowsDB1[0] == null){
        res.send("{\"updateSuccessful\": false}");
      } 
      //otherwise, 
      var parse1 = JSON.stringify(rowsDB1[0]);
      var gpxIDObj = JSON.parse(parse1);
      var gpxID = gpxIDObj.gpx_id; 
      console.log("GPX_ID IS: " + gpxID);

      //select all rows from DB with gpxID
      var [rowsDB2, fieldsDB2] = await connection.execute("SELECT * from ROUTE"
                                  + " WHERE gpx_id= " + gpxID 
                                  + " ORDER BY route_id ASC");
      console.log("TEST2");
      for (let row of rowsDB2){
        var parse1 = JSON.stringify(row);
        var dataFromRow = JSON.parse(parse1);
        
        console.log(dataFromRow);
        routeArray.push(dataFromRow);
      }  
      
      // route id we want is route_id of route at 'index' of routeArray
      var routeID = routeArray[req.query.index].route_id;
      console.log("ROUTE ID IS: " + routeID);

      //now update the route_name of the route we have ID for
      await connection.execute("UPDATE ROUTE SET route_name= '" + req.query.newName + "' WHERE route_id =" + routeID);
      console.log("TEST3");
    }catch(e){
        console.log("Query error: "+e);
    }finally {
        if (connection && connection.end) {
          connection.end();
          res.send("{\"updateSuccessful\": true}");
        } else {
          res.send("{\"updateSuccessful\": false}");
        }
    }
});
var dqid = 1;                // selected DQset
var refDQsets = [];
var selectedRefDQ = 0;

var subdetFlagsArray = new Array();
var subdetParamsArray = new Array();

function subdetFlags(subdet, flag){
  this.subdet = subdet;
  this.flag = flag;
}

function algStruct(algname,value,low2,low,high,high2,flag){
  this.name = algname;
  this.value = value;
  this.low2 = low2;
  this.low = low;
  this.high = high;
  this.high2 = high2;
  this.flag = flag;
}

function parStruct(param, flag, obj, overlap, drawoption, algdata){
  this.parameter = param;
  this.flag = flag;
  this.obj = obj;
  this.overlap = overlap;
  this.drawoption = drawoption;
  this.algdata = algdata;
}

function subdetStruct(subdet, flag, pardata){
  this.subdet = subdet;
  this.flag = flag;
  this.pardata = pardata;
}

function loadDoc() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      createTable(this);
      }
  };
  xhttp.open("GET", "runs/dqsets.xml", true);
  xhttp.send();
}


function createTable(xml) {
  var i;
  var xmlDoc = xml.responseXML;
  var table="<tr><th>DQID</th><th>Sim or Data</th><th>Start Date</th><th>Start Time</th><th>End Date</th><th>End time</th><th>TAG</th></tr>";
  var x = xmlDoc.getElementsByTagName("DQID");
  console.log(x.length);
  for (i = 0; i <x.length; i++) { 
    table += "<tr><td>" +
      "<a href=\"lzdqmainframes.html?dqset=" +
      x[i].attributes.getNamedItem("id").nodeValue +
      "\">" +
      x[i].attributes.getNamedItem("id").nodeValue +
      "</a>" +
      "</td><td>" +
      x[i].getElementsByTagName("SIMDATA")[0].childNodes[0].nodeValue +
      "</td><td>" +
      x[i].getElementsByTagName("STARTDAY")[0].childNodes[0].nodeValue +
      "</td><td>" +
      x[i].getElementsByTagName("STARTTIME")[0].childNodes[0].nodeValue +
      "</td><td>" +
      x[i].getElementsByTagName("ENDDAY")[0].childNodes[0].nodeValue +
      "</td><td>" +
      x[i].getElementsByTagName("ENDTIME")[0].childNodes[0].nodeValue +
      "</td><td>" +
      x[i].getElementsByTagName("TAG")[0].childNodes[0].nodeValue +
      "</td></tr>";
  }
  document.getElementById("dqlist").innerHTML = table;
}


function loadRefDQfromXML() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      createSelect(this);
      }
  };
  xhttp.open("GET", "runs/refdq.xml", true);
  xhttp.send();
}

function createSelect(xml) {
  var i;
  var xmlDoc = xml.responseXML;
  var x = xmlDoc.getElementsByTagName("DQID");
  console.log(x.length);

  var z = document.createElement("SELECT");
  z.setAttribute("id", "selectDQ");
  z.setAttribute("onchange", "getSelect()");
  //  z.setAttribute("onchange", "myFunction()");
  document.getElementById("changeRef").appendChild(z);

  var findNext = 1;

  for (i = 0; i <x.length; i++) { 
    var option = document.createElement("option");
    option.text = x[i].attributes.getNamedItem("id").nodeValue;
    refDQsets.push(x[i].attributes.getNamedItem("id").nodeValue);
    if (findNext == 1) {
      selectedRefDQ = x[i].attributes.getNamedItem("id").nodeValue;
      if (selectedRefDQ != dqid) findNext = 0;
    }
    z.add(option);
    console.log(option);
  }
  document.getElementById("refDQset").innerHTML = selectedRefDQ;
}

function getSelect() {
  var x = document.getElementById("selectDQ").selectedIndex;
  selectedRefDQ = refDQsets[x];
  //  document.getElementById("demo1").innerHTML = x;
  console.log(x);
  document.getElementById("refDQset").innerHTML = selectedRefDQ;
}

function findDQID(){
  var urlParams = new URLSearchParams(location.search);      
  urlParams.has('dqset');  // true
  dqid = urlParams.get('dqset');    // 1234
  document.getElementById("dqid").innerHTML = dqid;
  //  document.write("hello 1");
  //  document.write(runNumber);
  fillMeta();
  
}

function fillMeta(){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      readMeta(this);
    }
  };
  var metafile = "runs/"+dqid+"/dqsetmeta.xml";
  xhttp.open("GET", metafile, true);
  xhttp.send();
}

function readMeta(xml) {
  var i1, i, ii, iii;
  var xmlDoc = xml.responseXML;
  var x = xmlDoc.getElementsByTagName("SUBDET");
  console.log("x.length: "+x.length);
  for (i = 0; i <x.length; i++) {
    var subdet = x[i].attributes.getNamedItem("id").nodeValue;
    var subdetFlag = x[i].attributes.getNamedItem("flag").nodeValue;
    console.log(subdet+": "+subdetFlag);
  }  
  subdetFlagsArray.length = 0;
  subdetParamsArray.length = 0;

  for (i1 = 0; i1 <x.length; i1++) {
    var subdet = x[i1].attributes.getNamedItem("id").nodeValue;
    var subdetFlag = x[i1].attributes.getNamedItem("flag").nodeValue;
    console.log(subdet+": "+subdetFlag);
    var substruct = new subdetFlags(subdet,subdetFlag);
    subdetFlagsArray.push(substruct);
    var xPars = x[i1].childNodes;
    console.log(xPars.length);
    var pardata = new Array();
    for (i = 0; i <xPars.length; i++) {
      if (xPars[i].nodeType == 1){
	//	console.log("1 ("+i+"):"+xPars[i].nodeValue);
	//	console.log("1 ("+i+"):"+xPars[i].nodeName);
	//	console.log("1 ("+i+"):"+xPars[i].nodeType);
	var parid = xPars[i].attributes.getNamedItem("id").nodeValue;
	//	console.log("1 ("+i+"):"+parid);      
	var parflag = xPars[i].attributes.getNamedItem("flag").nodeValue;
	//	console.log("1 ("+i+"):"+parid+" : "+parflag);
	var parobj =  xPars[i].attributes.getNamedItem("obj").nodeValue;
	var parover =  xPars[i].attributes.getNamedItem("overlap").nodeValue;
	var pardraw =  xPars[i].attributes.getNamedItem("drawoption").nodeValue;
	console.log(parid+":"+parflag+":"+parobj+":"+parover+":"+pardraw);
	var algdata = new Array();
	var xAlgs = xPars[i].childNodes;
	//       console.log("1 ("+i+"):"+xAlgs.length);
	for (ii = 0; ii <xAlgs.length; ii++) {
	  // 	console.log("2 ("+ii+"):"+xAlgs[ii].nodeValue);
	  // 	console.log("2 ("+ii+"):"+xAlgs[ii].nodeName);
	  // 	console.log("2 ("+ii+"):"+xAlgs[ii].nodeType);
	  if (xAlgs[ii].nodeType == 1){
	    //	    console.log("2 ("+ii+"):"+xAlgs[ii].attributes.getNamedItem("name").nodeValue);
	    var xAlgNodes = xAlgs[ii].childNodes;
	    //  	  console.log("2 ("+ii+"):"+xAlgNodes.length);
	    var algname, val, low2, low, high, high2, flag;
	    algname = xAlgs[ii].attributes.getNamedItem("name").nodeValue;
	    for (iii = 0; iii <xAlgNodes.length; iii++) {
	      if (xAlgNodes[iii].nodeType == 1){
		//		console.log("3 ("+ii+"):"+xAlgNodes[iii].nodeName);
		//		console.log("3 ("+ii+"):"+xAlgNodes[iii].childNodes[0].nodeValue);    
		//		console.log("3 ("+ii+"):"+xAlgNodes[iii].nodeType);    
		if (xAlgNodes[iii].nodeName == 'VALUE') val = xAlgNodes[iii].childNodes[0].nodeValue;
		if (xAlgNodes[iii].nodeName == 'LOW2') low2 = xAlgNodes[iii].childNodes[0].nodeValue;
		if (xAlgNodes[iii].nodeName == 'LOW') low = xAlgNodes[iii].childNodes[0].nodeValue;
		if (xAlgNodes[iii].nodeName == 'HIGH') high = xAlgNodes[iii].childNodes[0].nodeValue;
		if (xAlgNodes[iii].nodeName == 'HIGH2') high2 = xAlgNodes[iii].childNodes[0].nodeValue;
		if (xAlgNodes[iii].nodeName == 'FLAG') flag = xAlgNodes[iii].childNodes[0].nodeValue;
	      }
	    }
	    var algstr = new algStruct(algname,val,low2,low,high,high2,flag);
	    algdata.push(algstr);
	  }
	}
	var parstr = new parStruct(parid, parflag, parobj, parover, pardraw, algdata);
	pardata.push(parstr);
      }
    }
    subdetParamsArray.push(new subdetStruct(subdet, subdetFlag, pardata));
  } 
  console.log("read:" + subdetFlagsArray.length);
  createNavdiv();
}

function createNavdiv(){
  var navDivString="<h2>Subsystems</h2> <div class=\"tab\">";
  var i;
  for (i = 0; i <subdetFlagsArray.length; i++) { 
    var subdet = subdetFlagsArray[i].subdet;
    var flag =  subdetFlagsArray[i].flag;
    console.log("in createNavdiv: "+subdet+":"+flag);
    navDivString += "<button class=\"tablinks "+flag+"\" onclick=\"openSubdet(event, '"+subdet+"')\">"+subdet+" ("+flag+")"+"</button>";
  }
  navDivString += "</div>";
  document.getElementById("navdiv").innerHTML = navDivString;     
}



function createSubdetPanel(subdet){
  var displayDivString="<div style=\"text-align:center\"><h2>LZ "+subdet+"</h2></div>";
  var i, ii;
  
  for (i=0; i<subdetParamsArray.length; i++){
    if (subdet == subdetParamsArray[i].subdet){
      var paramArray = subdetParamsArray[i].pardata;
      for (ii = 0; ii <paramArray.length; ii++) { 
	var param = paramArray[ii].parameter;
	var flag =  paramArray[ii].flag;
	console.log(param+" : "+flag);
	displayDivString += "<div class=\"floatinglinks "+flag+"\"><img src=\"http://hep.ph.liv.ac.uk/~burdin/jsroot564/../lzdqmoffline/runs/"+dqid+"/"+subdet+"."+param+".png\" height=\"200\" width=\"236\" alt=\""+subdet+"."+param+".png\" onclick=\"imgClick('"+subdet+"','"+param+"')\"></div>";
      }
    }
  }
  document.getElementById("displaydiv").innerHTML = displayDivString;     
}

function openSubdet(evt, subdet){
  console.log("openSubdet: "+subdet);
  createSubdetPanel(subdet);
  var tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }
  evt.currentTarget.className += " active";
}

function imgClick(subdet, param){
  console.log("in imgClick");
  // subdetParamsArray.push(new subdetParams(subdet, parid, parflag, algdata));
 console.log(subdetParamsArray.length);

 var i, ii, iii;
 var algtablestart = "<table class=\"algres\" style=\"100%\"><tr><th>Algorithm</th><th colspan=\"6\">Results</th></tr><tr><td>name</td><td class=\"red\">Low2</td><td class=\"yellow\">Low</td><td><b>Value</b></td><td class=\"yellow\">High</td><td class=\"red\">High2</td><td>Flag</td></tr><tr><td></td><td></td><td></td><td></td><td></td><td></td><td></td></tr>";

 var algstring = "";
 var totalflag;
 var inobj, overlap, drawoption;
 for (i=0; i<subdetParamsArray.length; i++){
   if (subdet == subdetParamsArray[i].subdet){
     var paramArray = subdetParamsArray[i].pardata;
     for (ii = 0; ii <paramArray.length; ii++) { 
       var flag =  paramArray[ii].flag;
       if (param == paramArray[ii].parameter){
	 console.log("par:"+paramArray[ii].parameter);
	 totalflag = paramArray[ii].flag;
	 inobj = paramArray[ii].object;
	 overlap = paramArray[ii].overlap;
	 drawoption = paramArray[ii].drawoption;
	 console.log("algdata.length:"+paramArray[ii].algdata.length);
	 for (iii=0; iii<paramArray[ii].algdata.length; iii++){
	   algstring += "<tr><td>"+ paramArray[ii].algdata[iii].name + 
	     "</td><td class=\"red\">" + paramArray[ii].algdata[iii].low2 + 
	     "</td><td class=\"yellow\">"+paramArray[ii].algdata[iii].low +
	     "</td><td><b>"+paramArray[ii].algdata[iii].value +
	     "</b></td><td class=\"yellow\">"+paramArray[ii].algdata[iii].high +
	     "</td><td class=\"red\">" + paramArray[ii].algdata[iii].high2 + 
	     "</td><td class=\"" + paramArray[ii].algdata[iii].flag + "\">" + paramArray[ii].algdata[iii].flag + "</td></tr>";
	 }
       }
     }
   }
 }

 console.log(algstring);
 //function algStruct(algname,value,low2,low,high,high2,flag){
 
 var algtableend = "<tr><td>Overall</td><td></td><td></td><td></td><td></td><td></td><td class=\""+totalflag+"\">"+totalflag+"</td></tr></table></td></tr></tbody></table>";
 
 var algtablestr = algtablestart + algstring + algtableend;
 
 var imageString="";
 

 imageString = "<div style=\"text-align:center\"><h2>"+subdet+"."+param+"</h2></div><table width=\"800\"></table><br><table width=\"800\" height=\"700\" cellpadding=\"10\"><tbody><tr><td valign=\"top\"><div id=\"drawroot\" style=\"width:650px; height:500px\"></div></td><td valign=\"top\">";

 //<iframe width=\"600\" height=\"500\" src='http://hep.ph.liv.ac.uk/~burdin/jsroot564/?files=[../lzdqmoffline/runs/"+dqid+"/"+dqid+".root]&item="+dqid+".root/"+subdet+"_"+param+";1&nobrowser&opt=plc'></iframe>

 var displayDivString = imageString+algtablestr;
 
 document.getElementById("displaydiv").innerHTML = displayDivString;
 //  console.log(displayDivString);

 var filename = "runs/"+dqid+"/"+dqid+".root";
 var hobj = subdet+"_"+param+";1";
 
 // var leg = JSROOT.Create("TLegend");
 // JSROOT.extend(leg, { fX1NDC: 0.2, fY1NDC:0.75, fX2NDC: 0.6, fY2NDC:0.9 });
 var drawopt = "";
 var drawoptref = "";
 if (drawoption == "default"){
   drawopt = "line_red,plc";
   drawoptref = "hist,line_blue,nostat,same";
 } else {
   drawopt = drawoption;
   if (inobj != "h1") drawopt+=",nostat";
   drawoptref = drawoption+",nostat,same";
 }
 
 JSROOT.OpenFile(filename, function(rootfile) {
		   rootfile.ReadObject(hobj, function(obj) {
					 obj.fName = dqid+":"+obj.fName;
					 JSROOT.draw("drawroot", obj, drawopt);
					 //leg.fPrimitives.Add(CreateLegendEntry(obj, "DQset "+dqid));
				       });
		 });
 console.log("overlap="+overlap);
 if (overlap == "yes"){
   var filenameref = "runs/"+selectedRefDQ+"/"+selectedRefDQ+".root";
   
   JSROOT.OpenFile(filenameref, function(rootfile) {
		     rootfile.ReadObject(hobj, function(obj) {
					   console.log("title:"+obj.fTitle);
					   console.log("name:"+obj.fName);
					   obj.fName = selectedRefDQ+":"+obj.fName;
					   JSROOT.draw("drawroot", obj, drawoptref);
					   //leg.fPrimitives.Add(CreateLegendEntry(obj, "DQset "+selectedRefDQ));
					 });
		   });
 }
 // JSROOT.draw("drawroot",leg);
 
}

function CreateLegendEntry(obj, lbl) {
  var entry = JSROOT.Create("TLegendEntry"); 
  entry.fObject = obj;
  entry.fLabel = lbl;
  entry.fOption = "l";
  return entry; 
}

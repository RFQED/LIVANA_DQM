#include "TXMLEngine.h"
#include <cstring>

void readaddxmldq(const char* file1 = "example.xml", const char* file2 = "example1.xml", const char* output = "example2.xml")
{
   // First create engine
   TXMLEngine* xml = new TXMLEngine;
   // Now try to parse xml file
   // Only file with restricted xml syntax are supported
   XMLDocPointer_t xmldoc1 = xml->ParseFile(file1);
   if (xmldoc1==0) {
      delete xml;
      return;
   }
   // take access to main node
   XMLDocPointer_t xmldoc2 = xml->ParseFile(file2);
   if (xmldoc2==0) {
      delete xml;
      return;
   }
   // take access to main node
   XMLNodePointer_t mainnode1 = xml->DocGetRootElement(xmldoc1);
   // display recursively all nodes and subnodes
   printf("node: %s\n",xml->GetNodeName(mainnode1));

   XMLNodePointer_t mainnode2 = xml->DocGetRootElement(xmldoc2);
   // display recursively all nodes and subnodes

   XMLAttrPointer_t attr = xml->GetFirstAttr(mainnode2);

   printf("node: %s, attr: %s, value: %s\n",xml->GetNodeName(mainnode2),xml->GetAttrName(attr),xml->GetAttrValue(attr));

   if (strncmp(xml->GetNodeName(mainnode2),"DQID",4) == 0){
     XMLNodePointer_t dqid = xml->NewChild(mainnode1, 0, "DQID");
     xml->NewAttr(dqid, 0, "id",xml->GetAttrValue(attr));
     
     XMLNodePointer_t meta = xml->GetChild(mainnode2);
     while (meta!=0) {
       printf("node: %s\n",xml->GetNodeName(meta));
       if (strncmp(xml->GetNodeName(meta),"META",4) == 0){
	 printf("adding node: %s\n",xml->GetNodeName(meta));
	 XMLNodePointer_t metaadded = xml->NewChild(dqid, 0, "META");
	 XMLNodePointer_t metachild = xml->GetChild(meta);
	 printf("adding meta node: %s\n",xml->GetNodeName(metachild));
	 xml->AddChild(metaadded,metachild);
	 //	 break;
       }
       if (strncmp(xml->GetNodeName(meta),"SUBDET",6) == 0){
	 printf("snode: %s\n",xml->GetNodeName(meta));
	 //	 break;
       }
       meta = xml->GetNext(meta);
     }
   }
      
   //   xml->AddChild(mainnode1,mainnode2);

   // while (dqset != 0){
   //   xml->AddChild(mainnode1,dqset);
   //   dqset = xml->GetNext(mainnode2);
   // }

   XMLDocPointer_t xmldoc3 = xml->NewDoc();
   xml->DocSetRootElement(xmldoc3, mainnode1);
   // // Save document to file
   xml->SaveDoc(xmldoc3, output);

//     xml->FreeDoc(xmldoc1);
//     xml->FreeDoc(xmldoc2);
//    xml->FreeDoc(xmldoc3);
   delete xml;
}

   // // this function display all accessible information about xml node and its children
   // printf("%*c node: %s\n",level,' ', xml->GetNodeName(node));
   // // display namespace
   // XMLNsPointer_t ns = xml->GetNS(node);
   // if (ns!=0)
   //    printf("%*c namespace: %s refer: %s\n",level+2,' ', xml->GetNSName(ns), xml->GetNSReference(ns));
   // // display attributes
   // XMLAttrPointer_t attr = xml->GetFirstAttr(node);
   // while (attr!=0) {
   //     printf("%*c attr: %s value: %s\n",level+2,' ', xml->GetAttrName(attr), xml->GetAttrValue(attr));
   //     attr = xml->GetNextAttr(attr);
   // }
   // // display content (if exists)
   // const char* content = xml->GetNodeContent(node);
   // if (content!=0)
   //    printf("%*c cont: %s\n",level+2,' ', content);
   // // display all child nodes
   // XMLNodePointer_t child = xml->GetChild(node);
   // while (child!=0) {
   //    DisplayNode(xml, child, level+2);
   //    child = xml->GetNext(child);
   // }

// Dataparser.cpp
// Author: Matt Barnett
// Update as of 20/02/09: Code finds all instances of a specified string. Then copies out the data below that to a vector.

#include <TUCNDataParser.h>

#include <cassert>    // Includes assert(v == u)
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>


using namespace std;

TUCNDataParser::TUCNDataParser()
{
	// -- Constructor
	fNumberOfLines = 0;
	fFileName = "";
}


//   *-*-*-*-*-*-*-*-*-*-*-*-*-*-METHODS-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//								  			=======


bool TUCNDataParser::LoadFile(string filename)
{
	// Read in the File and store each line in a vector<string> while recording the number of lines. 
	fFileName = filename;

	ifstream in;
  	in.open(fFileName.data());
	if ( !in.is_open() ) {
	    cout << "TUCNDataParser::Constructor: could not open file: " << fFileName << endl;
	    cout << endl;
	    return false;
	}
	cout << "Opened " << fFileName << endl;
	
	string line;	// Temp storage where each line from instream is read to. 
	for (unsigned int linecount = 1 ; getline(in , line) ; linecount++) {
		fFileLines.push_back(line);
		fNumberOfLines = linecount;
	}
	cout << "Number of Lines: " << fNumberOfLines << endl;
	
	return true;
}

//______________________________________________________________________________
bool TUCNDataParser::GetData(vector<string>& data, string searchstring) 
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This is what we are searching the file for.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	fSearchString = searchstring;	
	size_t sizesearchstring = fSearchString.size();
//	cout << "String to be searched for: " << fSearchString << endl;
//	cout << "String size: " << sizesearchstring << endl << endl;
		
	vector<unsigned int> instancelinenumber;  // For each instance a vector stores line number, then number of tabs before instance
	vector<unsigned int> instancetabnumber;
		
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This loop writes every line of the file to a string array. It also searches each line for a specific string and if found 
	// stores its position in the line and the number of tabs before it. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (unsigned int linecount = 0 ; linecount < fNumberOfLines ; linecount++) {		
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Search line to find ALL instances of string. If found write out the position.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		
		size_t position = 0;		
		while (position != string::npos) {
			position = fFileLines[linecount].find(fSearchString, position);
			
			if (position != string::npos) {
				instancelinenumber.push_back(linecount);  // Storing the line number where an instance of our searchstring was found
//				cout << "Line Number: " << linecount << endl;
//				size_t linesize = fFileLines[linecount].size();
//				cout << "Length of Line: " << linesize << endl;
//				cout << "Position in line of the first character: " << position << endl;
		
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// Count the number of tabs before the string we we just found
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				unsigned int tabcounter = 0;
				size_t tabposition = fFileLines[linecount].find("\t", 0);
			
				if (tabposition != string::npos) { // npos is a static constant member of the string class that represents a nonexistent character position
//					cout << "Tabs before string positioned at: ";
					while (tabposition != string::npos && tabposition < position) { //step through line until no more tabs are found or we reach the string
						tabcounter++;
//						cout << tabposition << ", ";  // Remember a string position counts from 0, so first character is at 0
						tabposition = fFileLines[linecount].find( "\t" , ++tabposition); // Look for next tab after this one
					} 
					instancetabnumber.push_back(tabcounter); // Storing the number of tabs before the instance of our searchstring
//					cout << endl << "No. of tabs before point: " << tabcounter << endl << endl;
				} else { 
					cout << "Error: NO tabs in line!" << endl << endl; 
				}
				
				position = position + sizesearchstring; // Move position to the end of the string we just searched for. 			
			}
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Check the number of instances, write out their data
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	assert( instancelinenumber.size()  == instancetabnumber.size() );
//	cout << "Number of instances found: " << instancelinenumber.size() << endl;
	
//	for (unsigned int i = 0 ; i < instancelinenumber.size() ; i++) {
//		cout << "Instance Line Number: " << instancelinenumber[i] << "\t" <<  "No. of tabs before Instance: " << instancetabnumber[i] << endl;
//	}
		
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Go to line after where instance was found. Move to start of instance using number of previously determined number of tabs before instance. 
	// Find position of the next tab after the instance. Copy characters between these two positions into vector
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	// Loop over the number of instances of the search string we found; This should eventually always be one.
	for (unsigned int j = 0; j < instancelinenumber.size(); j++) { 	
		// Loop over the number of rows/lines in our column, starting from the line right after the one we just found the column title in
		for (unsigned int linecount = ++instancelinenumber[j]; linecount < fNumberOfLines; linecount++) {
			// Loop over the number of tabs until we reach the start of column. Store the position of this column in firsttabposition. 
			size_t firsttabposition = 0;
			for (unsigned int tabcounter = 0; tabcounter < instancetabnumber[j] ; tabcounter++) {
				firsttabposition = fFileLines[linecount].find("\t", ++firsttabposition);
			}
			
			// Store the position of the next tab after the data in nexttabposition. Check that nexttabposition > firsttabposition
			size_t nexttabposition = fFileLines[linecount].find("\t", ++firsttabposition);
			assert(nexttabposition > firsttabposition);

			// Here we store in the string vector column value the characters between the tab position in "instancetabposition", 
			// and the next tab position stored in "position" from the line given by "instancelinenumber".
			data.push_back(fFileLines[linecount].substr(firsttabposition, nexttabposition - firsttabposition)); 
		}
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Write out the data we just found
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
//	cout << endl << endl << "Data Copied: " << data[0] << endl;
//	cout << "Number of rows of Data copied: " << data.size() << endl ;
//	for (unsigned int i = 0 ; i < data.size() ; i++) {
//		cout << i << ": " << data[i] << endl;
//	}
			
	return true;
} 	

//______________________________________________________________________________
bool TUCNDataParser::ConvertVectorInt(vector<string>& data, vector<unsigned int>& int_data)
{
	for (unsigned int i = 0; i < data.size(); i++) {   
		istringstream ss(data[i]); 		// Using istringstream to convert string to type int
		unsigned int value;
		ss >> value;	
		if (ss.fail()) {
			int_data.push_back(0);
//	   	cout << "Failed to convert \"" << data[i] << "\"" << endl;
			return false;
	 	} else {
			int_data.push_back(value);
//			cout << "String:" << "\t" << data[i] << "\t" << "Integer:" << "\t" << int_data[i] << endl;
	 	}
	}
	return true;
}

//______________________________________________________________________________
bool TUCNDataParser::ConvertVectorDouble(vector<string>& data, vector<double>& double_data)
{
	for (unsigned int i = 0; i < data.size(); i++) {   
		istringstream ss(data[i]); 		// Using istringstream to convert string to type int
		double value;
		ss >> value;	
		if (ss.fail()) {
			double_data.push_back(0);
	   	cout << "Failed to convert \"" << data[i] << "\"" << endl;
			return false;
	 	} else {
			double_data.push_back(value);
//			cout << "String:" << "\t" << data[i] << "\t" << "Double:" << "\t" << double_data[i] << endl;
	 	}
	}
	return true;
}

















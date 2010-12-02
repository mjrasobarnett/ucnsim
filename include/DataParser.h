#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <string>
#include <vector>

using namespace std;

class DataParser  {
	private:
		unsigned int fNumberOfLines;
		string fFileName;
		string fSearchString;
		vector<string> fFileLines;
		
		
	public:
		DataParser();
		virtual ~DataParser(void) {};
		
		// -- Methods
		bool LoadFile(string filename);
		bool GetData(vector<string>& data, string searchstring);
		bool ConvertVectorInt(vector<string>& data, vector<unsigned int>& int_data);
		bool ConvertVectorDouble(vector<string>& data, vector<double>& double_data);
		
};
#endif

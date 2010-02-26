#ifndef TUCNDATAPARSER_H
#define TUCNDATAPARSER_H

#include <string>
#include <vector>

using namespace std;

class TUCNDataParser  {
	private:
		unsigned int fNumberOfLines;
		string fFileName;
		string fSearchString;
		vector<string> fFileLines;
		
		
	public:
		TUCNDataParser();
		virtual ~TUCNDataParser(void) {};
		
		// -- Methods
		bool LoadFile(string filename);
		bool GetData(vector<string>& data, string searchstring);
		bool ConvertVectorInt(vector<string>& data, vector<unsigned int>& int_data);
		bool ConvertVectorDouble(vector<string>& data, vector<double>& double_data);
		
};
#endif

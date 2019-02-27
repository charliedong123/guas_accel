#include <iostream>
#include <fstream>
using namespace std;

int main()
{
    float tempValue;
    int i, j;
    ifstream input( "origImag.bin", ios::in );
    if( ! input )
    {
        cerr << "Open input file error!" << endl;
        exit( -1 );
    }
    ofstream output( "origImagLarge.bin", ios::out );
    if( ! output )
    {
        cerr << "Open output file error!" << endl;
        exit( -1 );
    }
	
	for( i = 0; i < 65025; i++ )
    {
		input >> tempValue;
        for( j = 0 ; j < 672; j++ )
        {
			tempValue = tempValue + 1;	
            cout << tempValue;
        }
    }

    input.close();
    output.close();
    return 0;
}
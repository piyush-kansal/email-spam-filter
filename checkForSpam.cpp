#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "stdint.h" 
#include <cstring>
#include "hash_32a.c"

using namespace std;

const string _DELIM_STR_ = " ";
const string _NULL_STR_ = "";

struct nGram
{
	string str;
	double prob;
};

struct position
{
	int pos;
	position* next;
};

struct hashmap
{
	position* data;
};

vector<hashmap> nSPHash, sPHash;
vector<nGram> nSPGrams, sPGrams;

int createVector(vector<nGram>&, vector<hashmap>& hash, string File);
void InitializeHash();
void InsertHash(vector<hashmap>& hash, unsigned int, int);
int Get_pos(vector <nGram>&, int);

int main( int argc, char* argv[] )
{
	if( argc != 6 ) {
                cout << "Invalid no of arguments" << endl;
                cout << "Usage: ./checkForSpam <not-spam-prob-file> <spam-prob-file> <file-to-be-checked>" << endl;
                return 1; 
        }

        string notSpamProbFile = argv[1];
        string spamProbFile = argv[2]; 
        string toCheckFile = argv[3];
        unsigned int notSpamCnt = atoi( argv[4] );
        unsigned int spamCnt = atoi( argv[5] );

        nSPHash.reserve( FNV_32_PRIME );
        sPHash.reserve( FNV_32_PRIME );

	InitializeHash();

	int nSPCnt = createVector( nSPGrams, nSPHash, notSpamProbFile );
	int sPCnt = createVector( sPGrams, sPHash, spamProbFile );

	double nSP = 1.0;
	double sP = 1.0;

	ifstream file (toCheckFile.c_str());
	
	if(file.is_open())
	{
		while ( file.good() ) 
		{
			string curLine;
			getline(file, curLine);

			if ( curLine == _NULL_STR_ )
				break;
			else
			{
				int x = fnv_32a_str( (char*)curLine.c_str(), 0 );
                		int hashVal = x % FNV_32_PRIME;

				if( nSPHash[hashVal].data ) {
					position *curPos = nSPHash[hashVal].data;

					while( curPos ) {
						if( nSPGrams[curPos->pos].str == curLine )
							nSP *= nSPGrams[curPos->pos].prob;

						curPos = curPos->next;
					}
				}
		
				if( sPHash[hashVal].data ) {
					position *curPos = sPHash[hashVal].data;

					while( curPos ) {
						if( sPGrams[curPos->pos].str == curLine )
							sP *= sPGrams[curPos->pos].prob;

						curPos = curPos->next;
					}
				}
			}	
		}
	}

	nSP = ((double)notSpamCnt)/((double)notSpamCnt + (double)spamCnt);
	sP = ((double)spamCnt)/((double)notSpamCnt + (double)spamCnt);

        cout << "Probability for Not Spam :: " << nSP << endl;
        cout << "Probability for Spam :: " << sP << endl;

        if( nSP > sP )
                cout << "It is a not a spam" << endl;
        else if( nSP < sP )
                cout << "It is a spam" << endl;
        else
                cout << "OOPS !! I just cant figure it out. May be you didnt train me well ;)" << endl;

	return 0;
}

void InitializeHash()
{
	for(int i = 0; i<FNV_32_PRIME; i++)
	{
		nSPHash[i].data = NULL;
		sPHash[i].data = NULL;
	}
}

int createVector( vector<nGram>& grams, vector<hashmap>& hash, string File )
{
	ifstream file (File.c_str());
	int i = 0;
	int x = 0;

	if(file.is_open())
	{
		while ( file.good() ) 
		{
			string curLine;
			nGram curGram;
			getline(file, curLine);

			if ( curLine == _NULL_STR_ )
				break;
			else
			{
				string curString, curNum;
				int curPos = curLine.find_last_of ( _DELIM_STR_ );
				curString = curLine.substr( 0, curPos );
				curNum = curLine.substr( curPos + 1, curLine.length() - curPos );
				
				curGram.prob = atof ( curNum.data() );
				curGram.str = curString;
				x = fnv_32a_str( (char*)curString.c_str(), 0 );
				int hash1 = x % FNV_32_PRIME;
				InsertHash( hash, hash1, i );
				grams.push_back( curGram );
				i++;
			}
		}
	}

	file.close();
	return i;
}

/*
int Get_pos(vector <nGram> &gram, int i)
{
	position *ptr = Hash[gram[i].hash2].data;
	int pos1 = -1;

	while(ptr!=NULL)
	{
		int pos = ptr->pos;
		string a, b;

		a = find_suffix(gram[i].str, NGRAM-1);
		b = find_prefix(gram[pos].str, NGRAM-1);

		if(strcmp(a.c_str(),b.c_str())==0)
		{
			if(gram[pos].occ>0)
			{
				if(pos1==-1)
				{
					pos1 = pos;
                		}
                		else
					return -1;
			}
		}

		ptr = ptr->next;
	}

	return pos1;
}*/

void InsertHash(vector<hashmap>& Hash, unsigned int hash, int index)
{
	if(Hash[hash].data == NULL)
	{
		position* ppos = new position;
		ppos->pos = index;
		ppos->next = NULL;
		Hash[hash].data = ppos;
	}
	else
	{
		position* ppos = Hash[hash].data;

		while(ppos->next!=NULL)
		{
			ppos = ppos->next;
		}

		position* temp_pos = new position;
		temp_pos->pos = index;
		temp_pos->next = NULL;
		ppos->next = temp_pos;
	}
}

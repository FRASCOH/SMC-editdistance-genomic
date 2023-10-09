#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;


Integer max(Integer a, Integer b) {
    return b.select((a > b), a);
}

int snake(vector<int> sequence, int lengthAlice, int lengthBob, int intBitSize, int k, int j) {
    int i = j - k;

    vector<Integer> sequenceAlice;
    vector<Integer> sequenceBob;
    int nrow = lengthBob;
    int ncol = lengthAlice;
    vector<vector<Integer>> matrix(nrow+1, vector<Integer> (ncol+1));
    Integer ok(intBitSize, 1, ALICE);
    Integer notok(intBitSize, 0, ALICE);
    int x;

    for (x = 0; x < lengthAlice; x++)
        sequenceAlice.push_back(Integer (intBitSize, sequence[x], ALICE));
    for (x = 0; x < lengthBob; x++)
        sequenceBob.push_back(Integer (intBitSize, sequence[x], BOB));

    while(i < lengthAlice && j < lengthBob ){
    Integer result = ok.select((sequenceAlice[i] == sequenceBob[j]), notok);
    	if(result.reveal<int>() == 1){
            i++; 
            j++;
     	}
     	    	
     }  
    return j;
}


int edit_distance_wu(vector<int> sequence, int lengthAlice, int lengthBob, int intBitSize) {
    vector<Integer> sequenceAlice;
    vector<Integer> sequenceBob;
    vector<Integer> row1 (lengthBob+1);
    vector<Integer> row2 (lengthBob+1);
    Integer zero(intBitSize, 0, ALICE);
    Integer one(intBitSize, 1, ALICE);
    int x, k;
    int p = -1;
    int delta = lengthBob - lengthAlice;
    int offset = lengthAlice + 1;
    vector<int> fp (lengthAlice + lengthBob + 3);
    fill(&fp[0], &fp[lengthAlice + lengthBob + 3], -1);

    for (x = 0; x < lengthAlice; x++)
        sequenceAlice.push_back(Integer (intBitSize, sequence[x], ALICE));

    for (x = 0; x < lengthBob; x++)
        sequenceBob.push_back(Integer (intBitSize, sequence[x], BOB));

     do {
        p++;
        for (k = -p; k <= delta-1; k++) {
            fp[k+offset] = snake(sequence, lengthAlice, lengthBob, intBitSize, k, max(fp[k-1+offset]+1, fp[k+1+offset]));
        }
        for (k = delta+p; k >= delta+1; k--) {
            fp[k+offset] = snake(sequence, lengthAlice, lengthBob, intBitSize, k, max(fp[k-1+offset]+1, fp[k+1+offset]));
        }
        fp[delta+offset] = snake(sequence, lengthAlice, lengthBob, intBitSize, delta, max(fp[delta-1+offset]+1, fp[delta+1+offset]));
    } while (fp[delta+offset] != lengthBob);
    
    return (delta + 2*p);
    
}

int main(int argc, char** argv) {
    int port, party;
    parse_party_and_port(argv, &party, &port);
    NetIO * io = new NetIO(party==ALICE ? nullptr : "127.0.0.1", port);

    string line;
    int lengthAlice = 0;
    int lengthBob = 0;
    vector<int> nucleobasesAlice;
    vector<int> nucleobasesBob;
    string txt = argv[3] != NULL ? argv[3] : "1000";
    string prefix = "input/InputEditDistance";
    string suffix = ".txt";
    ifstream inputFile(prefix+txt+suffix);
    if (inputFile) {
        getline(inputFile, line);
        lengthAlice = line.length();
        for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
            switch(*it) {
            case 'A':
            case 'a':
                nucleobasesAlice.push_back(1);
                break;
            case 'C':
            case 'c':
                nucleobasesAlice.push_back(2);
                break;
            case 'G':
            case 'g':
                nucleobasesAlice.push_back(3);
                break;
            case 'T':
            case 't':
                nucleobasesAlice.push_back(4);
                break;
            }
        }

        getline(inputFile, line);
        lengthBob = line.length();
        for(std::string::iterator it = line.begin(); it != line.end(); ++it) {
            switch(*it)    {
            case 'A':
            case 'a':
                nucleobasesBob.push_back(1);
                break;
            case 'C':
            case 'c':
                nucleobasesBob.push_back(2);
                break;
            case 'G':
            case 'g':
                nucleobasesBob.push_back(3);
                break;
            case 'T':
            case 't':
                nucleobasesBob.push_back(4);
                break;
            }
        }
        inputFile.close();
    }
    else {
        cout << "Cannot open input file" << endl;
        delete io;
        return 0;
    }

    if(lengthAlice < lengthBob){
        swap(lengthAlice, lengthBob);
        nucleobasesAlice.swap(nucleobasesBob);
    }
    
    int intBitSize = ceil(log(lengthAlice)/log(2))+1;
    cout << "Integer bit size = " << intBitSize << endl;
    cout << "String length 1 = " << lengthAlice << endl;
    cout << "String length 2 = " << lengthBob << endl << endl;

    struct timespec start, end; 
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    setup_semi_honest(io, party);
    int result = edit_distance_wu((party == 1) ? nucleobasesAlice : nucleobasesBob, lengthAlice, lengthBob, intBitSize);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

    cout << "Edit distance:\t" << result << endl;

    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed = (seconds + nanoseconds*1e-9)*1000;
    
    cout << "Time (milliseconds) = " << elapsed << endl;
    
    finalize_semi_honest();
    
    delete io;
}
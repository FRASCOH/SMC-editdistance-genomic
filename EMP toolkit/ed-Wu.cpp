#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include "emp-sh2pc/emp-sh2pc.h"
using namespace emp;
using namespace std;


Integer maximum(Integer a, Integer b) {
    return b.select((a > b), a);
}


Integer snake( vector<Integer> sequenceAlice, vector<Integer> sequenceBob, int s1len, int s2len, int intBitSize, int k, Integer j) {
    cout<< "snake 1"<<endl;
    Integer i_emp(intBitSize, j.reveal<int>() - k, ALICE);
    int j_int = j.reveal<int>();
    Integer one(intBitSize, 1, ALICE);
    Integer zero(intBitSize, 0, ALICE);
    cout<< "snake 2"<<endl;
    if (j_int >= s2len || k >= s1len) {
        return j;
    }
    cout<< "snake 3"<<endl;
    Integer result = j.select((sequenceAlice[k] != sequenceBob[j_int]), zero);
    if(result.reveal<int>() != 0) return j;

    return snake(sequenceAlice, sequenceBob, s1len, s2len, intBitSize, k + 1, j + one);
}

int edit_distance_wu(vector<int> sequence, int lengthAlice, int lengthBob, int intBitSize) {
    vector<Integer> sequenceAlice;
    vector<Integer> sequenceBob;
    vector<Integer> row1 (lengthBob+1);
    vector<Integer> row2 (lengthBob+1);
    Integer zero(intBitSize, 0, ALICE);
    Integer one(intBitSize, 1, ALICE);
    Integer one_neg(32, -1, PUBLIC);
    int x, k=0;
    int p = -1;
    int delta = lengthBob - lengthAlice;
    int offset = lengthAlice + 1;
    vector<Integer> fp (lengthAlice + lengthBob + 3);
    
    for (x = 0; x < (lengthAlice + lengthBob + 3); x++)
        fp[x] = one_neg;
    
    //fill(&fp[0], &fp[lengthAlice + lengthBob + 3], -1);
   
    Integer* ptr_fp = &fp[0];
    
    for (x = 0; x < lengthAlice; x++)
        sequenceAlice.push_back(Integer (intBitSize, sequence[x], ALICE));

    for (x = 0; x < lengthBob; x++)
        sequenceBob.push_back(Integer (intBitSize, sequence[x], BOB));

     Integer lengthBob_emp(32, lengthBob, PUBLIC);
     
     
        int ptr_int= 0; 
     
     cout<<"Inizio do while"<<endl;
     
    
     do {
        p++;
        cout<<"Dentro do while"<<endl;
        cout<<"k: " << k << " p: "<<p<< " delta: "<<delta<<endl;
        for (k = -p; k <= delta-1; k++) {
        cout<< "Primo for: "<< k <<endl;
            *(ptr_fp + k + offset) = snake(sequenceAlice, sequenceBob, lengthAlice, lengthBob, intBitSize, k, maximum( *(ptr_fp + k - 1+ offset) + one, *(ptr_fp + k + 1 + offset)));
            
        }
        for (k = delta+p; k >= delta+1; k--) {
        cout<< "Secondo for: "<< k <<endl;
            *(ptr_fp + k + offset) = snake(sequenceAlice, sequenceBob, lengthAlice, lengthBob, intBitSize, k, maximum( *(ptr_fp + k - 1 + offset) + one, *(ptr_fp + k + 1 + offset)));
        }
        cout<< "Pre fine for" <<endl;
        //cout<<"k: " << k << " p: "<<p<< " delta: "<<delta<<endl;
       // *(ptr_fp + delta + offset) 
        fp[delta+offset] = snake(sequenceAlice, sequenceBob, lengthAlice, lengthBob, intBitSize, delta, maximum( fp[delta-1+offset] + one, fp[delta + 1 + offset]));
        
       ptr_int = (*(ptr_fp + delta + offset)).reveal<int>();
        cout<< "Pre fine for, PTR_INT: " <<ptr_int<<endl;
    } while (ptr_int != lengthBob);
    
    
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

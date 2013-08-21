//
//  AddressConversion.cpp
//  AddressConversion
//
//  Created by Eric Alford on 3/12/13.
//  Copyright (c) 2013 Eric Alford. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

char **arguments;
static int indexx=1;
int offset=0, bytes = 512, physicalKnown=-1, clusterKnown=-1, logicalKnown=-1, clusterSize=-1, reservedSectors=-1, numFatTables=-1, fatLength=-1;
bool logical = false, physical = false, cluster = false, byteAddress = false;

void calculateAddress(){
    if(logical){
        if(logicalKnown != -1){
            if(byteAddress){
                cout << logicalKnown*bytes << endl;
            }
            else{
                cout << logicalKnown << endl;
            }
        }
        else if(physicalKnown != -1){
            if(byteAddress){
                cout << (physicalKnown-offset)*bytes << endl;
            }
            else{
                cout << physicalKnown-offset << endl;
            }
        }
        else if(clusterKnown !=-1){
            if(byteAddress){
                int val = bytes*((((clusterKnown-2)*clusterSize)+reservedSectors+(numFatTables*fatLength))-offset);
                cout << val << endl;
            }
            else{
                int val = ((((clusterKnown-2)*clusterSize)+reservedSectors+(numFatTables*fatLength))-offset);
                cout << val << endl;
            }
        }
    }
    else if(physical){
        if(logicalKnown != -1){
            if(byteAddress){
                cout << (logicalKnown+offset)*bytes << endl;
            }
            else{
                cout << logicalKnown+offset << endl;
            }
        }
        else if(physicalKnown != -1){
            if(byteAddress){
                cout << physicalKnown*bytes << endl;
            }
            else{
                cout << physicalKnown << endl;
            }
        }
        else if(clusterKnown !=-1){
            if(byteAddress){
                int val = bytes*((((clusterKnown-2)*clusterSize)+reservedSectors+(numFatTables*fatLength))+offset);
                cout << val << endl;
            }
            else{
                int val = ((((clusterKnown-2)*clusterSize)+reservedSectors+(numFatTables*fatLength))+offset);
                cout << val << endl;
            }
        }
    }
    else if(cluster){
        if(clusterKnown !=-1){
            if(byteAddress){
                cout << clusterKnown*bytes << endl;
            }
            else{
                cout << clusterKnown << endl;
            }
        }
        else if(logicalKnown != -1){
            if(byteAddress){
                int val = (((logicalKnown-(reservedSectors+numFatTables*fatLength))/clusterSize)+2)*bytes;
                cout << val << endl;
            }
            else{
                int val = (((logicalKnown-(reservedSectors+numFatTables*fatLength))/clusterSize)+2);
                cout << val << endl;
            }
        }
        else if(physicalKnown != -1){
            if(byteAddress){
                int log = (physicalKnown-offset);
                int val = (((log-(reservedSectors+numFatTables*fatLength))/clusterSize)+2)*bytes;
                cout << val << endl;
            }
            else{
                int log = (physicalKnown-offset);
                int val = (((log-(reservedSectors+numFatTables*fatLength))/clusterSize)+2);
                cout << val << endl;
            }
        }
        else
            cout << "Incorrect Format. Command must be in the format AddressConversion -L|-P|-C [-b offset] [-B [-s bytes]] [-l address] [-p address] [-c address -k sectors -r sectors -t tables -f sectors]" << endl;
    }
}

bool clusterInfo(){
    string cmd = arguments[indexx];
    if(cmd == "-k"){
        indexx++;
        stringstream(arguments[indexx]) >> clusterSize;
        indexx++;
    }
    else if(cmd.substr(0,14) == "--cluster-size"){
        stringstream(cmd.substr(15,cmd.size())) >> clusterSize;
        indexx++;
    }
    
    cmd = arguments[indexx];
    if(cmd == "-r"){
        indexx++;
        stringstream(arguments[indexx]) >> reservedSectors;
        indexx++;
    }
    else if(cmd.substr(0,10) == "--reserved"){
        stringstream(cmd.substr(11,cmd.size())) >> reservedSectors;
        indexx++;
    }
    
    cmd = arguments[indexx];
    if(cmd == "-t"){
        indexx++;
        stringstream(arguments[indexx]) >> numFatTables;
        indexx++;
    }
    else if(cmd.substr(0,12) == "--fat-tables"){
        stringstream(cmd.substr(13,cmd.size())) >> numFatTables;
        indexx++;
    }
    
    cmd = arguments[indexx];
    if(cmd == "-f"){
        indexx++;
        stringstream(arguments[indexx]) >> fatLength;
    }
    else if(cmd.substr(0,12) == "--fat-length"){
        stringstream(cmd.substr(13, cmd.size())) >> fatLength;
    }
    
    if(clusterSize==-1 || reservedSectors==-1 || numFatTables==-1 || fatLength==-1)
        return false;
    return true;
}

int main(int argc, char* argv[]) {
    arguments = argv;            //make the arguments array global.
    string cmd = argv[indexx];
    //Are we returning the logical, physical or cluster address?
    if(cmd == "-L" || cmd == "--logical")
        logical = true;
    else if(cmd == "-P" || cmd == "--physical")
        physical = true;
    else if(cmd == "-C" || cmd == "--cluster")
        cluster = true;
    else{
        cout << "Incorrect Format. Command must be in the format AddressConversion -L|-P|-C [-b offset] [-B [-s bytes]] [-l address] [-p address] [-c address -k sectors -r sectors -t tables -f sectors]" << endl;
        exit(0);
    }
    indexx++;
    
    //are we dealing with a byte offset?
    cmd = argv[indexx];
    if(cmd == "-b"){
        indexx++;
        offset = atoi(argv[indexx]);
        indexx++;
    }
    else if(cmd.substr(0,17) == "--partition-start"){
        stringstream(cmd.substr(18, cmd.size())) >> offset;
        indexx++;
    }
    
    //are we returning the byte address?
    cmd = argv[indexx];
    if(cmd == "-B"){
        byteAddress = true;
        indexx++;
    }
    else if(cmd.substr(0, 14) == "--byte-address"){
        byteAddress = true;
        indexx++;
    }
    
    //Did the user specify a different sector size?
    cmd = argv[indexx];
    if(cmd == "-s"){
        indexx++;
        stringstream(argv[indexx]) >> bytes;
        indexx++;
    }
    else if(cmd.substr(0,13) == "--sector-size"){
        stringstream(cmd.substr(14, cmd.size())) >> bytes;
        indexx++;
    }
    
    
    cmd = argv[indexx];
    if(cmd == "-l"){
        indexx++;
        stringstream(argv[indexx]) >> logicalKnown;
        indexx++;
    }
    else if(cmd.substr(0,15)=="--logical-known"){
        stringstream(cmd.substr(16,cmd.size())) >> logicalKnown;
        indexx++;
    }
    
    cmd = argv[indexx];
    if(cmd == "-p"){
        indexx++;
        stringstream(argv[indexx]) >> physicalKnown;
        indexx++;
    }
    else if(cmd.substr(0,16) == "--physical-known"){
        stringstream(cmd.substr(17,cmd.size())) >> physicalKnown;
        indexx++;
    }
    
    if(strlen(*argv) > 6){
        cmd = argv[indexx];
        if(cmd == "-c"){
            indexx++;
            stringstream(argv[indexx]) >> clusterKnown;
            indexx++;
            if(!clusterInfo()){ //this ensures that all inputs -k -r -t -f are given.
                cout << "Incorrect Format. Command must be in the format AddressConversion -L|-P|-C [-b offset] [-B [-s bytes]] [-l address] [-p address] [-c address -k sectors -r sectors -t tables -f sectors]" << endl;
                exit(0);
            }
        }
        else if(cmd.substr(0,15) == "--cluster-known"){
            stringstream(cmd.substr(16,cmd.size())) >> clusterKnown;
            indexx++;
            if(!clusterInfo()){ //this ensures that all inputs -k -r -t -f are given.
                cout << "Incorrect Format. Command must be in the format AddressConversion -L|-P|-C [-b offset] [-B [-s bytes]] [-l address] [-p address] [-c address -k sectors -r sectors -t tables -f sectors]" << endl;
                exit(0);
            }
        }
        else if(cmd == "-k"){
            if(!clusterInfo()){ //this ensures that all inputs -k -r -t -f are given.
                cout << "Incorrect Format. Command must be in the format AddressConversion -L|-P|-C [-b offset] [-B [-s bytes]] [-l address] [-p address] [-c address -k sectors -r sectors -t tables -f sectors]" << endl;
                exit(0);
            }
        }
    }
    calculateAddress();
}
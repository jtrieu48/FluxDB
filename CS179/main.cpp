#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <cstdio>
#include <filesystem>
#include "database.h"
#include "collection.h"
#include "document.h"
#include "inputHandler.h"

using namespace std;
using namespace rapidjson;
using std::filesystem::directory_iterator;

int main(){
    vector<Database*> allDatabases;
    InputHandler inputManager;

    FILE* fp = fopen("testJSONFiles/sample1.json", "rb"); // non-Windows use "r"
 
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document d;
    d.ParseStream(is);

    fclose(fp);
    //cout << readBuffer;
    /*
    doc->setData(readBuffer);
    //doc->print(); /works
    */

    bool complete = false;
	while (!complete)
	{
        int option = inputManager.displayMenu();
        //ADD
        if (option == 1){ 
            int addOpt = inputManager.displayAddMenu();
            if (addOpt == 1){ //add doc
                inputManager.addDoc(&allDatabases);
            }
            else if (addOpt == 2){ //add coll
                inputManager.addColl(&allDatabases);
            }
            else if (addOpt == 3){ //add db
                inputManager.addDB(&allDatabases);
            }
        }

        //REMOVE
        else if (option == 2){
            int subOpt = inputManager.displaySubMenu();
            if (subOpt == 1){ //remove doc
                inputManager.removeDoc(&allDatabases);
            }
            else if (subOpt == 2){ //remove coll
                inputManager.removeColl(&allDatabases);
            }
            else if (subOpt == 3){ //remove DB
                inputManager.removeDB(&allDatabases);
            }
        }
        else if (option == 4){ //exit
            complete = true;
        }
    }

    //this for loops prints all databases and contents 
    for (int i = 0; i < allDatabases.size(); i++){
        allDatabases.at(i)->printAll();
    }
 
    return 0;
}
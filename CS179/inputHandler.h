#pragma once

#include <iostream>
#include "collection.h"
#include <string>
#include "direct.h" //uncomment this when using windows
//if on mac, change all "\" to "/" to fix assertion error
//#include "unistd.h" //use this on MAC
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "database.h"

using namespace std;
using std::filesystem::directory_iterator;
using namespace rapidjson;

class InputHandler{
public:
    int displayMenu();
    int displayAddMenu();
    int displaySubMenu();
    int displayUpdateMenu();
    void addDB(vector<Database*>*);
    void addColl(vector<Database*>*);
    void addDoc(vector<Database*>*);
    void readData(vector<Database*>*);
    void removeDB(vector<Database*>*);
    void removeColl(vector<Database*>*);
    void updateDB(vector<Database*>*);
    void updateColl(vector<Database*>*);
};

//displays main menu for user and returns chosen option 
int InputHandler::displayMenu()
{
    std::string str;
    int option;
	cout << "Please select an option." << endl;
	cout << "1. Add" << endl;
	cout << "2. Remove" << endl;
	cout << "3. Update" << endl;
    cout << "4. Print" << endl;
    cout << "5. Quit" << endl;
    getline(cin, str);
    option = stoi(str);
	return option;
}

//if add is chosen, display the ADD menu and return chosen option
int InputHandler::displayAddMenu(){
    std::string str;
    int option;
    cout << "Please select an option:" << endl;
    cout << "1. Add a document" << endl;
    cout << "2. Add a collection" << endl;
    cout << "3. Add a database" << endl;
    getline(cin,str);
    option = stoi(str);
    return option;
}

//if update is chosen, display the UPDATE menu and return chosen option
int InputHandler::displayUpdateMenu(){
    std::string str;
    int option;
    cout << "1. Update a document" << endl;
    cout << "2. Update a collection" << endl;
    cout << "3. Update a database" << endl;
    getline(cin,str);
    option = stoi(str);
    return option;
}

//if remove is chosen, display the remove menu and return option
int InputHandler::displaySubMenu(){
    std::string temp;
    cout << "Please select an option: " << endl;
    cout << "1. Remove a document" << endl;
    cout << "2. Remove a collection" << endl;
    cout << "3. Remove a database" << endl;
    getline(cin,temp);
    int option = stoi(temp);
    return option;
}
//adds database
void InputHandler::addDB(vector<Database*>* DB){
    string DBname, temp;
    cout << "Enter database name: ";
    getline(cin, DBname);
    Database *databasePtr = new Database(DBname);
    temp = databasePtr->getPath();
    const char *c = temp.c_str();
    if(mkdir(c) == -1){
        cerr << " Error : " << strerror(errno) << endl; //check which error its giving
        cout << "Error in creating database." << endl;
    } else {
        DB->push_back(databasePtr);
        cout << "Database created!" << endl;
    }
}

//adds empty collection to database
void InputHandler::addColl(vector<Database*>* DB){
    string DBchoose, path, temp;
    if (!DB->empty()){
        for (int i = 0; i < DB->size(); i++){
            cout << i << ". " << DB->at(i)->getName() << endl;
        }
        getline(cin, DBchoose);
        cout << "Enter collection path: ";
        getline(cin, path);
        Collection* collPtr = new Collection(path, DB->at(stoi(DBchoose))->getPath());
        DB->at(stoi(DBchoose))->addColl(collPtr);
        ofstream myFile (collPtr->getPath());
        myFile.close();
    }
}

void InputHandler::addDoc(vector<Database*>* DB){
    string input, DBchoose, collChoose, path;
    cout << "Choose a database: " << endl;
    for (int i = 0; i < DB->size(); i++){
        cout << i << ". " << DB->at(i)->getName() << endl;
    }
    getline(cin, DBchoose);
    if (!DB->at(stoi(DBchoose))->getCollections().empty()){
        cout << "Choose a collection: " << endl;
        DB->at(stoi(DBchoose))->print();
        getline(cin, collChoose);
        cout << "Enter document: ";
        getline(cin, input);
        const char* json = input.c_str();
        path = DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->getPath();
        const char *jsonPath = path.c_str();
        Document* d = new Document();
        Document d2;

        //read in existing data from file, push back new json object
        FILE* fp = fopen(jsonPath, "r"); // non-Windows use "r"
        char readBuffer[65536];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        d->ParseStream(is);
        fclose(fp);
        d2.Parse(json);
        Value v(d2, d->GetAllocator());
        d->PushBack(v, d->GetAllocator());

        //rewrite to the file with updated array of json objects
        char writeBuffer[65536];
        FILE* fp2 = fopen(jsonPath, "w");
        FileWriteStream os(fp2, writeBuffer, sizeof(writeBuffer));
        Writer<FileWriteStream> writer(os);
        d->Accept(writer);
        fclose(fp2);

        //create a doc* and set data to new inputted json obj
        //add this new doc* to our collection
        Document* docPass = new Document();
        docPass->CopyFrom(d2, docPass->GetAllocator());
        DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->addDoc(docPass);
    } else {
        cout << "No collection to choose from. " << endl;
    }
}


//takes in a reference of vector of database*, populates the vector with data
void InputHandler::readData(vector<Database*>* DB){
    string path = "STORAGE\\";
    string DBname, collName;
    string path2, path3;
    for (const auto & file : directory_iterator(path)){
        path2 = file.path().string(); // STORAGE\Database1
        DBname = path2.substr(path2.find("\\")+1, path2.length()-path2.find("\\"));
        Database* databasePtr = new Database(DBname);
        DB->push_back(databasePtr);
        for (const auto & file: directory_iterator(path2)){
            path3 = file.path().string(); //STORAGE\Database1\sample1.json
            string temp = path3.substr(path3.find("\\")+1, path3.length()-path3.find("\\")); 
            collName = temp.substr(temp.find("\\")+1, temp.length()-temp.find("\\")); 
            Collection* collPtr = new Collection(collName, DB->back()->getPath());
            DB->back()->addColl(collPtr);
            string temp2 = collPtr->getPath();
            const char *jsonPath = temp2.c_str();
            Document* d = new Document();
            FILE* fp = fopen(jsonPath, "rb");
            char readBuffer[65536];
            FileReadStream is(fp, readBuffer, sizeof(readBuffer));
            d->ParseStream(is);
            fclose(fp);
            
            for (rapidjson::Value::ConstValueIterator itr = d->Begin(); itr != d->End(); ++itr) {
                const rapidjson::Value& attribute = *itr;
                assert(attribute.IsObject()); // each attribute is an object
                Document *t = new Document();
                t->CopyFrom(attribute, t->GetAllocator());
                collPtr->addDoc(t);
            }
        }
    }
}

void InputHandler::removeDB(vector<Database*>* DB){
    string DBchoose, temp;
    if (DB->size() != 0){ //if database !empty, display them
        cout << "Choose a database: " << endl;
        for (int i = 0; i < DB->size(); i++){
            cout << i << ". " << DB->at(i)->getName() << endl;
        }
        getline(cin, DBchoose);
        temp = DB->at(stoi(DBchoose))->getPath();
        const char *c = temp.c_str();
        if (!DB->at(stoi(DBchoose))->getCollections().empty()) {
            DB->at(stoi(DBchoose))->deleteFiles();
        }
        if(rmdir(c) == -1){
            //cerr << " Error : " << strerror(errno) << endl;
            cout << "Error in removing database." << endl;
        } else {
            DB->erase(DB->begin() + stoi(DBchoose));
            cout << "Database removed!" << endl;
        }
    } else { //database empty
        cout << "No databases to choose from." << endl;
    }
}

void InputHandler::removeColl(vector<Database*>* DB){
    string DBchoose, collChoose, temp;
    if (DB->size() != 0){ //if database !empty, display them
        cout << "Choose a database: " << endl;
        for (int i = 0; i < DB->size(); i++){
            cout << i << ". " << DB->at(i)->getName() << endl;
        }
        getline(cin, DBchoose);
        cout << "Choose a collection from this database: " << endl;
        DB->at(stoi(DBchoose))->print(); //assuming DBchoose is int, prints all collections in the chosen DB
        getline(cin, collChoose);
        temp = DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->getPath();
        const char *c = temp.c_str();
        if(remove(c) != 0){
            //cerr << " Error : " << strerror(errno) << endl;
            cout << "Error in removing collection." << endl;
        } else {
            DB->at(stoi(DBchoose))->removeColl(stoi(collChoose));
            cout << "Collection removed." << endl;
        }
    } else { //database empty
        cout << "No databases to choose from." << endl;
    }
}

//updates database
void InputHandler::updateDB(vector<Database*>* DB){
    string DBchoose, tempName;
    string tempDB;
    cout << "Choose a database: " << endl;
    for (int i = 0; i < DB->size(); i++){
        cout << i << ". " << DB->at(i)->getName() << endl;
    }
    getline(cin, DBchoose);

    cout << "Rename " << DB->at(stoi(DBchoose))->getName() << " to: " <<endl;
    getline(cin, tempName);

    tempDB = DB->at(stoi(DBchoose))->getPath().c_str();
    
    const char* newName = ("STORAGE\\"+tempName).c_str();
    const char* oldName = tempDB.c_str();
    DB->at(stoi(DBchoose))->setPath(tempName);
    //cout << "new name: " << newName << endl;
    //cout << "old name: " << oldName << endl;
    rename(oldName, newName);
    DB->at(stoi(DBchoose))->setName(tempName);
    cout << "Database name updated!" << endl;

    //cout << DB->at(stoi(DBchoose))->getName() << endl;
}

//updates collection
void InputHandler::updateColl(vector<Database*>* DB){
    string DBchoose, collChoose;
    string tempName, tempDB, tempColl, DBname;
    cout << "Choose a database: " << endl;
    for (int i = 0; i < DB->size(); i++){
        cout << i << ". " << DB->at(i)->getName() << endl;
    }
    getline(cin, DBchoose);

    cout << "Choose a collection from this database: " << endl;
    DB->at(stoi(DBchoose))->print(); //assuming DBchoose is int, prints all collections in the chosen DB
    getline(cin, collChoose);

    cout << "Rename " << DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->getName() << " to: " <<endl;
    getline(cin, tempName);

    //puts selected DB and Coll into a temp string
    tempDB = DB->at(stoi(DBchoose))->getPath().c_str();
    tempColl = DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->getPath().c_str();
    DBname = DB->at(stoi(DBchoose))->getName();

    cout<< DBname <<endl;

    cout << "tempName: " << tempName << endl;

    //renaming
    const char* newName = ("STORAGE\\"+DBname+"\\"+tempName).c_str();
    const char* oldName = tempColl.c_str();
    DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->setPath(DBname+"\\"+tempName);
    //cout << endl << DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->getPath()<< endl;
    //cout << "new name: " << newName << endl;
    //cout << "old name: " << oldName << endl;
    rename(oldName, newName);
    DB->at(stoi(DBchoose))->getCollection(stoi(collChoose))->setName(tempName);
    cout << "Collection name updated!" << endl;

    //cout << DB->at(stoi(DBchoose))->getName() << endl;
}

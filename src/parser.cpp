//
//  parser.cpp
//  lab3
//
//  Modified by Tarek Abdelrahman on 2020-10-04.
//  Created by Tarek Abdelrahman on 2018-08-25.
//  Copyright © 2018-2020 Tarek Abdelrahman.
//
//  Permission is hereby granted to use this code in ECE244 at
//  the University of Toronto. It is prohibited to distribute
//  this code, either publicly or to third parties.


#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#include "globals.h"
#include "Shape.h"

// This is the shape array, to be dynamically allocated
Shape** shapesArray;

// The number of shapes in the database, to be incremented 
int shapeCount = 0;

// The value of the argument to the maxShapes command
int max_shapes;
int new_max_shapes;

// ECE244 Student: you may want to add the prototype of
// helper functions you write here
void executeCommand (stringstream &lineStream, string &command);
void callMaxShapes (stringstream &lineStream);
void callCreate (stringstream &lineStream);
void callMove (stringstream &lineStream);
void callRotate (stringstream &lineStream);
void callDraw (stringstream &lineStream);
void callDelete (stringstream &lineStream);

bool intValid (stringstream &lineStream, int num);
bool nameValid (stringstream &lineStream, string name, int& pos, bool draw_or_delete, bool &check_all);
bool noExcessArguments (stringstream &lineStream);

// ******************************************************************************
// Main
// ******************************************************************************

int main() {

    string line;
    string command;

    shapesArray = NULL;     // Initially set the shapesArray to NULL

    cout << "> ";         // Prompt for input
    getline(cin, line);    // Get a line from standard input

    while ( !cin.eof () ) {
        // Put the line in a linestream for parsing
        // Making a new sstream for each line so flags etc. are cleared
        stringstream lineStream (line);
        
        // Read from string stream into the command
        // The only way this can fail is if the eof is encountered
        lineStream >> command;

        // Check for the command and act accordingly
        // ECE244 Student: Insert your code here

        if (lineStream.fail()) cout << "Error: invalid command" << endl;    // Empty input string
        else {
            bool valid_command = false;
            for (string keyWord : keyWordsList){
                if (keyWord != "all" && keyWord == command){
                    valid_command = true;
                    break;
                }
            }
            if (valid_command) executeCommand(lineStream, command);          // One of valid commands
            else cout << "Error: invalid command" << endl;                  // Non-empty input string, but invalid command
        }
        
        // Once the command has been processed, prompt for the next command
        cout << "> ";
        getline(cin, line);
        
    }  // End input loop until EOF.
    
    return 0;
}

// ******************************************************************************
// Command Executions
// ******************************************************************************

// Run program with valid command
void executeCommand (stringstream &lineStream, string &command){
    if (command == "maxShapes"){ callMaxShapes(lineStream); }
    else if (command == "create"){ callCreate(lineStream); }
    else if (command == "move"){ callMove(lineStream); }
    else if (command == "rotate"){ callRotate(lineStream); }
    else if (command == "draw"){ callDraw(lineStream); }
    else if (command == "delete"){ callDelete(lineStream); }
}

// maxShapes
void callMaxShapes (stringstream &lineStream){
    lineStream >> new_max_shapes;
    // For first argument
    if (!intValid(lineStream, new_max_shapes)) return;                  // Error: maxShapes 10+; maxShapes strstr; maxShapes -22; maxShapes (stop)
    
    // Check excess arguments
    if (noExcessArguments(lineStream)){                                 // Error: maxShapes 10 strstr; maxShapes 10 20
        // clear old shapesArray (only when shapesArray is not NULL)
        if (shapesArray != NULL){
            for (int i = 0; i < max_shapes; i++){
                if (shapesArray[i] == NULL) continue;
                delete shapesArray[i];
                shapesArray[i] = NULL;
            }
            delete [] shapesArray;
        }
        
        // create new shapesArray                                       // Valid: maxShapes 10____; maxShapes 23
        Shape** newShapesArray = new Shape* [new_max_shapes];
        for (int i = 0; i < new_max_shapes; i++){
            newShapesArray[i] = NULL;
        }
        shapesArray = newShapesArray;
        max_shapes = new_max_shapes;
        shapeCount = 0;
        cout << "New database: max shapes is " << max_shapes << endl;
    }
}

// create
void callCreate (stringstream &lineStream){
    string name;
    string type;
    int locx, locy, sizex, sizey;
    
    // name
    lineStream >> name;
    for (int i = 0; i < 7; i++){
        if (name == keyWordsList[i]){
            cout << "Error: invalid shape name" << endl;            // Error: create all ...; Input always converted to name string
            return;
        }
    }
    for (int i = 0; i < 4; i++){
        if (name == shapeTypesList[i]){
            cout << "Error: invalid shape name" << endl;            // Error: create circle ...; Input always converted to name string
            return;
        }
    }
    for (int i = 0; i < max_shapes; i++){
        if (shapesArray[i] == NULL) continue;
        if (shapesArray[i] -> getName() == name){
            cout << "Error: shape " << name << " exists" << endl;   // Error: create s1 ...; then; create s1...
            return;
        }
    }
    if (lineStream.fail()){
        cout << "Error: too few arguments" << endl;                 // Error: create (stop)
        return;
    }
    
    // type
    lineStream >> type;
    if (lineStream.fail()){
        cout << "Error: too few arguments" << endl;                 // Error: create circle (stop)
        return;
    }
    bool validType = false;
    for (string y : shapeTypesList){
        if (type == y){ 
            validType = true;                                       // validType = true when type is identified
            break;
        }
    }
    if (!validType){
        cout << "Error: invalid shape type" << endl;                // Error: create s1 parabola
        return;
    }

    //locx, locy
    lineStream >> locx;
    if (!intValid(lineStream, locx)) return;
    lineStream >> locy;
    if (!intValid(lineStream, locy)) return;

    //size x, size y
    lineStream >> sizex;
    if (!intValid(lineStream, sizex)) return;
    lineStream >> sizey;
    if (!intValid(lineStream, sizey)) return;
    if (type == "circle" && (sizey != sizex)){
        cout << "Error: invalid value" << endl;                     // create c circle 10 2 3 4.6 -> invalid argument
        return;
    }

    // Excess arguments
    if (noExcessArguments(lineStream)){
        if (shapeCount >= max_shapes){
            cout << "Error: shape array is full" << endl;
            return;
        }
        shapesArray[shapeCount] = new Shape(name, type, locx, locy, sizex, sizey);
        shapeCount++;
        cout << "Created " << name << ": " << type << " " << locx << " " << locy << " " << sizex << " " << sizey << endl;
    }
}

// move
void callMove (stringstream &lineStream){
    string name;
    int locx, locy, pos;
    bool check_all = false;

    // name
    lineStream >> name;
    if (!nameValid(lineStream, name, pos, false, check_all)) return;

    // locx, locy
    lineStream >> locx;
    if (!intValid(lineStream, locx)) return;
    lineStream >> locy;
    if (!intValid(lineStream, locy)) return;

    // Excess arguments
    if (noExcessArguments(lineStream)){
        shapesArray[pos] -> setXlocation(locx);
        shapesArray[pos] -> setYlocation(locy);
        cout << "Moved " << name << " to " << locx << " " << locy << endl;
    }
}

// rotate
void callRotate (stringstream &lineStream){
    string name;
    int degree, pos;
    bool check_all = false;

    // name
    lineStream >> name;
    if (!nameValid(lineStream, name, pos, false, check_all)) return;

    // degree
    lineStream >> degree;
    if (!intValid(lineStream, degree)) return;
    if (degree > 360){
        cout << "Error: invalid value" << endl;
        return;
    }

    // Excess arguments
    if (noExcessArguments(lineStream)){
        shapesArray[pos] -> setRotate(degree);
        cout << "Rotated " << name << " by " << degree << " degrees" << endl;
    }
}

// draw
void callDraw (stringstream &lineStream){
    string name;
    int pos;
    bool check_all = false;

    // name
    lineStream >> name;
    if (!nameValid(lineStream, name, pos, true, check_all)) return;
    
    // Excess arguments
    if (noExcessArguments(lineStream)){
        // draw all
        if (check_all){
            cout << "Drew all shapes" << endl;
            if (shapesArray != NULL){
                for (int i = 0; i < max_shapes; i++){
                    if (shapesArray[i] == NULL) continue;
                    shapesArray[i]->draw();
                }
            }
        }
        // If draw specific shape
        else{
            cout << "Drew ";
            shapesArray[pos]->draw();
        }
    }
}

// delete
void callDelete (stringstream &lineStream){
    string name;
    int pos;
    bool check_all = false;

    // name
    lineStream >> name;
    if (!nameValid(lineStream, name, pos, true, check_all)) return;

    // Excess arguments
    if (noExcessArguments(lineStream)){
        // delete all
        if (check_all){
            cout << "Deleted: all shapes" << endl;
            if (shapesArray != NULL){
                for (int i = 0; i < max_shapes; i++){
                    if (shapesArray[i] == NULL) continue;
                    delete shapesArray[i];
                }
                delete [] shapesArray;
                shapesArray = NULL;
                shapeCount = 0;
            }
        }
        // If draw specific shape
        else{
            cout << "Deleted shape " << name << endl;
            delete shapesArray[pos];
            shapesArray[pos] = NULL;
        }
    }
}

// ******************************************************************************
// Helper Functions
// ******************************************************************************
bool intValid (stringstream &lineStream, int num){
    if (lineStream.fail()){
        if (lineStream.eof()) {
            cout << "Error: too few arguments" << endl; // Error: Missing the current int argument
            return false;
        }         
        else {
            cout << "Error: invalid argument" << endl;  // Error: Type mismatch
            return false;
        }                 
    }
    char a = lineStream.peek();
    if (!lineStream.eof() && a != ' ' && a != '\t') {
        cout << "Error: invalid argument" << endl;      // Error: ... 10+; ... 21.4
        return false;
    }       

    if (num < 0){
        cout << "Error: invalid value" << endl;         // Error: Negative value
        return false;                           
    }                 
    return true;
}

bool nameValid (stringstream &lineStream, string name, int& pos, bool draw_or_delete, bool &check_all){
    // name was missing
    if (lineStream.fail()){
        cout << "Error: too few arguments" << endl;
        return false;
    }
    // Error when name matches with keyword and shapetype lists
    for (int i = 0; i < 7; i++){
        if (name == keyWordsList[i]){
            if (name == "all" && draw_or_delete){           // Allows the use of "all" when draw or deleting
                check_all = true;
                return true;
            }
            else cout << "Error: invalid shape name" << endl;
            return false;
        }
    }
    for (int i = 0; i < 4; i++){
        if (name == shapeTypesList[i]){
            cout << "Error: invalid shape name" << endl;
            return false;
        }
    }
    // name does not exist in shapesArray
    bool found = false;
    if (shapesArray != NULL){
        for (int i = 0; i < max_shapes; i++){
            if (shapesArray[i] == NULL) continue;
            if (shapesArray[i] -> getName() == name){
                found = true;
                pos = i;
                break;
            }
        }
    }
    if (!found){
        cout << "Error: shape " << name << " not found" << endl;
        return false;
    }
    return true;
}

bool noExcessArguments (stringstream &lineStream){
    char temp;
    lineStream >> temp;
    if (!lineStream.fail()){
        cout << "Error: too many arguments" << endl;
        return false;
    }
    else return true;
}

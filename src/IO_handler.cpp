//
//  IO_handler.cpp
//  Gaia
//
//  Created by Marino Wang on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "IO_handler.hpp"

using namespace std;
namespace pt = boost::property_tree;

IO_handler::IO_handler(int type){
    ctype = type;
}

void IO_handler::SendKeyBoardInput(int direction){
    
    switch(direction){
        case 0:
            currDir = "w";
            break;
        case 1:
            currDir = "a";
            break;
        case 2:
            currDir = "s";
            break;
        case 3:
            currDir = "d";
            break;
        
    }
    //ifPressed = true;
}

void IO_handler::SendMouseInput(int leftOrRight, glm::vec2 start, glm::vec2 end){
    if(leftOrRight == 0){
        cerr << "WHY YOU ARE HERE???" << endl;
        return;
    }
    else if(leftOrRight == 1){
        startPos = start;
        endPos = end;
        currBut = "l";

        //cout << endPos.x << " " << endPos.y << endl;
//        cerr << currBut << endl;
//        cout << "drag start: " << startPos[0] << ", " << startPos[1] << endl;
//        cout << "drag end: " << endPos[0] << ", " << endPos[1] << endl;
    }
    else if(leftOrRight == 2){
        startPos = start;
        endPos = end;
        currBut = "r";

//        cerr << currBut << endl;
//        cout << "drag start: " << startPos[0] << ", " << startPos[1] << endl;
//        cout << "drag end: " << endPos[0] << ", " << endPos[1] << endl;
    }
    else{
        cerr << "???????" << endl;
        return;
    }
    //ifClicked = true;
}

void IO_handler::SendPackage(chat_client* c){
    pt::ptree root;
    pt::ptree cmd;
    
    pt::ptree cmd_key;
    pt::ptree cmd_mouse;
    
    pt::ptree mouse_l;
    pt::ptree mouse_r;
    pt::ptree allPos[4];
    
    cmd_key.put("key", currDir);
    
    cmd_mouse.put("mouse", currBut);
        
    allPos[0].put("", startPos.x);
    allPos[1].put("", startPos.y);
    allPos[2].put("", endPos.x);
    allPos[3].put("", endPos.y);
        
    if(currBut.compare("l")){
        for(int i=0; i<4; i++){
            mouse_l.push_back(std::make_pair("", allPos[i]));
        }
    }
    else if (currBut.compare("r")) {
        for(int i=0; i<4; i++){
            mouse_r.push_back(std::make_pair("", allPos[i]));
        }
    }
    else {
        cerr << "string comparision error in io_handller 96" << endl;
    }
    
    currBut = "";
    currDir = "";

    
    cmd_mouse.add_child("mouse_l", mouse_l);
    cmd_mouse.add_child("mouse_r", mouse_r);
    

    cmd.push_back(std::make_pair("", cmd_key));
    cmd.push_back(std::make_pair("", cmd_mouse));
    
    root.add_child("cmd", cmd);
    
    stringstream ss;
    write_json(ss, root, false);
    c->write(ss.str());
}





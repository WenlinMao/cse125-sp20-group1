//
//  chat_message.hpp
//  game-server
//
//  Created by peizhenwu on 2020/4/18.
//  Copyright © 2020 peizhenwu. All rights reserved.
//

#ifndef chat_message_hpp
#define chat_message_hpp

#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>


using namespace std;
namespace pt = boost::property_tree;


class chat_message
{
public:
    int x1, y1, x2, y2;

    glm::mat4 transM1, transM2;

    vector <float> height_map;

    chat_message() : x1(5), x2(5), y1(5), y2(5)
    {
        transM1 = glm::mat4(1.0f);
        transM1[3] = glm::vec4(0,-10,0,1);
        transM2 = glm::mat4(1.0f);
        transM2[3] = glm::vec4(58,0,-54,1);

        for (int i = 0; i < 16000; i++){
            height_map.push_back(1);
        }

    }
  
    void update1(char op){
        switch (op) {
            case 'w':
                y1++;
                transM1[3][2]--;
                break;
            case 'a':
                x1--;
                transM1[3][0]--;
                break;
            case 's':
                y1--;
                transM1[3][2]++;
                break;
            case 'd':
                x1++;
                transM1[3][0]++;
                break;
        }
    }


    void update2(char op){
        switch (op) {
            case 'w':
                y2++;
                transM2[3][2]--;
                break;
            case 'a':
                x2--;
                transM2[3][0]--;
                break;
            case 's':
                y2--;
                transM2[3][2]++;
                break;
            case 'd':
                x2++;
                transM2[3][0]++;
                break;
        }
    }

    void editTerrain(std::vector<glm::vec2> editPoints, float height){
        //TODO: update height_map
        std::cout << "Edit Points: " << std::endl;
        std::cout << editPoints[0][0] << ","<< editPoints[0][1] << std::endl;
        std::cout << editPoints[1][0] << ","<< editPoints[1][1] << std::endl;
    }
    std::string data()
    {
        pt::ptree root;
        pt::ptree obj;

        pt::ptree obj1;
        pt::ptree obj2;

        pt::ptree m1;
        pt::ptree m2;

        pt::ptree matrix1[16];
        pt::ptree matrix2[16];

        pt::ptree height_root;

        obj1.put("id", 1);
        for(int i=0;i<4;i++){
          for(int j=0;j<4;j++){
            matrix1[4*i+j].put("", transM1[i][j]);
          }
        }
        for(int i=0;i<16;i++){
          m1.push_back(std::make_pair("", matrix1[i]));
        }
        obj1.add_child("transformation", m1);

        obj2.put("id", 2);
        for(int i=0;i<4;i++){
          for(int j=0;j<4;j++){
            matrix2[4*i+j].put("", transM2[i][j]);
          }
        }
        for(int i=0;i<16;i++){
          m2.push_back(std::make_pair("", matrix2[i]));
        }
        obj2.add_child("transformation", m2);


        obj.push_back(std::make_pair("", obj1));
        obj.push_back(std::make_pair("", obj2));

        // build and add current height map node to root
        for(int i = 0; i < height_map.size(); i++){
            pt::ptree node;
            node.put("", height_map[i]);
            height_root.push_back(std::make_pair("", node));
        }

        root.add_child("Obj", obj);

        root.add_child("height_map" ,height_root);

        stringstream ss;
        write_json(ss, root, false);
        return ss.str();
    }
};

#endif /* chat_message_hpp */

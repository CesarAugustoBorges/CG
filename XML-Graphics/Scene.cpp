//
// Created by cesar on 2/25/19.
//

#include "Scene.h"

void Scene::addModel(Model m){
    this->models.push_front(m);
}

Scene::Scene(){
    this->models;
}
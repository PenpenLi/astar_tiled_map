#ifndef _MAP_LAYER_H_
#define _MAP_LAYER_H_
#include <cocos2d.h>
#include "Role.h"

#include <iostream>  

#include "Role.h"

#include "common/navigation/GraphMap.h"


USING_NS_CC;

class Role;

class MapLayer : public Layer
{
public:
    MapLayer();
    ~MapLayer();
    int _screenWidth, _screenHeight;  // 屏幕宽度和高度
    virtual bool init();
    void update(float dt);
    void setViewpointCenter(Vec2 pos);

    void initMapWithFile(const char * filepath);

    static cocos2d::Scene* createScene();
    CREATE_FUNC(MapLayer);

    GraphMap& getGraph() {return *m_graph;}

private:
    Role *m_hero;  //玩家角色
    
    GraphMap *m_graph;
};

#endif
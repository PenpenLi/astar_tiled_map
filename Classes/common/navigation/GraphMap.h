
#ifndef GRAPHMAP_H
#define GRAPHMAP_H

#include "cocos2d.h"
USING_NS_CC;

class GraphPoint
{
public:
    int x;
    int y;

    GraphPoint() {}
    GraphPoint(int _x, int _y):x(_x), y(_y) {}
};


class GraphMap 
{
private:
    enum MapType
    {
        Type_None = 0,
        Type_Normal,             /*��������, ������*/
        Type_ISO,                   /*45 ������*/
        Type_Staggered,        /*45 �����ν���*/
    };
    
    int m_mapWidth;
    int m_mapHeight;

    Size m_gridSize;
    
    MapType m_type;

    int *m_walkable; /*��ǵ�ͼÿһ���Ƿ������*/

    TMXTiledMap *m_tiledMap;
    
public:
    GraphMap(TMXTiledMap* map);

    ~GraphMap();

    TMXTiledMap *getTiledMap() {return m_tiledMap;}
    
    bool isMapTypeOfNormal() {return m_type == Type_Normal;}

    bool isMapTypeOfISO() {return m_type == Type_ISO;}
    
    bool isMapTypeOfStaggered() {return m_type == Type_Staggered;}

    bool isPointInMap(int x, int y);
    
    void setWalkable(int x, int y, bool isWalkable);

    bool isWalkable(int x, int y);

    int getWidth() {return m_mapWidth;}

    int getHeight() {return m_mapHeight;}

    int getGridWidth() {return (int)m_gridSize.width;}

    int getGridHeight() {return (int)m_gridSize.height;}
    
    Vec2 getTileCoordByPosition(Vec2 pos);

    Vec2 getPositionByTileCoord(int x, int y);
    
    /*��Ŀ���Ϊ�ϰ���ʱ, ��ȡ���滻��, Ҫ���� ԭ�������*/
    GraphPoint getReplaceablePoint(int src_x, int src_y, int target_x, int target_y);
    
};

#endif 



#include "GraphMap.h"
#include "PathLineTest.h"

/*��Ҫ��������ת���Լ��ϰ����ж�*/


GraphMap::GraphMap(TMXTiledMap* map):m_type(Type_None), m_tiledMap(map)
{

    m_mapWidth = (int)map->getMapSize().width;

    m_mapHeight = (int)map->getMapSize().height;

    m_gridSize = map->getTileSize();

    //init walkable map
    m_walkable = new int[m_mapWidth * m_mapHeight];
    
    memset(m_walkable, 0, sizeof(int)* (m_mapWidth * m_mapHeight));

    TMXLayer* canWalkLayer = map->getLayer("walkable"); 
    assert(canWalkLayer != NULL);
    
    for (int x = 0; x < m_mapWidth; x++)
    {
        for (int y = 0; y < m_mapHeight; y++)
        {
            // ʹ��TMXLayer���tileGIDAt ������ȡTileMap����ϵ��ġ�ȫ��Ψһ��ʶ��GID
            int tileGid = canWalkLayer->getTileGIDAt(Vec2(x, y)); 
            if (tileGid && tileGid > 0) /*�������ͼ����������,����Ϊ�ϰ���, ����ɽ�����չ�Ա�ǲ�ͬ·��*/
            {
                #if 0
                // ʹ��GID������ָ��tile�����ԣ�����һ��Value
                Value properties = map->getPropertiesForGID(tileGid);
                // ���ص�Valueʵ����һ��ValueMap
                ValueMap map = properties.asValueMap();
                // ����ValueMap, ������ϰ�����
                std::string value = map.at("canWalk").asString();
                if (value.compare("true") == 0)
                {
                    setWalkable(x, y, true);
                }
               #else 
                    setWalkable(x, y, true);
              #endif 
            }
        }
    }    
    
    /*����֮��ɾ���ϰ���ʾ��, ��ʡ�ڴ�!!!!!*/
    canWalkLayer->removeFromParent(); 


    switch (map->getMapOrientation())
    {
        case TMXOrientationOrtho:
            m_type = Type_Normal;
            break;
        case TMXOrientationIso:
            m_type = Type_ISO;
            break;
        case TMXOrientationStaggered:
            m_type = Type_Staggered;
            break;
    };
    assert(m_type != Type_None);
    
}

GraphMap::~GraphMap()
{
    if (m_walkable != nullptr)
    {
        delete []m_walkable;
    }
}

bool GraphMap::isPointInMap(int x, int y)
{
    return (x >= 0 && x <= m_mapWidth && y >= 0 && y <= m_mapHeight);
}

void GraphMap::setWalkable(int x, int y, bool isWalkable)
{
    if (isPointInMap(x, y))
    {
        m_walkable[y*m_mapWidth+x] = isWalkable ? 1:0;
    }
}

bool GraphMap::isWalkable(int x, int y)
{
    if (!isPointInMap(x, y))
    {
        return false;
    }
    return m_walkable[y*m_mapWidth+x] > 0;
}



/*pos: ����ڵ�ͼԭ�����Ļ����(��Ϊ��ͼ�ɻ���)*/
Vec2 GraphMap::getTileCoordByPosition(Vec2 pos)
{
    int x, y;
    if (isMapTypeOfNormal()) /*ֱ�������ͼ*/
    {
        x = pos.x / m_gridSize.width;
        y = (m_mapHeight * m_gridSize.height - pos.y) / m_gridSize.height;
    }
    else if (isMapTypeOfISO())    /*45 ������*/
    {
        Vec2 originP(m_mapHeight * m_gridSize.width/2, m_mapHeight*m_gridSize.height);
        
        x = (int)((pos.x - originP.x)/m_gridSize.width + (originP.y - pos.y)/m_gridSize.height);
        y = (int)(( originP.y - pos.y)/m_gridSize.height - (pos.x - originP.x)/m_gridSize.width);
    }
    else if (isMapTypeOfStaggered()) /*45 �����ν���*/
    {	
        y = m_mapHeight - 2 - ((2 * (int)pos.y) / (int)m_gridSize.height);
        x = pos.x / m_gridSize.width - (y % 2) / 2.0f;
    }
    return Vec2(x, y);
}


Vec2 GraphMap::getPositionByTileCoord(int x, int y)
{
    int _x, _y;
    
    float halfGridWidth = m_gridSize.width/2;
    float halfGridHeight = m_gridSize.height/2;
    
    if (isMapTypeOfNormal())    /*ֱ�������ͼ*/
    {
        _x = (x * m_gridSize.width) + halfGridWidth;
        _y = (m_mapHeight * m_gridSize.height) -(y * m_gridSize.height) - halfGridHeight;
    }
    else if (isMapTypeOfISO())    /*45������*/
    {
        Vec2 originP(m_mapHeight * halfGridWidth, m_mapHeight*m_gridSize.height);
        _x = originP.x + (x - y) * halfGridWidth;
        _y = originP.y - (x + y+1) * halfGridHeight; /*��ȥ����߶�ʹ֮λ�ڸ����м�*/
    }
    else if (isMapTypeOfStaggered()) /*45 �����ν���*/
    {
        _x = x * m_gridSize.width + (y%2)*halfGridWidth;
        _y = (m_mapHeight - (y + 1))*halfGridHeight - halfGridHeight;
    }
    
    return Vec2(_x, _y);
}



/* ��Ŀ��㲻�ɴ�ʱ, ѡ������Χһ���ɴ�ĵ���Ϊ�滻��;
  * ���ɴα��������Ҳ��������, ����ԭĿ���.
*/
GraphPoint GraphMap::getReplaceablePoint(int src_x, int src_y, int target_x, int target_y)
{
    GraphPoint point = GraphPoint(target_x, target_y);

    if(isWalkable(target_x, target_y))
    {
        return point;
    }

    Vec2 pos = getPositionByTileCoord(src_x, src_y);
    
    // 4 ������
    GraphPoint leftUp, rightUp, leftDown, rightDown;

    //����Ŀ�����Χ���10 Ȧ
    for (int i = 1; i <= 10 ; i++)
    {
        if (isMapTypeOfNormal()||isMapTypeOfISO())
        {
            leftUp = GraphPoint(target_x-i, target_y-i);
            rightUp = GraphPoint(target_x+i, target_y-i);
            leftDown = GraphPoint(target_x-i, target_y+i);
            rightDown = GraphPoint(target_x+i, target_y+i); 
        }
        else if (isMapTypeOfStaggered())
        {
            leftUp = GraphPoint(target_x, target_y-2*i);
            rightUp = GraphPoint(target_x+i, target_y);
            leftDown = GraphPoint(target_x-i, target_y);
            rightDown = GraphPoint(target_x, target_y+2*i);   
        }
        else
        {
            assert(false);
        }

        /*�ֱ����ĸ����㹹�ɵı��������ĸ����Ƿ��п���, �ҳ���ԭ���������ĵ�*/
        std::vector<GraphPoint> points1 = getLineGrids(*this, leftUp.x, leftUp.y, rightUp.x, rightUp.y);
        std::vector<GraphPoint> points2 = getLineGrids(*this, rightUp.x, rightUp.y, rightDown.x, rightDown.y);
        std::vector<GraphPoint> points3 = getLineGrids(*this, rightDown.x, rightDown.y, leftDown.x, leftDown.y);
        std::vector<GraphPoint> points4 = getLineGrids(*this, leftDown.x, leftDown.y, leftUp.x, leftUp.y);
        points1.insert(points1.end(), points2.begin(), points2.end());
        points1.insert(points1.end(), points3.begin(), points3.end());
        points1.insert(points1.end(), points4.begin(), points4.end());

        float distance = 99999999.0f;
        bool found = false;
        for (auto &p:points1)
        {
            if (isWalkable(p.x, p.y))
            {
                auto  tmp = getPositionByTileCoord(p.x, p.y);
                float len = (pos.x-tmp.x)*(pos.x-tmp.x) + (pos.y-tmp.y)*(pos.y-tmp.y);
                if (len < distance)
                {
                    found = true;
                    distance = len;
                    point = p;
                }
            }
        }
        
        if (found)
        {
            return point;
        }
    }


    return point;
}    


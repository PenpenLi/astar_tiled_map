#include "MapLayer.h"
#include "common/navigation/PathManager.h"
#include "common/LogDebug.h"

MapLayer::MapLayer()
{
}
MapLayer::~MapLayer()
{
    delete m_graph;
}

Scene* MapLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = MapLayer::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
bool MapLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    this->initMapWithFile("chi_normal.tmx");  /*正常直角地图*/
   //this->initMapWithFile("chi_iso.tmx");            /*45度棱形地图*/
   //this->initMapWithFile("chi_staggered.tmx"); /*45度棱形交错地图*/
	
    auto gameListener = EventListenerTouchOneByOne::create();
    // 响应触摸事件函数
    gameListener->onTouchBegan = [](Touch* touch, Event* event){return true; };
    gameListener->onTouchEnded = [=](Touch *touch, Event *event)
    {
        // OpenGL坐标
        Vec2 touchLocation = touch->getLocation();
        // 将触摸点坐标转换成相对的Node坐标
        Vec2 nodeLocation = this->convertToNodeSpace(touchLocation);

        // 用玩家位置作为起点，触摸点作为终点，转换为网格坐标，在地图上查找最佳到达路径
        Vec2 to = m_graph->getTileCoordByPosition(nodeLocation);

        // 如果终点是不可通过（即有障碍物）的位置，则直接return
        //if (!m_graph->isWalkable(to.x, to.y))
        //{
        //    return;
        //}

        m_hero->requestPathToPositon( (int)to.x,  (int)to.y);
    };

    
    // 添加场景优先事件监听器
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(gameListener, this);

    //设置起始和结束点
    this->scheduleUpdate();

    return true;
	
}
void MapLayer::initMapWithFile(const char * filepath)
{
    TMXTiledMap *m_tileMap = TMXTiledMap::create(filepath);
    m_tileMap->setPosition(Vec2(0, 0));
    this->addChild(m_tileMap);
    
    m_graph = new GraphMap(m_tileMap);

    m_hero = new Role(*m_graph);
    if (m_graph->isMapTypeOfStaggered())
    {
        m_hero->setPosition(m_graph->getPositionByTileCoord(32,91)); 
    }
    else if (m_graph->isMapTypeOfISO())
    {
        m_hero->setPosition(m_graph->getPositionByTileCoord(27, 27)); 
    }
    else 
    {
        m_hero->setPosition(m_graph->getPositionByTileCoord(33,25)); 
    } 
    m_hero->setSumLifeValue(200);
    m_hero->setCurtLifeValue(m_hero->getSumLifeValue());
    m_hero->setState(Role::ActionState::IDLE_STATE_UP);
    this->addChild(m_hero, 2);
}


//这个是移动地图,同时跟踪X,Y轴标准算法
void MapLayer::setViewpointCenter(Vec2 pos)  
{
    Size winSize = Director::getInstance()->getWinSize();
    //如果主角坐标小于屏幕的一半，则取屏幕中点坐标，否则取对象的坐标  
    int x = MAX(pos.x, winSize.width / 2);
    int y = MAX(pos.y, winSize.height / 2);

    //如果X、Y的坐标大于右上角的极限值，则取极限值的坐标（极限值是指不让地图超出屏幕造成出现黑边的极限坐标 )
    x = MIN(x, (m_graph->getWidth()* m_graph->getGridWidth()) - winSize.width / 2);
    y = MIN(y, (m_graph->getHeight() * m_graph->getGridHeight()) - winSize.height / 2);

    //对象当前所在坐标
    Vec2 actualPosition = Vec2(x, y);

    //计算屏幕中点和所要移动的目的点之间的距离
    Vec2 centerOfView = Vec2(winSize.width / 2, winSize.height / 2);
    Vec2 viewPoint = centerOfView - actualPosition;

    this->setPosition(viewPoint);
}





void MapLayer::update(float delta)
{	
    this->setViewpointCenter(m_hero->getPosition());

    PathManager::instance()->updateSearches();
}


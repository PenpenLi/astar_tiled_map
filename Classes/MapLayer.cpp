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
    
    this->initMapWithFile("chi_normal.tmx");  /*����ֱ�ǵ�ͼ*/
   //this->initMapWithFile("chi_iso.tmx");            /*45�����ε�ͼ*/
   //this->initMapWithFile("chi_staggered.tmx"); /*45�����ν����ͼ*/
	
    auto gameListener = EventListenerTouchOneByOne::create();
    // ��Ӧ�����¼�����
    gameListener->onTouchBegan = [](Touch* touch, Event* event){return true; };
    gameListener->onTouchEnded = [=](Touch *touch, Event *event)
    {
        // OpenGL����
        Vec2 touchLocation = touch->getLocation();
        // ������������ת������Ե�Node����
        Vec2 nodeLocation = this->convertToNodeSpace(touchLocation);

        // �����λ����Ϊ��㣬��������Ϊ�յ㣬ת��Ϊ�������꣬�ڵ�ͼ�ϲ�����ѵ���·��
        Vec2 to = m_graph->getTileCoordByPosition(nodeLocation);

        // ����յ��ǲ���ͨ���������ϰ����λ�ã���ֱ��return
        //if (!m_graph->isWalkable(to.x, to.y))
        //{
        //    return;
        //}

        m_hero->requestPathToPositon( (int)to.x,  (int)to.y);
    };

    
    // ��ӳ��������¼�������
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(gameListener, this);

    //������ʼ�ͽ�����
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


//������ƶ���ͼ,ͬʱ����X,Y���׼�㷨
void MapLayer::setViewpointCenter(Vec2 pos)  
{
    Size winSize = Director::getInstance()->getWinSize();
    //�����������С����Ļ��һ�룬��ȡ��Ļ�е����꣬����ȡ���������  
    int x = MAX(pos.x, winSize.width / 2);
    int y = MAX(pos.y, winSize.height / 2);

    //���X��Y������������Ͻǵļ���ֵ����ȡ����ֵ�����꣨����ֵ��ָ���õ�ͼ������Ļ��ɳ��ֺڱߵļ������� )
    x = MIN(x, (m_graph->getWidth()* m_graph->getGridWidth()) - winSize.width / 2);
    y = MIN(y, (m_graph->getHeight() * m_graph->getGridHeight()) - winSize.height / 2);

    //����ǰ��������
    Vec2 actualPosition = Vec2(x, y);

    //������Ļ�е����Ҫ�ƶ���Ŀ�ĵ�֮��ľ���
    Vec2 centerOfView = Vec2(winSize.width / 2, winSize.height / 2);
    Vec2 viewPoint = centerOfView - actualPosition;

    this->setPosition(viewPoint);
}





void MapLayer::update(float delta)
{	
    this->setViewpointCenter(m_hero->getPosition());

    PathManager::instance()->updateSearches();
}


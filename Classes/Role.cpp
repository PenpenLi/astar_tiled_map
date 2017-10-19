
#include "Role.h"
#include "common/LogDebug.h"

#include "common/navigation/PathLineTest.h"
#include "common/navigation/PathSmooth.h"


Role::Role(GraphMap &graph) :
m_down_idle(NULL),
m_up_idle(NULL),
m_left_idle(NULL),
m_right_idle(NULL),
m_leftdown_idle(NULL),
m_rightdown_idle(NULL),
m_leftup_idle(NULL),
m_rightup_idle(NULL),
m_down_walk(NULL),
m_up_walk(NULL),
m_left_walk(NULL),
m_right_walk(NULL),
m_leftdown_walk(NULL),
m_rightdown_walk(NULL),
m_leftup_walk(NULL),
m_rightup_walk(NULL),
m_AStar(NULL),
m_graph(graph),
m_speed(160.0f),
currActionState(ActionState::ACTION_STATE_IDLE)
{
    //this->setRoleDirection(RolelTurnRight);//设定初始朝向
    if (this->init())
    {
        this->autorelease();
    }
}
Role::~Role(void)
{

}

bool Role::init(){
	bool ret = false;
	do
	{	
		//加载模型文件
		std::string fileName = "orc.c3b";  //"orc.c3b";
		_player = Sprite3D::create(fileName);
		_player->setScale(3.0f);
		_player->setPosition(Vec2(0, 0));
		_player->setGlobalZOrder(1);
		_player->setRotation3D(Vec3(0, 180, 0));
		addChild(_player);
		//加载武器
		auto sp = Sprite3D::create("axe.c3b");
		sp->setGlobalZOrder(1);
		//将武器放到玩家手上
		_player->getAttachNode("Bip001 R Hand")->addChild(sp);
		//获取骨骼动画信息
		auto animation = Animation3D::create(fileName);
		if (animation)
		{
			auto _idle = Animate3D::create(animation);
			_idle->retain();
			//让精灵循环播放动作
			Sequence* pSequence = Sequence::create(_idle, NULL);
			_player->runAction(RepeatForever::create(pSequence));
		}

		bMoving = false;
		PathCurrentStep = 0;
		PathSteps = 0;
		ret = true;
	} while (0);

	return ret;
}



void Role::setState(ActionState actionState)
{
	this->stopAllActions();
	switch (actionState)
	{
	case ActionState::IDLE_STATE_UP:	
		_player->setRotation3D(Vec3(0, 0, 0));
		break;
	case ActionState::IDLE_STATE_DOWN:
		_player->setRotation3D(Vec3(0, 180, 0));
		break;
	case ActionState::IDLE_STATE_LEFT:
		_player->setRotation3D(Vec3(0, 90, 0));
		break;
	case ActionState::IDLE_STATE_RIGHT:
		_player->setRotation3D(Vec3(0, -90, 0));
		break;
	case ActionState::IDLE_STATE_LEFTUP:
		_player->setRotation3D(Vec3(0, -135, 0));
		break;
	case ActionState::IDLE_STATE_RIGHTUP:
		_player->setRotation3D(Vec3(0, 45, 0));
		break;
	case ActionState::IDLE_STATE_LEFTDOWN:
		_player->setRotation3D(Vec3(0, -45, 0));
		break;
	case ActionState::IDLE_STATE_RIGHTDOWN:
		_player->setRotation3D(Vec3(0, 135, 0));
		break;
	case ActionState::RUN_STATE_UP:
		_player->setRotation3D(Vec3(0, 0, 0));
		break;
	case ActionState::RUN_STATE_DOWN:
		_player->setRotation3D(Vec3(0, 180, 0));
		break;
	case ActionState::RUN_STATE_LEFT:
		_player->setRotation3D(Vec3(0, 90, 0));
		break;
	case ActionState::RUN_STATE_RIGHT:
		_player->setRotation3D(Vec3(0, -90, 0));
		break;
	case ActionState::RUN_STATE_LEFTUP:
		_player->setRotation3D(Vec3(0, -135, 0));
		break;
	case ActionState::RUN_STATE_RIGHTUP:
		_player->setRotation3D(Vec3(0, -45, 0));
		break;
	case ActionState::RUN_STATE_LEFTDOWN:
		_player->setRotation3D(Vec3(0, 45, 0));
		break;
	case ActionState::RUN_STATE_RIGHTDOWN:
		_player->setRotation3D(Vec3(0, 135, 0));
		break;
	default:
		break;
	}
	currActionState = actionState;
}


void Role::moveToPosition(int x, int y)
{ 
    this->stopAllActions(); 
    Vec2 target_pos = m_graph.getPositionByTileCoord( x, y);
    float duration = target_pos.distance(this->getPosition()) / m_speed;
    if (duration > 0)
    {
        AILOG("moveToPosition: (%d, %d), duration=%f", x, y, duration);
        this->runAction(MoveTo::create(duration, target_pos));
    }
}

void Role::moveToPath(std::vector<GraphPoint> &path)
{
    this->stopAllActions();
      
    Vector<FiniteTimeAction* > Actions;
    Vec2 curPos = this->getPosition();
    Vec2 nextPos;
    for (int i=path.size()-1; i>=0; i--)
    {
        GraphPoint p = path[i];
        //AILOG("PATH: %d, %d", p.x, p.y);
        nextPos = m_graph.getPositionByTileCoord( p.x, p.y);
        float duration = nextPos.distance(curPos) / m_speed;
        
        FiniteTimeAction *pAct = MoveBy::create(duration, Vec2(nextPos.x-curPos.x, nextPos.y-curPos.y));        
        Actions.pushBack(pAct);

        curPos = nextPos;

        //显示路径测试用
        //auto icon = Sprite::create("res/walk2.png");
        //icon->setPosition(nextPos);
        //this->getParent()->addChild(icon);
    }

    if (Actions.size() > 0)
    {
        auto pSeq = Sequence::create(Actions);
        this->runAction(pSeq); 
    }



#if 0
        //角色当前所在格子坐标
    	auto currpos = tileCoordForPosition(this->getPosition());
    	if (currpos.x==this->path.at(PathSteps)->x && currpos.y == this->path.at(PathSteps)->y)
    	{
    		//已到达目的地
    		this->setState(ActionState::IDLE_STATE_UP);
    	} 
    	else
    	{
    		for (int i = 0; i < PathSteps; i++)
    		{
    			auto netStep = Vec2(this->path.at(i + 1)->x, this->path.at(i + 1)->y);
    			Vec2 v = Vec2(this->path.at(i)->x, this->path.at(i)->y)-netStep;
    			float len = v.getLength();// 计算每一段的距离
    			float duration = len / 10 * 1.2; //计算每段行走的时间
    			float rad = v.getAngle(Vec2(1, 1));
    			float Degree = CC_RADIANS_TO_DEGREES(rad);
    			log("the degree=========> %f",Degree);


    			if (Degree >= 0 && Degree < 45)
    			{
    				setState(ActionState::RUN_STATE_LEFT);
    			}
    			if (Degree >= 45 && Degree < 90)
    			{
    				setState(ActionState::RUN_STATE_LEFTDOWN);
    			}
    			if (Degree >= 90 && Degree < 135)
    			{
    				setState(ActionState::RUN_STATE_RIGHTDOWN);
    			}
    			if (Degree >= 135 && Degree < 180)
    			{
    				setState(ActionState::RUN_STATE_RIGHT);
    			}
    			if (Degree >-135 && Degree <= -180)
    			{
    				setState(ActionState::RUN_STATE_RIGHTUP);
    			}

    			FiniteTimeAction *pAct = MoveTo::create(duration, positionForTileCoord(netStep));
    			Actions.pushBack(pAct);
                
    		}
    		if (Actions.size()<=1)
    		{
    			return;
    		}

    		auto pSeq = Sequence::create(Actions);
    		pSeq->setTag(1);
    		this->runAction(pSeq);
    		
    	}
#endif 
}


void Role::requestPathToPositon(int to_x, int to_y)
{
    if (m_AStar != NULL)
    {
        PathManager::instance()->unRegisterPlan(m_AStar);
    }
    
    Vec2 from = m_graph.getTileCoordByPosition(this->getPosition());
    
    AILOG("requestPathToPositon: from(%0.f, %0.f),  to(%d, %d)", from.x, from.y, to_x, to_y);
        
    m_beginTime = std::chrono::steady_clock::now();
     
#if 1

    GraphPoint reachPoint;

    //isLineReach(m_graph,13, 30, 20, 35, reachPoint);

    /*若目标不可达,  则寻找替换点*/
    if (!m_graph.isWalkable(to_x, to_y))
    {
        GraphPoint p = m_graph.getReplaceablePoint((int)from.x,  (int)from.y, to_x, to_y);
        to_x = p.x;
        to_y = p.y;
        
        AILOG("get replayceable point: (%d, %d)", p.x, p.y);
        
        auto icon = Sprite::create("res/walk2.png");
        icon->setPosition(m_graph.getPositionByTileCoord(p.x, p.y));
        this->getParent()->addChild(icon);
    }

    
    if (isLineReach(m_graph, (int)from.x,  (int)from.y, to_x, to_y, reachPoint))
    {
        moveToPosition(to_x, to_y);
        
        auto duration_s = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_beginTime).count() /1000.0f;
        AILOG("line reach time cost : %f s ", duration_s);        
    }
    else
    {  
        //using A star
        m_AStar = new AStarTimeSliced<CostCaculator_Astar>(m_graph, (int)from.x,  (int)from.y, to_x, to_y, [=](int result){
        
                                                            auto duration_s = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_beginTime).count() / 1000.0f;
                                                            AILOG("a-star path find time cost : %f s ", duration_s); 
                                                            
                                                            if (result == target_found)
                                                            {
                                                                Vec2 curPoint = m_graph.getTileCoordByPosition(this->getPosition());
                                                                
                                                                auto paths = m_AStar->getPath();
                                                                
                                                                //because A star not include source point, and if pre-go while searching, so source point and current position should be consider while smooth.
                                                                paths.push_back(GraphPoint(from.x, from.y)); 
                                                                paths.push_back(GraphPoint(curPoint.x, curPoint.y));

                                                                if (PathManager::instance()->isPreGotoTarget())
                                                                {
                                                                    std::vector<GraphPoint> tmp = getLineGrids(m_graph,(int)from.x,  (int)from.y, (int)curPoint.x, (int)curPoint.y);
                                                                    paths = mergePath(m_graph, tmp, paths);
                                                                } 
                                                                else 
                                                                {
                                                                    smoothPath(m_graph, paths);
                                                                }
                                                                
                                                                moveToPath(paths);
                                                            }
                                                        });
        PathManager::instance()->registerPlan(m_AStar); 

        //pre -go while searching
        if (PathManager::instance()->isPreGotoTarget())
        {
            moveToPosition(reachPoint.x, reachPoint.y);
        }
    }




#else 

    /*
        使用Dijkstras  搜索指定目标坐标时比A 星效率低;  
        适用于搜索附近某一类型的物品, 或者地图上各个点到当前位置的最优路径. 
        只需要修改CostCaculator_Dijkstras::isSatisfied() 方法.
     */
    m_dijkstras = new DijkstrasTimeSliced<CostCaculator_Dijkstras>(m_graph, (int)from.x,  (int)from.y, to_x, to_y, [=](int result){
        
                                                        AILOG("path find result: %d ", result);
                                                        if (result == target_found)
                                                        {
                                                            auto paths = m_dijkstras->getPath();
                                                            
                                                            moveToPath(paths);
                                                        }
                                                    });
    PathManager::instance()->registerPlan(m_dijkstras);
#endif 
}




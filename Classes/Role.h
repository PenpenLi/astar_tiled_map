#ifndef _ROLE_H_
#define _ROLE_H_
#include "cocos2d.h"


#include "common/navigation/PathManager.h"
#include "common/navigation/CostCaculator_Astar.h"
#include "common/navigation/CostCaculator_Dijkstras.h"
#include <vector>

USING_NS_CC;
//using namespace std;
    
//基础角色类，主角和NPC都需要继承它


class Role :public Node
{
public:
	Role(GraphMap &graph);
	~Role(void);
	/* 角色状态设定,初始化角色状态等 */
	CC_SYNTHESIZE(std::string, name, name);							//角色名称
	CC_SYNTHESIZE(float, curtLifevalue, CurtLifeValue);				//角色当前生命值
	CC_SYNTHESIZE(float, sumLifevalue, SumLifeValue);					//角色总体生命值

public:
	enum class ActionState {
		ACTION_STATE_IDLE = 0,
		RUN_STATE_UP,
		RUN_STATE_DOWN,
		RUN_STATE_LEFT,
		RUN_STATE_RIGHT,
		RUN_STATE_LEFTUP,
		RUN_STATE_RIGHTUP,
		RUN_STATE_LEFTDOWN,
		RUN_STATE_RIGHTDOWN,
		IDLE_STATE_UP,
		IDLE_STATE_DOWN,
		IDLE_STATE_LEFT,
		IDLE_STATE_RIGHT,
		IDLE_STATE_LEFTUP,
		IDLE_STATE_RIGHTUP,
		IDLE_STATE_LEFTDOWN,
		IDLE_STATE_RIGHTDOWN,
	};

public:
	Action* m_down_idle;
	Action* m_right_idle;
	Action* m_up_idle;
	Action* m_left_idle;
	Action* m_leftdown_idle;
	Action* m_rightdown_idle;
	Action* m_leftup_idle;
	Action* m_rightup_idle;

	Action* m_down_walk;
	Action* m_right_walk;
	Action* m_up_walk;
	Action* m_left_walk;
	Action* m_leftdown_walk;
	Action* m_rightdown_walk;
	Action* m_leftup_walk;
	Action* m_rightup_walk;

	//角色初始化
	virtual bool init();
 
    
	void setState(ActionState actionState);
	bool bMoving;
	unsigned int PathCurrentStep;
	unsigned int PathSteps;
	
	ActionState currActionState;
	Sprite3D *_player;

    void moveToPosition(int x, int y); 
    void moveToPath(std::vector<GraphPoint> &path);

    
    void requestPathToPositon(int to_x, int to_y);
private:
    AStarTimeSliced<CostCaculator_Astar> *m_AStar;

    DijkstrasTimeSliced<CostCaculator_Dijkstras> *m_dijkstras;
    
    GraphMap &m_graph;

    float m_speed;

    std::chrono::steady_clock::time_point m_beginTime; /*供测试算法耗时*/
};
#endif
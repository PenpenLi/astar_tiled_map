
#ifndef PATHSMOOTH_H
#define PATHSMOOTH_H

#include "PathLineTest.h"
#include <algorithm>


/*注意: 这里处理的路径点points 数组, 存储顺序统一为: 目标== points[0]*/






/*判断3 点是否共线*/
inline bool isOnLine(GraphMap &graph, float x1, float y1, float x2, float y2, float x3, float y3)
{
    bool result = false;

    if(graph.isMapTypeOfStaggered())
    {
        float ratio = graph.getGridWidth()/graph.getGridHeight();
        x1 = (x1 + (((int)y1)&1)*0.5)*ratio;
        y1 = y1*0.5;
        x2 = (x2 + (((int)y2)&1)*0.5)*ratio;
        y2 = y2*0.5;
        x3 = (x3 + (((int)y3)&1)*0.5)*ratio;
        y3 = y3*0.5;    
    }

    return IS_ZERO((y2-y1)*(x3-x2)-(y3-y2)*(x2-x1));
}


/*points : 数组存储顺序: target----->source , 目标点位于下标0 */
inline void smoothPath(GraphMap &graph, std::vector<GraphPoint> &points)
{
    if (points.size() < 3 ) 
    {
        return;
    }
    
    // 将共线的点合并
    GraphPoint p1, p2, p3;
    for (std::vector<GraphPoint>::iterator it = points.begin()+2; it != points.end(); it++)
    {
        p1 = *(it-2);
        p2 = *(it-1);
        p3 = *it;
        if (isOnLine(graph, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y))
        {
            it = points.erase(it-1);
        }

        if (it == points.end()) break;
    }
    
    /* 判断是否可跳过下个点直接到达下下个点, 如果可以则移除中间点;
      * 因为目标点保存在数组前面, 所以采取反向遍历, 从源点开始润滑; 
      * 注意删除方法.
      */
    for (std::vector<GraphPoint>::reverse_iterator it = points.rbegin()+2; it != points.rend(); it++)
    {
        p3 = *(it);
        p1 = *(it-2);
        
        if (isLineReach(graph, p1.x, p1.y, p3.x, p3.y))
        {
            std::vector<GraphPoint>::reverse_iterator it2 = it-1;
            it = std::vector<GraphPoint>::reverse_iterator(points.erase((++it2).base())); //notice !!!
        }

        if (it == points.rend()) break;
    } 
}


/*合并路径并且润滑; 
 * 方法:  找出points1 的源点和目标点与points2 中的点连线
    points : 数组存储顺序: target----->source , 目标点位于下标0 ;
 */
inline std::vector<GraphPoint> mergePath(GraphMap &graph, std::vector<GraphPoint> &points1, std::vector<GraphPoint> &points2)
{
    
    std::vector<GraphPoint> path;
    
    std::vector<GraphPoint>::reverse_iterator it_back1 = points2.rbegin();
    std::vector<GraphPoint>::reverse_iterator it_back2;
    
    int distanceMin = 99999999;
    for (auto it1 = points1.rbegin(); it1 != points1.rend(); it1++)
    {
        path.push_back(*it1);
        
        for (auto it2 = points2.rbegin()+1; it2 != points2.rend(); it2++)
        {
            if (isLineReach(graph, (*it1).x, (*it1).y, (*it2).x, (*it2).y))
            {
                it_back1 = it2;
            }
            else 
            {
                break;
            }            
        } 

        int length = getPointDistanceSq(graph, (*it1), (*it_back1)) + getPointDistanceSq(graph, (*it_back1), points2.front());
        
        if (length < distanceMin)
        {
            distanceMin = length;
            it_back2 = it_back1;
        }
        else 
        {
            break;
        }
    }
    
    path.push_back(*it_back2);
    path.insert(path.end(), it_back2, points2.rend());
    
    reverse(path.begin(), path.end());
    smoothPath(graph, path);
    
    return path;
    
}



#endif 


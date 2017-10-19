
//���ٲ�ѯ�Ƿ��ֱ�ߵ���Ŀ��, �����ڲ���Ҫ��ʱ����Ȼʹ��A ������
#ifndef PATH_LINE_TEST_H
#define PATH_LINE_TEST_H 

#include <vector>
#include "GraphMap.h"

#include "common/LogDebug.h"

/* ˼·: (��ֱ�ǵ�ͼ) ����յ�λ�ڵ�Ԫ������;  ��ֱ����x ��н���-45 ~ 45 ��֮��ʱ, 
  * ���ú���������ȡ����y ����, ��ʱ������ƫ��0.5, 1, ... 1, 0.5 ;  
  * ��֮������ ����ȡ����x ����, ��ʱ������ƫ��0.5, 1, ... 1, 0.5 ;  
  * ����ĸ�������, ������һ��������(������յ�), Ҳ����������������
  * ����(�罻���ڸ��ӵı���), Ҳ����4 �����ӹ���(�罻���ڸ��Ӷ���), 
  * �ֱ������صĸ����Ƿ�����ϰ���, �����������, �򷵻�true 
*/




#define NUM_ROUND(a) (int)(a+ (a>0 ? 0.5 : -0.5))  //��������
#define NUM_TAIL(a) (a-(int)(a))  //ȡС���㲿��
#define IS_ZERO(a) ( a > -0.001 && a < 0.001)



/*���ݽ���(��ͼ)���������صĸ���*/
inline std::vector<GraphPoint> getGridsByNormalPoint(GraphMap &graph, float x, float y)
{
    std::vector<GraphPoint> grids;
    
    float tail_x = NUM_TAIL(x); 
    float tail_y = NUM_TAIL(y); 

    bool is_ver_x = IS_ZERO(tail_x - 0.5) || IS_ZERO(tail_x + 0.5); // x �Ƿ�λ�ڸ��Ӷ���
    bool is_ver_y = IS_ZERO(tail_y - 0.5)||IS_ZERO(tail_y + 0.5);  // y �Ƿ�λ�ڸ��Ӷ���

    
    if (is_ver_x && is_ver_y) //λ�ڸ��Ӷ���, ��Χ��4������
    {
        grids.push_back(GraphPoint(NUM_ROUND(x-0.5),  NUM_ROUND(y-0.5)));//����
        grids.push_back(GraphPoint(NUM_ROUND(x+0.5),  NUM_ROUND(y-0.5))); //����
        grids.push_back(GraphPoint(NUM_ROUND(x-0.5),  NUM_ROUND(y+0.5)));//����
        grids.push_back(GraphPoint(NUM_ROUND(x+0.5),  NUM_ROUND(y+0.5)));//����
    }
    else if (is_ver_x && !is_ver_y) //���Ҹ����м�
    {
        grids.push_back(GraphPoint(NUM_ROUND(x-0.5),  NUM_ROUND(y)));//��
        grids.push_back(GraphPoint(NUM_ROUND(x+0.5),  NUM_ROUND(y)));//��
    }
    else if (!is_ver_x && is_ver_y) //���¸����м�
    {
        grids.push_back(GraphPoint(NUM_ROUND(x),  NUM_ROUND(y-0.5))); //��
        grids.push_back(GraphPoint(NUM_ROUND(x),  NUM_ROUND(y+0.5)));//��     
    }
    else //λ�ڸ���������Ϊԭ����յ�
    {
        grids.push_back(GraphPoint(NUM_ROUND(x), NUM_ROUND(y))); 
    }

    //���Խ��ĵ�
    for (std::vector<GraphPoint>::iterator it = grids.begin(); it != grids.end(); it++)
    {
        if (!graph.isPointInMap((*it).x, (*it).y))
        {
            it = grids.erase(it);
        }
        
        if (it == grids.end()) break;
    }

    return grids;
}


inline std::vector<GraphPoint> getGridsByISOPoint(GraphMap &graph, float x, float y)
{
    std::vector<GraphPoint> grids;

    Vec2 pos = graph.getTileCoordByPosition(Vec2(x, y));
    
    int halfGridW = graph.getGridWidth()/2;
    int halfGridH = graph.getGridHeight()/2;
    
    int isInt_x = IS_ZERO(NUM_TAIL(x/halfGridW)); 
    int isInt_y = IS_ZERO(NUM_TAIL(y/halfGridH));

    grids.push_back(GraphPoint(pos.x, pos.y));
    
    if (isInt_x && isInt_y)
    {
        int int_count_x = (int)(x /halfGridW);
        int int_count_y = (int)(y /halfGridH);
        
        //x, yͬΪ������ż��ʱ, ��Ӧ���Ƕ���λ��, ����Ҫ�����, ��, �Ҷ���
        if ((int_count_x+int_count_y) % 2 == 0 ) 
        {
            grids.push_back(GraphPoint(pos.x-1, pos.y));//left 
            grids.push_back(GraphPoint(pos.x, pos.y-1));//right
            grids.push_back(GraphPoint(pos.x-1, pos.y-1));//up
        }
    }
    
    //���Խ��ĵ�
    for (std::vector<GraphPoint>::iterator it = grids.begin(); it != grids.end(); it++)
    {
        if (!graph.isPointInMap((*it).x, (*it).y))
        {
            it = grids.erase(it);
        }

        if (it == grids.end()) break;
    }

    return grids;
}



inline std::vector<GraphPoint> getGridsByStaggeredPoint(GraphMap &graph, float x, float y)
{
    
    std::vector<GraphPoint> grids;
    
    float ratio = graph.getGridWidth()/graph.getGridHeight();
    float ratio_half = 0.5*ratio;
    
    bool isInt_x = NUM_TAIL(x/ratio_half) == 0.0;
    bool isInt_y = NUM_TAIL(y/0.5) == 0.0;
    int int_count_x = (int)(x/ratio_half);
    int int_count_y = (int)(y/0.5);

     /*��õĳ�ʼ��������, ����Ҫ����ʵ��΢��*/
    int grid_x = (int)(x/ratio);
    int grid_y = int_count_y;

    if (isInt_x && isInt_y)  /*��x Ϊratio/2 ��������y Ϊ0.5 ������ʱ, ����Ϊ�����м�򶥵� */
    {
        if ((int_count_x + int_count_y)%2 == 0 )  /*int_count_x, int_count_yͬΪ��/ż��ʱ, ����Ϊ��������*/
        {
            grids.push_back(GraphPoint(grid_x, grid_y));
        } 
        else //����, �����Χ�ĸ�����
        {  
            //AILOG("check ver: (%d, %d)   (%d, %d)  (%d, %d)  (%d, %d) ",
            //    grid_x, grid_y-1, grid_x, grid_y+1, grid_x+(int_count_x&1)-1, grid_y, grid_x+(int_count_x&1), grid_y);
            grids.push_back(GraphPoint(grid_x, grid_y-1)); //up
            grids.push_back(GraphPoint(grid_x, grid_y+1));  //down
            grids.push_back(GraphPoint(grid_x+(int_count_x&1)-1, grid_y)); //left
            grids.push_back(GraphPoint(grid_x+(int_count_x&1), grid_y));//right 
        }
    }
    else if (isInt_x && !isInt_y)
    {
        if (int_count_x %2 == 0) 
        {
            grid_y = int_count_y + (int_count_y&1);
        }
       else 
        {
            grid_y = int_count_y + 1 - (int_count_y&1);
        }
       grids.push_back(GraphPoint(grid_x, grid_y));
    }
    else if (!isInt_x && isInt_y)
    {
        if (int_count_y%2==0)
        {
            grid_x = (int)((x+ratio_half)/ratio);            
        }
        grids.push_back(GraphPoint(grid_x, grid_y));
    }
    else 
    {
        assert(false);
    }

    //���Խ��ĵ�
    for (std::vector<GraphPoint>::iterator it = grids.begin(); it != grids.end(); it++)
    {
        if (!graph.isPointInMap((*it).x, (*it).y))
        {
            it = grids.erase(it);
        }
        
        if (it == grids.end()) break;
    }
    
    return grids;
}


/*��ȡ�߶ξ�����������ظ���*/
inline std::vector<GraphPoint> getLineGridsForNormal(GraphMap &graph,
                                                                                            int source_x,
                                                                                            int source_y,
                                                                                            int target_x, 
                                                                                            int target_y)
{
    std::vector<GraphPoint> grids;
    
    assert(graph.isMapTypeOfNormal());

    grids.push_back(GraphPoint(source_x, source_y));
    
    
    //��������ֱ�ߵ�һ�㷽��ax + by + c = 0 �Ľ�: 
    // a = y2 - y1  ,  b = x1 - x2 , c = x2 * y1 - x1 * x2 
    float a = (float)(target_y - source_y);
    float b = (float)(source_x - target_x);
    float c = (float)(target_x * source_y - source_x * target_y);
    
    //�жϱ�������
    int dist_x = target_x > source_x ? (target_x - source_x) : (source_x - target_x);
    int dist_y = target_y > source_y ? (target_y - source_y) : (source_y - target_y);

    bool isLoopHorizontal = dist_x >= dist_y ; /*�Ƿ�Ϊˮƽ�������*/
    
    if (isLoopHorizontal) //ˮƽ�����󽻵�
    {
        if (b != 0 )
        {
            float direction = source_x < target_x ? 1.0 : -1.0;
            float offset = direction * 0.5;
            for (float x = source_x+offset; (direction>0&&x<=target_x)||(direction<0&&x>=target_x) ; x+=(2*offset))
            {
                 float y = (-c-a*x)/b;
                 
                auto tmp = getGridsByNormalPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
                
                if (x + offset ==  target_x) /*��ֹ©���յ�(��Ϊ����Ϊ2*offset ) */
                {
                    x -= offset;
                }
            }
        }
    }
    else 
    {
        if (a != 0)
        {
            float direction = source_y < target_y ? 1.0 : -1.0;
            float offset = direction * 0.5;
            for (float y = source_y+offset; (direction>0&&y<=target_y)||(direction<0&&y>=target_y); y+=(2*offset))
            {
                float x = (-c - b * y)/a;

                auto tmp = getGridsByNormalPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
                
                if (y + offset ==  target_y) /*��ֹ©���յ�(��Ϊ����Ϊ2*offset ) */
                {
                    y -= offset;
                }            
            }
        }
    }
    
    return grids;
}

/*��ȡ�߶ξ�����������ظ���*/
inline std::vector<GraphPoint> getLineGridsForISO(GraphMap &graph,
                                                                                            int source_x,
                                                                                            int source_y,
                                                                                            int target_x, 
                                                                                            int target_y) 
{
    std::vector<GraphPoint> grids;
    
    assert(graph.isMapTypeOfISO()); 

    grids.push_back(GraphPoint(source_x, source_y));
    
    Vec2 src = graph.getPositionByTileCoord(source_x, source_y);
    Vec2 dst = graph.getPositionByTileCoord(target_x, target_y);

    //��������ֱ�ߵ�һ�㷽��ax + by + c = 0 �Ľ�: 
    // a = y2 - y1  ,  b = x1 - x2 , c = x2 * y1 - x1 * x2 
    float a = dst.y - src.y;
    float b = src.x - dst.x;
    float c = dst.x * src.y - src.x * dst.y;

    
    //�жϱ�������
    float ratio = graph.getGridWidth()/graph.getGridHeight();
    float len_x = dst.x > src.x ? (dst.x - src.x) : (src.x - dst.x);
    float len_y = dst.y > src.y ? (dst.y - src.y) : (src.y - dst.y);
    
    bool isLoopHorizontal = len_x > ratio*len_y ; /*�Ƿ�Ϊˮƽ�������*/
    
    if (isLoopHorizontal)
    {
        assert(b != 0);
        if (b != 0 )
        {
            float direction = src.x < dst.x ? 1.0 : -1.0;
            float offset = direction * graph.getGridWidth()/2;
            for (float x = src.x+offset; (direction>0&&x<=dst.x)||(direction<0&&x>=dst.x) ; x+=offset)
            {
                float y = (-c-a*x)/b;
                
                auto tmp = getGridsByISOPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
            } 
        }
    }
    else 
    {
        /*��ֱ�������*/ 
        if (a != 0 )
        {
            float direction = src.y < dst.y ? 1.0 : -1.0;
            float offset = direction * graph.getGridHeight()/2;
            for (float y = src.y+offset; (direction>0&&y<=dst.y)||(direction<0&&y>=dst.y); y+=offset)
            {
                float x = (-c - b * y)/a;

                auto tmp = getGridsByISOPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
            }
        }
    }
    
    return grids;
}

/*��ȡ�߶ξ�����������ظ���*/

/*�ж�45 �����ν����ͼ��ĳ���������漰�ĸ����Ƿ������
    ����x, y Ϊ�����任�����������, �ǵ�ͼ����,����, Ҫôx Ϊ
    ratio/2 ������, Ҫôy Ϊ0.5 ��������
*/
inline std::vector<GraphPoint> getLineGridsForStaggered(GraphMap &graph,
                                                                                            int source_x,
                                                                                            int source_y,
                                                                                            int target_x, 
                                                                                            int target_y) 
{
    std::vector<GraphPoint> grids;
    
    assert(graph.isMapTypeOfStaggered()); 

    grids.push_back(GraphPoint(source_x, source_y));

    float ratio = graph.getGridWidth()/graph.getGridHeight();
    
    /*  ���������Ҵ�������ӿ��*/ 
    float src_x = (source_x + (source_y&1)*0.5) *ratio;
    float src_y = source_y*0.5;
    float dst_x = (target_x + (target_y&1)*0.5) * ratio; 
    float dst_y = target_y * 0.5;

    //��������ֱ�ߵ�һ�㷽��ax + by + c = 0 �Ľ�: 
    // a = y2 - y1  ,  b = x1 - x2 , c = x2 * y1 - x1 * x2 
    float a = dst_y - src_y;
    float b = src_x - dst_x;
    float c = dst_x * src_y - src_x * dst_y;

    //�жϱ�������
    float len_x = dst_x > src_x ? (dst_x - src_x) : (src_x - dst_x);
    float len_y = dst_y > src_y ? (dst_y - src_y) : (src_y - dst_y);
    
    bool isLoopHorizontal = len_x >= ratio*len_y ; /*�Ƿ�Ϊˮƽ�������*/
    
    if (isLoopHorizontal) //ˮƽ�����󽻵�
    {
        if (b != 0 )
        {
            float direction = src_x < dst_x ? 1.0 : -1.0;
            float offset = direction * 0.5*ratio;
            for (float x = src_x+offset; (direction>0&&x<=dst_x)||(direction<0&&x>=dst_x) ; x+=offset)
            {
                float y = (-c-a*x)/b;

                auto tmp = getGridsByStaggeredPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
            }
        }
    }    
    else 
    {
        if (a != 0 )
        {
            float direction = src_y < dst_y ? 1.0 : -1.0;
            float offset = direction * 0.5;
            for (float y = src_y+offset; (direction>0&&y<=dst_y)||(direction<0&&y>=dst_y); y+=offset)
            {
                float x = (-c - b * y)/a;

                auto tmp = getGridsByStaggeredPoint(graph, x, y);
                grids.insert(grids.end(), tmp.begin(), tmp.end()); //�ϲ�׷��
            }
        }
    }

    return grids;
}

/*����֮��ֱ�߾��������и���(����ԭ��Ŀ���)*/
inline std::vector<GraphPoint> getLineGrids(GraphMap &graph,
                                                                        int source_x,
                                                                        int source_y,
                                                                        int target_x, 
                                                                        int target_y) 
{
    std::vector<GraphPoint> grids;
    
    if (graph.isMapTypeOfNormal())
    {
        grids = getLineGridsForNormal(graph, source_x, source_y, target_x, target_y);
    }
    else if (graph.isMapTypeOfISO())
    {
        grids = getLineGridsForISO(graph, source_x, source_y, target_x, target_y);
    }
    else if (graph.isMapTypeOfStaggered())
    {
        grids = getLineGridsForStaggered(graph, source_x, source_y, target_x, target_y);
    }
    else
    {
        assert(false);
    }

    return grids;
}


/* �ж��Ƿ��ֱ�ߵ���Ŀ���, ���ҷ����������ߵ�(��Ŀ������ĸ���) ����*/
inline bool isLineReach(GraphMap &graph,
                                        int source_x,
                                        int source_y,
                                        int target_x, 
                                        int target_y, 
                                        GraphPoint &reachPoint)
{
    bool ret = true;
    std::vector<GraphPoint> points = getLineGrids(graph, source_x, source_y, target_x, target_y);

    for (auto &p: points)
    {
        if (!graph.isWalkable(p.x, p.y))
        {
            ret = false;
            break;
        }
        reachPoint = GraphPoint(p.x, p.y);
    }
    
    return ret;
}


/* �ж��Ƿ��ֱ�ߵ���Ŀ���*/
inline bool isLineReach(GraphMap &graph,
                                        int source_x,
                                        int source_y,
                                        int target_x, 
                                        int target_y)
{
    GraphPoint tmp;
    return isLineReach(graph, source_x, source_y, target_x, target_y, tmp);
}


inline bool getLineCrossPoint(GraphMap &graph, 
                                                    GraphPoint p1,
                                                    GraphPoint p2, 
                                                    GraphPoint p3, 
                                                    GraphPoint p4, 
                                                    GraphPoint &crossPoint)
{
    Vec2 src1 = graph.getPositionByTileCoord(p1.x, p1.y);
    Vec2 dst1 = graph.getPositionByTileCoord(p2.x, p2.y);
    Vec2 src2 = graph.getPositionByTileCoord(p3.x, p3.y);
    Vec2 dst2 = graph.getPositionByTileCoord(p4.x, p4.y);
    
    //��������ֱ�ߵ�һ�㷽��ax + by + c = 0 �Ľ�: 
    // a = y2 - y1  ,  b = x1 - x2 , c = x2 * y1 - x1 * x2 
    float a1 = dst1.y - src1.y;
    float b1 = src1.x - dst1.x;
    float c1 = dst1.x * src1.y - src1.x * dst1.y;    

    float a2 = dst2.y - src2.y;
    float b2 = src2.x - dst2.x;
    float c2 = dst2.x * src2.y - src2.x * dst2.y;    

    if ((a1 *b2 == a2 * b1) ) /*ֱ��ƽ��*/
    {
        return false;
    }
    
   // if (a1*c2 == a2*c1) /*ֱ���غ�*/
   // {
   // }
    
    float x = (b1*c2-b2*c1)/(a1*b2-a2*b1);
    float y = (a1*c2-a2*c1)/(a2*b1-a1*b2);

    Vec2 point = graph.getTileCoordByPosition(Vec2(x, y));
    
    crossPoint = GraphPoint(point.x, point.y);
        
    return true;
}


inline int getPointDistanceSq(GraphMap &graph, GraphPoint p1, GraphPoint p2)
{
    Vec2 src = graph.getPositionByTileCoord(p1.x, p1.y);
    Vec2 dst = graph.getPositionByTileCoord(p2.x, p2.y);
    return ((src.x-dst.x)*(src.x-dst.x) + (src.y-dst.y)*(src.y-dst.y));
}


#endif 


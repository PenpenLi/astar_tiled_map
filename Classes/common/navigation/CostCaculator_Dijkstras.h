
#ifndef COSTCACULATOR_DIJKSTRAS_H
#define COSTCACULATOR_DIJKSTRAS_H


class  CostCaculator_Dijkstras 
{
private:
    enum ValCost
    {
        kCost10 = 10, /*正方形地图横向移动一格*/
        kCost14 = 14, /*正方形地图斜向移动一格*/
        kCost20 = 20,  /*( 45度交错)横向移动一格*/
        kCost12 = 12,  /*( 45度交错)斜向移动一格*/
        kCost5 = 5,     /*( 45度交错)上下移动一格*/
    };

    
public:
    CostCaculator_Dijkstras() {}

    template <class graph>
    static int caculateG(graph &G, int from_x, int from_y, int to_x, int to_y)
    {
        int cost = 0;
        
        if (G.isMapTypeOfNormal()) /*正方形*/
        {
            cost = (abs(from_x - to_x) + abs(from_y - to_y)) == 1 ? kCost10 : kCost14;
        }
        else if (G.isMapTypeOfStaggered()) /* 45 度菱形交错*/
        {
            if (from_y == to_y) /* 左右*/
            {
                cost = kCost20;
            }
            else if (from_y + 2 == to_y || from_y - 2 == to_y) /*上下*/
            {
                cost = kCost5 * 2;
            }
            else /*斜向*/
            {
                cost = kCost12;
            }
        }

        return cost;
    }

    template <class graph>
    static bool isSatisfied(graph &G, int x, int y, int target_x, int target_y)
    {
        if (x == target_x && y == target_y)
        {
            return true;
        }
        
        return false;
    }
    
};


#endif 


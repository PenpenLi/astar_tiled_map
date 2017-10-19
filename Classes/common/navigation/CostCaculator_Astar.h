
#ifndef COSTCACULATOR_ASTAR_H
#define COSTCACULATOR_ASTAR_H


class  CostCaculator_Astar 
{
private:
    enum ValCost
    {
        kCost10 = 10, /*正方形地图横向移动一格*/
        kCost14 = 14, /*正方形地图斜向移动一格*/
        kCost20 = 20,  /*( 45度交错)横向移动一格*/
        kCost12 = 12,  /*( 45度交错)斜向移动一格*/
        kCost5 = 5,     /*( 45度交错)上下移动一格*/
        kCost60 = 60, /*45度横向移动一格*/
        kCost30 = 30, /*45度横向移动半格或者上下移动1 格*/
        kCost15 = 15, /*45度上下移动半格*/
        kCost33 = 33, /*45度斜向移动一格*/
    };

    
public:
    CostCaculator_Astar() {}

    template <class graph>
    static int caculateG(graph &G, int from_x, int from_y, int to_x, int to_y) /*相邻节点计算G */
    {
        int cost = 0;
        
        if (G.isMapTypeOfNormal()) /*正方形*/
        {
            cost = (abs(from_x - to_x) + abs(from_y - to_y)) == 1 ? kCost10 : kCost14;
        }
        else if (G.isMapTypeOfISO()) /* 45 度菱形*/
        {
            if (from_x == to_x || from_y == to_y) /*斜向*/
            {
                cost = kCost33;
            }
            else if (from_x - to_x == from_y - to_y)/*上下*/
            {
                cost = kCost30;
            }
            else /*左右*/
            {
                cost = kCost60;
            }
        }
        else if (G.isMapTypeOfStaggered()) /* 45 度菱形交错*/
        {
            if (from_y == to_y) /* 左右*/
            {
                cost = kCost20;
            }
            else if (from_y + 2 == to_y || from_y - 2 == to_y) /*上下*/
            {
                cost = kCost10;//kCost5 * 2;
            }
            else /*斜向*/
            {
                cost = kCost12;
            }
        }

        return cost;
    }

    template <class graph>
    static int caculateH(graph &G, int from_x, int from_y, int to_x, int to_y)
    {
        int H = 0;

        //使用曼哈顿距离来估算
        if (G.isMapTypeOfNormal()) /*正方形*/
        {
            H = kCost10 * (abs(from_x - to_x) + abs(from_y - to_y));
        }
        else if (G.isMapTypeOfISO()) /* 45 度菱形*/
        {
            int tmp_x = from_x < to_x ? (to_x - from_x) : (from_x - to_x);
            int tmp_y = from_y < to_y ? (to_y - from_y) : (from_y - to_y);
            int H_x = abs(tmp_x-tmp_y)*kCost30;
            int H_y = (tmp_x+tmp_y+1)*kCost15;
            H = H_x + H_y;
        }        
        else if (G.isMapTypeOfStaggered()) /* 45 度菱形交错*/
        {
            int H_x = abs((to_x-from_x)*kCost20 + (to_y & 1 - from_y & 1) * kCost10);
            int H_y = abs(to_y - from_y)*kCost5;
            H = H_x + H_y;
        }

        return H;    
    }
    
};


#endif 


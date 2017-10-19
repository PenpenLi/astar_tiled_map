
#ifndef COSTCACULATOR_DIJKSTRAS_H
#define COSTCACULATOR_DIJKSTRAS_H


class  CostCaculator_Dijkstras 
{
private:
    enum ValCost
    {
        kCost10 = 10, /*�����ε�ͼ�����ƶ�һ��*/
        kCost14 = 14, /*�����ε�ͼб���ƶ�һ��*/
        kCost20 = 20,  /*( 45�Ƚ���)�����ƶ�һ��*/
        kCost12 = 12,  /*( 45�Ƚ���)б���ƶ�һ��*/
        kCost5 = 5,     /*( 45�Ƚ���)�����ƶ�һ��*/
    };

    
public:
    CostCaculator_Dijkstras() {}

    template <class graph>
    static int caculateG(graph &G, int from_x, int from_y, int to_x, int to_y)
    {
        int cost = 0;
        
        if (G.isMapTypeOfNormal()) /*������*/
        {
            cost = (abs(from_x - to_x) + abs(from_y - to_y)) == 1 ? kCost10 : kCost14;
        }
        else if (G.isMapTypeOfStaggered()) /* 45 �����ν���*/
        {
            if (from_y == to_y) /* ����*/
            {
                cost = kCost20;
            }
            else if (from_y + 2 == to_y || from_y - 2 == to_y) /*����*/
            {
                cost = kCost5 * 2;
            }
            else /*б��*/
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


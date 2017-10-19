
#ifndef COSTCACULATOR_ASTAR_H
#define COSTCACULATOR_ASTAR_H


class  CostCaculator_Astar 
{
private:
    enum ValCost
    {
        kCost10 = 10, /*�����ε�ͼ�����ƶ�һ��*/
        kCost14 = 14, /*�����ε�ͼб���ƶ�һ��*/
        kCost20 = 20,  /*( 45�Ƚ���)�����ƶ�һ��*/
        kCost12 = 12,  /*( 45�Ƚ���)б���ƶ�һ��*/
        kCost5 = 5,     /*( 45�Ƚ���)�����ƶ�һ��*/
        kCost60 = 60, /*45�Ⱥ����ƶ�һ��*/
        kCost30 = 30, /*45�Ⱥ����ƶ������������ƶ�1 ��*/
        kCost15 = 15, /*45�������ƶ����*/
        kCost33 = 33, /*45��б���ƶ�һ��*/
    };

    
public:
    CostCaculator_Astar() {}

    template <class graph>
    static int caculateG(graph &G, int from_x, int from_y, int to_x, int to_y) /*���ڽڵ����G */
    {
        int cost = 0;
        
        if (G.isMapTypeOfNormal()) /*������*/
        {
            cost = (abs(from_x - to_x) + abs(from_y - to_y)) == 1 ? kCost10 : kCost14;
        }
        else if (G.isMapTypeOfISO()) /* 45 ������*/
        {
            if (from_x == to_x || from_y == to_y) /*б��*/
            {
                cost = kCost33;
            }
            else if (from_x - to_x == from_y - to_y)/*����*/
            {
                cost = kCost30;
            }
            else /*����*/
            {
                cost = kCost60;
            }
        }
        else if (G.isMapTypeOfStaggered()) /* 45 �����ν���*/
        {
            if (from_y == to_y) /* ����*/
            {
                cost = kCost20;
            }
            else if (from_y + 2 == to_y || from_y - 2 == to_y) /*����*/
            {
                cost = kCost10;//kCost5 * 2;
            }
            else /*б��*/
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

        //ʹ�������پ���������
        if (G.isMapTypeOfNormal()) /*������*/
        {
            H = kCost10 * (abs(from_x - to_x) + abs(from_y - to_y));
        }
        else if (G.isMapTypeOfISO()) /* 45 ������*/
        {
            int tmp_x = from_x < to_x ? (to_x - from_x) : (from_x - to_x);
            int tmp_y = from_y < to_y ? (to_y - from_y) : (from_y - to_y);
            int H_x = abs(tmp_x-tmp_y)*kCost30;
            int H_y = (tmp_x+tmp_y+1)*kCost15;
            H = H_x + H_y;
        }        
        else if (G.isMapTypeOfStaggered()) /* 45 �����ν���*/
        {
            int H_x = abs((to_x-from_x)*kCost20 + (to_y & 1 - from_y & 1) * kCost10);
            int H_y = abs(to_y - from_y)*kCost5;
            H = H_x + H_y;
        }

        return H;    
    }
    
};


#endif 


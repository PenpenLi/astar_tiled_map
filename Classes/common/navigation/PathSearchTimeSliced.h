#ifndef TIME_SLICED_GRAPHALGORITHMS_H
#define TIME_SLICED_GRAPHALGORITHMS_H
#pragma warning (disable:4786)
//------------------------------------------------------------------------
//
//  Name: GraphAlgorithms_TimeSliced.h
//
//  Desc: classes to implement graph algorithms that can be distributed
//          over multiple update-steps
//
//          Any graphs passed to these functions must conform to the
//          same interface used by the SparseGraph
//          
//
//------------------------------------------------------------------------
#include <vector>
#include <list>

#include <functional>
#include "BinaryHeapSort.h"
#include "GraphMap.h"

#include "common/LogDebug.h"

//these enums are used as return values from each search update method
enum 
{
    target_found,
    target_not_found, 
    search_incomplete
};

class NodeInfo
{
public:
    int x;
    int y;    
    int F;
    int G;
    int inWitchList; // ListType
    NodeInfo *pParent;

    NodeInfo(int _x, int _y):x(_x), y(_y), F(0), G(0), pParent(nullptr) {}
};

typedef std::function<void(int result)> searchCallback; 


//------------------------ Graph_SearchTimeSliced -----------------------------
//
// base class to define a common interface for graph search algorithms
//-----------------------------------------------------------------------------
class SearchTimeSliced
{

public:
    SearchTimeSliced( GraphMap &G,
                                        int source_x,
                                        int source_y,
                                        int target_x, 
                                        int target_y,
                                        searchCallback callFunc):m_graph(G),
                                                                m_source(GraphPoint(source_x, source_y)),
                                                                m_target(GraphPoint(target_x, target_y)),
                                                                m_callback(callFunc)                                        
    {
        int len = m_graph.getWidth() * m_graph.getHeight();
        
        pMapNodes = new NodeInfo* [len];
        for (int i = 0; i<len; i++)
        {
            pMapNodes[i] = NULL;
        }        
    }
    
    ~SearchTimeSliced()
    {
        int len = m_graph.getWidth() * m_graph.getHeight();
        for (int i = 0; i < len; i++)
        {
            if (pMapNodes[i] != NULL)
                delete pMapNodes[i];
        }
        delete[] pMapNodes;        
    }

    //When called, this method runs the algorithm through one search cycle. The
    //method returns an enumerated value (target_found, target_not_found,
    //search_incomplete) indicating the status of the search
    virtual int cycleOnce() = 0;

    //when find finish ,then notify the result
    void notify(int result) 
    {
        if (m_callback != NULL)
        {
            m_callback(result);
        }
    }

    //path : target point put in front !! and the path not include source point 
    std::vector<GraphPoint> getPath()
    {
        std::vector<GraphPoint> path;
        NodeInfo *node = getNode(m_target.x, m_target.y);
        while (node->pParent)
        {
            path.push_back(GraphPoint(node->x, node->y));
            node = node->pParent;
        }

        return path;
    } 

    
protected:
    NodeInfo* getNode(int x, int y)
    {
        if (x < 0 ||x >= m_graph.getWidth() || y < 0 ||y >= m_graph.getHeight())
        {
            return NULL;
        }
    
        int index = m_graph.getWidth()* y + x;
        if (pMapNodes[index] == NULL)
        {
            pMapNodes[index] = new NodeInfo(x, y);
        }
        return pMapNodes[index];
    }

    std::vector<NodeInfo *> getSurroundPoints(int tx, int ty)
    {
        std::vector<NodeInfo *> points;

        if (m_graph.isMapTypeOfNormal()) 
        {
            for (int x = tx - 1; x <= tx + 1; x++)
            {
                for (int y = ty - 1; y <= ty + 1; y++)
                {
                    if (m_graph.isWalkable(x, y))
                    {
                        points.push_back(getNode(x, y));
                    }
                }
            }
        }
        else if (m_graph.isMapTypeOfISO()) 
        {
            NodeInfo* tmp[] = {
                                getNode(tx-1, ty+1),         /*left*/
                                getNode(tx+1, ty-1),         /*right*/                                
                                getNode(tx-1, ty-1),         /*up*/
                                getNode(tx+1, ty+1),        /*down*/
                                getNode(tx-1, ty),  /*left-up*/
                                getNode(tx, ty+1), /*left-down*/
                                getNode(tx, ty-1), /*right-up*/
                                getNode(tx+1, ty) /*right-down*/
                              };
            for (int i = 0; i < 8; i++)
            {
                if (tmp[i] != NULL && m_graph.isWalkable(tmp[i]->x, tmp[i]->y)) 
                {
                    points.push_back(tmp[i]);
                }  
            }               
        }
        
        else if (m_graph.isMapTypeOfStaggered())
        {        
            NodeInfo* tmp[] = {
                                getNode(tx-1, ty),         /*left*/
                                getNode(tx+1, ty),         /*right*/  
                                getNode(tx, ty-2),         /*up*/
                                getNode(tx, ty+2),         /*down*/
                                getNode(tx-1+(ty&1), ty-1), /*left-up*/
                                getNode(tx-1+(ty&1), ty+1), /*left-down*/
                                getNode(tx + (ty&1), ty-1), /*right-up*/
                                getNode(tx + (ty&1), ty+1) /*right-down*/
                              };
            for (int i = 0; i < 8; i++)
            {
                if (tmp[i] != NULL && m_graph.isWalkable(tmp[i]->x, tmp[i]->y)) 
                {
                    points.push_back(tmp[i]);
                }  
            }        
        }

        return points;
    }
    
private:
    NodeInfo **pMapNodes;  
    
protected:
    GraphPoint m_source;
    
    GraphPoint m_target;
    
    GraphMap& m_graph;
    
    searchCallback m_callback;

};




//-------------------------- AStarTimeSliced -----------------------------
//
//  a A* class that enables a search to be completed over multiple update-steps
//-----------------------------------------------------------------------------
template <class Calculator>
class AStarTimeSliced : public SearchTimeSliced
{
public:
    AStarTimeSliced( GraphMap &G,
                                    int source_x,
                                    int source_y,
                                    int target_x, 
                                    int target_y,
                                    searchCallback callFunc): 
                                    SearchTimeSliced(G, source_x, source_y, target_x, target_y, callFunc),
                                    pBinarySort(NULL)

{ 
    if (m_graph.isWalkable(target_x, target_y))
    { 
        pBinarySort = new BinaryHeapSort<NodeInfo>(m_graph.getWidth(), m_graph.getHeight());
        pBinarySort->addToOpenList(getNode(source_x, source_y)); 
    }
}

    ~AStarTimeSliced()
    {
        if (!pBinarySort)
        {
            delete pBinarySort;
        }
    }

    int cycleOnce()
    {
        if (!pBinarySort || pBinarySort->getOpenListSize() <= 0)
        {
            return target_not_found;
        }

        NodeInfo *minNode = pBinarySort->getMinNode();
        pBinarySort->addToCloseList(minNode);
        if (minNode->x == m_target.x && minNode->y == m_target.y)
        {
            return target_found;
        }

        /*检查周围8 个方向的点*/
        auto points = getSurroundPoints(minNode->x, minNode->y);
        for (auto &node : points)
        {
            if ((pBinarySort->getWitchList(node) != IN_LIST_CLOSE))
            {
                int tmpG = minNode->G + Calculator::caculateG(m_graph, minNode->x, minNode->y, node->x, node->y);
                int tmpH = Calculator::caculateH(m_graph, node->x, node->y, m_target.x, m_target.y);
                if (pBinarySort->getWitchList(node) == IN_LIST_OPEN)
                {
                    if (tmpG < node->G) //update node
                    {
                        node->G = tmpG;
                        node->F = node->G + tmpH;
                        node->pParent = minNode;

                        pBinarySort->resortOpenListForNode(node);
                    }
                }
                else 
                {
                    //add to open list.
                    node->G = tmpG;
                    node->F = node->G + tmpH;
                    node->pParent = minNode;
                    pBinarySort->addToOpenList(node);             
                }
            }
        }
     
        //there are still nodes to explore
        return search_incomplete;
    }        
    
private:
    BinaryHeapSort<NodeInfo> *pBinarySort; /*二叉堆搜索算法*/
};






//-------------------------- DijkstrasTimeSliced -----------------------------
//
//  a Dijkstras class that enables a search to be completed over multiple update-steps
//-----------------------------------------------------------------------------
template <class Calculator>
class DijkstrasTimeSliced : public SearchTimeSliced
{
public:
    DijkstrasTimeSliced( GraphMap &G,
                                            int source_x,
                                            int source_y,
                                            int target_x, 
                                            int target_y,
                                            searchCallback callFunc): 
                                            SearchTimeSliced(G, source_x, source_y, target_x, target_y, callFunc),
                                            pBinarySort(NULL)

{ 
    if (m_graph.isWalkable(target_x, target_y))
    { 
        pBinarySort = new BinaryHeapSort<NodeInfo>(m_graph.getWidth(), m_graph.getHeight());
        pBinarySort->addToOpenList(getNode(source_x, source_y)); 
    }
}

    ~DijkstrasTimeSliced()
    {
        if (!pBinarySort)
        {
            delete pBinarySort;
        }
    }

    int cycleOnce()
    {
        if (!pBinarySort || pBinarySort->getOpenListSize() <= 0)
        {
            return target_not_found;
        }

        NodeInfo *minNode = pBinarySort->getMinNode();
        pBinarySort->addToCloseList(minNode);
        if (Calculator::isSatisfied(m_graph, minNode->x, minNode->y, m_target.x, m_target.y))
        {
            return target_found;
        }
        
        /*检查周围8 个方向的点*/
        auto points = getSurroundPoints(minNode->x, minNode->y);
        for (auto &node : points)
        {
            if ((pBinarySort->getWitchList(node) != IN_LIST_CLOSE))
            {
                int tmpG = minNode->G + Calculator::caculateG(m_graph, minNode->x, minNode->y, node->x, node->y);
                int tmpH = 0;
                if (pBinarySort->getWitchList(node) == IN_LIST_OPEN)
                {
                    if (tmpG < node->G) //update node
                    {
                        node->G = tmpG;
                        node->F = node->G + tmpH;
                        node->pParent = minNode;

                        pBinarySort->resortOpenListForNode(node);
                    }
                }
                else 
                {
                    //add to open list.
                    node->G = tmpG;
                    node->F = node->G + tmpH;
                    node->pParent = minNode;
                    pBinarySort->addToOpenList(node);             
                }
            }
        }
     
        //there are still nodes to explore
        return search_incomplete;
    }        
    
private:
    BinaryHeapSort<NodeInfo> *pBinarySort; /*二叉堆搜索算法*/
};



#endif

#ifndef BINARYHEAPSORT_H
#define BINARYHEAPSORT_H 

/*¶þ²æ¶ÑÅÅÐò*/



typedef enum
{
    IN_LIST_NONE = 0,
    IN_LIST_OPEN,
    IN_LIST_CLOSE
}ListType;



template <class NodeInfo>
class BinaryHeapSort
{
private:
    NodeInfo** _OPEN_LIST;   //index must >=1 , for heap sort
        
    int _openSize; 
    
    int _width;

    int _height;
public:
    BinaryHeapSort(int w, int h):_width(w), _height(h)
    {
        _OPEN_LIST = new NodeInfo* [w * h + 1];
        
        _openSize = 0;
    }

    ~BinaryHeapSort()
    {
        delete []_OPEN_LIST;
    }

    void addToOpenList(NodeInfo *node)
    {
        //add to last
        node->inWitchList = IN_LIST_OPEN;
        _OPEN_LIST[++_openSize] = node;
        
        //resort 
        int last = _openSize;
        //check and make sure parent > child in bi tree.
        while (last > 1)
        {
            int half = last >>1; //parent node index 
            if (_OPEN_LIST[last]->F >= _OPEN_LIST[half]->F)
            {
                break;
            }
            
            //swap if child > parent 
            NodeInfo *tmp = _OPEN_LIST[last];
            _OPEN_LIST[last] = _OPEN_LIST[half];
            _OPEN_LIST[half] = tmp;
            
            last >>=1; //upper parent node index
        }
    }

    
    void addToCloseList(NodeInfo *node)
    {
        //add to last
        node->inWitchList = IN_LIST_CLOSE;
    }


    NodeInfo *getMinNode()
    {
       
        if (_openSize <= 0)
        {
            return nullptr;
        }
        
        NodeInfo *tmp = _OPEN_LIST[1];
        
       //take the last one to first.
       _OPEN_LIST[1] = _OPEN_LIST[_openSize--];

       //resort open list
        int head = 1;   
        int last = _openSize ;
        while ((head<<1)+1 <= last)
        {
            int lchild = head<<1;
            int rchild = lchild + 1;
            int childmin = _OPEN_LIST[lchild]->F < _OPEN_LIST[rchild]->F ? lchild:rchild;
            if (_OPEN_LIST[head]->F <= _OPEN_LIST[childmin]->F)
            {
                break;
            }

            //swap
            NodeInfo *tmp = _OPEN_LIST[head];
            _OPEN_LIST[head] = _OPEN_LIST[childmin];
            _OPEN_LIST[childmin] = tmp; 

            head = childmin;
        }
        
        return tmp;
    }
    
    void resortOpenListByIndex(int index)
    {
        int last = index;

        while (last > 1)
        {
            int half = last>>1;
            if (_OPEN_LIST[last]->F >= _OPEN_LIST[half]->F)
            {
                break;
            }

            //swap
            NodeInfo *tmp = _OPEN_LIST[last];
            _OPEN_LIST[last] = _OPEN_LIST[half];
            _OPEN_LIST[half] = tmp; 

            last >>=1;//upper parent node index
        }
    }


    void resortOpenListForNode(NodeInfo *node)
    {
        int index = 1;
        
        //find original index in open list 
        for (int i = 1; i <= _openSize; i++)
        {
            if (_OPEN_LIST[i] == node)
            {
                index = i; 
                break;
            }
        }

        resortOpenListByIndex(index);
    }

    
    int getOpenListSize()
    {
        return _openSize;
    }

    int getWitchList(NodeInfo *node)
    {
        return node->inWitchList;
    }
};

#endif 

#ifndef PATH_MANAGER_H
#define PATH_MANAGER_H
#pragma warning (disable:4786)
//-----------------------------------------------------------------------------
//
//  Name:   PathManager.h
//
//
//  Desc:   a template class to manage a number of graph searches, and to 
//          distribute the calculation of each search over several update-steps
//-----------------------------------------------------------------------------
#include <list>
#include <cassert>
#include "PathSearchTimeSliced.h"


class PathManager
{
private:
    //a container of all the active search requests
    std::list<SearchTimeSliced *> m_SearchRequests;

    //this is the total number of search cycles allocated to the manager. 
    //Each update-step these are divided equally amongst all registered path requests
    unsigned int m_iNumSearchCyclesPerUpdate;

    bool m_preGotoTarget; //pre go while searching.
    
public:
    static PathManager *instance() 
    {
        static PathManager pathMgr;
        
        return &pathMgr;
    }
    
    //every time this is called the total amount of search cycles available will
    //be shared out equally between all the active path requests. If a search
    //completes successfully or fails the method will notify the relevant bot
    void updateSearches()
    {
        int NumCyclesRemaining = m_iNumSearchCyclesPerUpdate;

        //iterate through the search requests until either all requests have been
        //fulfilled or there are no search cycles remaining for this update-step.
        std::list<SearchTimeSliced*>::iterator curSearch = m_SearchRequests.begin();
        while (NumCyclesRemaining-- && !m_SearchRequests.empty())
        {
            //make one search cycle of this path request
            int result = (*curSearch)->cycleOnce();

            //if the search has terminated remove from the list
            if ( (result == target_found) || (result == target_not_found) )
            {
                (*curSearch)->notify(result);
                
                //remove this path from the path list
                curSearch = m_SearchRequests.erase(curSearch);       
            }
            //move on to the next
            else 
            {
                ++curSearch;
            }

            //the iterator may now be pointing to the end of the list. If this is so,
            // it must be reset to the beginning.
            if (curSearch == m_SearchRequests.end())
            {
                curSearch = m_SearchRequests.begin();
            }
        }//end while
    }

    
    //a path planner should call this method to register a search with the 
    //manager. (The method checks to ensure the path planner is only registered once)
    void registerPlan(SearchTimeSliced* pPathPlanner)
    {
        //make sure the bot does not already have a current search in the queue
        if(std::find(m_SearchRequests.begin(),m_SearchRequests.end(),pPathPlanner) == m_SearchRequests.end())
        { 
            //add to the list
            m_SearchRequests.push_back(pPathPlanner);
        }
    }
    
    
    void unRegisterPlan(SearchTimeSliced* pPathPlanner)
    {
        m_SearchRequests.remove(pPathPlanner);
    }
    
    void setNumCyclePerUpdate(int cyclesPerUpdate) {m_iNumSearchCyclesPerUpdate = cyclesPerUpdate;}
    
    bool isPreGotoTarget(){return m_preGotoTarget;}
    
private:
    PathManager():m_iNumSearchCyclesPerUpdate(50), m_preGotoTarget(false) {}
};


#endif
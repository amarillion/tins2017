#ifndef DATA_H
#define DATA_H

/*
    event - eventlistener model
    inspired by the java event model

    for any type of data that should be used by more than one
    other class, inherit a datawrapper, e.g. tablewrapper for a table of data

    other classes interested in the data can register themselved using
    addListener and removeListener. The cool thing is, that only the listening
    classes need to know about the wrapper. The wrapper doesn't need to know about
    the listeners, and should work regardless whether there are 0 or 1 million 
    listeners.
    
    whenever table is changed, fire a changed event by calling FireChangedEvent().
    All the listeners will be notified of the change and can call specific methods
    on the data wrapper to get more info.

    if you don't want to use multiple inheritance, you can set up a listener class
    in the following way:

    class TableListener;
    
    class SomeListeningClass
    {
        private:
            TableListener listener;
        public:
            SomeListeningClass (TableWrapper table)
            {
                listener = new TableListener(this);
                table.AddListener (listener);
            }
            
            void doSomething(code)
            {
                // do something
            }            
    }

    class TableListener : public DataListener
    {
        private:
            SomeListeningClass parent;
        public:
            TableListener(SomeListeningClass p)
            {
                parent = p;
            }
            
            virtual void changed (int code = 0)
            {
                parent->doSomething(code);
            }
    }    
    
 
*/

#include <list>
#include <map>
#include <functional>

class DataListener
{
    public:
	virtual void changed (int code = 0) = 0;
	virtual ~DataListener() {}
};

using DataListenerFunction = std::function<void(int)>;

class DataWrapper
{
private:
	std::list <DataListener *> listeners;
	int nextFunctionHandle = 0;
	std::map <int, DataListenerFunction> listenerFunctions;
public:
	void FireEvent(int code);
	void AddListener (DataListener *listener);
	int AddListener (const DataListenerFunction &listener);
	void RemoveListener (DataListener *listener);
	void RemoveListener (int handle);
};

using ListListenerFunction = std::function<void(int, int)>;

class ListWrapper
{
private:
	int nextFunctionHandle = 0;
	std::map <int, ListListenerFunction> listenerFunctions;
public:
	enum { INSERT, DELETE, SINGLE_CHANGE, FULL_CHANGE };
	void FireEvent(int code, int position);
	int AddListener (const ListListenerFunction &listener);
	void RemoveListener (int handle);
};

#endif


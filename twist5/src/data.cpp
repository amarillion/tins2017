#include "data.h"

void DataWrapper::FireEvent (int code)
{
    std::list <DataListener *> :: iterator i;

    // notify all listeners of event
    for (i = listeners.begin(); i != listeners.end(); ++i)
    {
        (*i)->changed (code);
    }
}

void DataWrapper::AddListener (DataListener* listener)
{
    listeners.push_back (listener);
}

void DataWrapper::RemoveListener (DataListener *listener)
{
    listeners.remove (listener);
}


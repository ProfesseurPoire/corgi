#pragma once

#include <functional>
#include <map>

namespace corgi
{
    template<class... Args>
    class Event
    {
    public:
        /**
         * @brief   We return a key that can be used to remove the callback
         *          from the event in case we need it 
         * 
         * @tparam U 
         * @param callback 
         * @return int 
         */
        template<class U>
        int operator+=(U callback)
        {
            callbacks_.emplace(key, callback);
            key++;
            return key;
        }

        void operator-=(int key) { callbacks_.erase(key); }

        void operator()(Args... args)
        {
            for(auto& callback : callbacks_)
            {
                callback.second(std::forward<Args>(args)...);
            }
        }
        // TODO : I probably should just use a double array system
        // with one array storing the functions and the other one the ids
        // for performance
        std::map<int, std::function<void(Args...)>> callbacks_;

    private:
        int key = 0;
    };
}    // namespace corgi

#ifndef VIEW_H
#define VIEW_H

#include "controller.h"
#include "core/member.h"
#include <vector>

namespace genetic 
{

enum class ViewType {Generations, Population};
template <typename T>
class View
{
    protected:
        std::vector<Member<T>> members_;
        ViewType view_type_;
        virtual void run() = 0;

    public:
        void create(const std::vector<Member<T>>& members, ViewType view_type)
        {
            members_ = members;
            view_type_ = view_type;
            run();
        }
};

}

#endif
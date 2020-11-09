#include "ParaCL.h"

#include <iostream>

namespace ezg
{

    std::vector< std::unique_ptr <BaseOperation > > ParaCL::parse_(std::istream& source_)
    {
        std::vector< std::unique_ptr < BaseOperation > > result;

        char cur_char = source_.peek();
        while(source_)
        {
            skip_(source_, " \n\t\r");
            {
                auto mb_special = checkSpecialChar_(source_);
                if (mb_special != nullptr) {
                    result.push_back(std::move(mb_special));
                    continue;
                }
            }


        }

    }

    bool ParaCL::charIsEqual(char sim_, const std::string& str_)
    {
        bool result = false;
        for (size_t i = 0, k = str_.size(); i < k && !result; i++)
        {
            result = (sim_ == str_.at(i));
        }

        return result;
    }

    std::string ParaCL::get_str_(std::istream& stream_, const std::string& delim_)
    {
        std::string result;
        char _symbol = 0;
        while (stream_.get(_symbol)) {
            if (charIsEqual(_symbol, delim_)) {
                stream_.seekg(-1, std::ios_base::cur);
                break;
            }
            result.push_back(_symbol);
        }

        return result;
    }

    size_t ParaCL::skip_(std::istream& stream_, const std::string& symbols_)
    {
        size_t result = 0u;

        char cur = stream_.peek();
        while(stream_ && charIsEqual(cur, symbols_)) {
            cur = stream_.peek();
            result++;
        }

        return result;
    }

}//namespace ezg
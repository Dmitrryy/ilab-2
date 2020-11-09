#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <memory>

#include "BaseOperation.h"

namespace ezg
{

    class ParaCL{

        std::string m_programStr;

        BaseOperation* m_nodeProgram;

        std::istream& m_inStream  = std::cin;
        std::ostream& m_outStream = std::cout;

        std::string m_errorLog;
        size_t m_numW;
        size_t m_numErr;

    public:

        bool Load(std::istream& source_)
        {
            try {
                auto res_parse = parse_(source_);
            }
            catch (std::exception& ex_) {
                m_outStream << ex_.what();
            }

            assert(0);

            return true;
        }
        bool LoadFromFile(const std::string& fName_);

        int  simulation() const;


        //void setIStream(std::istream& istream_);
        //void setOStream(std::ostream& ostream_);

    private:

        std::vector< std::unique_ptr <BaseOperation > > parse_(std::istream& source_);

        std::unique_ptr< BaseOperation > checkSpecialChar_(std::istream& source_);

        bool charIsEqual(char sim_, const std::string& str_);

        std::string get_str_(std::istream& stream_, const std::string& delim_);
        size_t skip_(std::istream& stream_, const std::string& symbols_);
    };

}//namespace ezg
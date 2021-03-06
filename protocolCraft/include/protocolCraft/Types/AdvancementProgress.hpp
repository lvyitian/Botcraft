#pragma once

#include <map>

#include "protocolCraft/NetworkType.hpp"
#include "protocolCraft/Types/CriterionProgress.hpp"

namespace ProtocolCraft
{
    class AdvancementProgress : public NetworkType
    {
    public:
        void SetSize(const int size_)
        {
            size = size_;
        }

        void SetCriteria(const std::map<Identifier, CriterionProgress>& criteria_)
        {
            criteria = criteria_;
        }


        const int GetSize() const
        {
            return size;
        }

        const std::map<Identifier, CriterionProgress>& GetCriteria() const
        {
            return criteria;
        }

    protected:
        virtual void ReadImpl(ReadIterator& iter, size_t& length) override
        {
            size = ReadVarInt(iter, length);
            criteria.clear();
            for (int i = 0; i < size; ++i)
            {
                const Identifier identifier = ReadString(iter, length);
                criteria[identifier].Read(iter, length);
            }
        }

        virtual void WriteImpl(WriteContainer& container) const override
        {
            WriteVarInt(size, container);
            for (auto it = criteria.begin(); it != criteria.end(); it++)
            {
                WriteString(it->first, container);
                it->second.Write(container);
            }
        }

        virtual const picojson::value SerializeImpl() const override
        {
            picojson::value value(picojson::object_type, false);
            picojson::object& object = value.get<picojson::object>();

            object["size"] = picojson::value((double)size);

            object["criteria"] = picojson::value(picojson::array_type, false);
            picojson::array& array = object["criteria"].get<picojson::array>();

            for (auto it = criteria.begin(); it != criteria.end(); ++it)
            {
                picojson::value value2(picojson::object_type, false);
                picojson::object& object2 = value2.get<picojson::object>();
                object2["criterion_identifier"] = picojson::value(it->first);
                object2["criterion_progress"] = it->second.Serialize();
                array.push_back(value2);
            }

            return value;
        }

    private:
        int size;
        std::map<Identifier, CriterionProgress> criteria;
    };
}
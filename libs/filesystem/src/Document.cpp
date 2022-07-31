#include <corgi/Ini/Document.h>
#include <corgi/containers/Vector.h>

#include <fstream>
#include <string>

using namespace corgi::ini;
using namespace corgi;

// TODO : Put that in std::string
//
// Big split function D:
Vector<std::string> split(const std::string& str, char character)
{
    Vector<std::string> results;
    std::string         temp;
    // Basically we try to build a std::string as long as we don't find
    // the character or a space or a tab. If we find that, we push the current
    // std::string to our result list.
    for(int i = 0; i < str.size(); ++i)
    {
        if(!temp.empty())
        {
            if(str[i] == character || str[i] == ' ' || str[i] == '\t')
            {
                results.push_back(temp);
                temp.clear();
            }
            else
            {
                temp += str[i];
            }
        }
        else
        {
            if(!(str[i] == character || str[i] == ' ' || str[i] == '\t'))
                temp += str[i];
        }
    }

    if(!temp.empty())
        results.push_back(temp);
    return results;
}

bool Document::write(const std::string& filename) const
{
    std::ofstream file(filename.c_str());

    if(!file.is_open())
    {
        return false;
    }
    for(const auto& section : values_)
    {
        if(section.first != "no_section")
            file << "\n[" << section.first.c_str() << "]\n\n";

        for(const auto& value : section.second)
        {
            switch(value.second.index())
            {
                case 0:
                    file << value.first.c_str() << " = "
                         << std::to_string(std::get<0>(value.second)).c_str() << "\n";
                    break;
                case 1:
                    file << value.first.c_str() << " = "
                         << std::to_string(std::get<1>(value.second)).c_str() << "\n";
                    break;
                case 2:
                    file << value.first.c_str() << " = "
                         << std::get<2>(value.second).c_str() << "\n";
                    break;
                default:
                    break;
            }
        }
    }
    file.close();
    return true;
}

float& Document::get_float(const std::string& name, const std::string& section)
{
    return std::get<float>(values_[section][name]);
}

int& Document::get_int(const std::string& name, const std::string& section)
{
    return std::get<int>(values_[section][name]);
}

std::string& Document::get_string(const std::string& name, const std::string& section)
{
    return std::get<std::string>(values_[section][name]);
}

void Document::add(const std::string& name, const int& value, const std::string& section)
{
    values_[section][name] = value;
}

void Document::add(const std::string& name,
                   const float&       value,
                   const std::string& section)
{
    values_[section][name] = value;
}

void Document::add(const std::string& name,
                   const std::string& value,
                   const std::string& section)
{
    values_[section][name] = value;
}

bool Document::read(const std::string& path)
{
    // only use to cleanup later the settings
    // I do that instead of simply creating a new map because I don't want to lose the references
    // to the existing settings that are being used by the game. That way, I can edit the .ini file, hit a
    // key to call refresh, and see the difference in the game in real time

    std::map<std::string, std::map<std::string, int>> new_used_settings;

    std::ifstream file;
    file.open(path.c_str());

    std::string current_section = "no_section";

    if(!file.is_open())
    {
        return false;
    }

    std::string line;

    while(std::getline(file, line))
    {
        if(line[0] == ';')    // Means we're on a comment
        {
            continue;
        }

        if(line.empty())    // Nothing on the line
        {
            continue;
        }

        if(line[0] == '[')
        {
            current_section = "";

            for(auto i = 1u; i < line.size() && line[i] != ']'; ++i)
                current_section += line[i];
        }

        Vector<std::string> result = split(line.c_str(), '=');
        values_["lol"]["stuff"]    = 1;

        if(result.size() > 1)
        {
            //TODO : Add a constructor for this in Vector later on
            //Vector<std::string> v(result.begin() + 1, result.end());

            Vector<std::string> v;
            v.reserve(result.size() - 1);

            for(int i = 1; i < result.size(); i++)
            {
                v.emplace_back(result[i]);
            }

            new_used_settings[current_section][result[0]] = 0;

            // TODO : this doesn't work btw, so I guess I'll have to make my own function to check
            // if something is an integer or a float etc
            try
            {
                values_[current_section][result[0]] = std::stof(result[1]);
            }
            catch(...)
            {
                try
                {
                    values_[current_section][result[0]] =
                        std::stoi(result[1]);    // then we try to convert it to an int
                }
                catch(...)
                {
                    values_[current_section][result[0]] =
                        result[1];    // then we just get back the string as is
                }
            }
        }
    }

    // Cleaning up the settings that are no more used after refreshing the thing

    for(auto iterator = values_.begin(); iterator != values_.end();)
    {
        if(new_used_settings.count(iterator->first) == 0)
        {
            values_.erase(iterator++);
        }
        else
        {
            for(auto it = iterator->second.begin(); it != iterator->second.end();)
            {
                if(new_used_settings[iterator->first].count(it->first) == 0)
                {
                    values_[iterator->first].erase(it++);
                }
                else
                {
                    ++it;
                }
            }
            ++iterator;
        }
    }

    return true;
}

int Document::size() const
{
    int count = 0;

    for(auto& section : values_)
    {
        for(auto key : section.second)
        {
            count++;
        }
    }

    return count;
}

bool Document::exist(const std::string& key, const std::string& section) const
{
    if(values_.count(section) > 0)
    {
        return values_.at(section).count(key) > 0;
    }
    return false;
}

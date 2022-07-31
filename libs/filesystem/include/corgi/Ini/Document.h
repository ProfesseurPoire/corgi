#pragma once

#include <map>
#include <string>
#include <variant>

namespace corgi::ini
{
/*!
     * @brief Provides an interface to write settings to an ini
     *          file or to read settings from an ini file
     */
class Document
{

public:
    /*!
         * @brief Write the content of the object to a file
         *
         * If a file already exist at @ref path, it gets overwritten. A new
         * file is created otherwise.
         *
         * @param path Path to the file we want to write into
         *
         * @return Returns true if the function managed to write its content
         *         inside filename
         */
    bool write(const std::string& path) const;

    /*!
         * @brief Load the content of the file @ref path
         *
         * Any content the current document previously stored is discarded
         *
         * @param path Path to the file we want to read
         *
         * @return Returns true if the function managed to read from a file
         */
    bool read(const std::string& path);

    /*!
         * @brief Count how many key have been registered inside the document
         *
         * @return Returns how many key have been counted
         */
    [[nodiscard]] int size() const;

    /*!
         * @brief Check if a key exist inside the document
         *
         * @param name      Name of the key we're looking for
         * @param section   Section of the key we're looking for
         *
         * @returns Returns true if the key exist, false otherwise
         */
    [[nodiscard]] bool exist(const std::string& name,
                             const std::string& section = "no_section") const;

    /*!
         * @brief Search for a key's value inside the document
         *
         * Throw an exception if the key could not be found. Use @ref exist
         * if you're not sure the key actually exist
         *
         * @param name       key's name
         * @param section    keys's section name
         *
         * @returns Returns nullptr if the key could not be founded
         */
    [[nodiscard]] const std::variant<float, int, std::string>&
    value(const std::string& name, const std::string& section = "no_section") const;

    [[nodiscard]] float&       get_float(const std::string& name,
                                         const std::string& section = "no_section");
    [[nodiscard]] int&         get_int(const std::string& name,
                                       const std::string& section = "no_section");
    [[nodiscard]] std::string& get_string(const std::string& name,
                                          const std::string& section = "no_section");

    /*!
         * @name Adds or edit a key to the document.
         *
         * If the section argument is not specified, the key will be added to
         * the ungrouped which correspond to every key without a section
         *
         * @param name       Key's name
         * @param value      Data value
         * @param section    Key's section
         */

    void add(const std::string& name,
             const int&         value,
             const std::string& section = "no_section");
    void add(const std::string& name,
             const float&       value,
             const std::string& section = "no_section");
    void add(const std::string& name,
             const std::string& str,
             const std::string& section = "no_section");

    ///@}

private:
    // values are first grouped in sections
    std::map<std::string, std::map<std::string, std::variant<float, int, std::string>>>
        values_;
};
}    // namespace corgi::ini

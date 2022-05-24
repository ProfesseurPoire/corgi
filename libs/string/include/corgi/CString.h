#pragma once

namespace corgi
{
    /*
     * @brief   The CString namespace contains useful functions to manipulate
     *          C style strings (char*)
     */
    namespace CString
    {
        /*
         * @brief   Returns the size of a CStrings
         * 
         *          Counts how much character there is until reaching
         *          end of line. The end of line character isn't considered
         *          by the function
         *          
         * @param   str     C style string we want to get the size of
         * 
         * @return  Returns an integer with the size of the string
         */
        int size(const char* str);

        /*
         * @brief   Compares two strings together. 
         * 
         *          Compares str1 and str2 character by character. If one
         *          character from str1 is different from str2, the function 
         *          returns false. If str1 is smaller than str2 but everything
         *          matched, the function still returns true.
         *          
         *          This function is mainly meant to be used by search widgets.
         *          
         *          Also if caseSensitive is equals to false, the function
         *          won't consider the case.
         *          
         * @param str1  First string to be compared
         * @param str2  Second string to be compared
         * @param case_sensitive    Tells the functions to be case Sensitive
         *                          or not
         *                          
         * @return Returns true if the strings match, false otherwise
         */
        bool text_match(const char* str1, const char* str2, 
            bool case_sensitive= true);

        /*
         * @brief   Copy input string to output string
         *          
         *          Copy every character from input to ouput until it reachs 
         *          the null terminated character.
         *          
         *          Warning, output must be allocated beforehand
         *          
         * @param   in  String that will be copied
         * @param   out Strings that gets copied
         */
        void copy(const char* in, char* out);

        /*
         * @brief   Returns true if the given strings contains the same string
         * 
         *          Parse the two string character by character until the
         *          function reach the null terminated character.
         *          If the strings were equals until then, returns true,
         *          false otherwise
         *          
         * @param   first   First of the two string to be compared
         * @param   second  Second of the two string to be compared
         * 
         * @return  Returns true if first equals second, false otherwise
         */
        bool compare(const char* first, const char* second);
    }
}
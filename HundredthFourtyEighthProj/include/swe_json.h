/***************************************************************************
 *   Copyright (C) 2017 by SWE team <sdl.window.engine@gmail.com>          *
 *                                                                         *
 *   Part of the SWE: SDL Window Engine:                                   *
 *   https://github.com/AndreyBarmaley/sdl-window-engine                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _SWE_JSON_WRAPPER_
#define _SWE_JSON_WRAPPER_

#ifdef SWE_WITH_JSON

#include <any>
#include <list>
#include <vector>
#include <memory>
#include <utility>
#include <typeindex>
#include <initializer_list>

#include "swe_jsmn.h"
#include "swe_types.h"

#define JSON_WRAPPER_VERSION 20210912

/// @brief SWE namespace
namespace SWE
{
    class JsmnToken : protected jsmntok_t
    {
    public:
        JsmnToken();

        int		counts(void) const;
        int		start(void) const;
        int		size(void) const;

        bool		isKey(void) const;
        bool		isValue(void) const;
        bool		isPrimitive(void) const;
        bool		isString(void) const;
        bool		isArray(void) const;
        bool		isObject(void) const;

        const char*	typeString(void) const;
    };

    enum class JsonType { Null, Integer, Double, String, Boolean, Object, Array };

    class JsonContent;
    class JsonObject;
    class JsonArray;

    /// @brief: base json value interace
    class JsonValue
    {
    public:
        JsonValue() {}
        virtual ~JsonValue() {}

        virtual JsonType        getType(void) const = 0;
        virtual std::string     toString(void) const = 0;
        virtual int             getInteger(void) const = 0;
        virtual std::string     getString(void) const = 0;
        virtual double          getDouble(void) const = 0;
        virtual bool            getBoolean(void) const = 0;

        bool                    isNull(void) const;
        bool                    isBoolean(void) const;
        bool                    isInteger(void) const;
        bool                    isDouble(void) const;
        bool                    isString(void) const;
        bool                    isObject(void) const;
        bool                    isArray(void) const;
    };

    const JsonValue & operator>> (const JsonValue &, int &);
    const JsonValue & operator>> (const JsonValue &, std::string &);
    const JsonValue & operator>> (const JsonValue &, double &);
    const JsonValue & operator>> (const JsonValue &, bool &);

    template<typename T1, typename T2>
    const JsonValue & operator>> (const JsonValue & jv, std::pair<T1, T2> & val)
    {
        return jv >> val.first >> val.second;
    }

    /// @brief: base json null
    class JsonNull : public JsonValue
    {
    public:
        JsonType                getType(void) const override;
        std::string             toString(void) const override;
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;
    };

    /// @brief: json primitive interface
    class JsonPrimitive : public JsonValue
    {
    protected:
        std::any                value;

    public:
        JsonPrimitive(const bool & v) : value(v) {}
        JsonPrimitive(const int & v) : value(v) {}
        JsonPrimitive(const double & v) : value(v) {}
        JsonPrimitive(const std::string & v) : value(v) {}

        std::string             toString(void) const override;
    };

    /// @brief: json string
    class JsonString : public JsonPrimitive
    {
    public:
        JsonString(const std::string & val) : JsonPrimitive(val) {}

        JsonType		getType(void) const override;
        std::string             toString(void) const override;
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;
    };

    /// @brief: json double
    class JsonDouble : public JsonPrimitive
    {
    public:
        JsonDouble(const double & val) : JsonPrimitive(val) {}

        JsonType		getType(void) const override;
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;
    };

    /// @brief: json integer
    class JsonInteger : public JsonPrimitive
    {
    public:
        JsonInteger(const int & val) : JsonPrimitive(val) {}

        JsonType		getType(void) const override;
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;
    };

    /// @brief: json integer
    class JsonBoolean : public JsonPrimitive
    {
    public:
        JsonBoolean(const bool & val) : JsonPrimitive(val) {}

        JsonType		getType(void) const override;
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;
    };

    /// @brief: json container interface
    class JsonContainer : public JsonValue
    {
    public:
        JsonContainer() {}

        virtual bool		isValid(void) const { return false; }
        virtual size_t		size(void) const = 0;
        virtual void		clear(void) = 0;
    };

    /// @brief: JsonValue pointer
    struct JsonValuePtr : std::unique_ptr<JsonValue>
    {
        JsonValuePtr();
        JsonValuePtr(int);
        JsonValuePtr(bool);
        JsonValuePtr(double);
        JsonValuePtr(const std::string &);
        JsonValuePtr(const JsonArray &);
        JsonValuePtr(const JsonObject &);
        JsonValuePtr(JsonValue*);
        JsonValuePtr(const JsonValuePtr &);
        JsonValuePtr(JsonValuePtr &&) noexcept;

        JsonValuePtr &          operator=(const JsonValuePtr &);
        JsonValuePtr &          operator=(JsonValuePtr &&) noexcept;

        void			assign(const JsonValuePtr &);
    };

    /// @brief json array
    class JsonArray : public JsonContainer
    {
    private:
        int                     getInteger(void) const override;
        std::string             getString(void) const override;
        double                  getDouble(void) const override;
        bool                    getBoolean(void) const override;

    protected:
        std::vector<JsonValuePtr> content;
        friend class JsonContent;

    public:
        JsonArray() {}
        JsonArray(const JsonArray &);
        JsonArray(JsonArray && ja) noexcept;

        template<typename T>
        JsonArray(const std::initializer_list<T> & list)
        {
            content.reserve(list.size());
            for(auto & val : list) content.emplace_back(val);
        }

        JsonArray &	        operator=(const JsonArray &);
        JsonArray &	        operator=(JsonArray && ja) noexcept;

        size_t			size(void) const override;
        void			clear(void) override;
        JsonType		getType(void) const override;

        const JsonValue*	getValue(size_t index) const;
        const JsonObject*	getObject(size_t index) const;
        const JsonArray*	getArray(size_t index) const;

        std::string		toString(void) const override;

        int			getInteger(size_t index) const;
        std::string		getString(size_t index)  const;
        double			getDouble(size_t index)  const;
        bool			getBoolean(size_t index) const;
        bool			isValid(void) const override;
        bool			isValid(size_t index) const;

        void			addInteger(const int &);
        void			addString(const std::string &);
        void			addDouble(const double &);
        void			addBoolean(const bool &);

        void			addArray(const JsonArray &);
        void			addObject(const JsonObject &);
        void                    join(const JsonArray &);

        template<typename T>
        std::vector<T> toStdVector(void) const
        {
            std::vector<T> res;
            res.reserve(content.size());

            for(auto & ptr : content)
            {
                res.push_back(T());

                if(ptr.get())
                    *ptr.get() >> res.back();
            }

            return res;
        }

        template<typename T>
        std::list<T> toStdList(void) const
        {
            std::list<T> res;

            for(auto & ptr : content)
            {
                res.push_back(T());

                if(ptr.get())
                    *ptr.get() >> res.back();
            }

            return res;
        }

        template<class T>
        const JsonArray & operator>> (std::vector<T> & v) const
        {
            for(auto & ptr : content)
            {
                v.push_back(T());

                if(ptr.get())
                    *ptr.get() >> v.back();
            }

            return *this;
        }

        template<class T>
        const JsonArray & operator>> (std::list<T> & v) const
        {
            for(auto & ptr : content)
            {
                v.push_back(T());

                if(ptr.get())
                    *ptr.get() >> v.back();
            }

            return *this;
        }
    };

    JsonArray & operator<< (JsonArray &, const int &);
    JsonArray & operator<< (JsonArray &, const std::string &);
    JsonArray & operator<< (JsonArray &, const double &);
    JsonArray & operator<< (JsonArray &, const bool &);

    /// @brief json object
    class JsonObject : public JsonContainer
    {
    private:
        int			getInteger(void) const override;
        std::string		getString(void) const override;
        double			getDouble(void) const override;
        bool			getBoolean(void) const override;

    protected:
        swe_unordered_map<std::string, JsonValuePtr> content;
        friend class JsonContent;

        template<typename T>
        void addValue(const std::string & key, const T & val)
        {
            auto it = content.find(key);

            if(it != content.end())
                (*it).second = JsonValuePtr(val);
            else
                content.emplace(key, JsonValuePtr(val));
        }

    public:
        JsonObject() {}
        JsonObject(const JsonObject &);
        JsonObject(JsonObject && jo) noexcept;

        JsonObject &	        operator=(const JsonObject &);
        JsonObject &	        operator=(JsonObject && jo) noexcept;

        size_t			size(void) const override;
        void			clear(void) override;
        JsonType		getType(void) const override;
        bool			isValid(void) const override;

        std::string		toString(void) const override;
        bool			hasKey(const std::string &) const;
        std::list<std::string>	keys(void) const;

        const JsonValue*	getValue(const std::string &) const;
        const JsonObject*	getObject(const std::string &) const;
        const JsonArray*	getArray(const std::string &) const;

        bool			isNull(const std::string &) const;
        bool			isBoolean(const std::string &) const;
        bool			isInteger(const std::string &) const;
        bool			isDouble(const std::string &) const;
        bool			isString(const std::string &) const;
        bool			isObject(const std::string &) const;
        bool			isArray(const std::string &) const;

        JsonType		getType(const std::string &) const;
        int                 	getInteger(const std::string &, int def = 0) const;
        std::string         	getString(const std::string &, std::string def = "") const;
        double              	getDouble(const std::string &, double def = 0) const;
        bool                	getBoolean(const std::string &, bool def = false) const;

        void			addNull(const std::string &);
        void			addInteger(const std::string &, const int &);
        void			addString(const std::string &, const std::string &);
        void			addDouble(const std::string &, const double &);
        void			addBoolean(const std::string &, const bool &);

        void			addArray(const std::string &, const JsonArray &);
        void			addObject(const std::string &, const JsonObject &);
        void                    join(const JsonObject &);

        template<typename T>
        std::vector<T> getStdVector(const std::string & key) const
        {
            const JsonArray* jarr = getArray(key);
            return jarr ? jarr->toStdVector<T>() : std::vector<T>();
        }

        template<typename T>
        std::list<T> getStdList(const std::string & key) const
        {
            const JsonArray* jarr = getArray(key);
            return jarr ? jarr->toStdList<T>() : std::list<T>();
        }
    };

    /// @brief json content base class
    class JsonContent : protected std::vector<JsmnToken>
    {
        std::string		content;

    protected:
        std::string		stringToken(const JsmnToken &) const;
        jsmntok_t*		toJsmnTok(void);
        std::pair<JsonValue*, int>
        getValue(const const_iterator &, JsonContainer* cont) const;

    public:
        JsonContent() {}

        bool			parseString(const std::string &);
        bool			parseBinary(const char*, size_t);
        bool			readFile(const std::string &);

        bool			isObject(void) const;
        bool			isArray(void) const;

        JsonArray		toArray(void) const;
        JsonObject		toObject(void) const;

        bool			isValid(void) const;
    };

    /// @brief json content string
    class JsonContentString : public JsonContent
    {
    public:
        JsonContentString(const std::string &);
    };

    /// @brief json content file
    class JsonContentFile : public JsonContent
    {
    public:
        JsonContentFile(const std::string &);
    };
}

#endif // SWE_WITH_JSON
#endif // _SWE_JSON_WRAPPER_

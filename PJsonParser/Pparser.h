#ifndef PPARSER__
#define PPARSER__
#include <map>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <cassert>

#include <iostream>

#define assertm(exp, msg) assert(((void)msg, exp))

namespace PJsonParser{

    enum class TokenType{
        ARRAY_OPEN = 1, ARRAY_CLOSED,
        OBJECT_OPEN, OBJECT_CLOSED,

        COMMA,COLON,

        STRING, INT, FLOAT, BOOLEAN, NULLTYPE
    };

    enum class ObjectType{
        OBJECT = 1,
        ARRAY,
        FLOAT,
        INT,
        BOOL,
        STRING,
        NULLTYPE,
    };

    const std::vector<std::vector<char>> sequenzes{
        {'t', 'r', 'u', 'e'},
        {'f', 'a', 'l', 's', 'e'},
        {'n','u','l','l'}
    }; 


    struct Token{
        TokenType type;
        size_t id;
        std::string src;
    };

    std::vector<Token> lex(std::filesystem::path);
    size_t getUntil(std::vector<char>&, std::string&, char, size_t);
    size_t getNumber(std::vector<char>&, std::string&, size_t, bool&);
    int32_t getBool(std::vector<char>&, std::string&, size_t);
    int32_t getNull(std::vector<char>&, std::string&, size_t);

    //build the data structure
    class JValue;
    using JsonObject = std::map<std::string, std::shared_ptr<JValue>>;
    using JsonArray  = std::vector<std::shared_ptr<JValue>>;
        

    class JValue{
    public:
        JValue(ObjectType);

        JsonObject * getAsObject();
        JsonArray  * getAsArray();
        std::string* getAsString();
        double       getAsDouble();
        long         getAsLong();
        bool         getAsBool();
        bool         isNull();

        ObjectType getType();

        void setObject(JsonObject *);
        void setArray (JsonArray  *);
        void setString(std::string*);
        void setString(std::string );
        void setDouble(double      );
        void setLong  (long        );
        void setBool  (bool        );

        std::string toString(size_t=0);

        JValue& operator[](size_t);
        JValue& operator[](std::string);



        void operator=(long);
        void operator=(int);
        void operator=(double);
        void operator=(float);
        void operator=(bool);
        void operator=(std::string);
        void operator=(char const*&&);
        void operator=(ObjectType);
        void operator=(std::shared_ptr<JsonObject>);
        void operator=(std::shared_ptr<JsonArray>);
        void operator=(std::shared_ptr<JValue>);

    private:
        ObjectType type;
        union Values {
            JsonObject*  object;
            JsonArray*   array;
            std::string* str;
            double       fValue;
            long         lValue;
            bool         bValue;
        } values;

        

    };

    struct TreeReturn{
        std::shared_ptr<JValue> node;
        size_t index;
    };

    std::ostream& operator<<(std::ostream&, JValue);

    std::shared_ptr<JValue> parse(std::filesystem::path);

    TreeReturn buildObject(std::vector<Token>&, size_t);
    TreeReturn buildArray(std::vector<Token>&, size_t);
    std::shared_ptr<JValue> build(std::vector<Token>);
    std::shared_ptr<JValue> CreateJValue(Token&);
    std::shared_ptr<JValue> CreateJValue(ObjectType);


    bool write(std::filesystem::path, const std::shared_ptr<JValue>);


    bool isLiteral(Token&);

};

#endif
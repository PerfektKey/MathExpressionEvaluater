#include "Pparser.h"


namespace PJsonParser{

    std::vector<Token> lex(std::filesystem::path path){

        std::ifstream file;
        file.open(path, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);

        if (!file.read(buffer.data(), size)){
            std::cout << "-\n";
            std::cout << "Couldnt read data from file: " << path.generic_string() << "\n";
            exit(1);
        }
        /*for (char c : buffer)
            std::cout << c;
        std::cout <<"\n";*/

        size_t location = 0;
        size_t tokenID = 0;
        Token CurrentToken;
        char CurrentCharacter;

        std::vector<Token> Tokens;

        while(location < buffer.size()){
            CurrentCharacter = buffer.at(location++);
            //std::cout << "\t" << CurrentCharacter << "\n";
            CurrentToken.src.clear();

            if ( CurrentCharacter == '"'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::STRING;
                location = getUntil(buffer, CurrentToken.src, '"', location);
            }else 
            if ( CurrentCharacter == 'f' || CurrentCharacter == 't'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::BOOLEAN;
                int32_t tmp = getBool(buffer, CurrentToken.src, location);
                //std::cout << tmp << "\n";
                if (tmp == -1)
                    continue;
                location = tmp;
            }else
            if ( CurrentCharacter == 'n'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::NULLTYPE;
                int32_t tmp = getNull(buffer, CurrentToken.src, location);
                //std::cout << tmp << "\n";
                if (tmp == -1)
                    continue;
                location = tmp;
            }else
            if (CurrentCharacter == ':'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::COLON;
                CurrentToken.src = CurrentCharacter;
            }else
            if (CurrentCharacter == ','){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::COMMA;
                CurrentToken.src = CurrentCharacter;
            }else
            if (CurrentCharacter == '['){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::ARRAY_OPEN;
                CurrentToken.src = CurrentCharacter;
            }else
            if (CurrentCharacter == ']'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::ARRAY_CLOSED;
                CurrentToken.src = CurrentCharacter;
            }else
            if (CurrentCharacter == '{'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::OBJECT_OPEN;
                CurrentToken.src = CurrentCharacter;
            }else
            if (CurrentCharacter == '}'){
                CurrentToken.id = tokenID;
                CurrentToken.type = TokenType::OBJECT_CLOSED;
                CurrentToken.src = CurrentCharacter;
            }else{
                if (isdigit(CurrentCharacter) || CurrentCharacter == '-'){
                    CurrentToken.type = TokenType::INT;
                    bool isFloat = false;
                    location = getNumber(buffer, CurrentToken.src, location-2, isFloat);
                    if (isFloat)
                        CurrentToken.type = TokenType::FLOAT;
                }else continue;
            }

            ++tokenID;
            Tokens.push_back( CurrentToken );
        }

        //for (Token& t : Tokens)
        //    std::cout << "id: " << t.id << " src: " << t.src << "\n";
        return Tokens;
    }

    size_t getUntil(std::vector<char>& buffer, std::string& dest, char EndCharacter, size_t loc){
        char c;//the current charcater
        do{
            c = buffer.at(loc++);
            if (c != EndCharacter) dest += c;
        }while (c != EndCharacter);

        return loc;
    }

    size_t getNumber(std::vector<char>& buffer, std::string& dest, size_t loc, bool& Isfloat){
        char c;
        do{
            c = buffer.at(++loc);
            if ( isdigit(c) || c == '.' || c == '-') dest += c;
            if (c == '.') Isfloat = true;
        }while( isdigit(c) || c == '.' || c == '-');

        return loc;
    }



    int32_t getBool(std::vector<char>& buffer, std::string& dest, size_t loc){

        uint8_t index = 1;
        if(buffer.at(loc-1) == 't')
            index = 0;
        size_t offset = 0;
        char c;
        bool failed = false;

        do{
            c = buffer.at( loc+offset-1 );
            if (c != sequenzes.at(index).at(offset)){
                failed = true;
                break;
            }else dest += c;
            ++offset;
        }while(offset < sequenzes.at(index).size());

        return ((failed) ? -1 : (loc+offset-1));
    }
    int32_t getNull(std::vector<char>& buffer, std::string& dest, size_t loc){

        uint8_t index = 2;

        size_t offset = 0;
        char c;
        bool failed = false;

        do{
            c = buffer.at( loc+offset-1 );
            if (c != sequenzes.at(index).at(offset)){
                failed = true;
                break;
            }else dest += c;
            ++offset;
        }while(offset < sequenzes.at(index).size());

        return ((failed) ? -1 : (loc+offset-1));
    }




    JValue::JValue(ObjectType t){
        type = t;
        if (t == ObjectType::OBJECT)
            this->setObject(new JsonObject());
        else if(t == ObjectType::ARRAY)
            this->setArray(new JsonArray());
        else if(t == ObjectType::STRING)
            this->setString(new std::string());
    }

    JsonObject* JValue::getAsObject(){
        assertm(ObjectType::OBJECT == type, "invalid return type");
        return values.object;
    }
    JsonArray* JValue::getAsArray(){
        assertm(ObjectType::ARRAY == type, "invalid return type");
        return values.array;
    }
    std::string* JValue::getAsString(){
        assertm(ObjectType::STRING == type, "invalid return type");
        return values.str;
    }
    double JValue::getAsDouble(){
        assertm(ObjectType::FLOAT == type, "invalid return type");
        return values.fValue;
    }
    long JValue::getAsLong(){
        assertm(ObjectType::INT == type, "invalid return type");
        return values.lValue;
    }
    bool JValue::getAsBool(){
        assertm(ObjectType::BOOL == type, "invalid return type");
        return values.bValue;
    }
    ObjectType JValue::getType(){
        return type;
    }
    bool JValue::isNull(){
        return (type == ObjectType::NULLTYPE);
    }

    void JValue::setObject(JsonObject* v){
        type = ObjectType::OBJECT;
        values.object = v;
    }
    void JValue::setArray (JsonArray* v){
        type = ObjectType::ARRAY;
        values.array = v;
    }
    void JValue::setString(std::string* v){
        type = ObjectType::STRING;
        values.str = v;
    }
    void JValue::setString(std::string v){
        type = ObjectType::STRING;
        values.str = new std::string();
        (*(values.str)) = v;
    }
    void JValue::setDouble (double v){
        type = ObjectType::FLOAT;
        values.fValue = v;
    }
    void JValue::setLong(long v){
        type = ObjectType::INT;
        values.lValue = v;
    }
    void JValue::setBool  (bool v){
        type = ObjectType::BOOL;
        values.bValue = v;
    }

    std::string JValue::toString(size_t padding){
        //make padding lamda

        std::ostringstream stream;
        if (this->getType() == ObjectType::BOOL)
            return ((this->getAsBool()) ? "true" : "false");
        else if (this->getType() == ObjectType::INT)
            return std::to_string( this->getAsLong() );
        else if (this->getType() == ObjectType::FLOAT){
            stream << std::to_string(this->getAsDouble()) ;
            return stream.str();
        }else if (this->getType() == ObjectType::NULLTYPE)
            return "null";
        else if (this->getType() == ObjectType::STRING)
            return ( "\"" + (*(this->getAsString())) + "\"");
        else if (this->getType() == ObjectType::OBJECT){
            stream << "{\n";
            for (auto it = this->getAsObject()->begin();it != this->getAsObject()->end();++it){
                for (size_t p = 0;p < padding+4;++p)//add the padding
                    stream << ' ';
                stream << "\"" << it->first << "\"" << " : " << it->second->toString(padding+4) << ( (it != std::prev(this->getAsObject()->end()) ? ",\n" : "\n") ) ;
            }
            stream << std::setw(padding) << "}";
        }else if(this->getType() == ObjectType::ARRAY){
            stream << "[\n";
            for (size_t IndexArray = 0;IndexArray < this->getAsArray()->size();++IndexArray){
                std::shared_ptr<JValue>& value = this->getAsArray()->at(IndexArray);
                for (size_t p = 0;p < padding+4;++p)
                    stream << ' ';
                stream << value->toString(padding+4) << ( (IndexArray == this->getAsArray()->size()-1) ? "\n" : ",\n" );
            }
            stream << std::setw(padding) << "]";
        }
        return stream.str();
    }

    JValue& JValue::operator[](std::string key){
        if (this->getType() != ObjectType::OBJECT)//sets the values type if it has not been set already
            *this = ObjectType::OBJECT;
        JsonObject& Object = (*this->getAsObject());
        if ( !Object.count(key) ){
            Token t;
            t.type = TokenType::NULLTYPE;
            Object[key] = CreateJValue( t );
        }
        return *((*this->getAsObject())[key]);
    }
    JValue& JValue::operator[](size_t index){
        if (this->getType() != ObjectType::ARRAY)//sets the values type if it has not been set already
            *this = ObjectType::ARRAY;
        if (index == this->getAsArray()->size()){
            this->getAsArray()->push_back( CreateJValue(ObjectType::NULLTYPE) );
        }
        return *((*this->getAsArray())[index]);
    }

    void JValue::operator=(ObjectType OT){
        this->type = OT;
        if (OT == ObjectType::ARRAY)
            this->setArray( new JsonArray() );
        else if (OT == ObjectType::OBJECT)
            this->setObject( new JsonObject() );
        else if (OT == ObjectType::STRING)
            this->setString( new std::string() );
    }
    
    void JValue::operator=(bool b){
        this->setBool(b);
    }
    void JValue::operator=(long l){
        this->setLong(l);
    }
    void JValue::operator=(int i){
        this->setLong( i );
    }
    void JValue::operator=(double d){
        this->setDouble(d);
    }
    void JValue::operator=(float f){
        this->setDouble(f);
    }

    void JValue::operator=(std::string s){
        this->setString( s );
    }
    void JValue::operator=(char const*&& cprr){
        //std::cout << "IDK: " << cprr << "\n";
        *this = std::string(cprr);
    }

    //void JValue::operator=(std::shared_ptr<JValue> spv){
    //
    //}

    std::shared_ptr<JValue> parse(std::filesystem::path fpath){
        return build( lex(fpath) );
    }


    std::shared_ptr<JValue> build(std::vector<Token> Tokens){
        TreeReturn ret;
        if (Tokens.at(0).type == TokenType::OBJECT_OPEN)
            ret = buildObject(Tokens, 1);
        else if (Tokens.at(0).type == TokenType::ARRAY_OPEN )
            ret = buildArray(Tokens, 0);
        else{
            //std::cout << Tokens.at(0).src << " should have been a { or [\n";
            exit(1);
        }
        //std::cout << "hald-out: " << ret.node << "\n";
        return ret.node;
        #if 0 == 1
        JValue* root = new JValue(ObjectType::OBJECT);
        JsonObject& rootObject = *root->getAsObject();

        for (int i = 1;i < Tokens.size()-1;++i){
            Token& PreviousToken = Tokens.at(i-1);
            Token& CurrentToken = Tokens.at(i);
            Token& NextToken = Tokens.at(i+1);

            
            
            if (NextToken.type == TokenType::ARRAY_OPEN ){
                // std::cout << "Array at " << i << "\n";
                TreeReturn ret = buildArray(Tokens, i+2);
                rootObject[PreviousToken.src] = ret.node;
                i = ret.index;
                //std::cout << "array end at " << i << " " << Tokens.size() << "\n";
                continue;
            }else if (NextToken.type == TokenType::OBJECT_OPEN){
                TreeReturn ret = buildObject(Tokens, i+2);
                rootObject[PreviousToken.src] = ret.node;
                i = ret.index;
                continue;
            }else if (CurrentToken.type == TokenType::COLON){
                //std::cout << "\t\t" << PreviousToken.src << " " << CurrentToken.src << " " << NextToken.src << "\n";
                rootObject[PreviousToken.src] = CreateJValue(NextToken);
            }
        }

        return root;
        #endif
    }

    TreeReturn buildArray(std::vector<Token>& Tokens, size_t startIndex){
        std::shared_ptr<JValue> root = std::shared_ptr<JValue>(new JValue(ObjectType::ARRAY));
        JsonArray& rootArray = *root->getAsArray();

        TreeReturn tr;
        tr.node = root;

        size_t lastIndex;

        
        for (int i = startIndex;i < Tokens.size()-1;++i){
            //Token& PreviousToken = Tokens.at(i);
            Token& CurrentToken = Tokens.at(i);
            Token& NextToken = Tokens.at(i+1);

            lastIndex = i;

            if (CurrentToken.type == TokenType::ARRAY_CLOSED || NextToken.type == TokenType::ARRAY_CLOSED || i >= Tokens.size()-1){
                tr.index = i;
                tr.node = root;
                break;
            }
            //continue;

            if (NextToken.type == TokenType::ARRAY_OPEN){
                TreeReturn ret = buildArray(Tokens, i+0);
                rootArray.push_back( ret.node );
                i = ret.index;
            }else if(NextToken.type == TokenType::OBJECT_OPEN){
                TreeReturn ret = buildObject(Tokens, i+2);
                rootArray.push_back( ret.node );
                i = ret.index;
                //std::cout << "end\n";
                continue;

            }else if (NextToken.type != TokenType::COMMA){
                //std::cout << "\t  " << PreviousToken.src << " _ " << CurrentToken.src << " _ " << NextToken.src << "\n";
                rootArray.push_back( CreateJValue(NextToken) );
            }

            if (CurrentToken.type == TokenType::ARRAY_CLOSED || NextToken.type == TokenType::ARRAY_CLOSED || i >= Tokens.size()-1){
                tr.index = i;
                tr.node = root;
                break;
            }


        }

        tr.node = root;
        tr.index = lastIndex;

        return tr;
    }

    TreeReturn buildObject(std::vector<Token>& Tokens, size_t startIndex){
        std::shared_ptr<JValue> root = std::shared_ptr<JValue>(new JValue(ObjectType::OBJECT));
        JsonObject& rootObject = *root->getAsObject();
        //std::cout << "0.1:" << root << "\n";
        TreeReturn tr;
        
        for (int i = startIndex;i < Tokens.size()-1;++i){
            Token& PreviousToken = Tokens.at(i-1);
            Token& CurrentToken = Tokens.at(i);
            Token& NextToken = Tokens.at(i+1);

            if (CurrentToken.type == TokenType::OBJECT_CLOSED || i >= Tokens.size()-2){
                tr.index = i;
                tr.node = root;
                //std::cout << "0.2:" << tr.node << "\n";
                break;
            }
            //continue;
            //std::cout << PreviousToken.src << " _ " << CurrentToken.src << " _ " << NextToken.src << "\n";

            if (NextToken.type == TokenType::ARRAY_OPEN){
                //std::cout << "Array: " << PreviousToken.src << " _ " << CurrentToken.src << " _ " << NextToken.src << "\n";
                TreeReturn ret = buildArray(Tokens, i+1);
                rootObject[PreviousToken.src] = ret.node;
                i = ret.index;
                continue;
            }else if (NextToken.type == TokenType::OBJECT_OPEN){
                //std::cout << "Object: " << PreviousToken.src << " _ " << CurrentToken.src << " _ " << NextToken.src << "\n";
                TreeReturn ret = buildObject(Tokens, i+2);
                rootObject[PreviousToken.src] = ret.node;
                i = ret.index;
                //std::cout << "new: " << ret.node << "\n";
                //std::cout << "old: " << root << "\n";
                if (CurrentToken.type == TokenType::OBJECT_CLOSED || i >= Tokens.size()-2){
                    tr.index = i;
                    tr.node = root;
                    //std::cout << "0.2:" << tr.node << "\n";
                    break;
                }
                continue;
            }else if (CurrentToken.type == TokenType::COLON){
                //std::cout << PreviousToken.src << " _ " << CurrentToken.src << " _ " << NextToken.src << "\n";

                rootObject[PreviousToken.src] = CreateJValue( NextToken );


            }

        }
        //std::cout << "1: " << tr.node << "\n";
        return tr;
    }
    

    std::shared_ptr<JValue> CreateJValue(ObjectType OT){
        return std::shared_ptr<JValue>(new JValue(OT));
    }

    std::shared_ptr<JValue> CreateJValue(Token& t){
        if (t.type == TokenType::STRING){
            std::shared_ptr<JValue> ret = std::shared_ptr<JValue>(new JValue(ObjectType::STRING));
            std::string* str = new std::string(t.src);
            ret->setString(str);
            return ret;
        }else if (t.type == TokenType::INT){
            std::shared_ptr<JValue> ret = std::shared_ptr<JValue>(new JValue(ObjectType::INT));
            //std::cout << stoi(t.src) << "\n";
            ret->setLong(stoi(t.src));
            return ret;
        }else if (t.type == TokenType::FLOAT){
            std::shared_ptr<JValue> ret = std::shared_ptr<JValue>(new JValue(ObjectType::FLOAT));
            ret->setDouble( std::stof(t.src) );
            return ret;
        }else if (t.type == TokenType::BOOLEAN){
            std::shared_ptr<JValue> ret = std::shared_ptr<JValue>(new JValue(ObjectType::BOOL));
            ret->setBool( (t.src == "true") );
            return ret;
        }else if(t.type == TokenType::NULLTYPE){
            std::shared_ptr<JValue> ret = std::shared_ptr<JValue>(new JValue(ObjectType::NULLTYPE));
            return ret;
        }else{
            std::cout << "Parsing error at " << t.src << "\n";
            exit(1);
        }
        
        return std::shared_ptr<JValue>(nullptr);
    }

    bool isLiteral(Token& t){
        return ( 
            t.type == TokenType::STRING  || 
            t.type == TokenType::INT     ||
            t.type == TokenType::FLOAT   ||
            t.type == TokenType::BOOLEAN ||
            t.type == TokenType::NULLTYPE
            );
    }


    bool write(std::filesystem::path fpath, const std::shared_ptr<JValue> root){
        //create the file
        std::ofstream ofile(fpath);

        //check if ofile has been created

        ofile << root->toString();

        ofile.close();

        return true;
    }

};

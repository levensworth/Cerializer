#include "serialization.h"
// Array strucutre methods

arrayADT newArray()
{
    arrayADT new = calloc(1, sizeof(arrayCDT));
    new->size = 0 ;
    new->first = NULL;
    return new;
}


arrayNodeADT newArrayNode(void * value, Type type)
{
    arrayNodeADT new = calloc(1, sizeof(arrayNodeCDT));
    new->value = value;
    new->type = type;
    new->next = NULL;
    return new;
}

int addNodeToArray(arrayADT array, arrayNodeADT node)
{
    if(array == NULL || node == NULL)
        return -1;
    node->next = array->first;
    array->first = node;
    array->size++;
    return array->size;
}

int deleteNodeFromArray(arrayADT array, int index)
{
    if(array == NULL || index < 0 || index >= array->size)
        return -1;
    
    array->first =  removeNode(array->first, index);
    return 1;
}

void* getValueInArray(arrayADT array ,int index)
{
    if(array == NULL)
        return NULL;

    return searchForValue(array->first, array->size-1 - index);// tengo que hacer esto bien
}

Type getTypeFromIndex(arrayADT array, int index)
{
    if(array == NULL || index < 0)
        return Undefined;
    
    arrayNodeADT node = getNodeAt(array->first, array->size-1 - index);
    if(node == NULL)
        return Undefined;
    return node->type;
}

int getArraySize(arrayADT array)
{
    if(array == NULL)
        return -1;
    return array->size;
}

void freeArray(arrayADT array)
{
    freeList(array->first);
    free(array);
}

static void freeArrayNode(arrayNodeADT node)
{
    free(node->value);
    free(node);
}

static void freeList(arrayNodeADT node)
{
    if(node == NULL)
        return;
    freeList(node->next);
    freeArrayNode(node);
}

static void* searchForValue(arrayNodeADT node, int index)
{
    if(node == NULL)   
        return NULL;
    if(index == 0)
    {
        void* value = getTypeSpace(node->type, node->value);
        memcpy(value, node->value,getTypeSize(node->type, node->value));
        return value;
    }
    return searchForValue(node->next, index-1);
}

static arrayNodeADT getNodeAt(arrayNodeADT node, int index)
{
    if(node == NULL)
        return NULL;
    if(index == 0)
        return node;
    
    return getNodeAt(node->next, index-1);
}

static arrayNodeADT removeNode(arrayNodeADT node, int index)
{
    if(node == NULL)
        return node;
    if(index == 0)
        return node->next;
    
    node->next = removeNode(node->next, index-1);
    return node;
}

static void addNodeToEndOfArray(arrayADT array, arrayNodeADT node)
{
    if(array == NULL)
        return;
    array->first = appendNodeToArray(array->first, node);
    if(node != NULL)
        array->size++;
}

static arrayNodeADT appendNodeToArray(arrayNodeADT current, arrayNodeADT node)
{
    if(current == NULL)
        return node;

    current->next = appendNodeToArray(current->next, node);
    return current;
}


// serialization methods
char* arrayNodeToString(arrayNodeADT node)
{
    char * nodeRepresentation = toString(node->value, node->type);
    return nodeRepresentation;
}


// should make this function genereic for every type 
static void addArrayHeaders(bufferADT buffer, arrayADT array)
{
    int numberOfNodes = getArraySize(array);
    char numberOfAtributes[HEADER_SIZE+1] = {0};
    sprintf(numberOfAtributes,"%10d",numberOfNodes+1); // the plus one is for the structure type
    addToBuffer(buffer, numberOfAtributes,HEADER_SIZE);
    addToBuffer(buffer,intToString(Array), 1 );
    for(int i = numberOfNodes-1; i >= 0 ; i--)
    {
        Type currentType = getTypeFromIndex(array,i);
        addToBuffer(buffer,intToString(currentType), 1);
    }
    // add the type of structurefor this object
    
}

char * toString(void * value, Type type)
{
    switch(type)
    {
        case Integer:
            return intToString(*(int*)value);
        case Double:
            return doubleToString(*(double*)value);
        case String:
            return stringToString((char*)value);
        case Array:
            return arrayToString((arrayADT) value);
        case Object:
            return objectToString((objectADT) value);
        case Json:
            return jsonToString((jsonADT) value);
        case Undefined:
            printf("Undefine!\n");
            break;
    }

    printf("not implemented yet...\n");
    return NULL;

}





//------------------- Primitives string methods ----------------


char* intToString(int number)
{
    // this will get you the number of chars needed for the number
    int length = snprintf(NULL, 0, "%d", number);
    //the +1 y for null terminated string
    char* numberString = calloc(length+1, sizeof(char));
    snprintf(numberString, length+1, "%d", number);
    return numberString;
}

char* doubleToString(double number)
{
    // this will get you the number of chars needed for the number
    int length = snprintf(NULL, 0, "%.2f", number);
    //the +1 y for null terminated string
    char* numberString = calloc(length+1, sizeof(char));
    snprintf(numberString, length+1, "%.2f", number);
    return numberString;
}

char* stringToString( char* string)
{   
    char* quoteMark = "\"";
    char* newString = calloc(strlen(string)+3, sizeof(char));
    bufferADT buffer = newBuffer();
    addToBuffer(buffer, quoteMark,1);
    addToBuffer(buffer, string, strlen(string));
    addToBuffer(buffer, quoteMark, 1);
    getLastFromBuffer(buffer, newString, getBufferSize(buffer));
    freeBuffer(buffer);
    return newString;
}


char* arrayToString(arrayADT array)
{
    char* opening = "[";
    char* closing = "]";
    char* comma = ",";
    bufferADT buffer = newBuffer();
    // add headers
    addArrayHeaders(buffer, array);
    addToBuffer(buffer, opening,1);
    for(int i = getArraySize(array)-1 ; i >= 0; i--)
    {
        char* indexString = toString(getValueInArray(array,i), getTypeFromIndex(array,i));
        int size = strlen(indexString);
        addToBuffer(buffer, indexString, size);
        if(i != 0)// to prevent last item from putting a comma
            addToBuffer(buffer, comma, 1);
    }
    addToBuffer(buffer, closing, 1);

    char* composedArrayString = calloc(getBufferSize(buffer), sizeof(char));
    getLastFromBuffer(buffer,composedArrayString, getBufferSize(buffer));
    freeBuffer(buffer);
    return composedArrayString;    
}

char* objectToString(objectADT object)
{
    char* semicolon = ":";
    char* quoteMark = "\"";
    int size = strlen(getObjectKey(object));
    char * valueRepresentation = toString(getObjectValue(object), getObjectType(object));
    size += strlen(valueRepresentation) + 4;
    bufferADT buffer = newBuffer();
    addToBuffer(buffer, quoteMark, 1);
    addToBuffer(buffer,getObjectKey(object),strlen(getObjectKey(object)));
    addToBuffer(buffer, quoteMark, 1);
    addToBuffer(buffer, semicolon, 1);
    addToBuffer(buffer,valueRepresentation , strlen(valueRepresentation));

    char* objectRepresentation = calloc(size, sizeof(char));
    getLastFromBuffer(buffer, objectRepresentation, getBufferSize(buffer));
    freeBuffer(buffer);
    return objectRepresentation;

}


//------------------- End of primitives string methods ------------------------




//--------------------- beginning of buffer methods----------------------------
// this buffer is a stack

bufferADT newBuffer()
{
    bufferADT new = calloc(1, sizeof(bufferCDT));
    new->size = 0;
    new->buffer = NULL;
    return new;
}

static int addSpaceToBuffer(bufferADT buffer, int size)
{
    void * newMemmory = calloc(size + buffer->size, sizeof(char));
    if(memcpy(newMemmory, buffer->buffer, buffer->size) == NULL)
        return -1;
    free(buffer->buffer);
    buffer->buffer = newMemmory;
    buffer->size += size;
    return 1;
}


int addToBuffer(bufferADT buffer,void* value, int size)
{
    if(buffer == NULL || size < 0)
        return -1;

    addSpaceToBuffer(buffer, size);
    long direction = (long)buffer->buffer + (long)buffer->size;
    direction = direction - size;
    //return memcpy(buffer->buffer+buffer->size - size,value, size ) != NULL;   
    return memcpy((void*) direction,value, size ) != NULL;  
}



void getLastFromBuffer(bufferADT buffer, void* destination, int size)
{
    long direction = (long) buffer->buffer + (long) buffer->size;
    direction = direction - size;
    //memcpy(destination, buffer->buffer+buffer->size - size, size);
    memcpy(destination, (void*)direction, size);
    buffer->size -= size;    
}

int getBufferSize(bufferADT buffer)
{
    return buffer->size;
}

void freeBuffer(bufferADT buffer)
{
    free(buffer->buffer);
    free(buffer);
}


// --------------------------- end of buffer ----------------------------------

//------------------------ Beginning of object methods ------------------------

// NOTICE : the arrayADT in json should be changed as it comes  
//  with a lot of overhead for nothing.

objectADT newObject(char* key, void* value, Type type)
{
    objectADT new = calloc(1, sizeof(objectCDT));
    new->key = key;
    new->type = type;
    new->value = value;
    return new;
}

void* getObjectValue(objectADT object)
{
    if(object == NULL)
        return NULL;

    return object->value;
}

char* getObjectKey(objectADT object)
{
    if(object == NULL)
        return "none";
    
    return object->key;
}

Type getObjectType(objectADT object)
{
    if(object == NULL)
        return Undefined;
    return object->type;
}

void freeObject(objectADT object)
{
    free(object->value);
    free(object->key);
    free(object);
}


jsonADT newJsonObject()
{
    jsonADT new = calloc(1, sizeof(jsonCDT));
    new->nodes = newArray();
    new->size = 0;
    return new;
}

int freeJson(jsonADT json)
{
    if(json == NULL)
        return -1;
    freeArray(json->nodes);
    free(json);
    return 1;
}

int addObject(jsonADT json, objectADT object)
{
    if(json == NULL || object == NULL)
        return -1;

    arrayADT array  = json->nodes;
    arrayNodeADT node = newArrayNode(object, Object);
    int result = addNodeToArray(array, node);
    if(result != 0)
        json->size++;
    return result;
}

int removeObject(jsonADT json, char* key)
{
    if(json == NULL || key == NULL)
        return -1;

    arrayADT array = json->nodes;
    for(int i = 0; i < getArraySize(array); i++)
    {
        objectADT object = getValueInArray(array, i);
        char* currentKey = getObjectKey(object);
        if(strcmp(currentKey, key) == 0)
        {
            deleteNodeFromArray(array, i);
            return 1;
        }
    }
    return 0;
}

char * jsonToString(jsonADT json)
{
    if(json == NULL)
        return NULL;
    char* opening = "{";
    char* closing = "}";
    char* comma = ",";
    bufferADT buffer = newBuffer();
    addToBuffer(buffer, opening, 1);
    for(int i = json->size-1 ; i >= 0; i--)
    {
        objectADT object = getValueInArray(json->nodes, i);
        char* objectRepresentation = toString(object, Object);
        
        int size = strlen(objectRepresentation);
        addToBuffer(buffer,objectRepresentation, size);
        if(i  != 0)
            addToBuffer(buffer, comma,1);
    }
    addToBuffer(buffer, closing,1);
    char* jsonRepresentation = calloc(getBufferSize(buffer)+1, sizeof(char));
    getLastFromBuffer(buffer, jsonRepresentation, getBufferSize(buffer));
    freeBuffer(buffer);
    return jsonRepresentation;
}


//--------------------------- End of Object methods ---------------------------



// ---------------------- Begginning of deserialization mathods ---------------



arrayADT deserializeArray(char* string)
{
    char header [HEADER_SIZE] = {0};
    memcpy(header, string, HEADER_SIZE);
    int numberOfAtributes = valueOfInt(header);
    Type types [numberOfAtributes];
    for(int i = 0 ; i < numberOfAtributes; i++)
    {
        types[i] = *(string + HEADER_SIZE + i)-'0';
    }
    arrayADT array = parseArray(string + HEADER_SIZE + numberOfAtributes, types, numberOfAtributes);
    return array;
}

arrayADT  parseArray(char* string, Type types[], int numberOfAtributes)
{
    if(string == NULL)
        return NULL;
    
    // i assume the first character is the beginning of an array
    int weight = 1;
    int index = 1;
    int typeIndex = 0;
    State state;
    arrayADT array = newArray();
    bufferADT buffer = newBuffer();
    void* value;
    void* deserializeValue;
    if(numberOfAtributes == 0)
    {
        freeBuffer(buffer);
        return array;
    }
    do
    {
        char c = *(string + index);
        state = getStateArray(c, weight);
        switch(state)
        {
            case incrementWeight:
                weight++;
                addToBuffer(buffer, &c,1);
                break;
            case addValue:
                addToBuffer(buffer, &c,1);
                break;
            case createNode:
                value = calloc(getBufferSize(buffer), sizeof(char));
                getLastFromBuffer(buffer, value, getBufferSize(buffer));// this clears the buffer
                deserializeValue = getTypeSpace(types[typeIndex], value);
                deserializeType(deserializeValue, value, types[typeIndex]);
                //printf("the value to get is %s\n", *((char**)deserializeValue));
                free(value);
                arrayNodeADT node = newArrayNode(deserializeValue, types[typeIndex]);
                addNodeToEndOfArray(array, node);
                //addNodeToArray(array,node);
                typeIndex++;
                break;
            case decrementWeight:
                weight--;
                if(weight == 0)
                {
                    value = calloc(getBufferSize(buffer), sizeof(char));
                    getLastFromBuffer(buffer, value, getBufferSize(buffer));// this clears the buffer
                    deserializeValue = getTypeSpace(types[typeIndex], value);

                    deserializeType(deserializeValue, value, types[typeIndex]);
                    free(value);
                    arrayNodeADT node = newArrayNode(deserializeValue, types[typeIndex]);
                    //printf("the value to get is %s\n", *((char**)deserializeValue));
                    addNodeToEndOfArray(array, node);
                    typeIndex++;
                    break;
                }
                addToBuffer(buffer, &c, 1);
                break;
        }
        index++;
    }while(weight != 0);
    freeBuffer(buffer);
    return array;
}


static State getStateArray(char c, int weight)
{
    char openingBrackets = '[';
    char closingBrackets = ']';
    char comma = ',';
    if( c == openingBrackets)
        return incrementWeight;
    if( c ==  closingBrackets)
        return decrementWeight;
    if(c == comma)
    {
        if(weight > 1)
            return addValue;
        return createNode;
    }
    return addValue;
}


void deserializeType(void* destination, char* string, Type type)
{
    int resultInt;
    double resultDouble;
    char* resultString;
    arrayADT resultArray;
    switch(type)
    {
        case Integer:
            resultInt = valueOfInt(string);
            memcpy(destination, &resultInt, sizeof(int));
            break;
        case Double:
            resultDouble = valueOfDouble(string);
            memcpy(destination, &resultDouble, sizeof(double));
            break;
        case String:
            resultString = valueOfString(string);
            memcpy(destination, resultString, strlen(string));
            break;
        case Array:
            resultArray = deserializeArray(string);
            int size = sizeof(arrayCDT);
            memcpy(destination, resultArray, size);
            break;
        case Object:
            printf("not implemented yet\n");
            break;
        case Json:
            printf("not implemented yet\n");
            break;
        case Undefined:
            printf("not implemented!\n");
            memcmp(destination, NULL, sizeof(int));
            break;
    }
}

void deserializeArrayNodeAt(arrayADT array, int index, Type type)
{
    if(array == NULL || index < 0 || index > getArraySize(array))
        return;
    
    arrayNodeADT node = getNodeAt(array->first, index);
    deserializeType(node->value, node->value, type);
    node->type = type;
}


void* getTypeSpace(Type type, char* string)
{

    int size = 0;
    int count  = 1;
    switch(type)
    {
        case Integer:
            size = sizeof(int);
            break;
        case Double:
            size =sizeof(double);
            break;
        case String:
            size = sizeof(char);
            count = strlen(string);
            break;
        case Array:
            size = sizeof(arrayCDT);
            break;
        case Object:
            printf("not implemented yet\n");
            size = sizeof(objectCDT);
            break;
        case Json:
            printf("not implemented yet\n");
            size = sizeof(jsonCDT);
            break;
        case Undefined:
            printf("not implemented!\n");
            break;
    }
    return calloc(count, size);
}

int getTypeSize(Type type, char* string)
{

    int size = 0;
    int count  = 1;
    switch(type)
    {
        case Integer:
            size = sizeof(int);
            break;
        case Double:
            size =sizeof(double);
            break;
        case String:
            size = sizeof(char);
            count = strlen(string);
            break;
        case Array:
            size = sizeof(arrayCDT);
            break;
        case Object:
            printf("not implemented yet\n");
            size = sizeof(objectCDT);
            break;
        case Json:
            printf("not implemented yet\n");
            size = sizeof(jsonCDT);
            break;
        case Undefined:
            printf("not implemented!\n");
            break;
    }
    return count * size;
}


// ------------------------ Deserialization of primitives ---------------------

int valueOfInt(char* string)
{
    int index = 0;
    int c = *(string+index);
    while(!isdigit(c))
    {
        index++;
        c = *(string+index);
    }
    int value = atoi(string+index);
    return value;
}

double valueOfDouble(char* string)
{
    double value = strtod(string, NULL);
    return value;
}

char* valueOfString(char* string)
{
    int size = strlen(string)-2;// the -2 is to get rid of the "
    char* copyString = calloc(size+1, sizeof(char));
    memcpy(copyString, string+1, size ); // the +1 is to skipp "
    return copyString;
}



// ------------------------ End deserialization of primitives -----------------


// object deserialization
objectADT deserializeObject(char* string)
{
    objectADT object = parseObject(string);
    return object;
}

static objectADT parseObject(char* string )
{
    bufferADT buffer = newBuffer();
    char* key;
    void * value = NULL;
    int c;
    int index = 0;
    char semicolon = ':';
    stateObject state = keyState;
    do
    {
        c = *(string + index);
        switch(state)
        {
            case keyState:
                if( c != semicolon)
                    addToBuffer(buffer, &c, 1);
                else
                    state = endKey;
                break;
            case endKey:
                key = calloc(getBufferSize(buffer), sizeof(char));
                getLastFromBuffer(buffer, key, getBufferSize(buffer));
                state = valueState;
                addToBuffer(buffer, &c, 1);
                break;
            case valueState:
                if(c != EOF)
                    addToBuffer(buffer, &c, 1);
                else
                {
                    value = calloc(getBufferSize(buffer), sizeof(char));
                    getLastFromBuffer(buffer, value, getBufferSize(buffer));
                }
                break; 
        }
        index++;
    }while(c != EOF);
    freeBuffer(buffer);
    
    return newObject(key, value, Undefined);
}



// ---------------------- End of deserialization mathods ----------------------


// Global funcitons for serializign and deserializing

char* serialize(void* obj, Type type)
{
    char * serialization = toString(obj, type);
    char sizeOfSerializationString [HEADER_SIZE+1]= {0}; 
    int size = strlen(serialization);
    sprintf(sizeOfSerializationString,"%10d",size);
    char * serializedString = calloc(size+HEADER_SIZE, sizeof(char));
    strcat(serializedString, sizeOfSerializationString);
    strcat(serializedString, serialization);
    //printf("%s\n", serializedString);
    return serializedString;
}

// por ahora solo desserializa arrays porque fran lo usa asi
void* deserialize(char* string)
{
    return deserializeArray(string);
}


// this are independent serilization methods
char* serializeString(char* string)
{
    int size = strlen(string);
    char* serialize = calloc(size + HEADER_SIZE + 1, sizeof(char));
    sprintf(serialize, "%10d", size);
    memcpy(serialize+HEADER_SIZE, string, size);
    return serialize;
}


char* deserializeString(char* serializeString)
{
    char header [HEADER_SIZE] = {0};
    memcpy(header, serializeString, HEADER_SIZE);
    int size = valueOfInt(header);
    char* string = calloc(size+1, sizeof(char));
    memcpy(string, serializeString+HEADER_SIZE, size);
    return string;
}


//----------------------------- Tests ---------------------------------
int test_intToString(void)
{
    int x = 450;
    char* number = intToString(x);
    return strcmp(number, "450") == 0;
}

int test_doubleToString(void)
{
    double x = 450;
    char* number = doubleToString(x);
    return strcmp(number, "450.00") == 0;
}

int testNodeArray(void)
{
    arrayADT array = newArray();
    char* str  = "prueba";
    arrayNodeADT node = newArrayNode(str, String);
    addNodeToArray(array, node);
    char* result  =  getValueInArray(array, 0);
    return strcmp(str, result) == 0;
    
}


int testArrayToString(void)
{
    arrayADT array = newArray();
    int subject1 =  21;
    char* subject2 =  "pi";
    char* subject3 =  "paw";

    arrayNodeADT node1 = newArrayNode(&subject1, Integer);
    arrayNodeADT node2 = newArrayNode(subject2, String);
    arrayNodeADT node3 = newArrayNode(subject3, String);
    addNodeToArray(array, node1);
    addNodeToArray(array, node2);
    addNodeToArray(array, node3);
    char* string = arrayToString(array);
    char* expectedResult = "         3220[\"paw\",\"pi\",21]";
    return strcmp(string,expectedResult ) == 0;
}


int testArrayOfArraytoString(void)
{
    arrayADT array = newArray();
    arrayADT subjects = newArray();

    char* subject1 =  "poo";
    char* subject2 =  "pi";
    char* subject3 =  "paw";

    int age = 21;
    char* name =  "santi";

    arrayNodeADT node1 = newArrayNode(subject1, String);
    arrayNodeADT node2 = newArrayNode(subject2, String);
    arrayNodeADT node3 = newArrayNode(subject3, String);
    addNodeToArray(subjects, node1);
    addNodeToArray(subjects, node2);
    addNodeToArray(subjects, node3);
    arrayNodeADT nameNode = newArrayNode(name, String);
    arrayNodeADT ageNode = newArrayNode(&age, Integer);
    addNodeToArray(array, nameNode);
    addNodeToArray(array, ageNode);
    arrayNodeADT subjectsNode = newArrayNode(subjects, Array);
    addNodeToArray(array, subjectsNode);
    char* string = arrayToString(array);
    char* expectedResult = "         3302[         3222[\"paw\",\"pi\",\"poo\"],21,\"santi\"]";
    return strcmp(string,expectedResult ) == 0;
}

int testObjectToString()
{
    objectADT object = newObject("name", "Santi", String);
    char* objectRepresentation = objectToString(object);
    char* expectedResult = "\"name\":\"Santi\"";

    return strcmp(expectedResult, objectRepresentation) == 0;
}

int testJsonToString()
{
    jsonADT json = newJsonObject();
    objectADT object =  newObject("name", "santiago",String);
    addObject(json, object);

    char* string = jsonToString(json);
    char* expectedResult = "{\"name\":\"santiago\"}";

    return strcmp(string, expectedResult) == 0;
}

int testSerializeString()
{
    char* test = "this is a test";
    char* result = serializeString(test);
    char* expectedResult = "        14this is a test";
    return strcmp(expectedResult, result) == 0;
}


int testBuffer()
{
    bufferADT buffer = newBuffer();
    char* testString = "santi";
    int size = strlen(testString) + 1;
    addToBuffer(buffer, testString, size);
    char* str = malloc(size * sizeof(char));
    getLastFromBuffer(buffer, str, size);
    freeBuffer(buffer);
    return strcmp(str, testString) == 0;
}

int testSerialize()
{
    arrayADT array = newArray();
    arrayADT subjects = newArray();

    char* subject1 =  "poo";
    char* subject2 =  "pi";
    char* subject3 =  "paw";

    int age = 21;
    char* name =  "santi";

    arrayNodeADT node1 = newArrayNode(subject1, String);
    arrayNodeADT node2 = newArrayNode(subject2, String);
    arrayNodeADT node3 = newArrayNode(subject3, String);
    addNodeToArray(subjects, node1);
    addNodeToArray(subjects, node2);
    addNodeToArray(subjects, node3);
    arrayNodeADT nameNode = newArrayNode(name, String);
    arrayNodeADT ageNode = newArrayNode(&age, Integer);
    addNodeToArray(array, nameNode);
    addNodeToArray(array, ageNode);
    arrayNodeADT subjectsNode = newArrayNode(subjects, Array);
    addNodeToArray(array, subjectsNode);
    serialize(array, Array);
    return 1;
}


//  beginnging of deserialization tests ---------------------------------------

int testvalueOfDouble()
{
    char* value = "450.00";
    double expectedResult = 450.00;
    double number = valueOfDouble(value);
    return number - expectedResult == 0;
}

int testvalueOfInt()
{
    char* value = "21";
    int expectedResult = 21;
    int number = valueOfInt(value);
    return number - expectedResult == 0;
}


int testValueOfString()
{
    char* testString = "santiago";
    char* expectedResult =  " santiago";

    char* string = valueOfString(testString);
    return strcmp(string, expectedResult) == 0;
}


int testStringToArray()
{
    char* testArray ="         3302[         3222[\"paw\",\"pi\",\"poo\"],21,\"santi\"]";
    //char* testEasy = "         3222[\"santi\",\"fran\",\"bian\"]";
    arrayADT array = deserializeArray(testArray);
    printf("the first thing is the size ...%d\n",getArraySize(array));


    printf("secondly the content .... %s\n", getValueInArray(array,0));
    return 1;
}


int testStringToObject()
{
    char* objectString = "\"name\":\"santiago\"";
    objectADT object = deserializeObject(objectString);
    printf("the result was... %s\n", getObjectValue(object));
    return 1;
}

int testDeserializeString()
{
    char* test = "        14this is a test";
    char* result = deserializeString(test);
    char* expectedResult  = "this is a test";
    return strcmp(expectedResult, result) == 0;
}


int testSerializationDeserialization()
{
    arrayADT array = newArray();
    arrayADT subjects = newArray();

    char* subject1 =  "poo";
    char* subject2 =  "pi";
    char* subject3 =  "paw";
    int age = 21;
    char* name =  "santi";

    arrayNodeADT node1 = newArrayNode(subject1, String);
    arrayNodeADT node2 = newArrayNode(subject2, String);
    arrayNodeADT node3 = newArrayNode(subject3, String);
    addNodeToArray(subjects, node1);
    addNodeToArray(subjects, node2);
    addNodeToArray(subjects, node3);
    arrayNodeADT nameNode = newArrayNode(name, String);
    arrayNodeADT ageNode = newArrayNode(&age, Integer);
    addNodeToArray(array, nameNode);
    addNodeToArray(array, ageNode);
    arrayNodeADT subjectsNode = newArrayNode(subjects, Array);
    addNodeToArray(array, subjectsNode);
    printf("the first element is %s\n",  getValueInArray(array,0));
    char* str = serialize(array, Array);
    printf("the serialization thorws %s\n", str);
    printf("this is what the deserialization is recieving ..\n");
    printf("%s\n",str+10);
    arrayADT deserializedArray = deserialize(str+10);

    printf("the size is %d\n", getArraySize(deserializedArray) );
    //printf("the first element is %s\n", getValueInArray(deserializedArray,0));
    //str = serialize(deserializedArray, Array );
    //printf("the serialization thorws %s\n", str);
    return 1;
}


int main(void)
{
    printf("going to run tests...\n");
    printf(" the result was %s\n",(test_intToString() == 0)?"False" : "True" );
    printf(" the result was %s\n",(test_doubleToString() == 0)?"False" : "True");
    printf(" the result was %s\n",(testBuffer() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testArrayToString() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testArrayOfArraytoString() == 0)?"False" : "True");
    printf(" the result was %s\n",(testObjectToString() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testJsonToString() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testSerializeString() == 0)?"False" : "True" );
    printf("DONE! serialization tests\n");
    printf("\n");
    printf("\n");
    printf("Going to test deserializatino...\n");
    printf(" the result was %s\n",(testvalueOfDouble() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testvalueOfInt() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testDeserializeString() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testStringToArray() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testSerialize() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testSerializationDeserialization() == 0)?"False" : "True" );
    //printf(" the result was %s\n",(testStringToObject() == 0)?"False" : "True" );
}


// funcion serialize(Void*);

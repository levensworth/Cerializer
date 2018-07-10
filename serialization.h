#ifndef SERIALIZATIOIN_H
#define SERIALIZATIOIN_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// supported data types
// arrays must be the ones declare in here no native array will be supported
typedef enum type {Integer=0, Double=1, String=2, Array=3, Object=4, Json=5, Undefined=6} Type;

// enumw for decerialization
typedef enum state {incrementWeight, decrementWeight, addValue, createNode} State;
typedef enum stateObject {keyState, valueState, endKey} stateObject;

#define HEADER_SIZE 10 // the number of bytes need to string representation of max_int


typedef struct arrayNodeCDT{
    struct arrayNodeCDT * next;
    void * value;
    Type type;

} arrayNodeCDT;
typedef arrayNodeCDT * arrayNodeADT;

typedef struct arrayCDT{
    int size;
    arrayNodeADT first;
}arrayCDT;

typedef arrayCDT * arrayADT;

// this is object structures

typedef struct objectCDT{
    char* key;
    void* value;
    Type type;
}objectCDT;

typedef objectCDT* objectADT;

typedef struct jsonCDT{
    arrayADT nodes;
    int size;
}jsonCDT;
typedef jsonCDT* jsonADT;

// end on object structures

// buffer structures

typedef struct bufferCDT{
    void* buffer;
    int size;
}bufferCDT;

typedef bufferCDT* bufferADT;


// end of buffer structures

//---------------Array contract------------------------------------------------

arrayADT newArray();
arrayNodeADT newArrayNode(void * value, Type type);
int getArraySize(arrayADT array);
int addNodeToArray(arrayADT array, arrayNodeADT node);
void* getValueInArray(arrayADT array ,int index);
Type getTypeFromIndex(arrayADT array, int index);
int deleteNodeFromArray(arrayADT array, int index);

void freeArray(arrayADT array);
static void freeArrayNode(arrayNodeADT node);
static void freeList(arrayNodeADT node);
static void* searchForValue(arrayNodeADT node, int index);
static arrayNodeADT getNodeAt(arrayNodeADT node, int index);
static arrayNodeADT removeNode(arrayNodeADT node, int index);
char* arrayNodeToString(arrayNodeADT node);
char * toString(void * value, Type type);
static void addArrayHeaders(bufferADT buffer, arrayADT array);
static void addNodeToEndOfArray(arrayADT array, arrayNodeADT node);
static arrayNodeADT appendNodeToArray(arrayNodeADT current, arrayNodeADT node);
// primitives string methods
char* intToString(int number);
char* doubleToString(double number);
char* stringToString( char* string);
char* arrayToString(arrayADT array);

// serialization for array
char* serialize(void* obj, Type type);
// deserialization for array
void* deserialize(char* string);// para fran
void deserializeType(void* destination, char* string, Type type);
static State getStateArray(char c, int weight);
arrayADT deserializeArray(char* string);
void deserializeArrayNodeAt(arrayADT array, int index, Type type);
arrayADT  parseArray(char* string, Type types[], int numberOfAtributes);
int valueOfInt(char* string);
double valueOfDouble(char* string);
char* valueOfString(char* string);
void* getTypeSpace(Type type, char* string);// puede que sea static
int getTypeSize(Type type, char* string);
static objectADT parseObject(char* string );
//----------------End of array contract----------------------------------------

//------------------- beginnig of buffer contract -----------------------------
bufferADT newBuffer();
int addToBuffer(bufferADT buffer,void* value, int size);
int getBufferSize(bufferADT buffer);
void getLastFromBuffer(bufferADT buffer, void* destination, int size);
void freeBuffer(bufferADT buffer);
static int addSpaceToBuffer(bufferADT buffer, int size);
//------------------------------- end -----------------------------------------

//-------------------- Beginning of Object contract ---------------------------
objectADT newObject(char* key, void* value, Type type);
void* getObjectValue(objectADT object);
char* getObjectKey(objectADT object);
Type getObjectType(objectADT object);
char* objectToString(objectADT object);
void freeObject(objectADT object);
//------------------------ End of Object Contract -----------------------------

//-------------------- Beginning of Json contract -----------------------------

jsonADT newJsonObject();
int addObject(jsonADT json, objectADT object);
int removeObject(jsonADT json, char* key);
char * jsonToString(jsonADT json);
//------------------------ End of Json Contract -------------------------------

// serialization and deserialization primitives independently
char* deserializeString(char* serializeString);
char* serializeString(char* string);

#endif

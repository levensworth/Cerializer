#include "serialization.h"

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
    printf("this is what the des in=s recieving ..\n");
    printf("%s\n",str+10);
    arrayADT deserializedArray = deserialize(str+10);
    printf("the first element is %s\n", getValueInArray(deserializedArray,0));
    str = serialize(deserializedArray, Array );
    printf("the serialization thorws %s\n", str);
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
    printf("DONE! serialization tests\n");
    printf("\n");
    printf("\n");
    printf("Going to test deserializatino...\n");
    printf(" the result was %s\n",(testvalueOfDouble() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testvalueOfInt() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testStringToArray() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testSerialize() == 0)?"False" : "True" );
    printf(" the result was %s\n",(testSerializationDeserialization() == 0)?"False" : "True" );
    //printf(" the result was %s\n",(testStringToObject() == 0)?"False" : "True" );
}

